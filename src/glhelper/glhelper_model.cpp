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
    if ( !aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ) throw exception::model_exception { "failed to import model at path " + directory + "/" + entry + " with error " + importer.GetErrorString () };

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
void glh::model::model::render ( core::struct_uniform& material_uni, core::uniform& model_uni, const math::mat4& transform, const bool transparent_only )
{
    /* reload the cache of uniforms  */
    cache_uniforms ( material_uni, model_uni );

    /* render */
    render ( transform, transparent_only );
}
void glh::model::model::render ( const math::mat4& transform, const bool transparent_only ) const
{
    /* throw if uniforms are not already cached */
    if ( !cached_uniforms ) throw exception::uniform_exception { "attempted to render model without a complete uniform cache" };

    /* set transparent_only flag */
    draw_transparent_only = transparent_only;

    /* render the root node */
    render_node ( root_node, transform );
}



/* collision_check
 *
 * given a modelview matrix and a movement vector, detect whether the movement will collide with a surface
 *
 * transform: the modelview matrix to apply
 * movement: the movement vector as a direction with magnitude from the origin
 *
 * return: true if a collision will occur 
 */
bool glh::model::model::collision_check ( const math::mat4& transform, const math::vec3& movement ) const
{
    /* start the collision check at the root node */
    collision_check_node ( root_node, transform, movement, math::normalise ( movement ), math::modulus ( movement ) );

    return false;
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
    {
        add_material ( materials.at ( i ), * aiscene.mMaterials [ i ] );
    }

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
    {
        add_mesh ( meshes.at ( i ), * aiscene.mMeshes [ i ] );
    }

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

    /* set up the ambient texture stack textures
     * if ambient textures are present, use those
     * otherwise borrow the diffuse textures
     */
    if ( aimaterial.GetTextureCount ( aiTextureType_AMBIENT ) > 0 )
    {
        _material.ambient_stack.stack_size = std::min<unsigned> ( aimaterial.GetTextureCount ( aiTextureType_AMBIENT ), GLH_MODEL_MAX_TEXTURE_STACK_SIZE );
        for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_AMBIENT ) && i < GLH_MODEL_MAX_UV_CHANNELS; ++i )
        {
            add_texture ( _material.ambient_stack.levels.at ( i ), aimaterial, i, aiTextureType_AMBIENT );  
        }
    } else
    {
        _material.ambient_stack.stack_size = std::min<unsigned> ( aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ), GLH_MODEL_MAX_TEXTURE_STACK_SIZE );
        _material.ambient_stack.base_color = _material.diffuse_stack.base_color;
        for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ) && i < GLH_MODEL_MAX_UV_CHANNELS; ++i )
        {
            add_texture ( _material.ambient_stack.levels.at ( i ), aimaterial, i, aiTextureType_DIFFUSE );  
        }
    }
    

    /* set up the diffuse texture stack textures */
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ) && i < GLH_MODEL_MAX_UV_CHANNELS; ++i )
    {
        _material.diffuse_stack.stack_size = std::min<unsigned> ( aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ), GLH_MODEL_MAX_TEXTURE_STACK_SIZE );
        add_texture ( _material.diffuse_stack.levels.at ( i ), aimaterial, i, aiTextureType_DIFFUSE );  
    }

    /* set up the specular texture stack textures */
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_SPECULAR ) && i < GLH_MODEL_MAX_UV_CHANNELS; ++i )
    {
        _material.specular_stack.stack_size = std::min<unsigned> ( aimaterial.GetTextureCount ( aiTextureType_SPECULAR ), GLH_MODEL_MAX_TEXTURE_STACK_SIZE );
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

    /* check that the uv channel is actually possible */
    if ( _texture_stack_level.uvwsrc >= GLH_MODEL_MAX_UV_CHANNELS ) throw exception::model_exception { "level of texture stack requires UV channel greater than the maximum allowed channels" };

    /* check if already inported */
    for ( unsigned i = 0; i < textures.size (); ++i ) if ( textures.at ( i ).get_path () == texpath )
    {
        /* already imported, so purely set the index */
        _texture_stack_level.index = i;

        /* return texture reference */
        return _texture_stack_level;
    }

    /* not already imported, so import and set the index */
    textures.push_back ( core::texture2d { directory + "/" + texpath } );
    _texture_stack_level.index = textures.size () - 1;

    /* return texture reference */
    return _texture_stack_level;
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

    /* calculate generalisations */
    calculate_mesh_generalisations ( _mesh );

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
    {
        _mesh.array_object.set_vertex_attrib ( 2 + i, _mesh.vertex_data, 4, GL_FLOAT, GL_FALSE, sizeof ( vertex ), ( 6 + ( i * 4 ) ) * sizeof ( GLfloat ) );
    }
    for ( unsigned i = 0; i < _mesh.num_uv_channels; ++i )
    {
        _mesh.array_object.set_vertex_attrib ( 2 + GLH_MODEL_MAX_COLOR_SETS + i, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), ( 6 + ( GLH_MODEL_MAX_COLOR_SETS * 4 ) + ( i * 3 ) ) * sizeof ( GLfloat ) );
    }
    _mesh.array_object.bind_ebo ( _mesh.index_data );
}

