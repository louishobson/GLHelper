/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_composite_function.hpp
 * 
 * constructs for representing/using mathematical composite functions
 * 
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::MATH::COMPOSITE_FUNCTION
 * 
 * a representation of multiple functions bound together in a graph shape
 * the class takes a fingerprint as a template parameter in order to know the full extent of its graph at compile time
 * 
 * 
 * 
 * AN EXPLANATION OF FINGERPRINTS IN COMPOSITE FUNCTIONS
 * 
 * think of a composite function like a directed graph with two different types of node
 * one of these types, say node type F, is for a function
 *     directed arcs going into the node are parameters, the one leaving is the output
 * the other type, say node type P, is a for parameters
 *     these nodes have no input and one output
 *     the output of these nodes, if any, are the user-supplied inputs to the function
 *     from this point on, these user-supplied input will be called entry parameters:
 * 
 * ENTRY PARAMETERS: the user-supplied inputs to a composite function
 * 
 * these nodes and arcs between them are what define the composite function's layout and flow of inputs to outputs,
 * rather like a glorified function machine for multivariate functions
 * 
 * when this graph is drawn out, the position in 2d space is important
 * obviously the input parameters have a specific order
 * think of the hight of the parameter nodes being how their order of input is defined
 * 
 * 
 * 
 * the question is how to represent this graph structure in template form 
 * the representation of the graph in template form is called the fingerprint
 * 
 * FINGERPRINT: a unique type which describes the entirety of a composite function graph
 * 
 * I have implemented the following two template classes and their shortened aliases
 * 
 * template<class Rt, unsigned ...Ps> glh::meta::composite_function_node/cfn
 * template<class Rt> glh::meta::composite_parameter_node/cpn
 * 
 * the Rt type of each refers to the output type of that node
 * the values of Ps... define the parameters to a function, but why integers?
 * 
 * the whole fingerprint is implemented as an std::tuple<...> containing nodes in replacement of the '...'
 * the first node in the tuple must be a function node and is the final output of the composite function
 * from then on, the values of Ps... for the function node define the relative-index of the next node to use as an input
 * 
 * FINAL FUNCTION: the first node of a fingerprint, which must be a function node, and who's output will determine the output of the entire composite function
 * 
 * for example:
 * 
 * FIGURE (1)
 * 
 * std::tuple< cfn< int, 0, 1 >, cpn< int >, cpn< int > > 
 *                       |  |     |           |
 *                       +--|-----+           |
 *                          +-----------------+
 * 
 *         +-- int P
 *        /
 * int F < 
 *        \
 *         +-- int P
 *
 * the first function node returns an integer and takes two inputs at relative-indices 0 and 1
 * its first parameter comes from the node one index away - the adjacent parameter node
 * its second parameter comes from the node two indices away - the last node in the fingerprint, also a parameter node
 * for this example, there are therefore two entry parameters
 * here's another:
 * 
 * FIGURE (2)
 * 
 * std::tuple< cfn< int, 1, 2 >, cpn< int >, cfn< int, 1 >, cpn< int > >
 *                       |  |     |           |        |     |
 *                       +--|-----+           |        +-----+
 *                          +-----------------+
 *
 *         +-- int P
 *        /
 * int F < 
 *        \
 *         +-- int F --- int P
 * 
 * the first function node returns an integer and takes two inputs at relative-indices 0 and 1
 * its first parameter comes from the node one index away - the adjacent parameter node
 * its second parameter comes from the node two indices away - the penultimate node in the fingerprint, this time another function node
 * this function node returns an integer and takes only one input
 * this input comes from the node one index away - the final node in the fingerprint, now a parameter node
 * for this example, there are therefore still two entry parameters, but another function added into the mix
 * 
 * 
 * 
 * hopefully this is beginning to make sense
 * some terminology we added:
 * 
 * RELATIVE-INDEX: the index of another node in the fingerprint based on the position of another node (more specifically a function node)
 * 
 * two other types of index that are referred to are
 * 
 * PARAMETER-INDEX: the index of an entry parameter
 *                  equally, the nth parameter node which appears in the fingerprint
 * 
 * RAW-INDEX: the index of any node relative to the beginning of the fingerprint
 * 
 * just to clarify parameter indices, the last node in the FIGURE(2) has:
 *    a paramater-index of 1: it is the second entry parameter in the fingerprint
 *    a raw-index of 3: it is the fourth node in the fingerprint
 *    a relative-index of 1 from the perspective of the penultimate node
 * 
 * 
 * 
 * we have assumed through all of this that the all values of Ps... are positive
 * however, we can do more than assume this - we can guarantee it:
 *     firstly if a value of Ps... was zero, that would cause a loop in the tree since the relative 0th index of any node is itself
 *     this means that no function should ever end up with a value in Ps... which equals zero
 *     we can also guarantee that all of Ps... are never negative by how we order the nodes of the graph in the fingerprint - see below:
 * 
 * FIGURE (3)
 * 
 *        +-- F(1) --+
 *       /            \ 
 * F(0) <              > F(3) --- P(4)
 *       \            /
 *        +-- F(2) --+
 * 
 * the graph above diverges and then converges
 * the values next to each node determines the order in which they will be stored in the fingerprint
 * let's explain: (when a node is 'expanded', it is appended to the fingerprint)
 * 
 * first expand the final function node (F(0) in the diagram)
 * for each function node we expand, we look to expand the nodes it takes its inputs from one by one (e.g. F(0) takes F(1) and F(2) as inputs, so look to expand these next)
 * only if all of a node's output routes have been explored, do we actually expand this node
 * otherwise we move onto the next node or backtrace
 * 
 * to see this in practice for FIGURE (3):
 * 
 * F(0) is added first, since it is the final function
 * F(1) is added next, since it is F(0)'s first parameter and its output is only routed into F(0), which was just explored
 * F(3) is not added next, since it's output is routed to F(2) as well as F(1), and we have not yet explored F(2) yet
 * so we backtrace and put F(2) next, whose output is only routed into F(0), which is explored
 * now we put F(3) next, since although its output is routed to F(0) and F(1), we have already explored both of these nodes
 * we then add P(4), whose only output route is to F(3), which is explored
 * now we have ordered the whole tree
 * 
 * by ordering the nodes in this way, where multiple functions depend on the same node (like F(1) and F(2) both depending on F(3)), 
 * this multi-routed-output node always is expanded after all of those that depend on it (F(3) is expanded after both F(1) and F(2), rather than being expanded after F(1) and hence appearing before F(2))
 * this means that the relative indices are always positive
 * 
 * I'm not going to explain why this feature is useful, just believe me that it is and that it simplifies the code a lot
 * 
 * 
 * 
 * we can extract a lot of information about a fingerprint using the below metaprogramming template struct
 * 
 * template<class Fg> glh::meta::composite_fingerprint_info
 * 
 * to see the member types that it defines, see the description with the declaration
 * 
 * 
 * 
 * the next concept to understand is fingerprint substitution:
 * 
 * FINGERPRINT SUBSTITUTION: where an parameter node is replaced with another fingerprint
 * 
 * this forms the basis of binding:
 * 
 * COMPOSITE BINDING: when an entry parameter is replaced by a another function, be it a standalone function or another composite function
 *                    this creates a new composite function
 * 
 * the difference between these two words is that:
 *     binding refers to adding another function or functions onto the entry parameter of a composite function
 *     substitution refers to the modification of the fingerprint to represent this new change
 * see below for a simple example:
 * 
 * FIGURE (4.0)     
 * 
 * C: a composite function: 
 *        
 *     +-- P  
 *    /
 * F <
 *    \
 *     +-- P
 * 
 * N: normal, single-variable function: 
 * 
 * F -- P
 * 
 * N bound onto entry parameter index 1 of C:
 * 
 *     +-- P  
 *    /
 * F <
 *    \
 *     +-- F -- P
 *         ^
 *         where the fingerprint of N was substituted
 * 
 * the metaprogramming template struct below handles the fingerprint substitution
 * it is stupidly complicated, you are warned
 * 
 * template<class OgFg, class RepFg, unsigned Idx, unsigned ...Idxs> struct composite_fingerprint_substitution;
 * 
 * the two types are the original fingerprint and the fingerprint substitute respectively
 * Idx(s...) are the parameter-indices to substitute this replacement fingerprint
 * 
 * 
 * 
 * yes, that's right, more than one index can be supplied
 * this worked by first sorting Idx(s...) into descending order (all of the indices must also be unique)
 * then the largest of them is the parameter-index where RepFg is substituted
 * the rest of the parameter-indices are then 'rerouted'
 * this means that they are removed, and any function nodes which previously depended on it are changed to depend on the newly substituted part of the fingerprint
 * the descending order thing is done to ensure that what we said above about ordering the multi-routed-output nodes to be after all their dependant nodes remains true
 * 
 * 
 * 
 * this begs the question of the new order of entry parameters after substitution
 * let me explain:
 * 
 * FIGURE (5.0)
 * 
 * C1: a composite function:
 * 
 *     +-- P(1)
 *    /
 * F < --- P(2)
 *    \
 *     +-- P(3)
 * 
 * C2: another composite function
 * 
 *     +-- P(1)
 *    /
 * F < 
 *    \
 *     +-- P(2)
 * 
 * the numbers associated with each parameter node is the order of their input for that specific function
 * let's say we substite C2 onto the 3rd entry parameter of C1:
 * 
 * FIGURE (5.1)
 * 
 *     +-- P(1)
 *    /
 * F < --- P(2) 
 *    \          
 *     \         +-- P(3)
 *      \       /
 *       +-- F <
 *              \
 *               +-- P(4)
 * 
 * this new function now has four entry parameters in the order shown above - an order which should make logical sense
 * we replaced P(3) of C1 with a the fingerprint of C2, which has two entry parameters
 * it makes sense that P(1) and P(2) are left unchanged, and the two new entry parameters occur afterwards
 * 
 * a more complex example would be to substitute C2 onto the 1st AND 3rd entry parameter of C1
 * think back to how substitution was described to work
 * we first substitute the new fingerprint (C2) onto the highest index entry parameter supplied (3rd entry parameter, param-index 2):
 * 
 * FIGURE (5.2)
 * 
 * 
 *     +-- P(1)
 *    /
 * F < --- P(2) 
 *    \          
 *     \         +-- P(3)
 *      \       /
 *       +-- F <
 *              \
 *               +-- P(4)
 * 
 * okay, so that's the same as before
 * now we have to remove the 1st entry parameter and reroute any function nodes that depend on it to the newly inserted fingerprint
 * 
 * FIGURE (5.3)
 * 
 *     +--+
 *    /    \
 * F < -----\----- P(1) 
 *    \      \    
 *     \      \     +-- P(2)
 *      \      \   /
 *       +----- F <
 *                 \
 *                  +-- P(3)
 * 
 * what has happened here, is that P(1) in FIGURE (5.2) has been removed
 * the final function depended on P(1) as an input, so that input is rerouted to the newly inserted part of the fingerprint
 * the parameter nodes are ordered from the top down so that P(2) in FIGURE (5.2) becomes the new first entry parameter
 * 
 * understanding how the entry parameters are ordered after substitution is vital to knowing which entry parameter is which after a binding operation
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_COMPOSITE_FUNCTION_HPP_INCLUDED
#define GLHELPER_COMPOSITE_FUNCTION_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <tuple>
#include <type_traits>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_function.hpp */
#include <glhelper/glhelper_function.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace function
    {
        /* class composite_function
         *
         * class to handle a composite function
         * 
         * Rt: the function return type
         * Fg: the composite function fingerprint
         * Ps: the entry parameters to the function
         */
        template<class Rt, class Fg, class ...Ps> class composite_function;
    }

    namespace meta
    {
        /* struct nth_element(_t)
         *
         * get the nth element of a pack
         */
        template<unsigned Idx, class ...Es> struct nth_element;
        template<unsigned Idx, class ...Es> using nth_element_t = typename nth_element<Idx, Es...>::type;



        /* struct composite_function_node/cfn
         * struct composite_parameter_node/cpn
         * 
         * structures which make up a composite function fingerprint
         * 
         * Rt: the return type of the node
         * Ps: the indices of the nodes to use as parameters (for function nodes)
         */
        template<class Rt, unsigned... Ps> struct composite_function_node;
        template<class Rt, unsigned... Ps> using cfn = composite_function_node<Rt, Ps...>;
        template<class Rt> struct composite_parameter_node;
        template<class Rt> using cpn = composite_parameter_node<Rt>;

        /* struct is_composite_function_node/is_cfn
         * struct is_composite_parameter_node/is_cpn
         *
         * value member is true if the type supplied is a composite function/parameter node
         */
        template<class Nd> struct is_composite_function_node;
        template<class Nd> using is_cfn = is_composite_function_node<Nd>;
        template<class Nd> struct is_composite_parameter_node;
        template<class Nd> using is_cpn = is_composite_parameter_node<Nd>;

        /* struct composite_fingerprint_info
         *
         * gets information about a composite fingerprint, including:
         * 
         * num_param_nodes: no. of entry parameters
         * num_func_nodes: no. of function calls
         * return_type: the return type of the fingerprint
         * param_types: a tuple of entry parameters
         * param_cref_types: a tuple of const references to the entry parameters
         * func_types: a tuple of generic functions for each function node
         * func_uptr_types: a tuple of unique pointers to generic function types for each node
         * generic_type: the equivalent generic type
         * composite_type: the equivalent composite type
         */
        template<class Fg> struct composite_fingerprint_info;

        /* struct composite_fingerprint_info_minimal
         *
         * gets minimal information about a composite fingerprint, including:
         * 
         * num_param_nodes: no. of entry parameters
         * num_func_nodes: no. of function calls
         * return_type: the return type of the fingerprint
         * param_types: a tuple of entry parameters
         * func_types: a tuple of generic functions for each function node
         */
        template<class Fg> struct composite_fingerprint_info_minimal;

        /* struct composite_fingerprint_substitution(_t)
         *
         * takes two fingerprints and substitutes the second into the specified position of the first
         * 
         * OgFg: the original fingerprint
         * RepFg: the fingerprint to substitute
         * Idx(s): the entry parameter index/indices to substitute the fingeprint
         *  
         * if more than one index is supplied, the substitution will occur to the first index
         * the other entry parameters will be removed and linked to the newly substituted section of the fingerprint
         */
        template<class OgFg, class RepFg, unsigned Idx, unsigned ...Idxs> struct composite_fingerprint_substitution;
        template<class OgFg, class RepFg, unsigned Idx, unsigned ...Idxs> using composite_fingerprint_substitution_t = typename composite_fingerprint_substitution<OgFg, RepFg, Idx, Idxs...>::type;


    
        /* struct __max_of
         *
         * calculates the maximum of the integers supplied
         */
        template<unsigned ...Is> struct __max_of;

        /* struct __sum_of
         *
         * calculates the sum of the integers supplied
         */
        template<unsigned ...Is> struct __sum_of;

        /* __split_tuple
         *
         * function which takes a tuple, and splits it at the index specified
         * the index is included in the tail
         * the function returns a pair containing the head and tail
         */
        template<unsigned Idx, class ...Og, class ...Hs, class ...Ts, std::enable_if_t<sizeof...( Hs ) + sizeof...( Ts ) != sizeof...( Og )>..., std::enable_if_t<( sizeof...( Hs ) <  Idx )>...> auto 
        __split_tuple ( const std::tuple<Og...>& orig, const std::tuple<Hs...>& head = std::tuple<> {}, const std::tuple<Ts...>& tail = std::tuple<> {} );
        template<unsigned Idx, class ...Og, class ...Hs, class ...Ts, std::enable_if_t<sizeof...( Hs ) + sizeof...( Ts ) != sizeof...( Og )>..., std::enable_if_t<( sizeof...( Hs ) >= Idx )>...> auto 
        __split_tuple ( const std::tuple<Og...>& orig, const std::tuple<Hs...>& head = std::tuple<> {}, const std::tuple<Ts...>& tail = std::tuple<> {} );
        template<unsigned Idx, class ...Og, class ...Hs, class ...Ts, std::enable_if_t<sizeof...( Hs ) + sizeof...( Ts ) == sizeof...( Og )>...> auto 
        __split_tuple ( const std::tuple<Og...>& orig, const std::tuple<Hs...>& head = std::tuple<> {}, const std::tuple<Ts...>& tail = std::tuple<> {} );

        /* __insert_tuple
         * 
         * function which inserts a tuple at an index of another tuple
         * the index supplied is included after the tail
         * the new tuple is the returned
         */
        template<unsigned Idx, class ...Og, class ...Oth> auto __insert_tuple ( const std::tuple<Og...>& orig, const std::tuple<Oth...>& insert );

    }
}



