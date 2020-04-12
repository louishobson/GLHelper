/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * src/glhelper/glhelper_model.cpp
 * 
 * implementation of include/glhelper/glhelper_model.hpp
 * 
 */



/* INCLUDES */

/* include glhelper_model.hpp */
#include <glhelper/glhelper_model.hpp>



/* MODEL IMPLEMENTATION */

/* constructor
 *
 * import a model from a file
 * 
 * _directory: directory in which the model resides
 * _entry: the entry file to the model
 * _pps: post processing steps (or default recommended)
 */
glh::model::model::model ( const std::string& _directory, const std::string& _entry, const unsigned _pps )
    : directory { _directory }
    , entry { _entry }
    , pps { _pps | aiProcess_Triangulate }
{
    /* create the importer */
    Assimp::Importer importer;

    /* import the scene */
    const aiScene * aiscene = importer.ReadFile ( directory + "/" + entry, pps );

    /* check for failure */
    if ( !aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ) throw model_exception { "failed to import model at path " + directory + "/" + entry + " with error " + importer.GetErrorString () };

    /* process the scene */
    process_scene ( * aiscene );
}



/* render
 *
 * render the model
 * 
 * material_uni: material uniform to cache and set the material properties to
 * model_uni: a 4x4 matrix uniform to cache and apply set the model transformations to
 * transform: the overall model transformation to apply (identity by default)
 * transparent_only: only render meshes with possible transparent elements (false by default)
 */
void glh::model::model::render ( const struct_uniform& material_uni, const uniform& model_uni, const math::mat4& transform, const bool transparent_only )
{
    /* reload the cache of material uniforms  */
    cache_material_uniforms ( material_uni );

    /* reload the model uniform */
    cache_model_uniform ( model_uni );

    /* render */
    render ( transform, transparent_only );
}
void glh::model::model::render ( const math::mat4& transform, const bool transparent_only ) const
{
    /* throw if uniforms are not already cached */
    if ( !cached_material_uniforms || !cached_model_uniform ) throw uniform_exception { "attempted to render model without a complete uniform cache" };

    /* set transparent_only flag */
    draw_transparent_only = transparent_only;

    /* render the root node */
    render_node ( root_node, transform );
}

/* cache_material_uniforms
 *
 * cache a material uniform for later use
 * 
 * material_uni: the uniform to cache
 */
void glh::model::model::cache_material_uniforms ( const struct_uniform& material_uni )
{
    /* cache material uniform if not already cached */
    if ( !cached_material_uniforms || cached_material_uniforms->material_uni != material_uni )
    {
        cached_material_uniforms.reset ( new cached_material_uniforms_struct
        {
            material_uni,
            material_uni.get_struct_uniform ( "ambient_stack" ).get_uniform ( "stack_size" ),
            material_uni.get_struct_uniform ( "diffuse_stack" ).get_uniform ( "stack_size" ),
            material_uni.get_struct_uniform ( "specular_stack" ).get_uniform ( "stack_size" ),
            material_uni.get_struct_uniform ( "ambient_stack" ).get_uniform ( "base_color" ),
            material_uni.get_struct_uniform ( "diffuse_stack" ).get_uniform ( "base_color" ),
            material_uni.get_struct_uniform ( "specular_stack" ).get_uniform ( "base_color" ),
            material_uni.get_struct_uniform ( "ambient_stack" ).get_struct_array_uniform ( "levels" ),
            material_uni.get_struct_uniform ( "diffuse_stack" ).get_struct_array_uniform ( "levels" ),
            material_uni.get_struct_uniform ( "specular_stack" ).get_struct_array_uniform ( "levels" ),
            material_uni.get_uniform ( "blending_mode" ),
            material_uni.get_uniform ( "shininess" ),
            material_uni.get_uniform ( "shininess_strength" ),
            material_uni.get_uniform ( "opacity" )
         } );
    }
}

/* cache_model_uniform
 *
 * cache a model uniform for later sue
 * 
 * model_uni: the uniform to cache
 */
void glh::model::model::cache_model_uniform ( const uniform& model_uni )
{
    /* cache model uniform if not already cached */
    if ( !cached_model_uniform || * cached_model_uniform != model_uni )
    cached_model_uniform.reset ( new uniform { model_uni } );
}

/* cache_uniform
 *
 * cache all uniforms simultaneously
 *
 * material_uni: the material uniform to cache
 * model_uni: model uniform to cache
 */
