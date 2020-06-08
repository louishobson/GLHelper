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
 * _model_import_flags: import flags for the model (or default recommended)
 * _pps: post processing steps (or default recommended)
 */
glh::model::model::model ( const std::string& _directory, const std::string& _entry, const unsigned _model_import_flags, const unsigned _pps )
    : directory { _directory }
    , entry { _entry }
    , model_import_flags { _model_import_flags }
    , pps { _pps | aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_TransformUVCoords }
{
    /* create the importer */
    Assimp::Importer importer;

    /* import the scene */
    const aiScene * aiscene = importer.ReadFile ( directory + "/" + entry, pps );

    /* check for failure */
    if ( !aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ) throw exception::model_exception { "failed to import model at path " + directory + "/" + entry + " with error " + importer.GetErrorString () };

    /* process the scene */
    process_scene ( * aiscene );
}



/* render
 *
 * render the model
 * 
 * prog: the program to use for rendering
 * material_uni: material uniform to cache and set the material properties to
 * model_uni: a 4x4 matrix uniform to cache and apply set the model transformations to
 * transform: the overall model transformation to apply (identity by default)
 * flags: rendering flags (none by default)
 */
void glh::model::model::render ( const core::program& prog, core::struct_uniform& material_uni, core::uniform& model_uni, const math::mat4& transform, const unsigned flags )
{
    /* reload the cache of uniforms  */
    cache_uniforms ( material_uni, model_uni );

    /* render */
    render ( prog, transform, flags );
}
void glh::model::model::render ( const core::program& prog, const math::mat4& transform, const unsigned flags ) const
{
    /* throw if uniforms are not already cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "attempted to render model without a complete uniform cache" };

    /* cache the render flags */
    model_render_flags = flags;

    /* render the root node */
    render_node ( root_node, prog, transform );
}



/* cache_uniforms
 *
 * cache all uniforms
 *
 * material_uni: the material uniform to cache
 * model_uni: model uniform to cache
 */