/* NTH_ELEMENT DEFINITION */

/* struct nth_element
 *
 * get the nth element of a pack
 */
template<unsigned Idx, class E, class ...Es> struct glh::meta::nth_element<Idx, E, Es...> : nth_element<Idx - 1, Es...> {};
template<class E, class ...Es> struct glh::meta::nth_element<0, E, Es...> { typedef E type; };
template<unsigned Idx> struct glh::meta::nth_element<Idx> {};



/* COMPOSITE_FUNCTION/PARAMETER_NODE DEFINITION */

/* struct composite_function_node
 * struct composite_parameter_node
 * 
 * structures which make up a composite function fingerprint
 * 
 * Rt: the return type of the node
 * Ps: the indices of the nodes to use as parameters (for function nodes)
 */
template<class Rt, unsigned ...Ps> struct glh::meta::composite_function_node
{
    /* return type of the node and integer sequence parameters */
    typedef Rt return_type;
    typedef std::integer_sequence<unsigned, Ps...> params;
};
template<class Rt> struct glh::meta::composite_parameter_node
{
    /* return type of the node */
    typedef Rt return_type;
};



/* IS_COMPOSITE_FUNCTION/PARAMETER_NODE DEFINITION */

/* struct is_composite_function_node/is_cfn
 * struct is_composite_parameter_node/is_cpn
 *
 * value member is true if the type supplied is a composite function/parameter node
 */
