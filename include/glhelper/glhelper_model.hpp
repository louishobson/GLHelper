/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_model.hpp
 * 
 * importing and managing models using assimp
 * the model class expects uniform data to be formatted in a specific way to function
 * the formats and structures expected to be found in the shader program are explained along with the C++ classes
 * although the names of the actual structure types are not of importance,
 * the variable names within the structs cannot be modified
 * although there are a lot of structs in this header, the only one which should be used is the model class
 * the rest are used as internals to the model class, hence are not useful as stand-alone structs
 * 
 * 
 * 
 * STRUCT GLH::MODEL::IMPORT_FLAGS
 * 
 * struct of static integers for import flags
 * best way to have scoped enum-style interface, while allowing int conversion
 * 
 * 
 * 
 * STRUCT GLH::MODEL::RENDER_FLAGS
 * 
 * struct of static integers for rendering flags 
 * best way to have scoped enum-style interface, while allowing int conversion
 * 
 * 
 * 
 * CLASS GLH::MODEL::MODEL
 * 
 * stores a model in a renderable format
 * the model is set up in the constructor and is immediately renderable after construction
 * however, in order to render a model, the render method requires two uniform values:
 * 
 * material_uni: a struct_uniform referring to a material_struct in the program (to set the material info)
 * model_uni: a model uniform referring to a mat4 in the program (for the model matrix)
 * 
 * the model matrix uniform is purely a mat4
 * the material struct uniform is explained below, and the format of vertex attributes under that
 * 
 * for information about implementing logic in shaders, see pseudocode at the bottom of: 
 * http://assimp.sourceforge.net/lib_html/materials.html
 * 
 * 
 * 
 * 
 * MATERIAL_STRUCT
 * 
 * struct material_struct
 * {
 *     texture_stack_struct ambient_stack;
 *     texture_stack_struct diffuse_stack;
 *     texture_stack_struct specular_stack;
 * 
 *     int blending_mode;
 * 
 *     float shininess;
 *     float shininess_strength;
 * 
 *     float opacity;
 * };
 * 
 * this structure is for a material
 * 
 * *_stack: the texture stacks for ambient, diffuse and specular texture maps (see below)
 * blending_mode: how to blend the computed fragment color with the previous color
 * shininess: the shininess value for the material
 * shininess_strength: a multiple for the specular contribution for a pixel's color
 * opacity: the opacity of the material (1 = opaque, 0 = transparent)
 * 
 * 
 * 
 * TEXTURE_STACK_STRUCT
 * 
 * struct texture_stack_struct
 * {
 *     vec3 base_color;
 *     int stack_size;
 *     texture_stack_level_struct levels [];
 *     sampler2DArray sampler;
 * };
 * 
 * this structure is for a texture stack
 * 
 * stack_size: the number of levels of the stack
 * base_color: the base color of which textures in the stack are blended with
 * levels: an array of levels of the texture stack, of user-specified size (see below)
 * sampler: 2d texture array storing the textures in the stack
 * 
 * 
 * 
 * TEXTURE_STACK_LEVEL_STRUCT
 *  
 * struct texture_stack_level_struct
 * {
 *     int blend_operation;
 *     float blend_strength;
 *     int uvwsrc;
 * };
 * 
 * this structure is for a level of a texture stack
 * 
 * blend_operation: one of aiTextureOP
 * blend_strength: a multiple for each level of the stack to be multiplied by
 * uvwsrc: which UV channel the texture coordinates should be taken from
 * u/vdiv: divisors by which to divide UV coordinates by
 *
 * 
 * 
 * VERTEX ATTTRIBUTES
 * 
 * 0  : vec3    : vertices
 * 1  : vec3    : normals
 * 2  : vec4[x] : vertex colors
 * 2+x: vec3[y] : UV channels of texture coordinates
 * 
 * the number of vertex colors and UV channels is defined by GLH_MODEL_MAX_COLOR_SETS and GLH_MODEL_MAX_UV_CHANNELS respectively
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::MODEL_EXCEPTION
 * 
 * thrown when an error occurs in one of the model methods (e.g. if the model entry file or cannot be found)
 *
 */