void glh::model::model::cache_uniforms ( core::struct_uniform& material_uni, core::uniform& model_uni )
{
    /* if uniforms are not already cached, cache the new ones */
    if ( !cached_uniforms || cached_uniforms->material_uni != material_uni || cached_uniforms->model_uni != model_uni )
    {
        cached_uniforms.reset ( new cached_uniforms_struct
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
            material_uni.get_uniform ( "opacity" ),
            model_uni,
        } );
    }
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
        add_material ( materials.at ( i ), * aiscene.mMaterials [ i ] );

    /* add the texture pointers to the materials */
    for ( material& _material: materials )
    {
        for ( unsigned i = 0; i < _material.ambient_stack.stack_size; ++i ) _material.ambient_stack.levels.at ( i ).texture = &textures.at ( _material.ambient_stack.levels.at ( i ).index );
        for ( unsigned i = 0; i < _material.diffuse_stack.stack_size; ++i ) _material.diffuse_stack.levels.at ( i ).texture = &textures.at ( _material.diffuse_stack.levels.at ( i ).index );
        for ( unsigned i = 0; i < _material.specular_stack.stack_size; ++i ) _material.specular_stack.levels.at ( i ).texture = &textures.at ( _material.specular_stack.levels.at ( i ).index );
    }

    /* now add the meshes */
    meshes.resize ( aiscene.mNumMeshes );
    for ( unsigned i = 0; i < aiscene.mNumMeshes; ++i )
        add_mesh ( meshes.at ( i ), * aiscene.mMeshes [ i ] );

    /* now recursively process all of the nodes */
    root_node.parent = NULL;
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
    _material.ambient_stack.base_color = cast_vector ( temp_color ); else _material.ambient_stack.base_color = math::fvec3 { 0.0 };
    if ( aimaterial.Get ( AI_MATKEY_COLOR_DIFFUSE, temp_color ) == aiReturn_SUCCESS )
    _material.diffuse_stack.base_color = cast_vector ( temp_color ); else _material.diffuse_stack.base_color = math::fvec3 { 0.0 };
    if ( aimaterial.Get ( AI_MATKEY_COLOR_SPECULAR, temp_color ) == aiReturn_SUCCESS )
    _material.specular_stack.base_color = cast_vector ( temp_color ); else _material.specular_stack.base_color = math::fvec3 { 0.0 };

    /* apply sRGBA transformations to base colors */
    if ( model_import_flags & import_flags::GLH_AMBIENT_BASE_COLOR_SRGBA ) _material.ambient_stack.base_color = math::pow ( _material.ambient_stack.base_color, math::fvec3 { 2.2 } );
    if ( model_import_flags & import_flags::GLH_DIFFUSE_BASE_COLOR_SRGBA ) _material.diffuse_stack.base_color = math::pow ( _material.diffuse_stack.base_color, math::fvec3 { 2.2 } );
    if ( model_import_flags & import_flags::GLH_SPECULAR_BASE_COLOR_SRGBA ) _material.specular_stack.base_color = math::pow ( _material.specular_stack.base_color, math::fvec3 { 2.2 } );

    /* set up the ambient texture stack textures  */
    _material.ambient_stack.stack_size = aimaterial.GetTextureCount ( aiTextureType_AMBIENT );
    _material.ambient_stack.levels.resize ( _material.ambient_stack.stack_size );
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_AMBIENT ) && i < GLH_MODEL_MAX_UV_CHANNELS; ++i )
    {
        add_texture ( _material.ambient_stack, aimaterial, i, aiTextureType_AMBIENT, model_import_flags & import_flags::GLH_AMBIENT_TEXTURE_SRGBA );  
    }

    /* set up the diffuse texture stack textures */
    _material.diffuse_stack.stack_size = aimaterial.GetTextureCount ( aiTextureType_DIFFUSE );
    _material.diffuse_stack.levels.resize ( _material.diffuse_stack.stack_size );
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ) && i < GLH_MODEL_MAX_UV_CHANNELS; ++i )
    {
        add_texture ( _material.diffuse_stack, aimaterial, i, aiTextureType_DIFFUSE, model_import_flags & import_flags::GLH_DIFFUSE_TEXTURE_SRGBA );  
    }

    /* set up the specular texture stack textures */
    _material.specular_stack.stack_size = aimaterial.GetTextureCount ( aiTextureType_SPECULAR );
    _material.specular_stack.levels.resize ( _material.specular_stack.stack_size );
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_SPECULAR ) && i < GLH_MODEL_MAX_UV_CHANNELS; ++i )
    {
        add_texture ( _material.specular_stack, aimaterial, i, aiTextureType_SPECULAR, model_import_flags & import_flags::GLH_SPECULAR_TEXTURE_SRGBA );  
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
    _material.shading_model = temp_int; else _material.shading_model = aiShadingMode_Blinn;

    /* set definitely opaque flag */
    _material.definitely_opaque = is_definitely_opaque ( _material );

    /* return the material */
    return _material;
}

/* add_texture
 *
 * add a texture to a texture stack
 * 
 * _texture_stack the texture stack being configured
 * aimaterial: the material the texture is being added from
 * index: the index of the texture
 * aitexturetype: the type of the stack
 * is_srgb: true if the texture should be corrected to linear color space (defaults to false)
 * 
 * return: the texture stack level just added
 */