template<class Nd> struct glh::meta::is_composite_function_node : std::false_type {};
template<class Rt, unsigned ...Ps> struct glh::meta::is_composite_function_node<glh::meta::cfn<Rt, Ps...>> : std::true_type {};
template<class Nd> struct glh::meta::is_composite_parameter_node : std::false_type {};
template<class Rt> struct glh::meta::is_composite_parameter_node<glh::meta::cpn<Rt>> : std::true_type {};



/* COMPOSITE_FINGERPRINT_INFO DEFINITION */

/* struct composite_fingerprint_info
*
* gets information about a composite fingerprint, including:
* 
* num_entry_params: no. of entry parameters
* num_func_calls: no. of function calls
* return_type: the return type of the fingerprint
* entry_param_types: a tuple of entry parameters
* functions_types: a tuple of generic functions for each function node
* generic_type: the equivalent generic type
* composite_type: the equivalent composite type
*/
/* if first first node, which must be a function node */
template<class Fg> struct glh::meta::composite_fingerprint_info
    : composite_fingerprint_info_minimal<Fg>
{
    /* __create_from_param_types
     *
     * takes return_type param_types from this node and creates param_cref_types, generic_type and composite_type for this node
     */
    template<class Rt, class Tp> struct __create_from_param_types;
    template<class Rt, class ...Ts> struct __create_from_param_types<Rt, std::tuple<Ts...>>
    {
        /* change Ts to const references */
        typedef std::tuple<const Ts&...> param_cref_types;
        /* use as parameters to generic function, with Rt for this node as the return type */
        typedef function::generic_function<Rt, Ts...> generic_type;
        /* same as above, except also include the whole fingerprint to create a composite function */
        typedef function::composite_function<Rt, Fg, Ts...> composite_type;
    };

    /* __create_from_func_types
     *
     * takes func_types from this node and creates func_cref_types for this node
     */
    template<class Tp> struct __create_from_func_types;
    template<class ...Ts> struct __create_from_func_types<std::tuple<Ts...>>
    {
        /* simply wrap function types in unique_ptr's */
        typedef std::tuple<std::unique_ptr<Ts>...> func_uptr_types;
    };

    /* using base class' types */
    using typename composite_fingerprint_info_minimal<Fg>::return_type;
    using typename composite_fingerprint_info_minimal<Fg>::param_types;
    using typename composite_fingerprint_info_minimal<Fg>::func_types;

    /* set param_cref_types, generic_type and composite_type using __create_from_param_types */
    typedef __create_from_param_types<return_type, param_types> __from_param_types;
    typedef typename __from_param_types::param_cref_types param_cref_types;
    typedef typename __from_param_types::generic_type generic_type;
    typedef typename __from_param_types::composite_type composite_type;

    /* set func_uptr_types using __create_from_func_types */
    typedef typename __create_from_func_types<func_types>::func_uptr_types func_uptr_types;
};