/* HEADER GUARD */
#ifndef GLHELPER_MODEL_HPP_INCLUDED
#define GLHELPER_MODEL_HPP_INCLUDED



/* MACROS */

/* GLH_MODEL_MAX_COLOR_SETS
 *
 * the maximum number of color sets a model can contain
 * defaults to 1
 */
#ifndef GLH_MODEL_MAX_COLOR_SETS
    #define GLH_MODEL_MAX_COLOR_SETS 1
#endif

/* GLH_MODEL_MAX_UV_CHANNELS
 *
 * the maximum number of uv channels a model can contain
 * defaults to 4
 */
#ifndef GLH_MODEL_MAX_UV_CHANNELS
    #define GLH_MODEL_MAX_UV_CHANNELS 4
#endif



/* INCLUDES */

/* include core headers */
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

/* include assimp headers */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_buffer.hpp */
#include <glhelper/glhelper_buffer.hpp>

/* include glhelper_texture.hpp */
#include <glhelper/glhelper_texture.hpp>

/* include glhelper_shader.hpp */
#include <glhelper/glhelper_shader.hpp>

/* include glhelper_render.hpp */
#include <glhelper/glhelper_render.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>

/* include glhelper_region.hpp */
#include <glhelper/glhelper_region.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace model
    {
        /* MODEL STRUCTURES */

        /* struct vertex
         *
         * store vertex data
         */
        struct vertex;

        /* struct texture_stack_level
         *
         * stores information about a level of a texture stack
         */
        struct texture_stack_level;

        /* struct texture_stack
         *
         * store a stack of textures
         */
        struct texture_stack;

        /* struct material
         *
         * store material data
         */
        struct material;

        /* struct face
         *
         * a face of a mesh
         */
        struct face;

        /* struct mesh
         *
         * a mesh of a model
         */
        struct mesh;

        /* struct node
         *
         * a node within a model
         * stores relationships between meshes
         */
        struct node;



        /* FLAGS */

        /* struct import_flags
         *
         * model import flags
         */
        struct import_flags;

        /* struct render_flags
         *
         * model rendering flags
         */
        struct render_flags;



        /* MODEL CLASS */

        /* class model
         *
         * a class for a model
         */
        class model;
    }

    namespace exception
    {
        /* class model_exception : exception
         *
         * exception relating to models
         */
        class model_exception;
    }
}



/* VERTEX DEFINITION */

/* struct vertex
 *
 * store vertex data
 */
struct glh::model::vertex
{
    /* the vertex position */
    math::fvec3 position;

    /* normal vector */
    math::fvec3 normal;

    /* multiple color sets */
    std::array<math::fvec4, GLH_MODEL_MAX_COLOR_SETS> colorsets;

    /* multiple uv channels of texture coords */
    std::array<math::fvec3, GLH_MODEL_MAX_UV_CHANNELS> texcoords;
};



/* TEXTURE_STACK_LEVEL DEFINITION */

/* struct texture_stack_level
 *
 * a stores information about a level of a texture stack
 */
struct glh::model::texture_stack_level
{
    /* blend operation */
    int blend_operation;

    /* blend strength */
    float blend_strength;

    /* uv channel the texture coords should be taken from */
    unsigned uvwsrc;

    /* index of the image */
    unsigned image_index;
};



/* TEXTURE_STACK DEFINITION */

/* struct texture_stack
 *
 * store a stack of textures
 */
 struct glh::model::texture_stack
 {
    /* the base color */
    math::fvec3 base_color;

    /* the size of the stack */
    unsigned stack_size;

    /* stack width and height */
    unsigned stack_width;
    unsigned stack_height;
    
    /* true if stack has an alpha component */
    bool stack_has_alpha;

    /* wrapping modes */
    int wrapping_u;
    int wrapping_v;

    /* array of texture references */
    std::vector<texture_stack_level> levels;
 
    /* 2d texture array representing the stack */
    core::texture2d_array textures;
 };



/* MATERIAL DEFINITION */

/* struct material
 *
 * store material data
 */
struct glh::model::material
{
    /* texture stacks */
    texture_stack ambient_stack;
    texture_stack diffuse_stack;
    texture_stack specular_stack;