glh::model::texture_stack_level& glh::model::model::add_texture ( texture_stack& _texture_stack, const aiMaterial& aimaterial, const unsigned index, const aiTextureType aitexturetype, const bool is_srgb )
{
    /* temporary variables */
    aiString temp_string;
    float temp_float;
    int temp_int;

    /* get the location of the texture */
    aimaterial.GetTexture ( aitexturetype, index, &temp_string );
    std::string texpath { temp_string.C_Str () };
    
    /* replace possibly bas lettes */
    for ( char& c: texpath ) if ( c == '\\' ) c = '/';

    /* set the blend attributes */
    if ( aimaterial.Get ( AI_MATKEY_TEXOP ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    _texture_stack.levels.at ( index ).blend_operation = temp_int; else
    _texture_stack.levels.at ( index ).blend_operation = ( _texture_stack.base_color == math::fvec3 {}&& index == 0 ? 1 : 0 );
    if ( aimaterial.Get ( AI_MATKEY_TEXBLEND ( aitexturetype, index ), temp_float ) == aiReturn_SUCCESS ) 
    _texture_stack.levels.at ( index ).blend_strength = temp_float; else _texture_stack.levels.at ( index ).blend_strength = 1.0;

    /* set wrapping modes */
    if ( aimaterial.Get ( AI_MATKEY_MAPPINGMODE_U ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    _texture_stack.levels.at ( index ).wrapping_u = temp_int; else _texture_stack.levels.at ( index ).wrapping_u = aiTextureMapMode_Wrap;
    if ( aimaterial.Get ( AI_MATKEY_MAPPINGMODE_V ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    _texture_stack.levels.at ( index ).wrapping_v = temp_int; else _texture_stack.levels.at ( index ).wrapping_v = aiTextureMapMode_Wrap;

    /* set uvwsrc */
    if ( aimaterial.Get ( AI_MATKEY_UVWSRC ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    _texture_stack.levels.at ( index ).uvwsrc = temp_int; else _texture_stack.levels.at ( index ).uvwsrc = 0;

    /* check that the uv channel is actually possible */
    if ( _texture_stack.levels.at ( index ).uvwsrc >= GLH_MODEL_MAX_UV_CHANNELS ) throw exception::model_exception { "level of texture stack requires UV channel greater than the maximum allowed channels" };

    /* check if already imported */
    for ( unsigned i = 0; i < textures.size (); ++i ) if ( textures.at ( i ).get_path () == texpath )
    {
        /* already imported, so purely set the index */
        _texture_stack.levels.at ( index ).index = i;

        /* return texture reference */
        return _texture_stack.levels.at ( index );
    }

    /* not already imported, so import and set the index */
    textures.push_back ( core::texture2d { directory + "/" + texpath, is_srgb } );
    _texture_stack.levels.at ( index ).index = textures.size () - 1;

    /* check if is the same texture as the previous level in the stack */
    _texture_stack.levels.at ( index ).same_as_previous = ( index > 0 && _texture_stack.levels.at ( index ).index == _texture_stack.levels.at ( index - 1 ).index );

    /* return texture reference */
    return _texture_stack.levels.at ( index );
}

/* is_definitely_opaque
 *
 * determines if a material or mesh is definitely opaque
 * 
 * _material/_mesh: the material/mesh to check
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
    for ( unsigned i = 0; i < _material.ambient_stack.stack_size; ++i ) if ( textures.at ( _material.ambient_stack.levels.at ( i ).index ).get_channels () >= 4 ) return false;
    for ( unsigned i = 0; i < _material.diffuse_stack.stack_size; ++i ) if ( textures.at ( _material.diffuse_stack.levels.at ( i ).index ).get_channels () >= 4 ) return false;
    for ( unsigned i = 0; i < _material.specular_stack.stack_size; ++i ) if ( textures.at ( _material.specular_stack.levels.at ( i ).index ).get_channels () >= 4 ) return false;

    /* otherwise the material is definitely opaque */
    return true;
}
bool glh::model::model::is_definitely_opaque ( const mesh& _mesh )
{
    /* if material is not definitely opaque, return */
    if ( !_mesh.properties->definitely_opaque ) return false;

    /* loop through all vertices and color sets and check that alpha components are all 1 */
    for ( unsigned i = 0; i < _mesh.vertices.size (); ++i ) for ( unsigned j = 0; j < _mesh.vertices.at ( i ).colorsets.size (); ++j )
    if ( _mesh.vertices.at ( i ).colorsets.at ( i ).at ( 0 ) < 1.0 ) return false;

    /* otherwise the mesh is definitely opaque */
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
    /* set the number of color sets */
    _mesh.num_color_sets = std::min<unsigned> ( aimesh.GetNumColorChannels (), GLH_MODEL_MAX_COLOR_SETS );

    /* set the number of uv channels */
    _mesh.num_uv_channels = std::min<unsigned> ( aimesh.GetNumUVChannels (), GLH_MODEL_MAX_UV_CHANNELS );

    /* add vertices, normals and texcoords */
    _mesh.vertices.resize ( aimesh.mNumVertices );
    for ( unsigned i = 0; i < aimesh.mNumVertices; ++i )
    {
        /* add vertices and normals */
        _mesh.vertices.at ( i ).position = cast_vector ( aimesh.mVertices [ i ] );
        _mesh.vertices.at ( i ).normal = cast_vector ( aimesh.mNormals [ i ] );

        /* set color sets */
        for ( unsigned j = 0; j < _mesh.num_color_sets; ++j )
            _mesh.vertices.at ( i ).colorsets.at ( j ) = cast_vector ( aimesh.mColors [ j ][ i ] );

        /* apply gamma correction */
        if ( model_import_flags & import_flags::GLH_VERTEX_SRGBA ) for ( unsigned j = 0; j < _mesh.num_color_sets; ++j )
            _mesh.vertices.at ( i ).colorsets.at ( j ) = 
            math::fvec4 { math::pow ( math::fvec3 { _mesh.vertices.at ( i ).colorsets.at ( j ) }, math::fvec3 { 2.2 } ), _mesh.vertices.at ( i ).colorsets.at ( j ).at ( 3 ) };

        /* set uv channel coords */
        for ( unsigned j = 0; j < _mesh.num_uv_channels; ++j )
            _mesh.vertices.at ( i ).texcoords.at ( j ) = cast_vector ( aimesh.mTextureCoords [ j ][ i ] );
    }

    /* add material reference */
    _mesh.properties_index = aimesh.mMaterialIndex;
    _mesh.properties = &materials.at ( _mesh.properties_index );

    /* add faces */
    _mesh.faces.resize ( aimesh.mNumFaces );
    for ( unsigned i = 0; i < aimesh.mNumFaces; ++i ) add_face ( _mesh.faces.at ( i ), _mesh, aimesh.mFaces [ i ] );

    /* configure the buffers */
    configure_buffers ( _mesh );

    /* if GLH_CONFIGURE_REGIONS_ACCURATE and GLH_CONFIGURE_ONLY_ROOT_NODE_REGION is set, don't configure meshes
     * else if any mesh configuration flag is set, configure meshes
     */
    if ( !( model_import_flags & import_flags::GLH_CONFIGURE_REGIONS_ACCURATE && model_import_flags & import_flags::GLH_CONFIGURE_ONLY_ROOT_NODE_REGION ) )
        if ( model_import_flags & ( import_flags::GLH_CONFIGURE_REGIONS_FAST | import_flags::GLH_CONFIGURE_REGIONS_ACCEPTABLE | import_flags::GLH_CONFIGURE_REGIONS_ACCURATE ) ) 
            configure_mesh_region ( _mesh );

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
    /* assert that there are 3 indices */
    if ( aiface.mNumIndices != 3 ) throw exception::model_exception { "face has not been triangulated" };

    /* set indices */
    _face.indices.at ( 0 ) = aiface.mIndices [ 0 ];
    _face.indices.at ( 1 ) = aiface.mIndices [ 1 ];
    _face.indices.at ( 2 ) = aiface.mIndices [ 2 ];

    /* return face */
    return _face;
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
    for ( unsigned i = 0; i < ainode.mNumChildren; ++i ) 
    {
        /* set parent of child node and then set it up fully */
        _node.children.at ( i ).parent = &_node;
        add_node ( _node.children.at ( i ), * ainode.mChildren [ i ] );
    }

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

    /* configure region */
    if ( model_import_flags & ( import_flags::GLH_CONFIGURE_REGIONS_FAST | import_flags::GLH_CONFIGURE_REGIONS_ACCEPTABLE | import_flags::GLH_CONFIGURE_REGIONS_ACCURATE ) )
        configure_node_region ( _node );

    /* return node */
    return _node;
}



/* configure_buffers
 *
 * configure the buffers of a mesh
 * 
 * _mesh: the mesh to configure
 */
void glh::model::model::configure_buffers ( mesh& _mesh )
{
    /* buffer vertex data */
    _mesh.vertex_data.buffer_data ( _mesh.vertices.begin (), _mesh.vertices.end () );

    /* buffer index data */
    _mesh.index_data.buffer_data ( _mesh.faces.begin (), _mesh.faces.end () );

    /* configure the vao */
    _mesh.array_object.set_vertex_attrib ( 0, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 0 );
    _mesh.array_object.set_vertex_attrib ( 1, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 3 * sizeof ( GLfloat ) );
    for ( unsigned i = 0; i < _mesh.num_color_sets; ++i )
        _mesh.array_object.set_vertex_attrib ( 2 + i, _mesh.vertex_data, 4, GL_FLOAT, GL_FALSE, sizeof ( vertex ), ( 6 + ( i * 4 ) ) * sizeof ( GLfloat ) );
    for ( unsigned i = 0; i < _mesh.num_uv_channels; ++i )
        _mesh.array_object.set_vertex_attrib ( 2 + GLH_MODEL_MAX_COLOR_SETS + i, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), ( 6 + ( GLH_MODEL_MAX_COLOR_SETS * 4 ) + ( i * 3 ) ) * sizeof ( GLfloat ) );
    _mesh.array_object.bind_ebo ( _mesh.index_data );
}



/* mesh_max_min_components
 * node_max_min_components
 *
 * find the maximum and minimum xyz components of all of the vertices of the mesh/node
 * 
 * transform: transformation applied to all of the vertices
 * 
 * return: a pair of vec3s: the first if the max components, the second is the min components
 */
std::pair<glh::math::fvec3, glh::math::fvec3> glh::model::model::mesh_max_min_components ( const mesh& _mesh, const math::fmat4& transform ) const
{
    /* two vectors to store max/min coordinate components of vertices */
    math::fvec3 max_components, min_components;

    /* loop through vectices to fill max_min_components */
    for ( unsigned i = 0; i < _mesh.vertices.size (); ++i )
    {
        /* transform the vertex */
        const math::fvec3 tvertex { transform * math::fvec4 { _mesh.vertices.at ( i ).position, 1.0 } };

        /* compare against max/min positions */
        if ( tvertex.at ( 0 ) > max_components.at ( 0 ) || i == 0 ) max_components.at ( 0 ) = tvertex.at ( 0 );
        if ( tvertex.at ( 0 ) < min_components.at ( 0 ) || i == 0 ) min_components.at ( 0 ) = tvertex.at ( 0 );
        if ( tvertex.at ( 1 ) > max_components.at ( 1 ) || i == 0 ) max_components.at ( 1 ) = tvertex.at ( 1 );
        if ( tvertex.at ( 1 ) < min_components.at ( 1 ) || i == 0 ) min_components.at ( 1 ) = tvertex.at ( 1 );
        if ( tvertex.at ( 2 ) > max_components.at ( 2 ) || i == 0 ) max_components.at ( 2 ) = tvertex.at ( 2 );
        if ( tvertex.at ( 2 ) < min_components.at ( 2 ) || i == 0 ) min_components.at ( 2 ) = tvertex.at ( 2 );
    }

    /* return the components as a pair */
    return std::pair<math::fvec3, math::fvec3> { max_components, min_components };
}
std::pair<glh::math::fvec3, glh::math::fvec3> glh::model::model::node_max_min_components ( const node& _node, const math::fmat4& transform ) const
{
    /* transform the matrix */
    const math::fmat4 new_transform = transform * _node.transform;

    /* two vectors to store max/min coordinate components of vertices */
    math::fvec3 max_components, min_components;

    /* loop through child nodes to update max/min components */
    for ( unsigned i = 0; i < _node.children.size (); ++i )
    {
        /* get the node components */
        math::fvec3 node_max_components, node_min_components;
        std::tie ( node_max_components, node_min_components ) = node_max_min_components ( _node.children.at ( i ), new_transform );
        
        /* compare against max/min components */
        if ( node_max_components.at ( 0 ) > max_components.at ( 0 ) || i == 0 ) max_components.at ( 0 ) = node_max_components.at ( 0 );
        if ( node_min_components.at ( 0 ) < min_components.at ( 0 ) || i == 0 ) min_components.at ( 0 ) = node_min_components.at ( 0 );
        if ( node_max_components.at ( 1 ) > max_components.at ( 1 ) || i == 0 ) max_components.at ( 1 ) = node_max_components.at ( 1 );
        if ( node_min_components.at ( 1 ) < min_components.at ( 1 ) || i == 0 ) min_components.at ( 1 ) = node_min_components.at ( 1 );
        if ( node_max_components.at ( 2 ) > max_components.at ( 2 ) || i == 0 ) max_components.at ( 2 ) = node_max_components.at ( 2 );
        if ( node_min_components.at ( 2 ) < min_components.at ( 2 ) || i == 0 ) min_components.at ( 2 ) = node_min_components.at ( 2 );
    }

    /* loop through child meshes to update max/min components */
    for ( unsigned i = 0; i < _node.meshes.size (); ++i )
    {
        /* get the mesh components */
        math::fvec3 mesh_max_components, mesh_min_components;
        std::tie ( mesh_max_components, mesh_min_components ) = mesh_max_min_components ( * _node.meshes.at ( i ), new_transform );
        
        /* compare against max/min components */
        if ( mesh_max_components.at ( 0 ) > max_components.at ( 0 ) || ( _node.children.size () == 0 && i == 0 ) ) max_components.at ( 0 ) = mesh_max_components.at ( 0 );
        if ( mesh_min_components.at ( 0 ) < min_components.at ( 0 ) || ( _node.children.size () == 0 && i == 0 ) ) min_components.at ( 0 ) = mesh_min_components.at ( 0 );
        if ( mesh_max_components.at ( 1 ) > max_components.at ( 1 ) || ( _node.children.size () == 0 && i == 0 ) ) max_components.at ( 1 ) = mesh_max_components.at ( 1 );
        if ( mesh_min_components.at ( 1 ) < min_components.at ( 1 ) || ( _node.children.size () == 0 && i == 0 ) ) min_components.at ( 1 ) = mesh_min_components.at ( 1 );
        if ( mesh_max_components.at ( 2 ) > max_components.at ( 2 ) || ( _node.children.size () == 0 && i == 0 ) ) max_components.at ( 2 ) = mesh_max_components.at ( 2 );
        if ( mesh_min_components.at ( 2 ) < min_components.at ( 2 ) || ( _node.children.size () == 0 && i == 0 ) ) min_components.at ( 2 ) = mesh_min_components.at ( 2 );
    }

    /* return the components as a pair */
    return std::pair<math::fvec3, math::fvec3> { max_components, min_components };
}

/* mesh_furthest_distance
 * node_furthest_distance
 *
 * find the furthest distance from a point in a mesh/node
 * 
 * point: the point to find the furthest distance from
 * transform: the transformation to apply to all of the vertices (assumed to be applied to point)
 * 
 * return: the furthest distance from that point and a vertex
 */
float glh::model::model::mesh_furthest_distance ( const mesh& _mesh, const math::fvec3& point, const math::fmat4& transform ) const
{
    /* float to store max distance */
    float furthest_distance = 0.0;

    /* loop through vectices to find firthest distance */
    for ( const vertex& _vertex: _mesh.vertices )
    {
        /* transform the vertex and find the modulus from the point */
        const float distance = modulus ( point - math::vec3 { transform * math::fvec4 { _vertex.position, 1.0 } } );

        /* compare against furthest distance */
        if ( distance > furthest_distance ) furthest_distance = distance;
    }

    /* return the furthest distance */
    return furthest_distance;
}
float glh::model::model::node_furthest_distance ( const node& _node, const math::fvec3& point, const math::fmat4& transform ) const
{
    /* transform the matrix */
    const math::fmat4 new_transform = transform * _node.transform;

    /* float to store max distance */
    float furthest_distance = 0.0;

    /* loop through child nodes to find furthest distance */
    for ( const node& child: _node.children )
    {
        /* get distance */
        const float distance = node_furthest_distance ( child, point, new_transform );

        /* change furthest distance if necessary */
        if ( distance > furthest_distance ) furthest_distance = distance;
    }

    /* loop through meshes to find firthest distance */
    for ( const mesh * _mesh: _node.meshes )
    {
        /* get distance */
        const float distance = mesh_furthest_distance ( * _mesh, point, new_transform );

        /* change furthest distance if necessary */
        if ( distance > furthest_distance ) furthest_distance = distance;
    }

    /* return the furthest distance */
    return furthest_distance;
}



/* configure_mesh_region
 *
 * configure the region of a mesh
 * 
 * _mesh: the mesh to configure
 */
void glh::model::model::configure_mesh_region ( mesh& _mesh )
{
    /* get the maximum and minimum components of the mesh */
    math::fvec3 max_components, min_components;
    std::tie ( max_components, min_components ) = mesh_max_min_components ( _mesh );

    /* find central vertex */
    _mesh.mesh_region.centre = ( max_components + min_components ) / 2.0;

    /* if GLH_CONFIGURE_REGIONS_ACCEPTABLE or GLH_CONFIGURE_REGIONS_ACCEPTABLE is set, 
     * find the furthest vertex and set the radius to that
     * else find the maximum radius via the modulus of max_components
     */
    if ( model_import_flags & ( import_flags::GLH_CONFIGURE_REGIONS_ACCEPTABLE | import_flags::GLH_CONFIGURE_REGIONS_ACCURATE ) )
        _mesh.mesh_region.radius = mesh_furthest_distance ( _mesh, _mesh.mesh_region.centre );
    else _mesh.mesh_region.radius = math::modulus ( max_components );
}

/* configure_node_region
 *
 * configure the region of a node
 * child nodes must have already been processed
 * 
 *  _node: the node to configure
 */
void glh::model::model::configure_node_region ( node& _node )
{
    /* if not GLH_CONFIGURE_ONLY_ROOT_NODE_REGION and GLH_CONFIGURE_REGIONS_ACCURATE set, configure child nodes' regions */
    if ( !( model_import_flags & import_flags::GLH_CONFIGURE_REGIONS_ACCURATE && model_import_flags & import_flags::GLH_CONFIGURE_ONLY_ROOT_NODE_REGION ) )
        for ( node& child: _node.children ) configure_node_region ( child );

    /* if GLH_CONFIGURE_REGIONS_ACCEPTABLE is set, use max/min components of node */
    if ( model_import_flags & import_flags::GLH_CONFIGURE_REGIONS_ACCURATE )
    {
        /* get the maximum and minimum components of the node */
        math::fvec3 max_components, min_components;
        std::tie ( max_components, min_components ) = node_max_min_components ( _node );

        /* find central vertex */
        _node.node_region.centre = ( max_components + min_components ) / 2.0;

        /* set radius to the furthest distance */
        _node.node_region.radius = node_furthest_distance ( _node, _node.node_region.centre );
    } else
    /* else calculate region based on the child nodes and meshes regions */
    {
        /* set region to first node or first mesh */
        if ( _node.children.size () > 0 ) _node.node_region = _node.children.at ( 0 ).node_region;
        else _node.node_region = _node.meshes.at ( 0 )->mesh_region;

        /* loop through the child nodes and meshes and combind the regions */
        for ( const node& child: _node.children ) _node.node_region = region::combine ( _node.node_region, child.node_region );
        for ( const mesh * _mesh: _node.meshes ) _node.node_region = region::combine ( _node.node_region, _mesh->mesh_region );

        /* apply the transformation matrix */
        _node.node_region = _node.transform * _node.node_region;
    }
}



/* render_node
 *
 * render a node and all of its children
 * 
 * _node: the node to render
 * prog: the program to use for rendering
 * transform: the current model transformation from all the previous nodes
 */
void glh::model::model::render_node ( const node& _node, const core::program& prog, const math::fmat4& transform ) const
{
    /* create transformation matrix */
    math::fmat4 trans = transform * _node.transform;

    /* first render the child nodes */
    for ( const node& child: _node.children ) render_node ( child, prog, trans );

    /* set the model matrix */
    cached_uniforms->model_uni.set_matrix ( trans );

    /* render all of the meshes */
    for ( const mesh * _mesh: _node.meshes ) render_mesh ( * _mesh, prog );
}

/* render_mesh
 *
 * render a mesh
 * 
 * _mesh: the mesh to render
 * prog: the program to use for rendering
 */
void glh::model::model::render_mesh ( const mesh& _mesh, const core::program& prog ) const
{
    /* don't draw if transparent mode and mesh is definitely opaque */
    if ( ( model_render_flags & render_flags::GLH_TRANSPARENT_MODE ) && _mesh.definitely_opaque ) return;

    /* if face culling is on and material is two sided, disable face culling */
    const bool culling_active = core::renderer::face_culling_enabled ();
    if ( culling_active && _mesh.properties->two_sided ) core::renderer::disable_face_culling ();

    /* apply the material, if not disabled in flags */
    if ( !( model_render_flags & render_flags::GLH_NO_MATERIAL ) ) apply_material ( * _mesh.properties );

    /* draw elements */
    core::renderer::draw_elements ( prog, _mesh.array_object, GL_TRIANGLES, _mesh.faces.size () * 3, GL_UNSIGNED_INT, 0 );

    /* re-enable face culling if was previously disabled */
    if ( culling_active ) core::renderer::enable_face_culling ();
}

/* apply_material
 *
 * apply material uniforms during mesh rendering
 * 
 * _material: the material to apply
 */
void glh::model::model::apply_material ( const material& _material ) const
{
    /* apply the texture stacks */
    apply_texture_stack ( _material.ambient_stack, cached_uniforms->ambient_stack_size_uni, cached_uniforms->ambient_stack_base_color_uni, cached_uniforms->ambient_stack_levels_uni );
    apply_texture_stack ( _material.diffuse_stack, cached_uniforms->diffuse_stack_size_uni, cached_uniforms->diffuse_stack_base_color_uni, cached_uniforms->diffuse_stack_levels_uni );
    apply_texture_stack ( _material.specular_stack, cached_uniforms->specular_stack_size_uni, cached_uniforms->specular_stack_base_color_uni, cached_uniforms->specular_stack_levels_uni );

    /* set blending mode */
    cached_uniforms->blending_mode_uni.set_int ( _material.blending_mode );

    /* set shininess values */
    cached_uniforms->shininess_uni.set_float ( _material.shininess );
    cached_uniforms->shininess_strength_uni.set_float ( _material.shininess_strength );

    /* set opacity */
    cached_uniforms->opacity_uni.set_float ( _material.opacity );
}

/* apply_texture_stack
 *
 * apply a texture stack during mesh rendering
 * 
 * _texture_stack: the texture stack to apply
 * stack_size_uni/stack_base_color_uni/stack_levels_uni: cached stack uniforms
 */
void glh::model::model::apply_texture_stack ( const texture_stack& _texture_stack, core::uniform& stack_size_uni, core::uniform& stack_base_color_uni, core::struct_array_uniform& stack_levels_uni ) const
{
    /* set the stack size */
    stack_size_uni.set_int ( _texture_stack.stack_size );

    /* set the base color */
    stack_base_color_uni.set_vector ( _texture_stack.base_color );

    /* bind each level of the texture stack */
    for ( unsigned i = 0; i < _texture_stack.stack_size; ++i ) 
    {
        /* if same as previous level, purely set the correct */
        if ( _texture_stack.levels.at ( i ).same_as_previous )
        {
            stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( _texture_stack.levels.at ( i ).texture->bind_loop_previous () );
        } else
        {
            _texture_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( _texture_stack.levels.at ( i ).wrapping_u ) );
            _texture_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( _texture_stack.levels.at ( i ).wrapping_v ) );
            stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( _texture_stack.levels.at ( i ).texture->bind_loop () );
        }
        stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( _texture_stack.levels.at ( i ).blend_operation );
        stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( _texture_stack.levels.at ( i ).blend_strength );
        stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( _texture_stack.levels.at ( i ).uvwsrc );
    }
}