/* COMPOSITE_FINGERPRINT_INFO_MINIMAL DEFINITION */

/* struct composite_fingerprint_info_minimal
 *
 * gets minimal information about a composite fingerprint, including:
 * 
 * num_param_nodes: no. of entry parameters
 * num_func_nodes: no. of function calls
 * return_type: the return type of the fingerprint
 * param_types: a tuple of entry parameters
 * func_types: a tuple of generic functions for each function node
 */
/* if function node */
template<class Rt, unsigned ...Ps, class ...Nds> struct glh::meta::composite_fingerprint_info_minimal<std::tuple<glh::meta::cfn<Rt, Ps...>, Nds...>>
{
    /* __create_func_types
     *
     * takes func_types from the next node and creates the func_types for this node
     */
    template<class Tp> struct __create_func_types;
    template<class ...Ts> struct __create_func_types<std::tuple<Ts...>>
    {
        /* add a generic_function type to the front, where:
         * the return type is the return type for this node (ItRt)
         * the parameters are found by getting the return types from the parameter indices of this node (ItPs)
         */
        typedef std::tuple<function::generic_function<Rt, typename nth_element_t<Ps - 1, Nds...>::return_type...>, Ts...> type;
    };

    /* get fingerprint info of next node */
    typedef composite_fingerprint_info_minimal<std::tuple<Nds...>> __next_fg_info;

    /* keep num_param_nodes the same and increase num_func_nodes by one */
    static const unsigned num_param_nodes = __next_fg_info::num_param_nodes;
    static const unsigned num_func_nodes = __next_fg_info::num_func_nodes + 1;

    /* set return type to Rt */
    typedef Rt return_type;

    /* keep param_types the same and set func_types using __create_func_types */
    typedef typename __next_fg_info::param_types param_types;
    typedef typename __create_func_types<typename __next_fg_info::func_types>::type func_types;
};

