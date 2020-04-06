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
 * material: a struct uniform with the same members as glh::model::material
 *           the texture stacks should be arrays of structs containing members in glh::model::texture_reference
 * model_matrix: a 4x4 matrix uniform which arranges meshes to relative positions
 * normal_matrix: a 4x4 matrix uniform based produced from the model matrix
 * transform: the overall model transformation to apply (identity by default)
 */
void glh::model::model::render ( struct_uniform& material_uni, uniform& model_matrix, uniform& normal_matrix, const math::mat4 transform )
{
    /* reload the cache of material uniforms  */
    if ( cached_material_uniforms.get () == nullptr || cached_material_uniforms->material_uni != material_uni )
    {
        cached_material_uniforms.reset ( new cached_material_uniforms_struct
        {
            material_uni,
            material_uni.get_struct_uniform ( "ambient_stack" ).get_uniform ( "stack_size" ),
            material_uni.get_struct_uniform ( "diffuse_stack" ).get_uniform ( "stack_size" ),
            material_uni.get_struct_uniform ( "specular_stack" ).get_uniform ( "stack_size" ),
            material_uni.get_struct_uniform ( "ambient_stack" ).get_uniform ( "base_colour" ),
            material_uni.get_struct_uniform ( "diffuse_stack" ).get_uniform ( "base_colour" ),
            material_uni.get_struct_uniform ( "specular_stack" ).get_uniform ( "base_colour" ),
            material_uni.get_struct_uniform ( "ambient_stack" ).get_array_uniform<struct_uniform> ( "levels" ),
            material_uni.get_struct_uniform ( "diffuse_stack" ).get_array_uniform<struct_uniform> ( "levels" ),
            material_uni.get_struct_uniform ( "specular_stack" ).get_array_uniform<struct_uniform> ( "levels" ),
            material_uni.get_uniform ( "blending_mode" ),
            material_uni.get_uniform ( "shininess" ),
            material_uni.get_uniform ( "shininess_strength" ),
            material_uni.get_uniform ( "opacity" )
         } );
    }

    /* reload the model and normal matrix uniforms */
    if ( cached_model_uniform.get () == nullptr || * cached_model_uniform != model_matrix )
    cached_model_uniform.reset ( new uniform { model_matrix } );

    if ( cached_normal_uniform.get () == nullptr || * cached_normal_uniform != normal_matrix )
    cached_normal_uniform.reset ( new uniform { normal_matrix } );

    /* render the root node */
    render_node ( root_node, transform );
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
 * location: the material to configure
 * aimaterial: the assimp material object to add
 * 
 * return: the material just added
 */
glh::model::material& glh::model::model::add_material ( material& location, const aiMaterial& aimaterial )
{
    /* temportary get storage */
    aiColor3D temp_colour;
    float temp_float;
    int temp_int;

    /* set the base colours of the stacks */
    if ( aimaterial.Get ( AI_MATKEY_COLOR_AMBIENT, temp_colour ) == aiReturn_SUCCESS )
    location.ambient_stack.base_colour = cast_vector ( temp_colour ); else location.ambient_stack.base_colour = math::vec3 { 0.0 };
    if ( aimaterial.Get ( AI_MATKEY_COLOR_DIFFUSE, temp_colour ) == aiReturn_SUCCESS )
    location.diffuse_stack.base_colour = cast_vector ( temp_colour ); else location.diffuse_stack.base_colour = math::vec3 { 0.0 };
    if ( aimaterial.Get ( AI_MATKEY_COLOR_SPECULAR, temp_colour ) == aiReturn_SUCCESS )
    location.specular_stack.base_colour = cast_vector ( temp_colour ); else location.specular_stack.base_colour = math::vec3 { 0.0 };

    /* set up the ambient texture stack textures */
    location.ambient_stack.levels.resize ( aimaterial.GetTextureCount ( aiTextureType_AMBIENT ) );
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_AMBIENT ); ++i )
    {
        add_texture ( location.ambient_stack.levels.at ( i ), aimaterial, i, aiTextureType_AMBIENT );  
    }

    /* set up the diffuse texture stack textures */
    location.diffuse_stack.levels.resize ( aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ) );
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_DIFFUSE ); ++i )
    {
        add_texture ( location.diffuse_stack.levels.at ( i ), aimaterial, i, aiTextureType_DIFFUSE );  
    }

    /* set up the specular texture stack textures */
    location.specular_stack.levels.resize ( aimaterial.GetTextureCount ( aiTextureType_SPECULAR ) );
    for ( unsigned i = 0; i < aimaterial.GetTextureCount ( aiTextureType_SPECULAR ); ++i )
    {
        add_texture ( location.specular_stack.levels.at ( i ), aimaterial, i, aiTextureType_SPECULAR );  
    }

    /* get the blend mode */
    if ( aimaterial.Get ( AI_MATKEY_BLEND_FUNC, temp_int ) == aiReturn_SUCCESS )
    location.blending_mode = temp_int; else location.blending_mode = 0;

    /* get the shininess properties */
    if ( aimaterial.Get ( AI_MATKEY_SHININESS, temp_float ) == aiReturn_SUCCESS )
    location.shininess = temp_float; else location.shininess = 0.0;
    if ( aimaterial.Get ( AI_MATKEY_SHININESS_STRENGTH, temp_float ) == aiReturn_SUCCESS )
    location.shininess_strength = temp_float; else location.shininess_strength = 0.0;

    /* get the opacity */
    if ( aimaterial.Get ( AI_MATKEY_OPACITY, temp_float ) == aiReturn_SUCCESS )
    location.opacity = temp_float; else location.opacity = 1.0;

    /* get if two sided */
    if ( aimaterial.Get ( AI_MATKEY_TWOSIDED, temp_int ) == aiReturn_SUCCESS )
    location.two_sided = temp_int; else location.two_sided = false;

    /* get the shading mode and the program to use */
    if ( aimaterial.Get ( AI_MATKEY_SHADING_MODEL, temp_int ) == aiReturn_SUCCESS )
    location.shading_model = temp_int; else location.shading_model = 0;

    /* return the material */
    return location;
}

