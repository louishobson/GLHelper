/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_model.hpp
 * 
 * importing and managing models using assimp
 * 
 * 
 * 
 * the model class expects uniform data to be formatted in a specific way to function
 * 
 * the formats and structures expected to be found in the program are explained below
 * 
 * although the names of the actual structure types are not of importance,
 * the variable names within the struct cannot be modified
 * 
 * 
 * 
 * VERTEX ATTTRIBUTES
 * 
 * 0: vec3  : vertices
 * 1: vec3  : normals
 * 2: vec3[]: UV channels of texture coordinates
 *
 * how many UV channels you allow (size of the array at location 2) is up to the user
 * too many is not an issue, too few will be
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
 *     sampler2D texunit;
 * };
 * 
 * this structure is for a level of a texture stack
 * 
 * blend_operation: one of aiTextureOP
 * blend_strength: a multiple for each level of the stack to be multiplied by
 * uvwsrc: which UV channel the texture coordinates should be taken from
 * texunit: the sampler2D for the texture
 * 
 * 
 * 
 * TEXTURE_STACK_STRUCT
 * 
 * struct texture_stack_struct
 * {
 *     int stack_size;
 *     vec3 base_color;
 *     texture_stack_level_struct levels [];
 * };
 * 
 * this structure is for a texture stack
 * 
 * stack_size: the number of levels of the stack
 * base_color: the base color of which textures in the stack are blended with
 * levels: an array of levels of the texture stack, of user-specified size
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
 * *_stack: the texture stacks for ambient, diffuse and specular texture maps
 * blending_mode: how to blend the computed fragment color with the previous color
 * shininess: the shininess value for the material
 * shininess_strength: a multiple for the specular contribution for a pixel's color
 * opacity: the opacity of the material (1 = opaque, 0 = transparent)
 * 
 * 
 * 
 * UNIFORMS
 * 
 * in order to render a model, the render method requires three uniform values:
 * 
 * material_uni: a struct_uniform referring to a material_struct in the program (to set the material info)
 * model_uni: a normal uniform referring to a mat4 in the program (for the model matrix)
 * 
 * 
 * 
 * for information about implementing logic in shaders, see pseudocode at the bottom of: 
 * http://assimp.sourceforge.net/lib_html/materials.html
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_MODEL_HPP_INCLUDED
#define GLHELPER_MODEL_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <iostream>
#include <memory>
#include <vector>

/* include assimp headers */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>

/* include glhelper_buff.hpp */
#include <glhelper/glhelper_buff.hpp>

/* include glhelper_texture.hpp */
#include <glhelper/glhelper_texture.hpp>

/* include glhelper_render.hpp */
#include <glhelper/glhelper_render.hpp>

/* include glhelper_math.hpp */
#include <glhelper/glhelper_math.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

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
         * store a reference to a texture for use in a texture stack
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

        /* class model
         *
         * a class for a model
         */
        class model;

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
    math::vec3 position;

    /* normal vector */
    math::vec3 normal;

    /* multiple uv channels of texture coords */
    std::vector<math::vec3> texcoords;
};



/* TEXTURE_STACK_LEVEL DEFINITION */

/* struct texture_stack_level
 *
 * store a reference to a texture for use in a texture stack
 */
struct glh::model::texture_stack_level
{
    /* blend operation */
    int blend_operation;

    /* blend strength */
    double blend_strength;

    /* wrapping modes */
    int wrapping_u;
    int wrapping_v;

    /* uv channel the texture coords should be taken from */
    unsigned uvwsrc;

    /* index of the texture */
    unsigned index;

    /* texture reference */
    texture2d * texture;
};



/* TEXTURE_STACK DEFINITION */

/* struct texture_stack
 *
 * store a stack of textures
 */
 struct glh::model::texture_stack
 {
    /* the base color */
    math::vec3 base_color;

    /* array of texture references */
    std::vector<texture_stack_level> levels;
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
    double shininess;
    double shininess_strength;

    /* opacity */
    double opacity;

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
};




/* FACE DEFINITION */

/* struct face
 *
 * a face of a mesh
 */
struct glh::model::face
{
    /* the indices the face consists of */
    std::vector<unsigned> indices;

    /* pointers to vertices the face consists of */
    std::vector<vertex *> vertices;
};



/* MESH DEFINITION */

/* struct mesh
 *
 * a mesh of a model
 */
struct glh::model::mesh
{
    /* the vertices the mesh consists of */
    std::vector<vertex> vertices;

    /* the number of uv channels the mesh consists of for each vertex */
    unsigned num_uv_channels;

    /* index of the material the mesh is made from */
    unsigned properties_index;

    /* the material the mesh is made from */
    material * properties;

    /* an array of faces */
    std::vector<face> faces;



    /* the vbo the vertices are exported to */
    vbo vertex_data;

    /* the ebo the indices of the faces are exported to */
    ebo index_data;

    /* the vao controlling the vbo and the ebo */
    vao array_object;
};



/* NODE DEFINITION */

/* struct node
 *
 * a node within a model
 * stores relationships between meshes
 */
struct glh::model::node
{
    /* child nodes */
    std::vector<node> children;

    /* mesh indices */
    std::vector<unsigned> mesh_indices;

    /* meshes */
    std::vector<mesh *> meshes;