/* if parameter node */
template<class Rt, class ...Nds> struct glh::meta::composite_fingerprint_info_minimal<std::tuple<glh::meta::cpn<Rt>, Nds...>>
{
    /* __create_param_types
     *
     * takes param_types from the next node ands create the param_types for this node
     */
    template<class Tp> struct __create_param_types;
    template<class ...Ts> struct __create_param_types<std::tuple<Ts...>>
    {
        /* simply add return type for this node (ItRt) to the front */
        typedef std::tuple<Rt, Ts...> type;
    };

    /* get fingerprint info of next node */
    typedef composite_fingerprint_info_minimal<std::tuple<Nds...>> __next_fg_info;

    /* increase num_param_nodes by one and keep num_func_nodes the same */
    static const unsigned num_param_nodes = __next_fg_info::num_param_nodes + 1;
    static const unsigned num_func_nodes = __next_fg_info::num_func_nodes;

    /* set return type to Rt */
    typedef Rt return_type;

    /* set param_types using __create_param_types and keep func_types the same */
    typedef typename __create_param_types<typename __next_fg_info::param_types>::type param_types;
    typedef typename __next_fg_info::func_types func_types;
};

/* if end of nodes */
template<> struct glh::meta::composite_fingerprint_info_minimal<std::tuple<>>
{
    /* set num_param_nodes and num_func_nodes to 0 */
    static const unsigned num_param_nodes = 0;
    static const unsigned num_func_nodes = 0;

    /* set return type to void */
    typedef void return_type;

    /* set param_types and func_types to empty tuples */
    typedef std::tuple<> param_types;
    typedef std::tuple<> func_types;
};



/* COMPOSITE_FINGERPRINT_SUBSTITUTION DEFINITION */

/* struct composite_fingerprint_substitution
 *
 * takes two fingerprints and substitutes the second into the specified position of the first
 * 
 * OgFg: the original fingerprint
 * RepFg: the fingerprint to substitute
 * Idx(s): the entry parameter index/indices to substitute the fingeprint
 *  
 * if more than one index is supplied, the substitution will occur to the first index
 * the other entry parameters will be removed and linked to the newly substituted section of the fingerprint
 */