/* add_texture
 *
 * add a texture to a texture stack
 * 
 * location: the location of the level in the stack texture
 * aimaterial: the material the texture is being added from
 * index: the index of the texture
 * aitexturetype: the type of the stack
 * 
 * return: the texture stack level just added
 */
glh::model::texture_stack_level& glh::model::model::add_texture ( texture_stack_level& location, const aiMaterial& aimaterial, const unsigned index, const aiTextureType aitexturetype )
{
    /* temporary variables */
    aiString temp_string;
    float temp_float;
    int temp_int;

    /* get the location of the texture */
    aimaterial.GetTexture ( aitexturetype, index, &temp_string );
    std::string texpath { temp_string.C_Str () };

    /* set the blend attributes */
    if ( aimaterial.Get ( AI_MATKEY_TEXOP ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    location.blend_operation = temp_int; else location.blend_operation = 0;
    if ( aimaterial.Get ( AI_MATKEY_TEXBLEND ( aitexturetype, index ), temp_float ) == aiReturn_SUCCESS ) 
    location.blend_strength = temp_float; else location.blend_strength = 1.0;

    /* set wrapping modes */
    if ( aimaterial.Get ( AI_MATKEY_MAPPINGMODE_U ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    location.wrapping_u = temp_int; else location.wrapping_u = aiTextureMapMode_Wrap;
    if ( aimaterial.Get ( AI_MATKEY_MAPPINGMODE_V ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    location.wrapping_v = temp_int; else location.wrapping_v = aiTextureMapMode_Wrap;

    /* set uvwsrc */
    if ( aimaterial.Get ( AI_MATKEY_UVWSRC ( aitexturetype, index ), temp_int ) == aiReturn_SUCCESS )
    location.uvwsrc = temp_int; else location.uvwsrc = 0;

    /* check if already inported */
    for ( unsigned i = 0; i < textures.size (); ++i ) if ( textures.at ( i ).get_path () == texpath )
    {
        /* already imported, so purely set the index */
        location.index = i;

        /* return texture reference */
        return location;
    }

    /* not already imported, so import and set the index */
    textures.push_back ( texture2d { directory + "/" + texpath } );
    location.index = textures.size () - 1;

    /* return texture reference */
    return location;
}

/* add_mesh
 *
 * add a mesh to a node
 *
 * location: the mesh to configure
 * aimesh: the assimp mesh object to add
 * 
 * return: the mesh just added
 */
glh::model::mesh& glh::model::model::add_mesh ( mesh& location, const aiMesh& aimesh )
{
    /* add vertices, normals and texcoords */
    location.vertices.resize ( aimesh.mNumVertices );
    for ( unsigned i = 0; i < aimesh.mNumVertices; ++i )
    {
        /* add vertices and normals */
        location.vertices.at ( i ).position = cast_vector ( aimesh.mVertices [ i ] );
        location.vertices.at ( i ).normal = cast_vector ( aimesh.mNormals [ i ] );

        /* resize texcoords to number of uv components and set them */
        location.vertices.at ( i ).texcoords.resize ( aimesh.GetNumUVChannels () );
        for ( unsigned j = 0; j < aimesh.GetNumUVChannels (); ++j )
        location.vertices.at ( i ).texcoords.at ( j ) = cast_vector ( aimesh.mTextureCoords [ j ][ i ] );
    }

    /* set the number of uv channels */
    location.num_uv_channels = aimesh.GetNumUVChannels ();

    /* add material reference */
    location.properties_index = aimesh.mMaterialIndex;
    location.properties = &materials.at ( location.properties_index );

    /* add faces */
    location.faces.resize ( aimesh.mNumFaces );
    for ( unsigned i = 0; i < aimesh.mNumFaces; ++i ) add_face ( location.faces.at ( i ), location, aimesh.mFaces [ i ] );

    /* configure the buffers */
    configure_buffers ( location );

    /* return the mesh */
    return location;
}

/* add_face
 *
 * add a face to a mesh
 * 
 * location: the face to configure
 * parent: the mesh parent to the face
 * aiface: the assimp face object to add
 * 
 * return: the face just added
 */
glh::model::face& glh::model::model::add_face ( face& location, mesh& parent, const aiFace& aiface )
{
    /* set indices */
    location.indices.resize ( aiface.mNumIndices );
    location.vertices.resize ( aiface.mNumIndices );
    for ( unsigned i = 0; i < aiface.mNumIndices; ++i )
    {
        location.indices.at ( i ) = aiface.mIndices [ i ];
        location.vertices.at ( i ) = &parent.vertices.at ( location.indices.at ( i ) );
    }

    /* return face */
    return location;
}

/* configure_buffers
 *
 * configure the buffers of a mesh
 * 
 * location: the mesh to configure
 */
void glh::model::model::configure_buffers ( mesh& location )
{
    /* create temporary array for vertex data */
    const unsigned components_per_vertex = 6 + ( 3 * location.num_uv_channels );
    GLfloat vertices [ location.vertices.size () * components_per_vertex ];
    for ( unsigned i = 0; i < location.vertices.size (); ++i )
    {
        vertices [ ( i * components_per_vertex ) + 0 ] = location.vertices.at ( i ).position.at ( 0 );
        vertices [ ( i * components_per_vertex ) + 1 ] = location.vertices.at ( i ).position.at ( 1 );
        vertices [ ( i * components_per_vertex ) + 2 ] = location.vertices.at ( i ).position.at ( 2 );
        vertices [ ( i * components_per_vertex ) + 3 ] = location.vertices.at ( i ).normal.at ( 0 );
        vertices [ ( i * components_per_vertex ) + 4 ] = location.vertices.at ( i ).normal.at ( 1 );
        vertices [ ( i * components_per_vertex ) + 5 ] = location.vertices.at ( i ).normal.at ( 2 );
        for ( unsigned j = 0; j < location.num_uv_channels; ++j )
        {
            vertices [ ( i * components_per_vertex ) + ( j * 3 ) + 6 ] = location.vertices.at ( i ).texcoords.at ( j ).at ( 0 );
            vertices [ ( i * components_per_vertex ) + ( j * 3 ) + 7 ] = location.vertices.at ( i ).texcoords.at ( j ).at ( 1 );
            vertices [ ( i * components_per_vertex ) + ( j * 3 ) + 8 ] = location.vertices.at ( i ).texcoords.at ( j ).at ( 2 );
        }        
    }

    /* create temporary array to store indices */
    GLuint indices [ location.faces.size () * 3 ];
    for ( unsigned i = 0; i < location.faces.size (); ++i )
    {
        indices [ ( i * 3 ) + 0 ] = location.faces.at ( i ).indices.at ( 0 );
        indices [ ( i * 3 ) + 1 ] = location.faces.at ( i ).indices.at ( 1 );
        indices [ ( i * 3 ) + 2 ] = location.faces.at ( i ).indices.at ( 2 );
    }

    /* buffer the data */
    location.vertex_data.buffer_data ( sizeof ( vertices ), vertices, GL_STATIC_DRAW );
    location.index_data.buffer_data ( sizeof ( indices ), indices, GL_STATIC_DRAW );

    /* configure the vao */
    location.array_object.set_vertex_attrib ( 0, location.vertex_data, 3, GL_FLOAT, GL_FALSE, components_per_vertex * sizeof ( GLfloat ), ( GLvoid * ) 0 );
    location.array_object.set_vertex_attrib ( 1, location.vertex_data, 3, GL_FLOAT, GL_FALSE, components_per_vertex * sizeof ( GLfloat ), ( GLvoid * ) ( 3 * sizeof ( GLfloat ) ) );
    for ( unsigned i = 0; i < location.num_uv_channels; ++i )
    {
        location.array_object.set_vertex_attrib ( 2 + i, location.vertex_data, 3, GL_FLOAT, GL_FALSE, components_per_vertex * sizeof ( GLfloat ), ( GLvoid * ) ( ( ( i * 3 ) + 6 ) * sizeof ( GLfloat ) ) );
    }
    location.array_object.bind_ebo ( location.index_data );
}

/* add_node
 *
 * recursively add nodes to the node tree
 * 
 * location: the node to configure
 * ainode: the assimp node object to add
 * 
 * return: the node just added
 */
glh::model::node& glh::model::model::add_node ( node& location, const aiNode& ainode )
{
    /* set all of the children first */
    location.children.resize ( ainode.mNumChildren );
    for ( unsigned i = 0; i < ainode.mNumChildren; ++i ) add_node ( location.children.at ( i ), * ainode.mChildren [ i ] );

    /* now configure the meshes */
    location.mesh_indices.resize ( ainode.mNumMeshes );
    location.meshes.resize ( ainode.mNumMeshes );
    for ( unsigned i = 0; i < ainode.mNumMeshes; ++i )
    {
        location.mesh_indices.at ( i ) = ainode.mMeshes [ i ];
        location.meshes.at ( i ) = &meshes.at ( location.mesh_indices.at ( i ) );
    }

    /* set transformation */
    location.transform = cast_matrix ( ainode.mTransformation );

    /* return node */
    return location;
}

/* render_node
 *
 * render the model
 * 
 * location: the node to render
 * transform: the current model transformation from all the previous nodes
 */
void glh::model::model::render_node ( const node& location, const math::mat4& transform ) const
{
    /* create transformation matrix */
    math::mat4 trans = transform * location.transform;

    /* first render the child nodes */
    for ( const node& child: location.children ) render_node ( child, trans );

    /* set the model and normal matrices */
    cached_model_uniform->set_matrix ( trans );
    cached_normal_uniform->set_matrix ( math::normal ( trans ) );

    /* render all of the meshes */
    for ( const mesh * m: location.meshes ) 
    {
        render_mesh ( * m );
    }
}

/* render_mesh
 *
 * render a mesh
 * 
 * location: the mesh to render
 */
void glh::model::model::render_mesh ( const mesh& location ) const
{
    /* set the stack sizes */
    cached_material_uniforms->ambient_stack_size_uni.set_int ( location.properties->ambient_stack.levels.size () );
    cached_material_uniforms->diffuse_stack_size_uni.set_int ( location.properties->diffuse_stack.levels.size () );
    cached_material_uniforms->specular_stack_size_uni.set_int ( location.properties->specular_stack.levels.size () );

    /* set the colour values */
    cached_material_uniforms->ambient_stack_base_colour_uni.set_vector ( location.properties->ambient_stack.base_colour );
    cached_material_uniforms->diffuse_stack_base_colour_uni.set_vector ( location.properties->diffuse_stack.base_colour );
    cached_material_uniforms->specular_stack_base_colour_uni.set_vector ( location.properties->specular_stack.base_colour ); 

    /* bind the textures and set the sampler values */
    unsigned i = 0, offset = 0;
    for ( i = 0; i < location.properties->ambient_stack.levels.size (); ++i, ++offset ) 
    {
        location.properties->ambient_stack.levels.at ( i ).texture->bind ( GL_TEXTURE0 + i + offset );
        location.properties->ambient_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( location.properties->ambient_stack.levels.at ( i ).wrapping_u ) );
        location.properties->ambient_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( location.properties->ambient_stack.levels.at ( i ).wrapping_v ) );
        cached_material_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( i + offset );
        cached_material_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( location.properties->ambient_stack.levels.at ( i ).blend_operation );
        cached_material_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( location.properties->ambient_stack.levels.at ( i ).blend_strength );
        cached_material_uniforms->ambient_stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( location.properties->ambient_stack.levels.at ( i ).uvwsrc );
    }
    for ( i = 0; i < location.properties->diffuse_stack.levels.size (); ++i, ++offset ) 
    {
        location.properties->diffuse_stack.levels.at ( i ).texture->bind ( GL_TEXTURE0 + i + offset );
        location.properties->diffuse_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( location.properties->diffuse_stack.levels.at ( i ).wrapping_u ) );
        location.properties->diffuse_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( location.properties->diffuse_stack.levels.at ( i ).wrapping_v ) );
        cached_material_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( i + offset );
        cached_material_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( location.properties->diffuse_stack.levels.at ( i ).blend_operation );
        cached_material_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( location.properties->diffuse_stack.levels.at ( i ).blend_strength );
        cached_material_uniforms->diffuse_stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( location.properties->diffuse_stack.levels.at ( i ).uvwsrc );
    }
    for ( i = 0; i < location.properties->specular_stack.levels.size (); ++i, ++offset ) 
    {
        location.properties->specular_stack.levels.at ( i ).texture->bind ( GL_TEXTURE0 + i + offset );
        location.properties->specular_stack.levels.at ( i ).texture->set_s_wrap ( cast_wrapping ( location.properties->specular_stack.levels.at ( i ).wrapping_u ) );
        location.properties->specular_stack.levels.at ( i ).texture->set_t_wrap ( cast_wrapping ( location.properties->specular_stack.levels.at ( i ).wrapping_v ) );
        cached_material_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "texunit" ).set_int ( i + offset );
        cached_material_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( location.properties->specular_stack.levels.at ( i ).blend_operation );
        cached_material_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( location.properties->specular_stack.levels.at ( i ).blend_strength );
        cached_material_uniforms->specular_stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( location.properties->specular_stack.levels.at ( i ).uvwsrc );
    }

    /* set blending mode */
    cached_material_uniforms->blending_mode_uni.set_int ( location.properties->blending_mode );

    /* set shininess values */
    cached_material_uniforms->shininess_uni.set_int ( location.properties->shininess );
    cached_material_uniforms->shininess_strength_uni.set_int ( location.properties->shininess_strength );

    /* set opacity */
    cached_material_uniforms->opacity_uni.set_int ( location.properties->opacity );

    /* bind the vao */
    location.array_object.bind ();

    /* draw elements */
    renderer::draw_elements ( GL_TRIANGLES, location.faces.size () * 3, GL_UNSIGNED_INT, 0 );
}