    /* blend mode
     * 
     * how to blend when alpha values are being used
     */
    int blending_mode;

    /* shininess values */
    float shininess;
    float shininess_strength;

    /* opacity */
    float opacity;

    /* two sided
     * 
     * if true, face culling should not be used
     */
    bool two_sided;

    /* shading models
     *
     * if not given, and shininess > 0, use Phone
     * else use Gourad
     */
    int shading_model;

    /* definitely opaque
     *
     * a boolean which is true if the material is definitely opaque
     * defined by all textures being <4 channel and opacity equalling 1
     */
    bool definitely_opaque;
};




/* FACE DEFINITION */

/* struct face
 *
 * a face of a mesh
 */
struct glh::model::face
{
    /* the indices the face consists of */
    std::array<unsigned, 3> indices;
};



/* MESH DEFINITION */

/* struct mesh
 *
 * a mesh of a model
 */
struct glh::model::mesh
{
    /* the number of color sets the mesh contains for each vertex */
    unsigned num_color_sets;

    /* the number of uv channels the mesh consists of for each vertex */
    unsigned num_uv_channels;

    /* the number of vertices */
    unsigned num_vertices;

    /* the number of faces the mesh contains */
    unsigned num_faces;

    /* the vertices the mesh consists of */
    std::vector<vertex> vertices;

    /* index if the material associated with this mesh */
    unsigned properties_index;

    /* the material the mesh is made from */
    material * properties;

    /* an array of faces */
    std::vector<face> faces;
    


    /* definitely opaque
     *
     * if true, the mesh is definitely opaque
     * this is defined by the material being definitly opaque, and all vertex colors having an alpha of 1
     */
    bool definitely_opaque;



    /* the vbo the vertices are exported to */
    core::vbo vertex_data;

    /* the ebo the indices of the faces are exported to */
    core::ebo index_data;

    /* the vao controlling the vbo and the ebo */
    core::vao array_object;



    /* a spherical region encompassing the mesh */
    region::spherical_region<> mesh_region;
};



/* NODE DEFINITION */

/* struct node
 *
 * a node within a model
 * stores relationships between meshes
 */
struct glh::model::node
{
    /* the number of children the node contains */
    unsigned num_children;
    
    /* the number of meshes the node contains */
    unsigned num_meshes;

    /* parent node */
    node * parent;

    /* child nodes */
    std::vector<node> children;

    /* mesh indices */
    std::vector<unsigned> mesh_indices;

    /* meshes */
    std::vector<mesh *> meshes;

    /* transformation relative to parent's node */
    math::fmat4 transform;



    /* a spherical region encompassing the node, including the transformation matrix */
    region::spherical_region<> node_region;
};



/* IMPORT_FLAGS DEFINITION */

/* struct import_flags
 *
 * model import flags
 */
struct glh::model::import_flags
{
    /* CORE VALUES */

    /* dummy value */
    static const unsigned GLH_NONE = 0x0000;

    /* use sRGBA texture stacks
     * texture will have the internal format GL_SRGB_ALPHA and base colors will be gamma corrected
     */
    static const unsigned GLH_AMBIENT_SRGBA = 0x0001;
    static const unsigned GLH_DIFFUSE_SRGBA = 0x0002;
    static const unsigned GLH_SPECULAR_SRGBA = 0x0004;

    /* use sRGBA vertex colors
     * vertex colors will be risen to the power of 2.2
     */
    static const unsigned GLH_VERTEX_SRGBA = 0x0008;



    /* configure regions fast
     * if set, mesh regions will be configured as fast as possible
     * will likely be significant over estimation
     */
    static const unsigned GLH_CONFIGURE_REGIONS_FAST = 0x0010;

    /* configure regions with acceptable overestimation
     * this will override fast region configuration
     */
    static const unsigned GLH_CONFIGURE_REGIONS_ACCEPTABLE = 0x0020;

    /* configure regions accurately
     * this will override fast and acceptable region configuration
     * this may take considerably longer
     */
    static const unsigned GLH_CONFIGURE_REGIONS_ACCURATE = 0x0040;