template<class OgFg, class RepFg, unsigned Idx, unsigned ...Idxs> struct glh::meta::composite_fingerprint_substitution
{
    /* __size
     *
     * gets the size of a fingerprint
     */
    template<class Fg> struct __size;
    template<class ...Nds> struct __size<std::tuple<Nds...>>
        : std::integral_constant<unsigned, sizeof...( Nds )> {};



    /* __concatonate
     *
     * takes two fingerprints and concatonates their nodes end to end
     */
    template<class Fg0, class Fg1> struct __concatonate;
    template<class ...Nd0s, class ...Nd1s> struct __concatonate<std::tuple<Nd0s...>, std::tuple<Nd1s...>>
        { typedef std::tuple<Nd0s..., Nd1s...> type; };

    

    /* __descending_order
     *
     * takes a set of indices as an integer sequence and puts them into descending order using a bubble sort
     * also asserts that no indices are the same
     * the member 'type' will be an integer sequence of the ordered indices
     * 
     * ISq0: integer sequence to order
     * ISq1/Chg: automatically set
     */
    template<class ISq0, class ISq1 = std::integer_sequence<unsigned>, bool Chg = false> struct __descending_order;

    /* if not end of list */
    template<unsigned ISq0Idx0, unsigned ISq0Idx1, unsigned ...ISq0Idxs, unsigned ...ISq1Idxs, bool Chg> 
    struct __descending_order<std::integer_sequence<unsigned, ISq0Idx0, ISq0Idx1, ISq0Idxs...>, std::integer_sequence<unsigned, ISq1Idxs...>, Chg>
        : __descending_order<
            /* if ISq0Idx0 < ISq0Idx1, swap around ISq0Idx0 and ISq0Idx1 to create ISq0 */
            std::integer_sequence<unsigned, ( ISq0Idx0 < ISq0Idx1 ? ISq0Idx0 : ISq0Idx1 ), ISq0Idxs...>,
            /* if ISq0Idx0 < ISq0Idx1, add ISq0Idx1 on to end of ISq1 */
            std::integer_sequence<unsigned, ISq1Idxs..., ( ISq0Idx0 < ISq0Idx1 ? ISq0Idx1 : ISq0Idx0 )>,
            /* set change */
            ( Chg || ISq0Idx0 < ISq0Idx1 )
        /* static assert that ISq0Idx0 and ISq0Idx1 are unique */
        > { static_assert ( ISq0Idx0 != ISq0Idx1, "cannot apply composite fingerprint substitution on the same param-index twice in one go" ); };

    /* if end of list and a swap took place */
    template<unsigned ISq0Idx, unsigned ...ISq1Idxs> struct __descending_order<std::integer_sequence<unsigned, ISq0Idx>, std::integer_sequence<unsigned, ISq1Idxs...>, true>
        : __descending_order<
            /* repeat the sort, with the pass that just occured */
            std::integer_sequence<unsigned, ISq1Idxs..., ISq0Idx>
        > {};

    /* if end of list and no swaps took place */
    template<unsigned ISq0Idx, unsigned ...ISq1Idxs> struct __descending_order<std::integer_sequence<unsigned, ISq0Idx>, std::integer_sequence<unsigned, ISq1Idxs...>, false>
    {
        /* set final type */
        typedef std::integer_sequence<unsigned, ISq1Idxs..., ISq0Idx> type;
    };



    /* __find
     *
     * changes a param-index to a raw-index
     * 
     * Fg: the fingerprint to seach
     * SchIdx: the param-index
     */
    template<class Fg, unsigned SchIdx> struct __find;

    /* if function node, move on to the next node */
    template<class Rt, unsigned ...Ps, class ...Nds, unsigned SchIdx> struct __find<std::tuple<cfn<Rt, Ps...>, Nds...>, SchIdx>
        : std::integral_constant<unsigned, __find<std::tuple<Nds...>, SchIdx>::value + 1> {};

    /* if parameter node, but not the node we are looking for, move onto the next node with SchIdx decreased by one */
    template<class Rt, class ...Nds, unsigned SchIdx> struct __find<std::tuple<cpn<Rt>, Nds...>, SchIdx>
        : std::integral_constant<unsigned, __find<std::tuple<Nds...>, SchIdx - 1>::value + 1> {};

    /* if parameter node we are looking for, set value to 0
     * the stack of inheritance will then unwind, adding one to the value each time
     * this will leave the top most value to be the raw-index of the parameter node
     */
    template<class Rt, class ...Nds> struct __find<std::tuple<cpn<Rt>, Nds...>, 0>
        : std::integral_constant<unsigned, 0> {};

    /* if SchIdx is out of range, produce a more readable static_assert error
     * the reason for SchIdx + 1 is that SchIdx >= 0, so SchIdx + 1 >= 1, so !( SchIdx ) is always false
     * however, we can't just write false, since the compiler will pick that up even if this specialisation is never instantiated
     * in other words, the static assert condition has to be dependant
     */
    template<unsigned SchIdx> struct __find<std::tuple<>, SchIdx>
        { static_assert ( !( SchIdx + 1 ), "cannot apply composite fingerprint substitution on an out of range param-index" ); };



    /* __replace
     *
     * replace the parameter node at a raw-index with another fingerprint
     * to reiterate, the node at the raw-index must be a parameter node
     * all the parameter indices of function nodes will also be adjusted accordingly
     * 
     * _OgFg: the original fingerprint to search through
     * _RepFg: the fingerprint to insert (which must not be empty)
     * SchIdx: the raw-index of the parameter node to relace
     */
    template<class _OgFg, class _RepFg, unsigned SchIdx> struct __replace;

    /* if function node */
    template<class Rt, unsigned ...Ps, class ...Nds, class _RepFg, unsigned SchIdx> struct __replace<std::tuple<cfn<Rt, Ps...>, Nds...>, _RepFg, SchIdx>
        /* concatonate... */
        : __concatonate<
            /* if any of the parameters refer to a node with index greater than SchIdx,
             * increase their value by one less than the size of the fingerprint being inserted
             * this means that after insertion, the parameter still refers to the same node
             */
            std::tuple<cfn<Rt, ( Ps > SchIdx ? Ps + __size<_RepFg>::value - 1 : Ps )...>>,
            /* continue replacing, reducing SchIdx by one */
            typename __replace<std::tuple<Nds...>, _RepFg, SchIdx - 1>::type
        > {};

    /* if parameter node, but not the parameter node we are looking to replace */
    template<class Rt, class ...Nds, class _RepFg, unsigned SchIdx> struct __replace<std::tuple<cpn<Rt>, Nds...>, _RepFg, SchIdx>
        /* concatonate... */
        : __concatonate<
            /* the parameter node just encountered is unchanged since SchIdx > 0  */
            std::tuple<cpn<Rt>>, 
            /* continue replacing, reducing SchIdx by one */
            typename __replace<std::tuple<Nds...>, _RepFg, SchIdx - 1>::type
        > {};

    /* if parameter node we are looking to replace */
    template<class OgRt, class ...OgNds, class ...RepNds> struct __replace<std::tuple<cpn<OgRt>, OgNds...>, std::tuple<RepNds...>, 0>
    {
        /* concatonate the replacement fingerprint and the rest of the original fingerprint, unchanged */
        typedef std::tuple<RepNds..., OgNds...> type;
    };



    /* __reroute
     *
     * removes the paramater node at a raw-index and reroutes any index referencing it to a new raw-index
     * to reiterate, the node at the raw-index must be a parameter node
     * all the parameter indices of function nodes will also be adjusted accordingly
     * 
     * _OgFg: the original fingerprint to search through
     * RmIdx: the raw-index to remove
     * NewIdx: the new raw-index to route all referencing indices to
     */
    template<class _OgFg, unsigned RmIdx, unsigned NewIdx> struct __reroute;

    /* if function node */
    template<class Rt, unsigned ...Ps, class ...Nds, unsigned RmIdx, unsigned NewIdx> struct __reroute<std::tuple<cfn<Rt, Ps...>, Nds...>, RmIdx, NewIdx>
        /* concatonate... */
        : __concatonate<
            /* if any of the parameters refer to a node with index equal to RmIdx, replace the index with NewIdx minus one, else
             * if any of the parameters refer to a node with index greater than RmIdx, decrease its value by one
             */
            std::tuple<cfn<Rt, ( Ps == RmIdx ? NewIdx - 1 : ( Ps > RmIdx ? Ps - 1 : Ps ) )...>>,
            /* continue rerouting, reducing RmIdx and NewIdx by one */
            typename __reroute<std::tuple<Nds...>, RmIdx - 1, NewIdx - 1>::type
        > {};

    /* if parameter node */
    template<class Rt, class ...Nds, unsigned RmIdx, unsigned NewIdx> struct __reroute<std::tuple<cpn<Rt>, Nds...>, RmIdx, NewIdx>
        /* concatonate... */
        : __concatonate<
            /* the parameter node just encountered is left unchanged */
            std::tuple<cpn<Rt>>,
            /* continue rerouting, reducing RmIdx and NewIdx by one */
            typename __reroute<std::tuple<Nds...>, RmIdx - 1, NewIdx - 1>::type
        > {};

    /* if node we are looking to remove */
    template<class Rt, class ...Nds, unsigned NewIdx> struct __reroute<std::tuple<cpn<Rt>, Nds...>, 0, NewIdx>
    {
        /* set type to the remaining nodes */
        typedef std::tuple<Nds...> type;
    };


  
    /* __substitute_ordered
     *
     * takes an original fingerprint, a fingerprint to substitute and a pre-ordered list of param-indices, and performs the substitution
     * 
     * _OgFg: the original fingerprint
     * _RepFg: the replacement fingerprint to substitute
     * _Idx(s): the pre-ordered param-indices to substitute
     */
    template<class _OgFg, class _RepFg, unsigned _Idx, unsigned..._Idxs> struct __substitute_ordered
    {
        /* get the raw-index of the largest param-index
         * this is the raw-index of the parameter node that_RepFg will be substituted into
         */
        static const unsigned __substitution_index = __find<_OgFg, _Idx>::value;

        /* replace this node with _RepFg */
        typedef typename __replace<_OgFg, _RepFg, __substitution_index>::type __substituted_fingerprint;

        /* __reroute_remaining_indices
         *
         * reroute the remaining indices to __substitution_index
         * 
         * Fg: the fingerprint to reroute the indices
         * SubIdx: the raw-index that _RepFg was substituted for
         * __Idxs: the param-indices to reroute
         */
        /* if all indices have been rerouted */
        template<class Fg, unsigned SubIdx, unsigned ...__Idxs> struct __reroute_remaining_indices
        {
            /* set type to Fg, since all indices have been rerouted */
            typedef Fg type;
        };
        /* if there are indices left to reroute */
        template<class Fg, unsigned SubIdx, unsigned __Idx, unsigned ...__Idxs> struct __reroute_remaining_indices<Fg, SubIdx, __Idx, __Idxs...>
            /* find the raw-index and reroute it to SubIdx, then
             * supply the outcome back to __reroute_remaining_indices, decreasing SubIdx by one
             * this is done, since rerouting removed a parameter index, so the substitution index is now one less
             */
            : __reroute_remaining_indices<typename __reroute<Fg, __find<Fg, __Idx>::value, SubIdx>::type, SubIdx - 1, __Idxs...> {};

        /* set type using __reroute_remaining_indices on __substituted_fingerprint */
        typedef typename __reroute_remaining_indices<__substituted_fingerprint, __substitution_index, _Idxs...>::type type;
    };



    /* __substitute
     *
     * takes an original fingerprint, a fingerprint to substitute and an unordered list of param-indices, and performs the substitution
     * 
     * _OgFg: the original fingerprint
     * _RepFg: the replacement fingerprint to substitute
     * _Idxs: the unordered param-indices to substitute
     */
    template<class _OgFg, class _RepFg, unsigned ..._Idxs> struct __substitute
    {
        /* __extract_indices_and_substitute
         * extracts the param-indices from an integer sequence and applies them to __substitute_ordered
         * 
         * ISq: the integer sequence to extract the indices from
         */
        template<class ISq> struct __extract_indices_and_substitute;
        template<unsigned ...__Idxs> struct __extract_indices_and_substitute<std::integer_sequence<unsigned, __Idxs...>>
            : __substitute_ordered<_OgFg, _RepFg, __Idxs...> {};

        /* set type by:
         * ordering _Idxs using __descending_order, then
         * using __extract_indices_and_substitute to extract the ordered indices and apply them to __substitute_ordered
         */
        typedef typename __extract_indices_and_substitute<typename __descending_order<std::integer_sequence<unsigned, _Idxs...>>::type>::type type;
    };



    /* set type using __substitute */
    typedef typename __substitute<OgFg, RepFg, Idx, Idxs...>::type type;
};



