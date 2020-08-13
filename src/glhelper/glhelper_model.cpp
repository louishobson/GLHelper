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
 * _pretransform_matrix: the pre-transformation matrix to use if GLH_PRETRANSFORM_VERTICES is set as an import flag
 */
glh::model::model::model ( const std::string& _directory, const std::string& _entry, const unsigned _model_import_flags, const math::mat4& _pretransform_matrix )
    : directory { _directory }
    , entry { _entry }
    , model_import_flags { _model_import_flags }
    , pps { aiProcessPreset_TargetRealtime_MaxQuality }
    , pretransform_matrix { _pretransform_matrix }
    , pretransform_normal_matrix { math::normal ( _pretransform_matrix ) }
    , alpha_test_program { alpha_test_vshader, alpha_test_gshader, alpha_test_fshader }
{
    /* add debone and optimise graph */
    pps |= aiProcess_Debone | aiProcess_OptimizeGraph;

    /* remove find degenerates flag, as this causes incomplete triangulation */
    pps &= ~aiProcess_FindDegenerates;

    /* modify pps based on import flags */
    if ( model_import_flags & import_flags::GLH_FLIP_V_TEXTURES ) pps |= aiProcess_FlipUVs;
    if ( model_import_flags & import_flags::GLH_PRETRANSFORM_VERTICES ) pps |= aiProcess_PreTransformVertices;

    /* throw if both GLH_IGNORE_VCOLOR_WHEN_ALPHA_TESTING and GLH_IGNORE_TEXTURE_COLOR_WHEN_ALPHA_TESTING are set */
    if ( model_import_flags & import_flags::GLH_IGNORE_VCOLOR_WHEN_ALPHA_TESTING && model_import_flags & import_flags::GLH_IGNORE_TEXTURE_COLOR_WHEN_ALPHA_TESTING )
        throw exception::model_exception { "cannot import model with both GLH_IGNORE_VCOLOR_WHEN_ALPHA_TESTING and GLH_IGNORE_TEXTURE_COLOR_WHEN_ALPHA_TESTING options set" };

    /* if alpha testing is requested, initialise the program and shaders */
    if ( model_import_flags & import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES )
    {
        alpha_test_vshader.include_files ( { "shaders/materials.glsl", "shaders/vertex.alpha_test.glsl" } );
        alpha_test_gshader.include_files ( { "shaders/materials.glsl", "shaders/geometry.alpha_test.glsl" } );
        alpha_test_fshader.include_files ( { "shaders/materials.glsl", "shaders/fragment.alpha_test.glsl" } );
        alpha_test_program.compile_and_link ();
    }

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
 * model_matrix_uni: a 4x4 matrix uniform to cache and apply set the model transformations to
 * transform: the overall model transformation to apply (identity by default)
 * flags: rendering flags (none by default)
 */
void glh::model::model::render ( core::struct_uniform& material_uni, core::uniform& model_matrix_uni, const math::mat4& transform, const unsigned flags )
{
    /* reload the cache of uniforms  */
    cache_uniforms ( material_uni, model_matrix_uni );

    /* render */
    render ( transform, flags );
}
void glh::model::model::render ( const math::mat4& transform, const unsigned flags ) const
{
    /* throw if uniforms are not already cached */
    if ( !cached_material_uniforms && ~flags & render_flags::GLH_NO_MATERIAL || !cached_model_matrix_uniform && ~flags & render_flags::GLH_NO_MODEL_MATRIX )
        throw exception::uniform_exception { "attempted to render model without a complete uniform cache" };

    /* cache the render flags */
    model_render_flags = flags;

    /* if imported with global vertex arrays configured... */
    if ( model_import_flags & import_flags::GLH_CONFIGURE_GLOBAL_VERTEX_ARRAYS )
    {
        /* bind global vertex arrays */
        global_vertex_arrays.bind ();

        /* render the root node */
        render_node ( root_node, transform );

        /* only unbind if GLH_LEAVE_GLOBAL_VERTEX_ARRAYS_BOUND is unset */
        if ( ~model_render_flags & render_flags::GLH_LEAVE_GLOBAL_VERTEX_ARRAYS_BOUND ) global_vertex_arrays.unbind ();
    } 
    /* else just render root node normally */
    else render_node ( root_node, transform );

}
void glh::model::model::render ( const unsigned flags )
{
    /* call overload with identity matrix */
    render ( math::identity<4> (), flags );
}



/* cache_uniforms
 *
 * cache all uniforms
 *
 * material_uni: the material uniform to cache
 * model_matrix_uni: model uniform to cache
 */
void glh::model::model::cache_uniforms ( core::struct_uniform& material_uni, core::uniform& model_matrix_uni )
{
    /* cache uniforms */
    cache_material_uniforms ( material_uni );
    cache_model_uniform ( model_matrix_uni );
}

/* cache_material_uniforms
 * cache_model_uniform
 * 
 * cache material and model matrix uniforms separately
 */
void glh::model::model::cache_material_uniforms ( core::struct_uniform& material_uni )
{
    /* if uniforms are not already cached, cache the new ones */
    if ( !cached_material_uniforms || cached_material_uniforms->material_uni != material_uni )
    {
        /* extract stack uniforms */
        auto& ambient_stack_uni  = material_uni.get_struct_uniform ( "ambient_stack" );
        auto& diffuse_stack_uni  = material_uni.get_struct_uniform ( "diffuse_stack" );
        auto& specular_stack_uni = material_uni.get_struct_uniform ( "specular_stack" );
        auto& emissive_stack_uni = material_uni.get_struct_uniform ( "emission_stack" );
        auto& normal_stack_uni   = material_uni.get_struct_uniform ( "normal_stack" );

        /* cache uniforms */
        cached_material_uniforms.reset ( new cached_material_uniforms_struct
        {
            material_uni,
            ambient_stack_uni.get_uniform ( "stack_size" ),
            diffuse_stack_uni.get_uniform ( "stack_size" ),
            specular_stack_uni.get_uniform ( "stack_size" ),
            emissive_stack_uni.get_uniform ( "stack_size" ),
            normal_stack_uni.get_uniform ( "stack_size" ),

            ambient_stack_uni.get_uniform ( "base_color" ),
            diffuse_stack_uni.get_uniform ( "base_color" ),
            specular_stack_uni.get_uniform ( "base_color" ),
            emissive_stack_uni.get_uniform ( "base_color" ),
            normal_stack_uni.get_uniform ( "base_color" ),
            
            ambient_stack_uni.get_struct_array_uniform ( "levels" ),
            diffuse_stack_uni.get_struct_array_uniform ( "levels" ),
            specular_stack_uni.get_struct_array_uniform ( "levels" ),
            emissive_stack_uni.get_struct_array_uniform ( "levels" ),
            normal_stack_uni.get_struct_array_uniform ( "levels" ),

            ambient_stack_uni.get_uniform ( "textures" ),
            diffuse_stack_uni.get_uniform ( "textures" ),
            specular_stack_uni.get_uniform ( "textures" ),
            emissive_stack_uni.get_uniform ( "textures" ),
            normal_stack_uni.get_uniform ( "textures" ),

            material_uni.get_uniform ( "blending_mode" ),
            material_uni.get_uniform ( "shininess" ),
            material_uni.get_uniform ( "shininess_strength" ),
            material_uni.get_uniform ( "opacity" ),
            material_uni.get_uniform ( "definitely_opaque" )
        } );
    }
}
void glh::model::model::cache_model_uniform ( core::uniform& model_matrix_uni )
{
    /* if not already cached, cache new uniform */
    if ( !cached_model_matrix_uniform || cached_model_matrix_uniform->model_matrix_uni != model_matrix_uni )
    {
        cached_model_matrix_uniform.reset ( new cached_model_matrix_uniform_struct
        {
            model_matrix_uni
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

    /* now loop through the meshes */
    meshes.resize ( aiscene.mNumMeshes );
    for ( unsigned i = 0; i < aiscene.mNumMeshes; ++i )
    {   
        /* add the mesh */
        add_mesh ( meshes.at ( i ), * aiscene.mMeshes [ i ] );

        /* if required to split the meshes, also split it */
        if ( model_import_flags & import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES ) split_mesh ( meshes.at ( i ) );
    }

    /* configure global vertex arrays if necessary */
    if ( model_import_flags & import_flags::GLH_CONFIGURE_GLOBAL_VERTEX_ARRAYS ) configure_global_vertex_arrays ();

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

    /* get base colors and set up texture stacks */
    add_texture_stack ( _material.ambient_stack, aimaterial, aiTextureType_AMBIENT, 
        ( aimaterial.Get ( AI_MATKEY_COLOR_AMBIENT, temp_color ) == aiReturn_SUCCESS ? cast_vector ( temp_color ) : math::fvec3 { 0.0 } ), model_import_flags & import_flags::GLH_AMBIENT_SRGBA );
    add_texture_stack ( _material.diffuse_stack, aimaterial, aiTextureType_DIFFUSE, 
        ( aimaterial.Get ( AI_MATKEY_COLOR_DIFFUSE, temp_color ) == aiReturn_SUCCESS ? cast_vector ( temp_color ) : math::fvec3 { 0.0 } ), model_import_flags & import_flags::GLH_DIFFUSE_SRGBA );
    add_texture_stack ( _material.specular_stack, aimaterial, aiTextureType_SPECULAR, 
        ( aimaterial.Get ( AI_MATKEY_COLOR_SPECULAR, temp_color ) == aiReturn_SUCCESS ? cast_vector ( temp_color ) : math::fvec3 { 0.0 } ), model_import_flags & import_flags::GLH_SPECULAR_SRGBA );
    add_texture_stack ( _material.emission_stack, aimaterial, aiTextureType_EMISSIVE, 
        ( aimaterial.Get ( AI_MATKEY_COLOR_EMISSIVE, temp_color ) == aiReturn_SUCCESS ? cast_vector ( temp_color ) : math::fvec3 { 0.0 } ) );
    add_texture_stack ( _material.normal_stack, aimaterial, aiTextureType_NORMALS, math::fvec3 { 0.0 } );

    /* get the blend mode */
    if ( aimaterial.Get ( AI_MATKEY_BLEND_FUNC, temp_int ) == aiReturn_SUCCESS )
    _material.blending_mode = temp_int; else _material.blending_mode = 0;

    /* get the shininess properties */
    if ( aimaterial.Get ( AI_MATKEY_SHININESS, temp_float ) == aiReturn_SUCCESS && temp_float >= 1.0 )
    _material.shininess = temp_float; else _material.shininess = 1.0;
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



/* add_texture_stack
 *
 * take a texture stack and add it to the store
 * 
 * _texture_stack: the texture stack to configure
 * aimaterial: the material to get the texture stack from
 * aitexturetype: the type of texture to add
 * base_color: base color of the texture, as recieved by assimp
 * use_srgb: true if colors should be gamma corrected
 * 
 * return: the texture_stack just added
 */
glh::model::texture_stack& glh::model::model::add_texture_stack ( texture_stack& _texture_stack, const aiMaterial& aimaterial, const aiTextureType aitexturetype, const math::fvec3 base_color, const bool use_srgb )
{
    /* temportary get storage */
    aiColor3D temp_color;
    float temp_float;
    int temp_int;
    aiString temp_string;



    /* set the base color */
    _texture_stack.base_color = math::fvec4 { base_color, 0.0 };

    /* apply sRGBA transformation to base color */
    if ( use_srgb ) _texture_stack.base_color = math::pow ( _texture_stack.base_color, math::fvec4 { 2.2 } );

    /* assume is definitely opaque */
    _texture_stack.definitely_opaque = true;

    /* get the stack size */
    _texture_stack.stack_size = aimaterial.GetTextureCount ( aitexturetype );
    _texture_stack.stack_width = 0; _texture_stack.stack_height = 0;

    /* if stack size is 0, set base color's alpha to 1.0 and return immediately */
    if ( _texture_stack.stack_size == 0 )
    {
        _texture_stack.base_color.at ( 3 ) = 1.0;
        return _texture_stack;
    }



    /* process each level of the stack */
    for ( unsigned i = 0; i < _texture_stack.stack_size; ++i )
    {
        /* set the blend attributes */
        if ( aimaterial.Get ( AI_MATKEY_TEXOP ( aitexturetype, i ), temp_int ) == aiReturn_SUCCESS )
        _texture_stack.levels.at ( i ).blend_operation = temp_int; else _texture_stack.levels.at ( i ).blend_operation = 0;
        if ( i == 0 && ( _texture_stack.base_color == math::fvec4 { 0.0 } || _texture_stack.base_color == math::fvec4 { 1.0 } ) && ( _texture_stack.levels.at ( 0 ).blend_operation <= 1 ) )
            { _texture_stack.base_color = math::fvec4 { 0.0 }; _texture_stack.levels.at ( 0 ).blend_operation = 1; }
        if ( aimaterial.Get ( AI_MATKEY_TEXBLEND ( aitexturetype, i ), temp_float ) == aiReturn_SUCCESS ) 
        _texture_stack.levels.at ( i ).blend_strength = temp_float; else _texture_stack.levels.at ( i ).blend_strength = 1.0;

        /* if blend operation of first level is not addition, set the opacity of the base color to 1.0 */
        if ( i == 0 && _texture_stack.levels.at ( i ).blend_operation != 1 ) _texture_stack.base_color.at ( 3 ) = 1.0;

        /* is i == 0, set wrapping modes, else assert that the wrapping modes are consistent */
        if ( aimaterial.Get ( AI_MATKEY_MAPPINGMODE_U ( aitexturetype, i ), temp_int ) != aiReturn_SUCCESS ) temp_int = aiTextureMapMode_Wrap;
        if ( i == 0 ) _texture_stack.wrapping_u = temp_int; else if ( temp_int != _texture_stack.wrapping_u )
            throw exception::model_exception { "wrapping modes are not consistent between texture stack levels" }; 
        if ( aimaterial.Get ( AI_MATKEY_MAPPINGMODE_V ( aitexturetype, i ), temp_int ) != aiReturn_SUCCESS ) temp_int = aiTextureMapMode_Wrap;
        if ( i == 0 ) _texture_stack.wrapping_v = temp_int; else if ( temp_int != _texture_stack.wrapping_v )
            throw exception::model_exception { "wrapping modes are not consistent between texture stack levels" }; 
        
        /* set uvwsrc, assuming the value of i if there is no explicit value
         * when importing meshes, if it turns out that a mesh only has one uv channel, the uvsrc of all the stacks of its material will be set to 0
         * long story short, the uvsrc may change when importing meshes
         */
        if ( aimaterial.Get ( AI_MATKEY_UVWSRC ( aitexturetype, i ), temp_int ) == aiReturn_SUCCESS )
        _texture_stack.levels.at ( i ).uvwsrc = temp_int; else _texture_stack.levels.at ( i ).uvwsrc = i;

        /* set the image index by importing the image */
        aimaterial.GetTexture ( aitexturetype, i, &temp_string );
        _texture_stack.levels.at ( i ).image_index = add_image ( directory + "/" + temp_string.C_Str () );

        /* if i == 0, set the width and height of the stack, else assert that dimensions are consistent */
        if ( i == 0 )
        {
            _texture_stack.stack_width = images.at ( _texture_stack.levels.at ( i ).image_index ).get_width ();
            _texture_stack.stack_height = images.at ( _texture_stack.levels.at ( i ).image_index ).get_height ();
        } else if ( _texture_stack.stack_width != images.at ( _texture_stack.levels.at ( i ).image_index ).get_width () || _texture_stack.stack_height != images.at ( _texture_stack.levels.at ( i ).image_index ).get_height () )
            throw exception::model_exception { "image dimensions are not consistent between texture stack levels" }; 

        /* if image is not definitely opaque, neither is the stack */
        _texture_stack.definitely_opaque &= images.at ( _texture_stack.levels.at ( i ).image_index ).is_definitely_opaque ();
    }
    
    
    
    /* set up texture storage */
    _texture_stack.textures.tex_storage ( _texture_stack.stack_width, _texture_stack.stack_height, _texture_stack.stack_size, ( use_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8 ) );

    /* loop through images and substitute their data in */
    for ( unsigned i = 0; i < _texture_stack.stack_size; ++i )
        _texture_stack.textures.tex_sub_image ( 0, 0, i, { images.at ( _texture_stack.levels.at ( i ).image_index ) } );

    /* set wrapping modes */
    _texture_stack.textures.set_s_wrap ( cast_wrapping ( _texture_stack.wrapping_u ) );
    _texture_stack.textures.set_t_wrap ( cast_wrapping ( _texture_stack.wrapping_v ) );

    /* set mag/min filters */
    _texture_stack.textures.set_mag_filter ( GL_LINEAR );
    _texture_stack.textures.set_min_filter ( GL_LINEAR_MIPMAP_LINEAR );

    /* generate mipmaps */
    _texture_stack.textures.generate_mipmap ();



    /* return the texture stack */
    return _texture_stack;
}



/* add_image
*
* load an image from a filepath
* 
* filepath: string for the filepath to the image
* 
* return: the index of the image in the global array
*/
unsigned glh::model::model::add_image ( const std::string& filepath )
{

    /* check if the image already exists */
    for ( unsigned i = 0; i < images.size (); ++i ) if ( images.at ( i ).get_path () == filepath ) return i;

    /* otherwise add new image
     * GLH_FLIP_V_TEXTURES no longer actually flips the texture because that's slow
     * it purely forces the assimp post process flag to flip the uv coords */
    //images.emplace_back ( filepath, 4, model_import_flags & import_flags::GLH_FLIP_V_TEXTURES );
    images.emplace_back ( filepath );

    /* return the size of images - 1 */
    return images.size () - 1;
}



/* is_definitely_opaque
 *
 * determines if a material or mesh is definitely opaque
 * 
 * _material/_mesh: the material/mesh to check
 * 
 * return: boolean for if is definitely opaque
 */
bool glh::model::model::is_definitely_opaque ( const material& _material ) const
{
    /* if opacity != 1.0, return false */
    if ( _material.opacity != 1.0 ) return false;

    /* return false if diffuse texture stack is not definitely opaque, and true if it is */
    return _material.diffuse_stack.definitely_opaque;
}
bool glh::model::model::is_definitely_opaque ( const mesh& _mesh ) const
{
    /* if texture color should be used in determining opacity, then
     * if if material is not definitely opaque, return false 
     */
    if ( ~model_import_flags & import_flags::GLH_IGNORE_TEXTURE_COLOR_WHEN_ALPHA_TESTING && !_mesh.properties->definitely_opaque ) return false;

    /* if vertex color should be used in determining opacity, then
     * loop through all vertex colors and check that alpha components are all 1.0
     */
    if ( ~model_import_flags & import_flags::GLH_IGNORE_VCOLOR_WHEN_ALPHA_TESTING ) 
        for ( unsigned i = 0; i < _mesh.vertices.size (); ++i ) if ( _mesh.vertices.at ( i ).vcolor.at ( 3 ) < 1.0 ) return false;

    /* otherwise return true */
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
    /* set the number of uv channels */
    _mesh.num_uv_channels = std::min<unsigned> ( aimesh.GetNumUVChannels (), GLH_MODEL_MAX_TEXTURE_STACK_SIZE );

    /* true if has vertex colors */
    const bool has_vcolors = aimesh.GetNumColorChannels ();

    /* add vertices, normals and texcoords */
    _mesh.num_vertices = aimesh.mNumVertices;
    _mesh.vertices.resize ( _mesh.num_vertices );
    for ( unsigned i = 0; i < aimesh.mNumVertices; ++i )
    {
        /* add vertex position, normal and tangent */
        _mesh.vertices.at ( i ).position = cast_vector ( aimesh.mVertices [ i ] );
        _mesh.vertices.at ( i ).normal = cast_vector ( aimesh.mNormals [ i ] );
        _mesh.vertices.at ( i ).tangent = cast_vector ( aimesh.mTangents [ i ] );

        /* transform them if pretransform is set */
        if ( model_import_flags & import_flags::GLH_PRETRANSFORM_VERTICES )
        {
            _mesh.vertices.at ( i ).position = math::fvec3 ( pretransform_matrix * math::fvec4 ( _mesh.vertices.at ( i ).position, 1.0 ) );
            _mesh.vertices.at ( i ).normal = pretransform_normal_matrix * _mesh.vertices.at ( i ).normal;
            _mesh.vertices.at ( i ).tangent = pretransform_normal_matrix * _mesh.vertices.at ( i ).tangent;
        }

        /* use Gram-Schmidt process to re-orthogonalize the normal and tangent vectors */
        _mesh.vertices.at ( i ).normal = math::normalize ( _mesh.vertices.at ( i ).normal );
        _mesh.vertices.at ( i ).tangent = math::normalize ( _mesh.vertices.at ( i ).tangent - ( math::dot ( _mesh.vertices.at ( i ).normal, _mesh.vertices.at ( i ).tangent ) * _mesh.vertices.at ( i ).normal ) );

        /* set vertex colors */
        _mesh.vertices.at ( i ).vcolor = ( has_vcolors ? cast_vector ( aimesh.mColors [ 0 ][ i ] ) : math::fvec4 ( 1.0 ) );

        /* apply gamma correction */
        if ( model_import_flags & import_flags::GLH_VERTEX_SRGBA ) _mesh.vertices.at ( i ).vcolor = 
            math::fvec4 { math::pow ( math::fvec3 { _mesh.vertices.at ( i ).vcolor }, math::fvec3 { 2.2 } ), _mesh.vertices.at ( i ).vcolor.at ( 3 ) };

        /* set uv channel coords */
        for ( unsigned j = 0; j < _mesh.num_uv_channels; ++j )
            _mesh.vertices.at ( i ).texcoords.at ( j ) = math::fvec2 ( cast_vector ( aimesh.mTextureCoords [ j ][ i ] ) );
    }

    /* add material reference */
    _mesh.properties_index = aimesh.mMaterialIndex;
    _mesh.properties = &materials.at ( _mesh.properties_index );

    /* if there is only one set of texture coords, set all uvsrc's to 0 */
    if ( _mesh.num_uv_channels == 1 )
    {
        for ( unsigned i = 0; i < _mesh.properties->ambient_stack.stack_size;  ++i ) _mesh.properties->ambient_stack.levels.at ( i ).uvwsrc  = 0;
        for ( unsigned i = 0; i < _mesh.properties->diffuse_stack.stack_size;  ++i ) _mesh.properties->diffuse_stack.levels.at ( i ).uvwsrc  = 0;
        for ( unsigned i = 0; i < _mesh.properties->specular_stack.stack_size; ++i ) _mesh.properties->specular_stack.levels.at ( i ).uvwsrc = 0;
        for ( unsigned i = 0; i < _mesh.properties->emission_stack.stack_size; ++i ) _mesh.properties->emission_stack.levels.at ( i ).uvwsrc = 0;
        for ( unsigned i = 0; i < _mesh.properties->normal_stack.stack_size;   ++i ) _mesh.properties->normal_stack.levels.at ( i ).uvwsrc   = 0;
    } else
    /* else check for out of bounds uvsrc */
    {
        bool out_of_bounds_uvsrc = false;
        for ( unsigned i = 0; i < _mesh.properties->ambient_stack.stack_size;  ++i ) out_of_bounds_uvsrc |= _mesh.properties->ambient_stack.levels.at ( i ).uvwsrc  >= _mesh.num_uv_channels;
        for ( unsigned i = 0; i < _mesh.properties->diffuse_stack.stack_size;  ++i ) out_of_bounds_uvsrc |= _mesh.properties->diffuse_stack.levels.at ( i ).uvwsrc  >= _mesh.num_uv_channels;
        for ( unsigned i = 0; i < _mesh.properties->specular_stack.stack_size; ++i ) out_of_bounds_uvsrc |= _mesh.properties->specular_stack.levels.at ( i ).uvwsrc >= _mesh.num_uv_channels;
        for ( unsigned i = 0; i < _mesh.properties->emission_stack.stack_size; ++i ) out_of_bounds_uvsrc |= _mesh.properties->emission_stack.levels.at ( i ).uvwsrc >= _mesh.num_uv_channels;
        for ( unsigned i = 0; i < _mesh.properties->normal_stack.stack_size;   ++i ) out_of_bounds_uvsrc |= _mesh.properties->normal_stack.levels.at ( i ).uvwsrc   >= _mesh.num_uv_channels;
        if ( out_of_bounds_uvsrc ) throw exception::model_exception { "uvsrc is out of bounds" };
    }

    /* add faces */
    _mesh.num_faces             = aimesh.mNumFaces;
    _mesh.num_opaque_faces      = 0;
    _mesh.num_transparent_faces = 0;
    _mesh.faces.resize ( _mesh.num_faces );
    for ( unsigned i = 0; i < aimesh.mNumFaces; ++i ) add_face ( _mesh.faces.at ( i ), _mesh, aimesh.mFaces [ i ] );

    /* set face indices to default */
    _mesh.start_of_faces                    = 0;
    _mesh.start_of_opaque_faces             = 0;
    _mesh.start_of_transparent_faces        = 0;
    _mesh.global_start_of_faces             = 0;
    _mesh.global_start_of_opaque_faces      = 0;
    _mesh.global_start_of_transparent_faces = 0;

    /* set definitely opaque */
    _mesh.definitely_opaque = is_definitely_opaque ( _mesh );

    /* if GLH_CONFIGURE_REGIONS_ACCURATE and GLH_CONFIGURE_ONLY_ROOT_NODE_REGION is set, don't configure meshes
     * else if any mesh configuration flag is set, configure meshes
     */
    if ( !( model_import_flags & import_flags::GLH_CONFIGURE_REGIONS_ACCURATE && model_import_flags & import_flags::GLH_CONFIGURE_ONLY_ROOT_NODE_REGION ) )
        if ( model_import_flags & ( import_flags::GLH_CONFIGURE_REGIONS_FAST | import_flags::GLH_CONFIGURE_REGIONS_ACCEPTABLE | import_flags::GLH_CONFIGURE_REGIONS_ACCURATE ) ) 
            configure_mesh_region ( _mesh );



    /* buffer vertex data */
    _mesh.vertex_data.buffer_storage ( _mesh.vertices.begin (), _mesh.vertices.end () );

    /* buffer index data 
     * don't use immutable storage if the meshes will be split
     */
    if ( model_import_flags & import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES )
        _mesh.index_data.buffer_data ( _mesh.faces.begin (), _mesh.faces.end () );
    else _mesh.index_data.buffer_storage ( _mesh.faces.begin (), _mesh.faces.end () );

    /* configure the vao */
    _mesh.vertex_arrays.set_vertex_attrib ( 0, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 0 * sizeof ( GLfloat ) );
    _mesh.vertex_arrays.set_vertex_attrib ( 1, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 3 * sizeof ( GLfloat ) );
    _mesh.vertex_arrays.set_vertex_attrib ( 2, _mesh.vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 6 * sizeof ( GLfloat ) );
    _mesh.vertex_arrays.set_vertex_attrib ( 3, _mesh.vertex_data, 4, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 9 * sizeof ( GLfloat ) );
    for ( unsigned i = 0; i < _mesh.num_uv_channels; ++i )
        _mesh.vertex_arrays.set_vertex_attrib ( 4 + i, _mesh.vertex_data, 2, GL_FLOAT, GL_FALSE, sizeof ( vertex ), ( 13 + i * 2 ) * sizeof ( GLfloat ) );
    _mesh.vertex_arrays.bind_ebo ( _mesh.index_data );



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
glh::model::face& glh::model::model::add_face ( face& _face, const mesh& _mesh, const aiFace& aiface )
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



/* split_mesh
 *
 * split a mesh into opaque and transparent faces
 * 
 * _mesh: the mesh to split
 */
void glh::model::model::split_mesh ( mesh& _mesh )
{
    /* if mesh is definitely opaque, set the number of opaque faces to be the same as the number of faces
     * since the index is already zero, this will cause the same vertices to be used by general faces as well as opaque faces
     */
    if ( _mesh.definitely_opaque ) _mesh.num_opaque_faces = _mesh.num_faces; else

    /* else if the material has an opacity of less than 1.0, set the number of transparent faces to be the same as the number of faces
     * since the index is already zero, this will cause the same vertices to be used by general faces as well as transparent faces
     */
    if ( _mesh.properties->opacity < 1.0 ) _mesh.num_transparent_faces = _mesh.num_faces; else
  
    /* else apply alpha testing on each face */
    {
        /* get ssbo size
         * there are two faces per byte, hence the division by two, and the size must be a multiple of 32
         */
        unsigned alpha_test_ssbo_size = std::ceil ( _mesh.num_faces / 2.0 );
        if ( alpha_test_ssbo_size % 32 != 0 ) alpha_test_ssbo_size += 32 - ( alpha_test_ssbo_size % 32 );

        /* create ssbo */
        core::ssbo alpha_test_ssbo;
        alpha_test_ssbo.buffer_storage ( alpha_test_ssbo_size, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );
        const unsigned zero = 0;
        alpha_test_ssbo.clear_data ( GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero );
        alpha_test_ssbo.bind ( 0 );

        /* create fbo */
        core::fbo alpha_test_fbo;
        alpha_test_fbo.draw_buffer ( GL_NONE );
        alpha_test_fbo.read_buffer ( GL_NONE );
        alpha_test_fbo.set_default_dimensions ( _mesh.properties->diffuse_stack.stack_width, _mesh.properties->diffuse_stack.stack_height );
        alpha_test_fbo.bind ();

        /* temporarily set diffuse texture stack to not use interpolation */
        _mesh.properties->diffuse_stack.textures.set_mag_filter ( GL_NEAREST );
        _mesh.properties->diffuse_stack.textures.set_min_filter ( GL_NEAREST_MIPMAP_NEAREST );

        /* use alpha test program */
        alpha_test_program.use ();

        /* cache the material uniform */
        cache_material_uniforms ( alpha_test_program.get_struct_uniform ( "material" ) );

        /* set viewport and disable some settings */
        core::renderer::disable_multisample ();
        core::renderer::viewport ( 0, 0, _mesh.properties->diffuse_stack.stack_width, _mesh.properties->diffuse_stack.stack_height );
        core::renderer::disable_depth_test ();
        core::renderer::disable_face_culling ();
        
        /* render the mesh
         * set the render flags to none
         * if GLH_CONFIGURE_GLOBAL_VERTEX_ARRAYS is set in import flags, temorarily remove it
         */
        model_render_flags = render_flags::GLH_NONE;
        if ( model_import_flags & import_flags::GLH_CONFIGURE_GLOBAL_VERTEX_ARRAYS )
        {
            model_import_flags &= ~import_flags::GLH_CONFIGURE_GLOBAL_VERTEX_ARRAYS;
            render_mesh ( _mesh );
            model_import_flags |= import_flags::GLH_CONFIGURE_GLOBAL_VERTEX_ARRAYS;
        } else render_mesh ( _mesh );
        
        /* wait for the draw command above to finish */
        glh::core::sync::finish_queue ();

        /* set texture stacks to use interpolation again */
        _mesh.properties->diffuse_stack.textures.set_mag_filter ( GL_LINEAR );
        _mesh.properties->diffuse_stack.textures.set_min_filter ( GL_LINEAR_MIPMAP_LINEAR );
        
        /* now loop through the faces... */
        for ( unsigned i = 0; i < _mesh.num_faces; ++i )
        {
            /* get the results of the alpha testing */
            const unsigned alpha_test_fb = alpha_test_ssbo.at<unsigned> ( i / 8 ) >> ( ( i % 8 ) * 4 );

            /* add to opaque mesh set if necessary */
            if ( ( ~model_import_flags & import_flags::GLH_IGNORE_VCOLOR_WHEN_ALPHA_TESTING && alpha_test_fb & 0x1 ) ||
                 ( ~model_import_flags & import_flags::GLH_IGNORE_TEXTURE_COLOR_WHEN_ALPHA_TESTING && alpha_test_fb & 0x4 ) )
            {
                ++_mesh.num_opaque_faces;
                _mesh.opaque_faces.push_back ( _mesh.faces.at ( i ) );
            }

            /* add to transparent mesh set if necessary */
            if ( ( ~model_import_flags & import_flags::GLH_IGNORE_VCOLOR_WHEN_ALPHA_TESTING && alpha_test_fb & 0x2 ) ||
                 ( ~model_import_flags & import_flags::GLH_IGNORE_TEXTURE_COLOR_WHEN_ALPHA_TESTING && alpha_test_fb & 0x8 ) )
             {
                ++_mesh.num_transparent_faces;
                _mesh.transparent_faces.push_back ( _mesh.faces.at ( i ) );
            }
        }
    }

    /* change index buffer to use immutable storage
     * if both the number of opaque and transparent faces are both either zero or the same as the max number of faces, then simply duplicate the index data from before
     */
    if ( ( _mesh.num_opaque_faces == 0 || _mesh.num_opaque_faces == _mesh.num_faces ) && ( _mesh.num_transparent_faces == 0 || _mesh.num_transparent_faces == _mesh.num_faces ) )
        _mesh.index_data.buffer_storage ( _mesh.faces.begin (), _mesh.faces.end () );
    else
    
    /* otherwise reformat the index data to contain the faces from alpha testing */
    {
        /* set the size of the buffer */
        _mesh.index_data.buffer_storage ( ( _mesh.num_faces + _mesh.num_opaque_faces + _mesh.num_transparent_faces ) * sizeof ( face ) );

        /* now buffer in the subdata, also setting the offsets for the types of face */
        _mesh.index_data.buffer_sub_data ( _mesh.faces.begin (), _mesh.faces.end (), 0 );

        _mesh.start_of_opaque_faces        = _mesh.num_faces * sizeof ( face );
        _mesh.global_start_of_opaque_faces = _mesh.num_faces * sizeof ( face );
        _mesh.index_data.buffer_sub_data ( _mesh.opaque_faces.begin (), _mesh.opaque_faces.end (), _mesh.num_faces );

        _mesh.start_of_transparent_faces        = _mesh.start_of_opaque_faces + _mesh.num_opaque_faces * sizeof ( face );     
        _mesh.global_start_of_transparent_faces = _mesh.start_of_opaque_faces + _mesh.num_opaque_faces * sizeof ( face );
        _mesh.index_data.buffer_sub_data ( _mesh.transparent_faces.begin (), _mesh.transparent_faces.end (), _mesh.num_faces + _mesh.num_opaque_faces );  
    }
}



/* configure_global_vertex_arrays
 *
 * configures the global vertex arrays
 */
void glh::model::model::configure_global_vertex_arrays ()
{
    /* collect the sizes of the buffers from each mesh
     * while doing so, change the global_start_of_..._faces values based on the running value of global_index_data_size
     */
    unsigned global_vertex_data_size = 0;
    unsigned global_index_data_size = 0;
    for ( mesh& _mesh: meshes )
    {
        _mesh.global_start_of_faces             += global_index_data_size;
        _mesh.global_start_of_opaque_faces      += global_index_data_size;
        _mesh.global_start_of_transparent_faces += global_index_data_size;
        global_vertex_data_size += _mesh.vertex_data.get_size ();
        global_index_data_size  += _mesh.index_data.get_size ();
    }

    /* resize the buffers */
    global_vertex_data.buffer_storage ( global_vertex_data_size );
    global_index_data.buffer_storage ( global_index_data_size );

    /* reset the sizes of the buffers to zero, ready to loop through the meshes again
     * the second loop will copy the vertex and index data into the global buffers
     */
    global_vertex_data_size = 0;
    global_index_data_size = 0;
    for ( mesh& _mesh: meshes )
    {
        /* copy the vertex and index data */
        global_vertex_data.copy_sub_data ( _mesh.vertex_data, _mesh.vertex_data.get_size (), 0, global_vertex_data_size );
        global_index_data.copy_sub_data ( _mesh.index_data, _mesh.index_data.get_size (), 0, global_index_data_size );

        /* increase the size values */
        global_vertex_data_size += _mesh.vertex_data.get_size ();
        global_index_data_size  += _mesh.index_data.get_size ();
    }
    
    /* wait for the copy commands above to finish */
    glh::core::sync::finish_queue ();

    /* reset the sizes of the buffers back to zero for the final time, ready for the third loop of the mehses
     * thid will modify the index data to the correct offsets in the global vertex data
     */
    global_vertex_data_size = 0;
    global_index_data_size = 0;
    for ( mesh& _mesh: meshes )
    {
        /* increase the values of the index data */
        for ( unsigned i = global_index_data_size; i < global_index_data_size + _mesh.index_data.get_size (); i += sizeof ( unsigned ) )
            global_index_data.at<unsigned> ( i / sizeof ( unsigned ) ) += global_vertex_data_size / sizeof ( vertex );

        /* increase the size values */
        global_vertex_data_size += _mesh.vertex_data.get_size ();
        global_index_data_size  += _mesh.index_data.get_size ();
    }

    /* finally unmap the global index data */
    global_index_data.unmap_buffer ();

    /* configure the vertex arrays */
    global_vertex_arrays.set_vertex_attrib ( 0, global_vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 0 * sizeof ( GLfloat ) );
    global_vertex_arrays.set_vertex_attrib ( 1, global_vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 3 * sizeof ( GLfloat ) );
    global_vertex_arrays.set_vertex_attrib ( 2, global_vertex_data, 3, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 6 * sizeof ( GLfloat ) );
    global_vertex_arrays.set_vertex_attrib ( 3, global_vertex_data, 4, GL_FLOAT, GL_FALSE, sizeof ( vertex ), 9 * sizeof ( GLfloat ) );
    for ( unsigned i = 0; i < GLH_MODEL_MAX_TEXTURE_STACK_SIZE; ++i )
        global_vertex_arrays.set_vertex_attrib ( 4 + i, global_vertex_data, 2, GL_FLOAT, GL_FALSE, sizeof ( vertex ), ( 13 + i * 2 ) * sizeof ( GLfloat ) );
    global_vertex_arrays.bind_ebo ( global_index_data );
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
        const float distance = modulus ( point - math::fvec3 { transform * math::fvec4 { _vertex.position, 1.0 } } );

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
 * transform: the current model transformation from all the previous nodes
 */
void glh::model::model::render_node ( const node& _node, const math::fmat4& transform ) const
{
    /* create transformation matrix */
    math::fmat4 trans = transform * _node.transform;

    /* first render the child nodes */
    for ( const node& child: _node.children ) render_node ( child, trans );

    /* set the model matrix, if no model matrix flag not set */
    if ( ~model_render_flags & render_flags::GLH_NO_MODEL_MATRIX ) 
        cached_model_matrix_uniform->model_matrix_uni.set_matrix ( trans );

    /* render meshes */
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
    /* don't draw if contains no faces */
    if ( model_import_flags & import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES )
    {
        if ( model_render_flags & render_flags::GLH_OPAQUE_MODE      ) { if ( _mesh.num_opaque_faces      == 0 ) return; } else
        if ( model_render_flags & render_flags::GLH_TRANSPARENT_MODE ) { if ( _mesh.num_transparent_faces == 0 ) return; } else
        if ( _mesh.num_faces == 0 ) return;
    } else 
    {
        if ( model_render_flags & render_flags::GLH_OPAQUE_MODE      ) { if ( _mesh.properties->opacity < 1.0 ) return; } else
        if ( model_render_flags & render_flags::GLH_TRANSPARENT_MODE ) { if ( _mesh.definitely_opaque         ) return; }
        if ( _mesh.num_faces == 0 ) return;
    }

    /* if face culling is on and material is two sided, disable face culling */
    const bool culling_active = core::renderer::face_culling_enabled ();
    if ( culling_active && _mesh.properties->two_sided ) core::renderer::disable_face_culling ();

    /* apply the material, if not disabled in flags */
    if ( ~model_render_flags & render_flags::GLH_NO_MATERIAL ) apply_material ( * _mesh.properties );

    /* draw elements */
    if ( model_import_flags & import_flags::GLH_CONFIGURE_GLOBAL_VERTEX_ARRAYS )
    {
        if ( model_import_flags & import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES && model_render_flags & render_flags::GLH_OPAQUE_MODE ) 
            core::renderer::draw_elements ( GL_TRIANGLES, _mesh.num_opaque_faces * 3, GL_UNSIGNED_INT, _mesh.global_start_of_opaque_faces ); else
        if ( model_import_flags & import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES && model_render_flags & render_flags::GLH_TRANSPARENT_MODE ) 
            core::renderer::draw_elements ( GL_TRIANGLES, _mesh.num_transparent_faces * 3, GL_UNSIGNED_INT, _mesh.global_start_of_transparent_faces );
        else core::renderer::draw_elements ( GL_TRIANGLES, _mesh.num_faces * 3, GL_UNSIGNED_INT, _mesh.global_start_of_faces );
    } else
    {
        _mesh.vertex_arrays.bind ();
        if ( model_import_flags & import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES && model_render_flags & render_flags::GLH_OPAQUE_MODE ) 
            core::renderer::draw_elements ( GL_TRIANGLES, _mesh.num_opaque_faces * 3, GL_UNSIGNED_INT, _mesh.start_of_opaque_faces ); else
        if ( model_import_flags & import_flags::GLH_SPLIT_MESHES_BY_ALPHA_VALUES && model_render_flags & render_flags::GLH_TRANSPARENT_MODE ) 
            core::renderer::draw_elements ( GL_TRIANGLES, _mesh.num_transparent_faces * 3, GL_UNSIGNED_INT, _mesh.start_of_transparent_faces );
        else core::renderer::draw_elements ( GL_TRIANGLES, _mesh.num_faces * 3, GL_UNSIGNED_INT, _mesh.start_of_faces );
        _mesh.vertex_arrays.unbind ();
    }

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
    apply_texture_stack 
    ( 
        _material.ambient_stack, 
        cached_material_uniforms->ambient_stack_size_uni, cached_material_uniforms->ambient_stack_base_color_uni,
        cached_material_uniforms->ambient_stack_levels_uni, cached_material_uniforms->ambient_stack_textures_uni 
    );
    apply_texture_stack 
    ( 
        _material.diffuse_stack,
        cached_material_uniforms->diffuse_stack_size_uni, cached_material_uniforms->diffuse_stack_base_color_uni,
        cached_material_uniforms->diffuse_stack_levels_uni,cached_material_uniforms->diffuse_stack_textures_uni 
    );
    apply_texture_stack 
    ( 
        _material.specular_stack, 
        cached_material_uniforms->specular_stack_size_uni, cached_material_uniforms->specular_stack_base_color_uni,
        cached_material_uniforms->specular_stack_levels_uni, cached_material_uniforms->specular_stack_textures_uni 
    );
    apply_texture_stack
    ( 
        _material.emission_stack, 
        cached_material_uniforms->emission_stack_size_uni, cached_material_uniforms->emission_stack_base_color_uni,
        cached_material_uniforms->emission_stack_levels_uni, cached_material_uniforms->emission_stack_textures_uni 
    );
    apply_texture_stack 
    ( 
        _material.normal_stack, 
        cached_material_uniforms->normal_stack_size_uni, cached_material_uniforms->normal_stack_base_color_uni,
        cached_material_uniforms->normal_stack_levels_uni, cached_material_uniforms->normal_stack_textures_uni 
    );

    /* set blending mode */
    cached_material_uniforms->blending_mode_uni.set_int ( _material.blending_mode );

    /* set shininess values */
    cached_material_uniforms->shininess_uni.set_float ( _material.shininess );
    cached_material_uniforms->shininess_strength_uni.set_float ( _material.shininess_strength );

    /* set opacity */
    cached_material_uniforms->opacity_uni.set_float ( _material.opacity );

    /* set definitely_opaque */
    cached_material_uniforms->definitely_opaque_uni.set_int ( _material.definitely_opaque );
}



/* apply_texture_stack
 *
 * apply a texture stack during mesh rendering
 * 
 * _texture_stack: the texture stack to apply
 * stack_size/base_color/levels/textures_uni: cached stack uniforms
 */
void glh::model::model::apply_texture_stack 
( 
    const texture_stack& _texture_stack, 
    core::uniform& stack_size_uni, core::uniform& stack_base_color_uni,
    core::struct_array_uniform& stack_levels_uni, core::uniform& stack_textures_uni 
) const
{
    /* set the base color */
    stack_base_color_uni.set_vector ( _texture_stack.base_color );

    /* set the stack size */
    stack_size_uni.set_int ( _texture_stack.stack_size );

    /* bind the texture array */
    stack_textures_uni.set_int ( _texture_stack.textures.bind_loop () );

    /* set up each level of the texture stack */
    for ( unsigned i = 0; i < _texture_stack.stack_size; ++i ) 
    {
        /* set uniforms for each stack level */
        stack_levels_uni.at ( i ).get_uniform ( "blend_operation" ).set_int ( _texture_stack.levels.at ( i ).blend_operation );
        stack_levels_uni.at ( i ).get_uniform ( "blend_strength" ).set_float ( _texture_stack.levels.at ( i ).blend_strength );
        stack_levels_uni.at ( i ).get_uniform ( "uvwsrc" ).set_int ( _texture_stack.levels.at ( i ).uvwsrc );
    }
}