void glh::model::model::cache_uniforms ( const struct_uniform& material_uni, const uniform& model_uni )
{
    /* cache all uniforms */
    cache_material_uniforms ( material_uni );
    cache_model_uniform ( model_uni );
}



/* process_scene
 *
 * build from a scene object
 * 
 * aiscene: the scene object to build from
 */
void glh::model::model::process_scene ( const aiScene& aiscene )
{
    /* firstly process all of the materials */
    materials.resize ( aiscene.mNumMaterials );
    for ( unsigned i = 0; i < aiscene.mNumMaterials; ++i )
    {
        add_material ( materials.at ( i ), * aiscene.mMaterials [ i ] );
    }

    /* add the texture pointers to the materials */
    for ( material& m: materials )
    {
        for ( texture_stack_level& tsl: m.ambient_stack.levels ) tsl.texture = &textures.at ( tsl.index );
        for ( texture_stack_level& tsl: m.diffuse_stack.levels ) tsl.texture = &textures.at ( tsl.index );
        for ( texture_stack_level& tsl: m.specular_stack.levels ) tsl.texture = &textures.at ( tsl.index );
    }

    /* now add the meshes */
    meshes.resize ( aiscene.mNumMeshes );
    for ( unsigned i = 0; i < aiscene.mNumMeshes; ++i )
    {
        add_mesh ( meshes.at ( i ), * aiscene.mMeshes [ i ] );
    }

    /* now recursively process all of the nodes */
    add_node ( root_node, * aiscene.mRootNode );
}

/* add_material
 *
 * take an assimp material object and add it to the store
 * this will, in turn, add any textures required to the store
 * 
 * _material: the material to configure
 * aimaterial: the assimp material object to add
 * 
 * return: the material just added
 */
glh::model::material& glh::model::model::add_material ( material& _material, const aiMaterial& aimaterial )
{
    /* temportary get storage */
    aiColor3D temp_color;
    float temp_float;
    int temp_int;

    /* set the base colors of the stacks */
    if ( aimaterial.Get ( AI_MATKEY_COLOR_AMBIENT, temp_color ) == aiReturn_SUCCESS )
    _material.ambient_stack.base_color = cast_vector ( temp_color ); else _material.ambient_stack.base_color = math::vec3 { 0.0 };
    if ( aimaterial.Get ( AI_MATKEY_COLOR_DIFFUSE, temp_color ) == aiReturn_SUCCESS )
    _material.diffuse_stack.base_color = cast_vector ( temp_color ); else _material.diffuse_stack.base_color = math::vec3 { 0.0 };
    if ( aimaterial.Get ( AI_MATKEY_COLOR_SPECULAR, temp_color ) == aiReturn_SUCCESS )
    _material.specular_stack.base_color = cast_vector ( temp_color ); else _material.specular_stack.base_color = math::vec3 { 0.0 };

    /* set up the ambient texture stack textures
     * if ambient textures are present, use those
     * otherwise borrow the diffuse textures
     */
    if ( aimaterial.GetTextureCount ( aiTextureType_AMBIENT ) > 0 )
    {
        _material.ambient_stack.levels.resize ( aimaterial.GetTextureCount ( aiTextureType_AMBIENT ) );
        for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_AMBIENT ); ++i )
        {
            add_texture ( _material.ambient_stack.levels.at ( i ), aimaterial, i, aiTextureType_AMBIENT );  
        }
    } else
    {
        _material.ambient_stack.base_color = _material.diffuse_stack.base_color;
        _material.ambient_stack.levels.resize ( aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ) );
        for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ); ++i )
        {
            add_texture ( _material.ambient_stack.levels.at ( i ), aimaterial, i, aiTextureType_DIFFUSE );  
        }
    }
    

    /* set up the diffuse texture stack textures */
    _material.diffuse_stack.levels.resize ( aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ) );
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ); ++i )
    {
        add_texture ( _material.diffuse_stack.levels.at ( i ), aimaterial, i, aiTextureType_DIFFUSE );  
    }

    /* set up the specular texture stack textures */
    _material.specular_stack.levels.resize ( aimaterial.GetTextureCount ( aiTextureType_SPECULAR ) );
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_SPECULAR ); ++i )
    {
        add_texture ( _material.specular_stack.levels.at ( i ), aimaterial, i, aiTextureType_SPECULAR );  
    }

    /* get the blend mode */
    if ( aimaterial.Get ( AI_MATKEY_BLEND_FUNC, temp_int ) == aiReturn_SUCCESS )
    _material.blending_mode = temp_int; else _material.blending_mode = 0;

    /* get the shininess properties */
    if ( aimaterial.Get ( AI_MATKEY_SHININESS, temp_float ) == aiReturn_SUCCESS )
    _material.shininess = temp_float; else _material.shininess = 0.0;
    if ( aimaterial.Get ( AI_MATKEY_SHININESS_STRENGTH, temp_float ) == aiReturn_SUCCESS )
    _material.shininess_strength = temp_float; else _material.shininess_strength = 1.0;

    /* get the opacity */
    if ( aimaterial.Get ( AI_MATKEY_OPACITY, temp_float ) == aiReturn_SUCCESS )
    _material.opacity = temp_float; else _material.opacity = 1.0;

    /* get if two sided */
    if ( aimaterial.Get ( AI_MATKEY_TWOSIDED, temp_int ) == aiReturn_SUCCESS )
    _material.two_sided = temp_int; else _material.two_sided = false;

    /* get the shading mode and the program to use */
    if ( aimaterial.Get ( AI_MATKEY_SHADING_MODEL, temp_int ) == aiReturn_SUCCESS )
    _material.shading_model = temp_int; else _material.shading_model = 0;

    /* set definitely opaque flag */
    _material.definitely_opaque = is_definitely_opaque ( _material );

    /* return the material */
    return _material;
}