/* __MAX_OF DEFINITION */

/* struct __max_of
 *
 * calculates the maximum of the integers supplied
 */
template<> struct glh::meta::__max_of<> 
    : std::integral_constant<unsigned, 0> {};
template<unsigned I, unsigned ...Is> struct glh::meta::__max_of<I, Is...> 
    : std::integral_constant<unsigned, ( I > __max_of<Is...>::value ? I : __max_of<Is...>::value )> {};



/* __SUM_OF DEFINITION */

/* struct __sum_of
*
* calculates the sum of the integers supplied
*/
template<> struct glh::meta::__sum_of<>
    : std::integral_constant<unsigned, 0> {};
template<unsigned I, unsigned ...Is> struct glh::meta::__sum_of<I, Is...> 
    : std::integral_constant<unsigned, I + __sum_of<Is...>::value> {};



/* __SPLIT_TUPLE IMPLEMENTATION */

/* __split_tuple
 *
 * function which takes a tuple, and splits it at the index specified
 * the index is included in the TAIL
 * the function returns a pair containing the head and tail
 */
template<unsigned Idx, class ...Og, class ...Hs, class ...Ts, std::enable_if_t<sizeof...( Hs ) + sizeof...( Ts ) != sizeof...( Og )>..., std::enable_if_t<( sizeof...( Hs ) <  Idx )>...> inline auto 
glh::meta::__split_tuple ( const std::tuple<Og...>& orig, const std::tuple<Hs...>& head, const std::tuple<Ts...>& tail )
    { return __split_tuple<Idx> ( orig, std::tuple_cat ( head, std::tuple<typename std::tuple_element<sizeof...( Hs ) + sizeof...( Ts ), std::tuple<Og...>>::type> 
        { std::get<sizeof...( Hs ) + sizeof...( Ts )> ( orig ) } ), tail ); }