    /* transformation relative to parent's node */
    math::mat4 transform;
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
     * _pps: post processing steps (or default recommended)
     */
    model ( const std::string& _directory, const std::string& _entry, const unsigned _pps = 
    aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenUVCoords | aiProcess_TransformUVCoords | aiProcess_GenNormals | 
    aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes | aiProcess_Debone );

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
     * material: a struct uniform with the same members as glh::model::material
     *           the texture stacks should be arrays of structs containing members in glh::model::texture_reference
     * model_matrix: a 4x4 matrix uniform which arranges meshes to relative positions
     * transform: the overall model transformation to apply (identity by default)
     */
    void render ( struct_uniform& material_uni, uniform& model_matrix, const math::mat4 transform = math::identity<4> () );



private:

    /* the directory the model is contained within */
    const std::string directory;

    /* the entry file for the model */
    const std::string entry;

    /* the post processing steps used to import the model */
    const unsigned pps;



    /* materials the model uses */
    std::vector<material> materials;

    /* textures the model uses */
    std::vector<texture2d> textures;

    /* the meshes the model uses */
    std::vector<mesh> meshes;

    /* root node */
    node root_node;



    /* struct for cached material uniforms */
    struct cached_material_uniforms_struct
    {
        struct_uniform material_uni;
        uniform ambient_stack_size_uni;
        uniform diffuse_stack_size_uni;
        uniform specular_stack_size_uni;
        uniform ambient_stack_base_color_uni;
        uniform diffuse_stack_base_color_uni;
        uniform specular_stack_base_color_uni;
        array_uniform<struct_uniform> ambient_stack_levels_uni; 
        array_uniform<struct_uniform> diffuse_stack_levels_uni; 
        array_uniform<struct_uniform> specular_stack_levels_uni;
        uniform blending_mode_uni;
        uniform shininess_uni;
        uniform shininess_strength_uni;
        uniform opacity_uni;
    };

    /* cached uniforms */
    std::unique_ptr<cached_material_uniforms_struct> cached_material_uniforms;
    std::unique_ptr<uniform> cached_model_uniform;



    /* cast_vector
     *
     * cast assimp vectors to glh vectors
     *
     * vec: the assimp vector to cast
     * 
     * return: a glh vector
     */
    template<typename T> math::vec2 cast_vector ( const aiVector2t<T>& vec ) { return math::vec2 { vec.x, vec.y }; }
    template<typename T> math::vec3 cast_vector ( const aiVector3t<T>& vec ) { return math::vec3 { vec.x, vec.y, vec.z }; }
    math::vec3 cast_vector ( const aiColor3D& vec ) { return math::vec3 { vec.r, vec.g, vec.b }; }
    template<typename T> math::vec4 cast_vector ( const aiColor4t<T>& vec ) { return math::vec4 { vec.r, vec.g, vec.b, vec.a }; }

    /* cast_matrix
     *
     * cast assimp matrices to glh matrices
     * 
     * mat: the assimp matrix to cast
     * 
     * return: a glh matrix
     */
    template<typename T> math::mat4 cast_matrix ( const aiMatrix4x4t<T>& mat )
    {
        math::mat4 result;
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
     * location: the material to configure
     * aimaterial: the assimp material object to add
     * 
     * return: the material just added
     */
    material& add_material ( material& location, const aiMaterial& aimaterial );

    /* add_texture
     *
     * add a texture to a texture stack
     * 
     * location: the location of the level in the texture stack
     * aimaterial: the material the texture is being added from
     * index: the index of the texture
     * aitexturetype: the type of the stack
     * 
     * return: the texture stack level just added
     */
    texture_stack_level& add_texture ( texture_stack_level& location, const aiMaterial& aimaterial, const unsigned index, const aiTextureType aitexturetype );

    /* add_mesh
     *
     * add a mesh to a node
     *
     * location: the mesh to configure
     * aimesh: the assimp mesh object to add
     * 
     * return: the mesh just added
     */
    mesh& add_mesh ( mesh& location, const aiMesh& aimesh );

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
    face& add_face ( face& location, mesh& parent, const aiFace& aiface );

    /* configure_buffers
     *
     * configure the buffers of a mesh
     * 
     * location: the mesh to configure
     */
    void configure_buffers ( mesh& location );

    /* add_node
     *
     * recursively add nodes to the node tree
     * 
     * location: the node to configure
     * ainode: the assimp node object to add
     * 
     * return: the node just added
     */
    node& add_node ( node& location, const aiNode& ainode );

    /* render_node
     *
     * render a node and all of its children
     * 
     * location: the node to render
     * transform: the current model transformation from all the previous nodes
     */
    void render_node ( const node& location, const math::mat4& transform ) const;

    /* render_mesh
     *
     * render a mesh
     * 
     * location: the mesh to render
     */
    void render_mesh ( const mesh& location ) const;

};



/* MODEL_EXCEPTION DEFINITION */

/* class model_exception : exception
 *
 * exception relating to models
 */
class glh::model::model_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit model_exception ( const std::string& __what )
        : exception ( __what )
    {}

    /* default zero-parameter constructor
     *
     * construct model_exception with no descrption
     */
    explicit model_exception () = default;

    /* default everything else and inherits what () function */

};



/* #ifndef GLHELPER_MODEL_HPP_INCLUDED */
#endif