/* add_texture
 *
 * add a texture to a texture stack
 * 
 * _texture_stack_level: the location of the level in the stack texture
 * aimaterial: the material the texture is being added from
 * index: the index of the texture
 * aitexturetype: the type of the stack
 * 
 * return: the texture stack level just added
 */
glh::model::texture_stack_level& glh::model::model::add_texture ( texture_stack_level& _texture_stack_level, const aiMaterial& aimaterial, const unsigned index, const aiTextureType aitexturetype )
{
    /* temporary variables */
    aiString temp_string;
    float temp_float;
    int temp_int;

    /* get the location of the texture */
    aimaterial.GetTexture ( aitexturetype, index, &temp_string );
    std::string texpath { temp_string.C_Str () };
    
    /* replace possibly bas lettes */
    for ( char& c: texpath )
    {
        if ( c == '\\' ) c = '/';
    }

    /* set the blend attributes */
    if ( aimaterial.Get ( AI_MATKEY_TEXOP ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    _texture_stack_level.blend_operation = temp_int; else _texture_stack_level.blend_operation = 0;
    if ( aimaterial.Get ( AI_MATKEY_TEXBLEND ( aitexturetype, index ), temp_float ) == aiReturn_SUCCESS ) 
    _texture_stack_level.blend_strength = temp_float; else _texture_stack_level.blend_strength = 1.0;

    /* set wrapping modes */
    if ( aimaterial.Get ( AI_MATKEY_MAPPINGMODE_U ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    _texture_stack_level.wrapping_u = temp_int; else _texture_stack_level.wrapping_u = aiTextureMapMode_Wrap;
    if ( aimaterial.Get ( AI_MATKEY_MAPPINGMODE_V ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    _texture_stack_level.wrapping_v = temp_int; else _texture_stack_level.wrapping_v = aiTextureMapMode_Wrap;

    /* set uvwsrc */
    if ( aimaterial.Get ( AI_MATKEY_UVWSRC ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    _texture_stack_level.uvwsrc = temp_int; else _texture_stack_level.uvwsrc = 0;

    /* check if already inported */
    for ( unsigned i = 0; i < textures.size (); ++i ) if ( textures.at ( i ).get_path () == texpath )
    {
        /* already imported, so purely set the index */
        _texture_stack_level.index = i;

        /* return texture reference */
        return _texture_stack_level;
    }

    /* not already imported, so import and set the index */
    textures.push_back ( texture2d { directory + "/" + texpath } );
    _texture_stack_level.index = textures.size () - 1;

    /* return texture reference */
    return _texture_stack_level;
}

/* is_definitely_opaque
 *
 * determines if a material is definitely opaque
 * see struct material for more info
 * 
 * _material: the material to check
 *
 * return: boolean for if is definitely opaque
 */
bool glh::model::model::is_definitely_opaque ( const material& _material )
{
    /* if opacity != 1.0, return false */
    if ( _material.opacity != 1.0 ) return false;

    /* loop through all textures and return false if any have 4 channels
     * use index rather than texture pointer in stack level as they have not yet been assigned
     */
    for ( const texture_stack_level& tsl: _material.ambient_stack.levels ) if ( textures.at ( tsl.index ).get_channels () >= 4 ) return false;
    for ( const texture_stack_level& tsl: _material.diffuse_stack.levels ) if ( textures.at ( tsl.index ).get_channels () >= 4 ) return false;
    for ( const texture_stack_level& tsl: _material.specular_stack.levels ) if ( textures.at ( tsl.index ).get_channels () >= 4 ) return false;

    /* otherwise the material is definitely opaque */
    return true;
}

/* add_mesh
 *
 * add a mesh to a node
 *
 * _mesh: the mesh to configure
 * aimesh: the assimp mesh object to add
 * 
 * return: the mesh just added
 */
glh::model::mesh& glh::model::model::add_mesh ( mesh& _mesh, const aiMesh& aimesh )
{
    /* add vertices, normals and texcoords */
    _mesh.vertices.resize ( aimesh.mNumVertices );
    for ( unsigned i = 0; i < aimesh.mNumVertices; ++i )
    {
        /* add vertices and normals */
        _mesh.vertices.at ( i ).position = cast_vector ( aimesh.mVertices [ i ] );
        _mesh.vertices.at ( i ).normal = cast_vector ( aimesh.mNormals [ i ] );

        /* resize texcoords to number of uv components and set them */
        _mesh.vertices.at ( i ).texcoords.resize ( aimesh.GetNumUVChannels () );
        for ( unsigned j = 0; j < aimesh.GetNumUVChannels (); ++j )
        _mesh.vertices.at ( i ).texcoords.at ( j ) = cast_vector ( aimesh.mTextureCoords [ j ][ i ] );
    }

    /* set the number of uv channels */
    _mesh.num_uv_channels = aimesh.GetNumUVChannels ();

    /* add material reference */
    _mesh.properties_index = aimesh.mMaterialIndex;
    _mesh.properties = &materials.at ( _mesh.properties_index );

    /* add faces */
    _mesh.faces.resize ( aimesh.mNumFaces );
    for ( unsigned i = 0; i < aimesh.mNumFaces; ++i ) add_face ( _mesh.faces.at ( i ), _mesh, aimesh.mFaces [ i ] );

    /* configure the buffers */
    configure_buffers ( _mesh );

    /* return the mesh */
    return _mesh;
}

/* add_face
 *
 * add a face to a mesh
 * 
 * _face: the face to configure
 * _mesh: the mesh parent to the face
 * aiface: the assimp face object to add
 * 
 * return: the face just added
 */
glh::model::face& glh::model::model::add_face ( face& _face, mesh& _mesh, const aiFace& aiface )
{
    /* set indices */
    _face.indices.resize ( aiface.mNumIndices );
    _face.vertices.resize ( aiface.mNumIndices );
    for ( unsigned i = 0; i < aiface.mNumIndices; ++i )
    {
        _face.indices.at ( i ) = aiface.mIndices [ i ];
        _face.vertices.at ( i ) = &_mesh.vertices.at ( _face.indices.at ( i ) );
    }

    /* return face */
    return _face;
}

/* configure_buffers
 *
 * configure the buffers of a mesh
 * 
 * _mesh: the mesh to configure
 */
void glh::model::model::configure_buffers ( mesh& _mesh )
{
    /* create temporary array for vertex data */
    const unsigned components_per_vertex = 6 + ( 3 * _mesh.num_uv_channels );
    GLfloat vertices [ _mesh.vertices.size () * components_per_vertex ];
    for ( unsigned i = 0; i < _mesh.vertices.size (); ++i )
    {
        vertices [ ( i * components_per_vertex ) + 0 ] = _mesh.vertices.at ( i ).position.at ( 0 );
        vertices [ ( i * components_per_vertex ) + 1 ] = _mesh.vertices.at ( i ).position.at ( 1 );
        vertices [ ( i * components_per_vertex ) + 2 ] = _mesh.vertices.at ( i ).position.at ( 2 );
        vertices [ ( i * components_per_vertex ) + 3 ] = _mesh.vertices.at ( i ).normal.at ( 0 );
        vertices [ ( i * components_per_vertex ) + 4 ] = _mesh.vertices.at ( i ).normal.at ( 1 );
        vertices [ ( i * components_per_vertex ) + 5 ] = _mesh.vertices.at ( i ).normal.at ( 2 );
        for ( unsigned j = 0; j < _mesh.num_uv_channels; ++j )
        {
            vertices [ ( i * components_per_vertex ) + ( j * 3 ) + 6 ] = _mesh.vertices.at ( i ).texcoords.at ( j ).at ( 0 );
            vertices [ ( i * components_per_vertex ) + ( j * 3 ) + 7 ] = _mesh.vertices.at ( i ).texcoords.at ( j ).at ( 1 );
            vertices [ ( i * components_per_vertex ) + ( j * 3 ) + 8 ] = _mesh.vertices.at ( i ).texcoords.at ( j ).at ( 2 );
        }        
    }

    /* create temporary array to store indices */
    GLuint indices [ _mesh.faces.size () * 3 ];
    for ( unsigned i = 0; i < _mesh.faces.size (); ++i )
    {
        indices [ ( i * 3 ) + 0 ] = _mesh.faces.at ( i ).indices.at ( 0 );
        indices [ ( i * 3 ) + 1 ] = _mesh.faces.at ( i ).indices.at ( 1 );
        indices [ ( i * 3 ) + 2 ] = _mesh.faces.at ( i ).indices.at ( 2 );
    }

    /* buffer the data */
    _mesh.vertex_data.buffer_data ( sizeof ( vertices ), vertices, GL_STATIC_DRAW );
    _mesh.index_data.buffer_data ( sizeof ( indices ), indices, GL_STATIC_DRAW );

    /* configure the vao */
    _mesh.array_object.set_vertex_attrib ( 0, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, components_per_vertex * sizeof ( GLfloat ), ( GLvoid * ) 0 );
    _mesh.array_object.set_vertex_attrib ( 1, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, components_per_vertex * sizeof ( GLfloat ), ( GLvoid * ) ( 3 * sizeof ( GLfloat ) ) );
    for ( unsigned i = 0; i < _mesh.num_uv_channels; ++i )
    {
        _mesh.array_object.set_vertex_attrib ( 2 + i, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, components_per_vertex * sizeof ( GLfloat ), ( GLvoid * ) ( ( ( i * 3 ) + 6 ) * sizeof ( GLfloat ) ) );
    }
    _mesh.array_object.bind_ebo ( _mesh.index_data );
}

/* add_node
 *
 * recursively add nodes to the node tree
 * 
 * _node: the node to configure
 * ainode: the assimp node object to add
 * 
 * return: the node just added
 */
glh::model::node& glh::model::model::add_node ( node& _node, const aiNode& ainode )
{
    /* set all of the children first */
    _node.children.resize ( ainode.mNumChildren );
    for ( unsigned i = 0; i < ainode.mNumChildren; ++i ) add_node ( _node.children.at ( i ), * ainode.mChildren [ i ] );

    /* now configure the meshes */
    _node.mesh_indices.resize ( ainode.mNumMeshes );
    _node.meshes.resize ( ainode.mNumMeshes );
    for ( unsigned i = 0; i < ainode.mNumMeshes; ++i )
    {
        _node.mesh_indices.at ( i ) = ainode.mMeshes [ i ];
        _node.meshes.at ( i ) = &meshes.at ( _node.mesh_indices.at ( i ) );
    }

    /* set transformation */
    _node.transform = cast_matrix ( ainode.mTransformation );

    /* return node */
    return _node;
}

/* render_node
 *
 * render the model
 * 
 * _node: the node to render
 * transform: the current model transformation from all the previous nodes
 */
void glh::model::model::render_node ( const node& _node, const math::mat4& transform ) const
{
    /* create transformation matrix */
    math::mat4 trans = transform * _node.transform;

    /* first render the child nodes */
    for ( const node& child: _node.children ) render_node ( child, trans );

    /* set the model matrix */
    cached_model_uniform->set_matrix ( trans );

    /* render all of the meshes */
    for ( const mesh * m: _node.meshes ) render_mesh ( * m );
}

/* render_mesh
 *
 * render a mesh
 * 
 * _mesh: the mesh to render
 */
void glh::model::model::render_mesh ( const mesh& _mesh ) const
{
    /* don't draw if transparent only is set and material is definitely opaque */
    if ( draw_transparent_only && _mesh.properties->definitely_opaque ) return;

    /* get if face culling is on, and disable if mesh is two sided */
    const bool culling_active = glh::renderer::face_culling_enabled ();
    if ( culling_active && _mesh.properties->two_sided ) glh::renderer::disable_face_culling ();

    /* set the stack sizes */
    cached_material_uniforms->ambient_stack_size_uni.set_int ( _mesh.properties->ambient_stack.levels.size () );
    cached_material_uniforms->diffuse_stack_size_uni.set_int ( _mesh.properties->diffuse_stack.levels.size () );
    cached_material_uniforms->specular_stack_size_uni.set_int ( _mesh.properties->specular_stack.levels.size () );

    /* set the color values */
    cached_material_uniforms->ambient_stack_base_color_uni.set_vector ( _mesh.properties->ambient_stack.base_color );
    cached_material_uniforms->diffuse_stack_base_color_uni.set_vector ( _mesh.properties->diffuse_stack.base_color );
    cached_material_uniforms->specular_stack_base_color_uni.set_vector ( _mesh.properties->specular_stack.base_color ); 

    /* bind the textures and set the sampler values */
    unsigned i = 0, offset = 0;
    for ( i = 0; i < _mesh.properties->ambient_stack.levels.size (); ++i, ++offset ) 
    {
        _mesh.properties->ambient_stack.levels.at ( i ).texture->bind ( offset );
        _mesh.properties->ambient_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( _mesh.properties->ambient_stack.levels.at ( i ).wrapping_u ) );
        _mesh.properties->ambient_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( _mesh.properties->ambient_stack.levels.at ( i ).wrapping_v ) );
        cached_material_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( offset++ );
        cached_material_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( _mesh.properties->ambient_stack.levels.at ( i ).blend_operation );
        cached_material_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( _mesh.properties->ambient_stack.levels.at ( i ).blend_strength );
        cached_material_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( _mesh.properties->ambient_stack.levels.at ( i ).uvwsrc );
    }
    for ( i = 0; i < _mesh.properties->diffuse_stack.levels.size (); ++i, ++offset ) 
    {
        _mesh.properties->diffuse_stack.levels.at ( i ).texture->bind ( offset );
        _mesh.properties->diffuse_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( _mesh.properties->diffuse_stack.levels.at ( i ).wrapping_u ) );
        _mesh.properties->diffuse_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( _mesh.properties->diffuse_stack.levels.at ( i ).wrapping_v ) );
        cached_material_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( offset++ );
        cached_material_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( _mesh.properties->diffuse_stack.levels.at ( i ).blend_operation );
        cached_material_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( _mesh.properties->diffuse_stack.levels.at ( i ).blend_strength );
        cached_material_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( _mesh.properties->diffuse_stack.levels.at ( i ).uvwsrc );
    }
    for ( i = 0; i < _mesh.properties->specular_stack.levels.size (); ++i, ++offset ) 
    {
        _mesh.properties->specular_stack.levels.at ( i ).texture->bind ( offset );
        _mesh.properties->specular_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( _mesh.properties->specular_stack.levels.at ( i ).wrapping_u ) );
        _mesh.properties->specular_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( _mesh.properties->specular_stack.levels.at ( i ).wrapping_v ) );
        cached_material_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( offset++ );
        cached_material_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( _mesh.properties->specular_stack.levels.at ( i ).blend_operation );
        cached_material_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( _mesh.properties->specular_stack.levels.at ( i ).blend_strength );
        cached_material_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( _mesh.properties->specular_stack.levels.at ( i ).uvwsrc );
    }

    /* set blending mode */
    cached_material_uniforms->blending_mode_uni.set_int ( _mesh.properties->blending_mode );

    /* set shininess values */
    cached_material_uniforms->shininess_uni.set_float ( _mesh.properties->shininess );
    cached_material_uniforms->shininess_strength_uni.set_float ( _mesh.properties->shininess_strength );

    /* set opacity */
    cached_material_uniforms->opacity_uni.set_float ( _mesh.properties->opacity );

    /* bind the vao */
    _mesh.array_object.bind ();

    /* draw elements */
    renderer::draw_elements ( GL_TRIANGLES, _mesh.faces.size () * 3, GL_UNSIGNED_INT, 0 );

    /* unbind the vao */
    _mesh.array_object.unbind ();

    /* re-enable face culling if was previously disabled */
    if ( culling_active && _mesh.properties->two_sided ) glh::renderer::enable_face_culling ();
}