template<unsigned Idx, class ...Og, class ...Hs, class ...Ts, std::enable_if_t<sizeof...( Hs ) + sizeof...( Ts ) != sizeof...( Og )>..., std::enable_if_t<( sizeof...( Hs ) >= Idx )>...> inline auto 
glh::meta::__split_tuple ( const std::tuple<Og...>& orig, const std::tuple<Hs...>& head, const std::tuple<Ts...>& tail )
    { return __split_tuple<Idx> ( orig, head, std::tuple_cat ( tail, std::tuple<typename std::tuple_element<sizeof...( Hs ) + sizeof...( Ts ), std::tuple<Og...>>::type> 
        { std::get<sizeof...( Hs ) + sizeof...( Ts )> ( orig ) } ) ); }

template<unsigned Idx, class ...Og, class ...Hs, class ...Ts, std::enable_if_t<sizeof...( Hs ) + sizeof...( Ts ) == sizeof...( Og )>...> inline auto 
glh::meta::__split_tuple ( const std::tuple<Og...>& orig, const std::tuple<Hs...>& head, const std::tuple<Ts...>& tail )
    { return std::make_pair ( head, tail ); }



/* __INSERT_TUPLE IMPLEMENTATION */

/* __insert_tuple
 * 
 * function which inserts a tuple at an index of another tuple
 * the index supplied is included after the tail
 * the new tuple is the returned
 */
template<unsigned Idx, class ...Og, class ...Oth> inline auto glh::meta::__insert_tuple ( const std::tuple<Og...>& orig, const std::tuple<Oth...>& insert )
{
    auto orig_split = __split_tuple<Idx> ( orig );
    return std::tuple_cat ( orig_split.first, insert, orig_split.second );
}






/* #ifndef GLHELPER_COMPOSITE_FUNCTION_HPP_INCLUDED */
#endif