    /* configure only root node region
     * this only applies when accurate regions are being used
     * no mesh or lower level nodes will have their regions calculated, just the root node
     */
    static const unsigned GLH_CONFIGURE_ONLY_ROOT_NODE_REGION = 0x0080;



    /* flip textures vertically
     *
     * if set, all imported textures will be flipped vertically
     */
    static const unsigned GLH_FLIP_V_TEXTURES = 0x0100;



    /* PRESET VALUES */
    
    /* preset for sRGBA visual texture stacks and vertex colors
     * these are the texture stacks that are likely to be used for coloring
     */
    static const unsigned GLH_VISUAL_SRGBA = GLH_AMBIENT_SRGBA | GLH_DIFFUSE_SRGBA | GLH_VERTEX_SRGBA;

};



/* RENDER_FLAGS DEFINITION */

/* struct render_flags
 *
 * model rendering flags
 */
struct glh::model::render_flags
{
    /* CORE VALUES */

    /* dummy value */
    static const unsigned GLH_NONE = 0x00;

    /* transparent mode 
     * meshes which are flagged as definitely opaque will not be drawn
     * this is useful for doing a separate opaque and transparent rendering calls
     */
    static const unsigned GLH_TRANSPARENT_MODE = 0x01;

    /* no matierials
     * no material-associated uniforms are set
     * this is useful for shadow mapping and such, where material is not important
     */
    static const unsigned GLH_NO_MATERIAL = 0x02;

};



/* MODEL DEFINITION */

/* class model
 *
 * a class for a model
 */
class glh::model::model
{
public:

    /* constructor
     *
     * import a model from a file
     * 
     * _directory: directory in which the model resides
     * _entry: the entry file to the model
     * _model_import_flags: import flags for the model (or default recommended)
     * _pps: post processing steps (or default recommended)
     */
    model ( const std::string& _directory, const std::string& _entry, const unsigned _model_import_flags = import_flags::GLH_NONE, const unsigned _pps = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Debone | aiProcess_OptimizeGraph );

    /* deleted zero parameter constructor */
    model () = delete;

    /* deleted copy constructor */
    model ( const model& other ) = delete;

    /* default move constructor */
    model ( model&& other ) = default;

    /* deleted copy assignment operator */
    model& operator= ( const model& other ) = delete;

    /* default destructor */
    ~model () = default;



    /* render
     *
     * render the model
     * 
     * material_uni: material uniform to cache and set the material properties to
     * model_uni: a 4x4 matrix uniform to cache and apply set the model transformations to
     * transform: the overall model transformation to apply (identity by default)
     * flags: rendering flags (none by default)
     */
    void render ( core::struct_uniform& material_uni, core::uniform& model_uni, const math::mat4& transform = math::identity<4> (), const unsigned flags = render_flags::GLH_NONE );
    void render ( const math::mat4& transform = math::identity<4> (), const unsigned flags = render_flags::GLH_NONE ) const;



    /* cache_uniforms
     *
     * cache all uniforms
     *
     * material_uni: the material uniform to cache
     * model_uni: model uniform to cache
     */
    void cache_uniforms ( core::struct_uniform& material_uni, core::uniform& model_uni );

    /* cache_material_uniforms
     * cache_model_uniform
     * 
     * cache material and model matrix uniforms separately
     */
    void cache_material_uniforms ( core::struct_uniform& material_uni );
    void cache_model_uniform ( core::uniform& model_uni );



    /* model_region
     *
     * get the region of the model based on a model matrix
     */
    region::spherical_region<> model_region ( const math::mat4& trans = math::identity<4> () ) { return trans * root_node.node_region; }



private:

    /* the directory the model is contained within */
    const std::string directory;

    /* the entry file for the model */
    const std::string entry;

    /* import flags */
    const unsigned model_import_flags;

    /* the post processing steps used to import the model */
    unsigned pps;

    /* the rendering flags currently being used */
    mutable unsigned model_render_flags;



    /* materials the model uses */
    std::vector<material> materials;

    /* images the model uses for its textures */
    std::vector<core::image> images;

    /* the meshes the model uses */
    std::vector<mesh> meshes;