/* calculate_mesh_generalisations
 * 
 * calculate generalisations of the mesh
 */
void glh::model::model::calculate_mesh_generalisations ( mesh& _mesh )
{
    /* CENTRE AND RADIUS */

    /* find average of all of the vertices */
    for ( const vertex& _vertex: _mesh.vertices ) _mesh.centre += _vertex.position;
    _mesh.centre /= _mesh.vertices.size ();

    /* set square radius to 0.0 */
    _mesh.square_radius = 0.0;

    /* set max/min dimensions to that of the first vertex */
    _mesh.max_components = _mesh.vertices.at ( 0 ).position;
    _mesh.min_components = _mesh.vertices.at ( 0 ).position;

    /* loop through the vertices to:
     * 1. find the distance to the furthest vertex
     * 2. fill in the correct values for the cuboid
     */
    for ( const vertex& _vertex: _mesh.vertices )
    {
        /* 1. */
        double testing_square_modulus = math::square_modulus ( _vertex.position - _mesh.centre );
        if ( testing_square_modulus > _mesh.square_radius ) _mesh.square_radius = testing_square_modulus;

        /* 2. */
        if ( _vertex.position.at ( 0 ) > _mesh.max_components.at ( 0 ) ) _mesh.max_components.at ( 0 ) = _vertex.position.at ( 0 );
        if ( _vertex.position.at ( 0 ) < _mesh.min_components.at ( 0 ) ) _mesh.min_components.at ( 0 ) = _vertex.position.at ( 0 );
        if ( _vertex.position.at ( 1 ) > _mesh.max_components.at ( 1 ) ) _mesh.max_components.at ( 1 ) = _vertex.position.at ( 1 );
        if ( _vertex.position.at ( 1 ) < _mesh.min_components.at ( 1 ) ) _mesh.min_components.at ( 1 ) = _vertex.position.at ( 1 );
        if ( _vertex.position.at ( 2 ) > _mesh.max_components.at ( 2 ) ) _mesh.max_components.at ( 2 ) = _vertex.position.at ( 2 );
        if ( _vertex.position.at ( 2 ) < _mesh.min_components.at ( 2 ) ) _mesh.min_components.at ( 2 ) = _vertex.position.at ( 2 );
    }

    /* set the radius */
    _mesh.radius = std::sqrt ( _mesh.square_radius );

    /* set the corners of the cuboid */
    _mesh.pxpypz = math::fvec4 { _mesh.max_components.at ( 0 ), _mesh.max_components.at ( 1 ), _mesh.max_components.at ( 2 ), 1.0 };
    _mesh.pxpynz = math::fvec4 { _mesh.max_components.at ( 0 ), _mesh.max_components.at ( 1 ), _mesh.min_components.at ( 2 ), 1.0 };
    _mesh.pxnypz = math::fvec4 { _mesh.max_components.at ( 0 ), _mesh.min_components.at ( 1 ), _mesh.max_components.at ( 2 ), 1.0 };
    _mesh.pxnynz = math::fvec4 { _mesh.max_components.at ( 0 ), _mesh.min_components.at ( 1 ), _mesh.min_components.at ( 2 ), 1.0 };
    _mesh.nxpypz = math::fvec4 { _mesh.min_components.at ( 0 ), _mesh.max_components.at ( 1 ), _mesh.max_components.at ( 2 ), 1.0 };
    _mesh.nxpynz = math::fvec4 { _mesh.min_components.at ( 0 ), _mesh.max_components.at ( 1 ), _mesh.min_components.at ( 2 ), 1.0 };
    _mesh.nxnypz = math::fvec4 { _mesh.min_components.at ( 0 ), _mesh.min_components.at ( 1 ), _mesh.max_components.at ( 2 ), 1.0 };
    _mesh.nxnynz = math::fvec4 { _mesh.min_components.at ( 0 ), _mesh.min_components.at ( 1 ), _mesh.min_components.at ( 2 ), 1.0 };
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
void glh::model::model::render_node ( const node& _node, const math::fmat4& transform ) const
{
    /* create transformation matrix */
    math::fmat4 trans = transform * _node.transform;

    /* first render the child nodes */
    for ( const node& child: _node.children ) render_node ( child, trans );

    /* set the model matrix */
    cached_uniforms->model_uni.set_matrix ( trans );

    /* render all of the meshes */
    for ( const mesh * _mesh: _node.meshes ) render_mesh ( * _mesh );
}

/* render_mesh
 *
 * render a mesh
 * 
 * _mesh: the mesh to render
 */
void glh::model::model::render_mesh ( const mesh& _mesh ) const
{
    /* don't draw if transparent only is set mesh is definitely opaque */
    if ( draw_transparent_only && _mesh.definitely_opaque ) return;

    /* get if face culling is on, and disable if mesh is two sided */
    const bool culling_active = core::renderer::face_culling_enabled ();
    if ( culling_active && _mesh.properties->two_sided ) core::renderer::disable_face_culling ();

    /* set the stack sizes */
    cached_uniforms->ambient_stack_size_uni.set_int ( _mesh.properties->ambient_stack.stack_size );
    cached_uniforms->diffuse_stack_size_uni.set_int ( _mesh.properties->diffuse_stack.stack_size );
    cached_uniforms->specular_stack_size_uni.set_int ( _mesh.properties->specular_stack.stack_size );

    /* set the color values */
    cached_uniforms->ambient_stack_base_color_uni.set_vector ( _mesh.properties->ambient_stack.base_color );
    cached_uniforms->diffuse_stack_base_color_uni.set_vector ( _mesh.properties->diffuse_stack.base_color );
    cached_uniforms->specular_stack_base_color_uni.set_vector ( _mesh.properties->specular_stack.base_color ); 

    /* bind the textures and set the sampler values */
    unsigned i = 0, tex_unit = 1;
    for ( i = 0; i < _mesh.properties->ambient_stack.stack_size; ++i ) 
    {
        _mesh.properties->ambient_stack.levels.at ( i ).texture->bind ( tex_unit );
        _mesh.properties->ambient_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( _mesh.properties->ambient_stack.levels.at ( i ).wrapping_u ) );
        _mesh.properties->ambient_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( _mesh.properties->ambient_stack.levels.at ( i ).wrapping_v ) );
        cached_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( tex_unit++ );
        cached_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( _mesh.properties->ambient_stack.levels.at ( i ).blend_operation );
        cached_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( _mesh.properties->ambient_stack.levels.at ( i ).blend_strength );
        cached_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( _mesh.properties->ambient_stack.levels.at ( i ).uvwsrc );
    }
    for ( i = 0; i < _mesh.properties->diffuse_stack.stack_size; ++i ) 
    {
        _mesh.properties->diffuse_stack.levels.at ( i ).texture->bind ( tex_unit );
        _mesh.properties->diffuse_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( _mesh.properties->diffuse_stack.levels.at ( i ).wrapping_u ) );
        _mesh.properties->diffuse_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( _mesh.properties->diffuse_stack.levels.at ( i ).wrapping_v ) );
        cached_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( tex_unit++ );
        cached_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( _mesh.properties->diffuse_stack.levels.at ( i ).blend_operation );
        cached_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( _mesh.properties->diffuse_stack.levels.at ( i ).blend_strength );
        cached_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( _mesh.properties->diffuse_stack.levels.at ( i ).uvwsrc );
    }
    for ( i = 0; i < _mesh.properties->specular_stack.stack_size; ++i, i ) 
    {
        _mesh.properties->specular_stack.levels.at ( i ).texture->bind ( tex_unit );
        _mesh.properties->specular_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( _mesh.properties->specular_stack.levels.at ( i ).wrapping_u ) );
        _mesh.properties->specular_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( _mesh.properties->specular_stack.levels.at ( i ).wrapping_v ) );
        cached_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( tex_unit++ );
        cached_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( _mesh.properties->specular_stack.levels.at ( i ).blend_operation );
        cached_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( _mesh.properties->specular_stack.levels.at ( i ).blend_strength );
        cached_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( _mesh.properties->specular_stack.levels.at ( i ).uvwsrc );
    }

    /* set blending mode */
    cached_uniforms->blending_mode_uni.set_int ( _mesh.properties->blending_mode );

    /* set shininess values */
    cached_uniforms->shininess_uni.set_float ( _mesh.properties->shininess );
    cached_uniforms->shininess_strength_uni.set_float ( _mesh.properties->shininess_strength );

    /* set opacity */
    cached_uniforms->opacity_uni.set_float ( _mesh.properties->opacity );

    /* draw elements */
    core::renderer::draw_elements ( _mesh.array_object, GL_TRIANGLES, _mesh.faces.size () * 3, GL_UNSIGNED_INT, 0 );

    /* re-enable face culling if was previously disabled */
    if ( culling_active && _mesh.properties->two_sided ) core::renderer::enable_face_culling ();
}

/* collision_check_node
 *
 * check a node for collisions
 * 
 * _node: the node to render
 * transform: the current modelview transformation from all the previous nodes
 * movement: the movement vector to check against
 * direction: the direction of the movement vector (normalised movement)
 * magnitude: the magnitude of the movment vector
 * 
 * return: true if a collision will occur
 */
bool glh::model::model::collision_check_node ( const node& _node, const math::fmat4& transform, const math::fvec3& movement, const math::fvec3& direction, const double magnitude ) const
{
    /* create transformation matrix */
    math::fmat4 trans = transform * _node.transform;

    /* first check child nodes
     * we need to remember the smallest returned movement vector
     */
    double square_modulus = math::square_modulus ( movement );
    for ( const node& child: _node.children ) 
    {
        double test_square_modulus = collision_check_node ( child, trans, movement, direction, magnitude );
        if ( test_square_modulus < square_modulus ) square_modulus = test_square_modulus;
    }

    /* loop through the meshes to see if any collisions occur */
    for ( const mesh * _mesh: _node.meshes )
    {


    }

    return false;
}