    /* root node */
    node root_node;



    /* struct for cached material uniforms */
    struct cached_material_uniforms_struct
    {
        core::struct_uniform& material_uni;
        core::uniform& ambient_stack_size_uni;
        core::uniform& diffuse_stack_size_uni;
        core::uniform& specular_stack_size_uni;
        core::uniform& ambient_stack_base_color_uni;
        core::uniform& diffuse_stack_base_color_uni;
        core::uniform& specular_stack_base_color_uni;
        core::struct_array_uniform& ambient_stack_levels_uni; 
        core::struct_array_uniform& diffuse_stack_levels_uni; 
        core::struct_array_uniform& specular_stack_levels_uni;
        core::uniform& ambient_stack_textures_uni;
        core::uniform& diffuse_stack_textures_uni;
        core::uniform& specular_stack_textures_uni;
        core::uniform& blending_mode_uni;
        core::uniform& shininess_uni;
        core::uniform& shininess_strength_uni;
        core::uniform& opacity_uni;
    };

    /* struct for cached model matrix uniform */
    struct cached_model_uniform_struct
    {
        core::uniform& model_uni;
    };

    /* cached material uniforms */
    std::unique_ptr<cached_material_uniforms_struct> cached_material_uniforms;

    /* cached model matrix uniform */
    std::unique_ptr<cached_model_uniform_struct> cached_model_uniform;



    /* cast_vector
     *
     * cast assimp vectors to glh vectors
     *
     * vec: the assimp vector to cast
     * 
     * return: a glh vector
     */
    template<typename T> math::vector<2, T> cast_vector ( const aiVector2t<T>& vec ) { return math::vector<2, T> { vec.x, vec.y }; }
    template<typename T> math::vector<3, T> cast_vector ( const aiVector3t<T>& vec ) { return math::vector<3, T> { vec.x, vec.y, vec.z }; }
    math::fvec3 cast_vector ( const aiColor3D& vec ) { return math::fvec3 { vec.r, vec.g, vec.b }; }
    template<typename T> math::vector<4, T> cast_vector ( const aiColor4t<T>& vec ) { return math::vector<4, T> { vec.r, vec.g, vec.b, vec.a }; }

    /* cast_matrix
     *
     * cast assimp matrices to glh matrices
     * 
     * mat: the assimp matrix to cast
     * 
     * return: a glh matrix
     */
    template<typename T> math::fmat4 cast_matrix ( const aiMatrix4x4t<T>& mat )
    {
        math::fmat4 result;
        for ( unsigned i = 0; i < 4; ++i ) for ( unsigned j = 0; j < 4; ++j ) result.at ( i, j ) = mat [ i ][ j ];
        return result;
    }



    /* cast_wrapping
     *
     * cast assimp wrapping macros to OpenGL wrapping macros
     */
    int cast_wrapping ( const int wrap ) const
    { if ( wrap == aiTextureMapMode_Wrap ) return GL_REPEAT; if ( wrap == aiTextureMapMode_Clamp ) return GL_CLAMP_TO_BORDER;
      if ( wrap == aiTextureMapMode_Mirror ) return GL_MIRRORED_REPEAT; else return GL_REPEAT; }



    /* process_scene
     *
     * build from a scene object
     * 
     * aiscene: the scene object to build from
     */
    void process_scene ( const aiScene& aiscene );

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
    material& add_material ( material& _material, const aiMaterial& aimaterial );

    /* add_texture_stack
     *
     * take a texture stack and add it to the store
     * 
     * _texture_stack: the texture stack to configure
     * aimaterial: the material to get the texture stack from
     * aitexturetype: the type of texture to add
     * base_color/__bgtype/__bgidx: the AI_MATKEY macro for the base color
     * use_srgb: true if colors should be gamma corrected
     * 
     * return: the texture_stack just added
     */
    texture_stack& add_texture_stack ( texture_stack& _texture_stack, const aiMaterial& aimaterial, const aiTextureType aitexturetype, const char * base_color, const unsigned int __bgtype, const unsigned int __bgidx, const bool use_srgb );

    /* add_image
     *
     * load an image from a filepath
     * 
     * filepath: string for the filepath to the image
     * 
     * return: the index of the image in the global array
     */
    unsigned add_image ( const std::string& filepath );

    /* is_definitely_opaque
     *
     * determines if a material or mesh is definitely opaque
     * 
     * _material/_mesh: the material/mesh to check
     * 
     * return: boolean for if is definitely opaque
     */
    bool is_definitely_opaque ( const material& _material );
    bool is_definitely_opaque ( const mesh& _mesh );

    /* add_mesh
     *
     * add a mesh to a node
     *
     * _mesh: the mesh to configure
     * aimesh: the assimp mesh object to add
     * 
     * return: the mesh just added
     */
    mesh& add_mesh ( mesh& _mesh, const aiMesh& aimesh );

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
    face& add_face ( face& _face, mesh& _mesh, const aiFace& aiface );

    /* add_node
     *
     * recursively add nodes to the node tree
     * 
     * _node: the node to configure
     * ainode: the assimp node object to add
     * 
     * return: the node just added
     */
    node& add_node ( node& _node, const aiNode& ainode );



    /* mesh_max_min_components
     * node_max_min_components
     *
     * find the maximum and minimum xyz components of all of the vertices of the mesh/node
     * 
     * _mesh/_node: the mesh/node to find the max/min components for 
     * transform: transformation applied to all of the vertices
     * 
     * return: a pair of vec3s: the first if the max components, the second is the min components
     */
    std::pair<math::fvec3, math::fvec3> mesh_max_min_components ( const mesh& _mesh, const math::fmat4& transform = math::identity<4> () ) const;
    std::pair<math::fvec3, math::fvec3> node_max_min_components ( const node& _node, const math::fmat4& transform = math::identity<4> () ) const;

    /* mesh_furthest_distance
     * node_furthest_distance
     *
     * find the furthest distance from a point in a mesh/node
     * 
     * _mesh/_node: the mesh/node to find the furthest distance in
     * point: the point to find the furthest distance from
     * transform: the transformation to apply to all of the vertices (assumed to be applied to point)
     * 
     * return: the furthest distance from that point and a vertex
     */
    float mesh_furthest_distance ( const mesh& _mesh, const math::fvec3& point, const math::fmat4& transform = math::identity<4> () ) const;
    float node_furthest_distance ( const node& _node, const math::fvec3& point, const math::fmat4& transform = math::identity<4> () ) const;



    /* configure_mesh_region
     *
     * configure the region of a mesh
     * 
     * _mesh: the mesh to configure
     */
    void configure_mesh_region ( mesh& _mesh );

    /* configure_node_region
     *
     * configure the region of a node
     * child nodes must have already been processed
     *
     * _node: the node to configure
     */
    void configure_node_region ( node& _node );



    /* render_node
     *
     * render a node and all of its children
     * 
     * _node: the node to render
     * transform: the current model transformation from all the previous nodes
     */
    void render_node ( const node& _node, const math::fmat4& transform ) const;

    /* render_mesh
     *
     * render a mesh
     * 
     * _mesh: the mesh to render
     */
    void render_mesh ( const mesh& _mesh ) const;

    /* apply_material
     *
     * apply material uniforms during mesh rendering
     * 
     * _material: the material to apply
     */
    void apply_material ( const material& _material ) const;

    /* apply_texture_stack
     *
     * apply a texture stack during mesh rendering
     * 
     * _texture_stack: the texture stack to apply
     * stack_size/base_color/levels/textures_uni: cached stack uniforms
     */
    void apply_texture_stack ( const texture_stack& _texture_stack, core::uniform& stack_size_uni, core::uniform& stack_base_color_uni, core::struct_array_uniform& stack_levels_uni, core::uniform& stack_textures_uni ) const;

};



/* MODEL_EXCEPTION DEFINITION */

/* class model_exception : exception
 *
 * exception relating to models
 */
class glh::exception::model_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit model_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct model_exception with no descrption
     */
    model_exception () = default;

    /* default everything else and inherits what () function */

};



/* #ifndef GLHELPER_MODEL_HPP_INCLUDED */
#endif