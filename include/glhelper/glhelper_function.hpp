/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_function.hpp
 * 
 * constructs for representing/using mathematical functions
 * 
 * notable functions include:
 * 
 */



/* HEADER GUARD */
#ifndef GLHELPER_FUNCTION_HPP_INCLUDED
#define GLHELPER_FUNCTION_HPP_INCLUDED



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



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace math
    {
        /* class generic_function
         *
         * a base class for any no-input, single or multivariate function
         * 
         * Rt: the return type of the function
         * Ps: the parameter type(s) of the function
         */
        template<class Rt, class ...Ps> class generic_function;

        /* class passthrough_function
         *
         * a simple single-variable function, which takes one input and returns it
         * 
         * P: the parameter type and return type
         */
        template<class P> class passthrough_function;

        /* class summation_function
         *
         * a simple multi-variate function, which takes multiple inputs and finds the sum
         * 
         * Rt: the return type of the function
         * Ps: the parameter type(s) of the function
         */
        template<class Rt, class ...Ps> class summation_function;

        /* class binomial_function : generic_function
         *
         * a simple single-variable binomial function in the form ax^n + bx^(n+1)..., where n can be any real constant
         * 
         * Rt: the return type of the function
         * P: the parameter type of the function (the type of x)
         * K: the constant type of the function (the type of a,b,...)
         */
        template<class Rt, class P, class K = P> class binomial_function;

        /* class gaussian_function : generic_function
         *
         * a single-variable gaussian function in the form  a exp ( -( x - b )^2 / 2c^2 )
         *
         * Rt: the return type of the function
         * P: the parameter type of the function (the type of x)
         * K: the constant type of the function (the type of a,b,c)
         */
        template<class Rt, class P, class K = P> class gaussian_function;

        /* class composite_function : generic_function
         * 
         * a multi-variate function fromed from the binding of several function's inputs and outputs
         * 
         * Fg: the composite function fingerprint
         * Ps: the entry parameter type(s) of the function
         */
        template<class Fg, class ...Ps> class composite_function;

    }

    namespace meta
    {
        /* struct is_function
         *
         * value will be true if the type supplied is a function
         */
        template<class T, class = void> struct is_function;

        /* struct nth_element(_t)
         *
         * get the nth element of a pack
         */
        template<unsigned Idx, class ...Es> struct nth_element;
        template<unsigned Idx, class ...Es> using nth_element_t = typename nth_element<Idx, Es...>::type;

        /* struct composite_generic(_t)
         * 
         * takes a composite function fingerprint, and sets the member type to the generic function it should refer to
         */
        template<class Fg> struct composite_generic;
        template<class Fg> using composite_generic_t = typename composite_generic<Fg>::type;

        /* struct composite(_t)
         *
         * takes a composite function fingerprint, and sets the member type to the equvalent composite function type
         */
        template<class Fg> struct composite;
        template<class Fg> using composite_t = typename composite<Fg>::type;

        /* struct composite_entry_params
         *
         * takes a composite function fingerprint, and sets the member value to the number of entry parameters to the composite function
         */
        template<class Fg> struct composite_entry_params;

        /* struct composite_function_calls
         *
         * takes a composite function fingerprint, and sets the member value to the number of functions the composite function would have to call
         */
        template<class Fg> struct composite_function_calls;

        /* struct composite_depth
         *
         * takes a composite function fingerprint, and sets the member value to the depth of the fingerprint
         * the depth is equal to the longest chain of functions
         */
        template<class Fg> struct composite_depth;

        /* struct composite_return(_t)
         *
         * takes a composite function fingerprint, and sets the member type to the return of that fingerprint
         */
        template<class Fg> struct composite_return;
        template<class Fg> using composite_return_t = typename composite_return<Fg>::type;

        /* struct composite_substitution(_t)
         *
         * takes a composite fingerprint and substitution fingerprint and index, and creates a new fingerprint for the composite function
         * 
         * SIdx: the input index to substitute the fingerprint at
         * SFg: the fingerprint to substitute
         * CIdx: the current index during the scan for SIdx
         * CFg: the current fingerprint during the scan for SIdx 
         */
        template<unsigned SIdx, class SFg, unsigned CIdx, class CFg> struct composite_substitution;
        template<unsigned SIdx, class SFg, unsigned CIdx, class CFg> using composite_substitution_t = typename composite_substitution<SIdx, SFg, CIdx, CFg>::type;

        /* struct composite_function_calls_before_entry_param
         *
         * takes a composite function fingerprint, and sets the member value to number of function calls before the entry parameter supplied
         * 
         * SIdx: the input index to search for
         * CFc: the current number of function calls encountered
         * CIdx: the current index during the scan for SIdx
         * CFg: the current fingerprint during the scan for SIdx 
         */
        template<unsigned SIdx, unsigned CFc, unsigned CIdx, class CFg> struct composite_function_calls_before_entry_param;

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

        /* struct __combine_function_types(_t)
         *
         * takes multiple types, some of which can be generic functions, and combines their types
         */
        template<class Rt, class ...Ps> struct __combine_function_types;
        template<class Rt, class ...Ps> using __combine_function_types_t = typename __combine_function_types<Rt, Ps...>::type;

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
 
    namespace math
    {
        /* make_composite
         *
         * makes a composite function from a non-composite function
         */
        template<class Rt, class ...Ps> auto make_composite ( const generic_function<Rt, Ps...>& f );
        template<class Fg, class ...Ps> auto make_composite ( const composite_function<Fg, Ps...>& f );

        /* bind
         *
         * takes two functions and binds the second onto the specified entry parameter of the first
         * a composite function type is then returned
         */
        template<unsigned Idx, class Rt, class ...P0s, class ...P1s> auto bind ( const generic_function<Rt, P0s...>& f0, const generic_function<meta::nth_element_t<Idx, P0s...>, P1s...>& f1 );
        template<unsigned Idx, class Fg, class ...P0s, class ...P1s> auto bind ( const composite_function<Fg, P0s...>& f0, const generic_function<meta::nth_element_t<Idx, P0s...>, P1s...>& f1 );
        template<unsigned Idx, class Rt, class ...P0s, class Fg, class ...P1s, std::enable_if_t<std::is_same<meta::nth_element_t<Idx, P0s...>, meta::composite_return_t<Fg>>::value>...> auto 
        bind ( const generic_function<Rt, P0s...>& f0, const composite_function<Fg, P1s...>& f1 );
        template<unsigned Idx, class Fg0, class ...P0s, class Fg1, class ...P1s, std::enable_if_t<std::is_same<meta::nth_element_t<Idx, P0s...>, meta::composite_return_t<Fg1>>::value>...> auto
        bind ( const composite_function<Fg0, P0s...>& f0, const composite_function<Fg1, P1s...>& f1 );
    }
}



/* IS_FUNCTION DEFINITION */

/* struct is_function
 *
 * value will be true if the type supplied is a function
 */
template<class T, class> struct glh::meta::is_function : std::false_type {};
template<class Rt, class ...Ps> template<class T> 
struct glh::meta::is_function<T, std::enable_if_t<std::is_base_of<glh::math::generic_function<Rt, Ps...>, T>::value>> : std::true_type {};



/* NTH_ELEMENT DEFINITION */

/* struct nth_element
 *
 * get the nth element of a pack
 */
template<unsigned Idx, class E, class ...Es> struct glh::meta::nth_element<Idx, E, Es...> : nth_element<Idx - 1, Es...> {};
template<class E, class ...Es> struct glh::meta::nth_element<0, E, Es...> { typedef E type; };



/* COMPOSITE_GENERIC DEFINITION */

/* struct composite_generic(_t)
 * 
 * takes a composite function fingerprint, and sets the member type to the generic function it should refer to
 */
template<class Fg> struct glh::meta::composite_generic { typedef Fg type; };
template<class Rt> struct glh::meta::composite_generic<glh::math::generic_function<Rt>>
{
    typedef math::generic_function<Rt> type;
};
template<class Rt, class ...Ps> struct glh::meta::composite_generic<glh::math::generic_function<Rt, Ps...>>
{
    typedef __combine_function_types_t<Rt, composite_generic_t<Ps>...> type;
};



/* COMPOSITE DEFINITION */

/* struct composite
 *
 * takes a composite function fingerprint, and sets the member type to the equvalent composite function type
 */
template<class Fg> struct glh::meta::composite
{
    template<class Fg_> struct __composite { typedef math::composite_function<Fg_, Fg_> type; };
    template<class Rt_, class ...Ps_> struct __composite<glh::math::generic_function<Rt_, Ps_...>> { typedef math::composite_function<Fg, Ps_...> type; };
    typedef typename __composite<composite_generic_t<Fg>>::type type;
};



/* COMPOSITE_ENTRY_PARAMS DEFINITION */

/* struct composite_entry_params
 *
 * takes a composite function fingerprint, and sets the member value to the number of entry parameters to the composite function
 */
template<class Fg> struct glh::meta::composite_entry_params { static const unsigned value = 1; };
template<class Rt, class ...Ps> struct glh::meta::composite_entry_params<glh::math::generic_function<Rt, Ps...>>
{
    static const unsigned value = __sum_of<composite_entry_params<Ps>::value...>::value;
};



/* COMPOSITE_FUNCTION_CALLS DEFINITION */

/* struct composite_function_calls
 *
 * takes a composite function fingerprint, and sets the member value to the number of functions the composite function would have to call
 */
template<class Fg> struct glh::meta::composite_function_calls { static const unsigned value = 0; };
template<class Rt, class ...Ps> struct glh::meta::composite_function_calls<glh::math::generic_function<Rt, Ps...>> 
{ 
    static const unsigned value = __sum_of<composite_function_calls<Ps>::value...>::value + 1;
};



/* COMPOSITE_DEPTH DEFINITION */

/* struct composite_depth
*
* takes a composite function fingerprint, ans sets the member value to the depth of the fingerprint
* the depth is equal to the longest chain of functions
*/
template<class Fg> struct glh::meta::composite_depth { static const unsigned value = 0; };
template<class Rt, class ...Ps> struct glh::meta::composite_depth<glh::math::generic_function<Rt, Ps...>>
{
    static const unsigned value = __max_of<composite_depth<Ps>::value...>::value + 1;
};



/* COMPOSITE_RETURN DEFINITION */

/* struct composite_return(_t)
 *
 * takes a composite function fingerprint, and sets the member type to the return of that fingerprint 
 */
template<class Fg> struct glh::meta::composite_return { typedef Fg type; };
template<class Rt, class ...Ps> struct glh::meta::composite_return<glh::math::generic_function<Rt, Ps...>> { typedef Rt type; };



/* COMPOSITE_SUBSTITUTION DEFINITION */

/* struct composite_substitution(_t)
 *
 * takes a composite fingerprint and substitution fingerprint and index, and creates a new fingerprint for the composite function
 * 
 * Fg: original fingerprint
 * SIdx: the input index to substitute the fingerprint at
 * SFg: the fingerprint to substitute
 * CIdx: the current index during the scan for SIdx
 * CFg: the current fingerprint during the scan for SIdx 
 */
template<unsigned SIdx, class SFg, unsigned CIdx, class CFg> struct glh::meta::composite_substitution
{
    static const unsigned New_CIdx = CIdx + 1;
    typedef std::conditional_t<SIdx == CIdx, SFg, CFg> type;
};
template<unsigned SIdx, class SFg, unsigned CIdx, class Rt> struct glh::meta::composite_substitution<SIdx, SFg, CIdx, glh::math::generic_function<Rt>>
{
    static const unsigned New_CIdx = CIdx;
    typedef glh::math::generic_function<Rt> type;
};
template<unsigned SIdx, class SFg, unsigned CIdx, class Rt, class P, class ...Ps> struct glh::meta::composite_substitution<SIdx, SFg, CIdx, glh::math::generic_function<Rt, P, Ps...>>
{
    typedef composite_substitution<SIdx, SFg, CIdx, P> Primary;
    typedef composite_substitution<SIdx, SFg, Primary::New_CIdx, math::generic_function<Rt, Ps...>> Rest;
    static const unsigned New_CIdx = Rest::New_CIdx;
    typedef __combine_function_types_t
    <Rt, 
        glh::math::generic_function<Rt, typename Primary::type>, 
        typename Rest::type
    > type;
};



/* COMPOSITE_FUNCTION_CALLS_BEFORE_ENTRY_PARAM DEFINITION */

/* struct composite_function_calls_before_entry_param
 *
 * takes a composite function fingerprint, and sets the member value to number of function calls before the entry parameter supplied
 * 
 * SIdx: the input index to search for
 * CFc: the current number of function calls encountered
 * CIdx: the current index during the scan for SIdx
 * CFg: the current fingerprint during the scan for SIdx 
 */
template<unsigned SIdx, unsigned CFc, unsigned CIdx, class CFg> struct glh::meta::composite_function_calls_before_entry_param
{
    static const unsigned New_CFc = CFc;
    static const unsigned New_CIdx = CIdx + 1;
    static const unsigned value = ( SIdx == CIdx ? CFc : 0 );
};
template<unsigned SIdx, unsigned CFc, unsigned CIdx, class Rt> struct glh::meta::composite_function_calls_before_entry_param<SIdx, CFc, CIdx, glh::math::generic_function<Rt>>
{
    static const unsigned New_CFc = CFc + 1;
    static const unsigned New_CIdx = CIdx;
    static const unsigned value = 0;
};
template<unsigned SIdx, unsigned CFc, unsigned CIdx, class Rt, class P, class ...Ps> struct glh::meta::composite_function_calls_before_entry_param<SIdx, CFc, CIdx, glh::math::generic_function<Rt, P, Ps...>>
{
    typedef composite_function_calls_before_entry_param<SIdx, CFc, CIdx, P> Primary;
    typedef composite_function_calls_before_entry_param<SIdx, Primary::New_CFc, Primary::New_CIdx, glh::math::generic_function<Rt, Ps...>> Rest;
    static const unsigned New_CFc = Rest::New_CFc;
    static const unsigned New_CIdx = Rest::New_CIdx;
    static const unsigned value = __max_of<Primary::value, Rest::value>::value;
};



/* __MAX_OF DEFINITION */

/* struct __max_of
 *
 * calculates the maximum of the integers supplied
 */
template<> struct glh::meta::__max_of<> { static const unsigned value = 0; };
template<unsigned I, unsigned ...Is> struct glh::meta::__max_of<I, Is...> 
{ 
    static const unsigned value = ( I > __max_of<Is...>::value ? I : __max_of<Is...>::value );
};



/* __SUM_OF DEFINITION */

/* struct __sum_of
*
* calculates the sum of the integers supplied
*/
template<> struct glh::meta::__sum_of<> { static const unsigned value = 0; };
template<unsigned I, unsigned ...Is> struct glh::meta::__sum_of<I, Is...> 
{
    static const unsigned value = I + __sum_of<Is...>::value;
};




/* __COMBINE_FUNCTION_TYPES DEFINITION */

/* struct __combine_function_types
 *
 * takes multiple types, some of which can be generic functions, and combines their types
 */
/* no types */
template<class Rt> struct glh::meta::__combine_function_types<Rt>
{
    typedef math::generic_function<Rt> type;
};
/* single scalar type */
template<class Rt, class P> struct glh::meta::__combine_function_types<Rt, P>
{
    typedef math::generic_function<Rt, P> type;
};
/* single function type */
template<class Rt, class Rt0, class ...Ps0> struct glh::meta::__combine_function_types<Rt, glh::math::generic_function<Rt0, Ps0...>>
{
    typedef math::generic_function<Rt, Ps0...> type;
};
/* multiple types, starting with two non-function types */
template<class Rt, class P0, class P1, class ...Ps> struct glh::meta::__combine_function_types<Rt, P0, P1, Ps...>
{
    typedef __combine_function_types_t<Rt, __combine_function_types_t<Rt, P0>, __combine_function_types_t<Rt, P1>, Ps...> type;
};
/* multiple types, starting with two function types */
template<class Rt, class Rt0, class ...P0s, class Rt1, class ...P1s, class ...Ps> 
struct glh::meta::__combine_function_types<Rt, glh::math::generic_function<Rt0, P0s...>, glh::math::generic_function<Rt1, P1s...>, Ps...>
{
    typedef __combine_function_types_t<Rt, glh::math::generic_function<Rt, P0s..., P1s...>, Ps...> type;
};



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



/* MAKE_COMPOSITE IMPLEMENTATION */

/* make_composite
 *
 * makes a composite function from a non-composite function
 */
template<class Rt, class ...Ps> inline auto glh::math::make_composite ( const generic_function<Rt, Ps...>& f )
{
    return meta::composite_t<generic_function<Rt, Ps...>> { std::make_tuple ( f.duplicate () ) };
}
template<class Fg, class ...Ps> inline auto glh::math::make_composite ( const composite_function<Fg, Ps...>& f )
{
    return f;
}



/* BIND IMPLEMENTATION */

/* bind
 *
 * takes two functions and binds the second onto the specified entry parameter of the first
 * a composite function type is then returned
 */
template<unsigned Idx, class Rt, class ...P0s, class ...P1s> inline auto glh::math::bind ( const generic_function<Rt, P0s...>& f0, const generic_function<meta::nth_element_t<Idx, P0s...>, P1s...>& f1 )
{
    return meta::composite_t<generic_function<Rt, P0s...>> { std::make_tuple ( f0.duplicate () ) }.template bind<Idx> ( f1 );
}
template<unsigned Idx, class Fg, class ...P0s, class ...P1s> inline auto glh::math::bind ( const composite_function<Fg, P0s...>& f0, const generic_function<meta::nth_element_t<Idx, P0s...>, P1s...>& f1 )
{
    return f0.template bind<Idx> ( f1 );
}
template<unsigned Idx, class Rt, class ...P0s, class Fg, class ...P1s, std::enable_if_t<std::is_same<glh::meta::nth_element_t<Idx, P0s...>, glh::meta::composite_return_t<Fg>>::value>...> inline auto
glh::math::bind ( const generic_function<Rt, P0s...>& f0, const composite_function<Fg, P1s...>& f1 )
{
    return meta::composite_t<generic_function<Rt, P0s...>> { std::make_tuple ( f0.duplicate () ) }.template bind<Idx> ( f1 );
}
template<unsigned Idx, class Fg0, class ...P0s, class Fg1, class ...P1s, std::enable_if_t<std::is_same<glh::meta::nth_element_t<Idx, P0s...>, glh::meta::composite_return_t<Fg1>>::value>...> inline auto
glh::math::bind ( const composite_function<Fg0, P0s...>& f0, const composite_function<Fg1, P1s...>& f1 )
{
    return f0.template bind<Idx> ( f1 );
}



/* GENERIC_FUNCTION DEFINITION */

/* class generic_function
 *
 * a base class for any no-input, single or multivariate function
 * 
 * Rt: the return type of the function
 * Ps: the parameter type(s) of the function
 */
template<class Rt, class ...Ps> class glh::math::generic_function
{
public:

    /* no user-defined constructors or destructors, just pure virtual methods */

    /* allow access to the types of the function */
    using return_type = Rt;
    static const unsigned param_count = sizeof...( Ps );

    /* pure virtual apply and operator()
     *
     * apply the function to some input parameters
     */
    virtual Rt apply ( const Ps&... ps ) const = 0;
    virtual Rt operator() ( const Ps&... ps ) const { return apply ( ps... ); }

    /* duplicate
     *
     * copy the function onto the heap and return a base class pointer to it
     */
    virtual generic_function * duplicate () const = 0;

};



/* PASSTHROUGH_FUNCTION DEFINITION */

/* class passthrough_function
*
* a simple single-variable function, which takes one input and returns it
* 
* P: the parameter type and return type
*/
template<class P> class glh::math::passthrough_function : public generic_function<P, P>
{
public:

    /* default everuthing */
    passthrough_function () = default;
    passthrough_function ( const passthrough_function& other ) = default;
    passthrough_function& operator= ( const passthrough_function& other ) = default;
    ~passthrough_function () = default;

    
    
    /* apply method */
    P apply ( const P& p ) const { return p; }

    /* duplicate method */
    generic_function<P, P> * duplicate () const { return new passthrough_function { * this }; }

};



/* SUMMATION_FUNCTION DEFINITION */

/* class summation_function
 *
 * a simple multi-variate function, which takes multiple inputs and finds the sum
 * 
 * Rt: the return type of the function
 * Ps: the parameter type(s) of the function
 */
template<class Rt, class ...Ps> class glh::math::summation_function : public generic_function<Rt, Ps...>
{

    /* assert that all of Rt and Ps... are arithmetic */
    static_assert ( std::is_arithmetic<Rt>::value && std::conjunction<std::is_arithmetic<Ps>...>::value, "summation function cannot be formed of non-arithmetic types" );

public:

    /* default everuthing */
    summation_function () = default;
    summation_function ( const summation_function& other ) = default;
    summation_function& operator= ( const summation_function& other ) = default;
    ~summation_function () = default;



    /* apply method */
    Rt apply ( const Ps&... p ) const { return __apply ( p... ); }

    /* duplicate method */
    generic_function<Rt, Ps...> * duplicate () const { return new summation_function { * this }; }



private:

    /* __apply
     *
     * helper method for apply
     */
    template<class P_, class ...Ps_> Rt __apply ( const P_& p, const Ps_&... ps ) const { return p + __apply ( ps... ); }
    Rt __apply () const { return Rt { 0 }; }

};



/* BINOMIAL FUNCTION DEFINITION */

/* class binomial_function : generic_function
 *
 * a simple single-variable binomial function in the form ax^n + bx^(n+1)..., where n can be any real constant
 * 
 * Rt: the return type of the function
 * P: the parameter type of the function (the type of x)
 * K: the constant type of the function (the type of a,b,...)
 */
template<class Rt, class P, class K> class glh::math::binomial_function : public generic_function<Rt, P>
{

    /* assert that all of Rt, P and K are arithmetic */
    static_assert ( std::is_arithmetic<Rt>::value && std::is_arithmetic<P>::value && std::is_arithmetic<K>::value, "binomial function cannot be formed of non-arithmetic types" );

public:

    /* default zero-parameter constructor */
    binomial_function () = default;

    /* full constructor
     *
     * the parameters supplied are the constants for a binomial in ascending order starting at n
     */
    template<class ...Ks> binomial_function ( const int n, const K& k, const Ks&... ks )
        : binomial_function { n + 1, ks... }
    { constants.insert ( std::make_pair ( n, k ) ); }

    /* do-nothing constructor, such that full constructor stops its recursion */
    binomial_function ( const int n ) {};

    /* default copy constructor */
    binomial_function ( const binomial_function& other ) = default;

    /* default copy assignment operator */
    binomial_function& operator= ( const binomial_function& other ) = default;

    /* default destructor */
    ~binomial_function () = default;



    /* apply method */
    Rt apply ( const P& p ) const;

    /* duplicate method */
    generic_function<Rt, P> * duplicate () const { return new binomial_function { * this }; }



    /* get/set_constant
     *
     * get/set the constant at the specified power of x
     */
    K get_constant ( const int n );
    void set_constant ( const int n, const K& k );



private:

    /* map of powers of x to their constants
     * contant assumed to be 0 if not present
     */
    std::map<int, K> constants;

};



/* GAUSSIAN_FUNCTION DEFINITION */

/* class gaussian_function : generic_function
 *
 * a single-variable gaussian function in the form  a exp ( -( x - b )^2 / 2c^2 )
 *
 * Rt: the return type of the function
 * P: the parameter type of the function (the type of x)
 * K: the constant type of the function (the type of a,b,c)
 */
template<class Rt, class P, class K> class glh::math::gaussian_function : public generic_function<Rt, P>
{

    /* assert that all of Rt, P and K are arithmetic */
    static_assert ( std::is_arithmetic<Rt>::value && std::is_arithmetic<P>::value && std::is_arithmetic<K>::value, "gaussian function cannot be formed of non-arithmetic types" );

public:

    /* zero-parameter constructor */
    gaussian_function ()
        : a { 1 }
        , b { 0 }
        , c { 1 }
    {}

    /* full constructor */
    gaussian_function ( const K& _a, const K& _b, const K& _c )
        : a { _a }
        , b { _b }
        , c { _c }
    {}

    /* default copy constructor */
    gaussian_function ( const gaussian_function& other ) = default;

    /* default copy assignment operator */
    gaussian_function& operator= ( const gaussian_function& other ) = default;

    /* default destructor */
    ~gaussian_function () = default;



    /* apply method */
    Rt apply ( const P& p ) const { return a * std::exp ( -( ( p - b ) * ( p - b ) ) / ( 2 * c * c ) ); }

    /* duplicate method */
    generic_function<Rt, P> * duplicate () const { return new gaussian_function { * this }; }



    /* get/set_a/b/c
     *
     * set the constants a, b and c
     */
    const K& get_a () const { return a; };
    const K& get_b () const { return b; };
    const K& get_c () const { return c; };
    void set_a ( const K& _a ) { a = _a; }
    void set_b ( const K& _b ) { a = _b; }
    void set_c ( const K& _c ) { a = _c; }



private:

    /* constants for a, b and c */
    K a;
    K b;
    K c;

};



/* COMPOSITE_FUNCTION SPECIALIZATION 1 DEFINITION */

/* class composite_function : generic_function
 * 
 * this specialization is for when Fg is (in its outer most level) a function 
 * 
 * Rt: return type of the upper most level of Fg
 * Ps_: the parameters of the upper most level of Fg
 * Ps: the entry parameters of Fg
 */
template<class Rt, class ...Ps_, class ...Ps> class glh::math::composite_function<glh::math::generic_function<Rt, Ps_...>, Ps...>
    : public generic_function<Rt, Ps...>
{
public:

    /* __set_fuuncs constructor */
    template<class ...Tp> composite_function ( const std::tuple<Tp *...>& tp )
        { __set_funcs ( tp ); }

    /* deleted zero-parameter constructor */
    composite_function () = default;

    /* copy constructor */
    composite_function ( const composite_function& other )
        { __set_funcs ( other.__duplicate_funcs () ); }

    /* default copy move constructor */
    composite_function ( composite_function&& other ) = default;

    /* copy assignment operator */
    composite_function& operator= ( const composite_function& other )
        { __set_funcs ( other.__duplicate_funcs () ); return * this; }

    /* default move assignment operator */
    composite_function& operator= ( composite_function&& other ) = default;

    /* default destructor */
    ~composite_function () = default;



    /* apply method */
    Rt apply ( const Ps&... ps ) const { return __apply ( std::tuple<const Ps&...> { ps... } ); }

    /* duplicate method */
    generic_function<Rt, Ps...> * duplicate () const { return new composite_function { * this }; }



    /* bind
     *
     * binds a function to an entry parameter
     */
    template<unsigned Idx, class ...Ps__> auto 
    bind ( const generic_function<meta::nth_element_t<Idx, Ps...>, Ps__...>& f ) const
    {
        return meta::composite_t<meta::composite_substitution_t<Idx, generic_function<meta::nth_element_t<Idx, Ps...>, Ps__...>, 0, glh::math::generic_function<Rt, Ps_...>>>
        {
            meta::__insert_tuple<meta::composite_function_calls_before_entry_param<Idx, 0, 0, glh::math::generic_function<Rt, Ps_...>>::value> ( __duplicate_funcs (), std::make_tuple ( f.duplicate () ) )
        };
    }
    template<unsigned Idx, class Fg, class ...Ps__, std::enable_if_t<std::is_same<meta::nth_element_t<Idx, Ps...>, meta::composite_return_t<Fg>>::value>...> auto 
    bind ( const composite_function<Fg, Ps__...>& f ) const
    {
        return meta::composite_t<meta::composite_substitution_t<Idx, Fg, 0, glh::math::generic_function<Rt, Ps_...>>>
        {
            meta::__insert_tuple<meta::composite_function_calls_before_entry_param<Idx, 0, 0, glh::math::generic_function<Rt, Ps_...>>::value> ( __duplicate_funcs (), f.__duplicate_funcs () )
        };
    }



    /* __apply
     *
     * takes a tuple of inputs from apply and uses them to call __apply for each of the functions in params
     * during this time, it recursively builds up a parameter pack of inputs for func
     */
    template<class ...Tp, class ...Ps__> Rt __apply ( const std::tuple<const Tp&...>& tp, const Ps__&... ps ) const
    {
        auto tp_split = meta::__split_tuple<meta::composite_entry_params<meta::nth_element_t<sizeof...( Ps__ ), Ps_...>>::value> ( tp );
        return __apply ( tp_split.second, ps..., std::get<sizeof...( Ps__ )> ( params ).__apply ( tp_split.first ) );
    }
    Rt __apply ( const std::tuple<>& tp, const meta::composite_return_t<Ps_>&... ps ) const { return func->apply ( ps... ); }

    /* __duplicate_funcs
     *
     * create a tuple of pointers to duplicated generic functions
     */
    template<unsigned Idx = 0, std::enable_if_t<Idx != sizeof...( Ps_ )>...> auto __duplicate_funcs () const
    {
        return std::tuple_cat ( std::get<Idx> ( params ).__duplicate_funcs (), __duplicate_funcs<Idx + 1> () );
    }
    template<unsigned Idx = 0, std::enable_if_t<Idx == sizeof...( Ps_ )>...> auto __duplicate_funcs () const
    {
        return std::make_tuple ( func->duplicate () );
    }

    /* __set_funcs
     *
     * takes a tuple of pointers to generic functions in order to set all of the funcs of this level and all of params
     */
    template<unsigned Idx = 0, class ...Tp, std::enable_if_t<Idx != sizeof...( Ps_ )>...> void __set_funcs ( const std::tuple<Tp *...>& tp )
    {
        auto tp_split = meta::__split_tuple<meta::composite_function_calls<meta::nth_element_t<Idx, Ps_...>>::value> ( tp );
        std::get<Idx> ( params ).__set_funcs ( tp_split.first ); 
        __set_funcs<Idx + 1> ( tp_split.second ); 
    }
    template<unsigned Idx = 0, std::enable_if_t<Idx == sizeof...( Ps_ )>...> void __set_funcs ( const std::tuple<generic_function<Rt, meta::composite_return_t<Ps_>...> *> tp )
    {
        func.reset ( std::get<0> ( tp ) );
    }

private:

    /* smart pointer to function */
    std::unique_ptr<generic_function<Rt, meta::composite_return_t<Ps_>...>> func;

    /* parameters to the composite function */
    std::tuple<meta::composite_t<Ps_>...> params;
 
};



/* COMPOSITE_FUNCTION SPECIALIZATION 2 DEFINITION */

/* class composite_function : passthrough_function
 * 
 * this specialization is for when Fg is a scalar value (i.e. for a parameter input)
 * this specialization purely returns the same value it takes in
 */
template<class Fg> class glh::math::composite_function<Fg, Fg> : public generic_function<Fg, Fg>
{
public:

    /* no user-defined constructors, just methods */

    /* apply method */
    Fg apply ( const Fg& p ) const { return p; }

    /* duplicate method */
    generic_function<Fg, Fg> * duplicate () const { return new composite_function {}; }

    /* __apply method */
    Fg __apply ( const std::tuple<const Fg&>& tp ) const { return std::get<0> ( tp ); }

    /* __duplicate_funcs method */
    std::tuple<> __duplicate_funcs () const { return std::tuple<> {}; }

    /* __set_funcs method */
    void __set_funcs ( const std::tuple<>& tp ) {}

};



/* BINOMIAL_FUNCTION IMPLEMENTATION */

/* apply method */
template<class Rt, class P, class K> inline
Rt glh::math::binomial_function<Rt, P, K>::apply ( const P& p ) const
{
    /* the value to output */
    Rt out { 0 };

    /* get iterator to power of x 0 */
    auto upper = constants.lower_bound ( 0 );

    /* get reverse iterator to the previous power, which will always be less than 0 */
    auto lower = constants.rbegin (); std::advance ( lower, std::distance ( upper, constants.end () ) );

    /* if upper is x^0, add the constant at x^0 multiplied by p to the 0 to out
     * then add one to upper
     * now both upper and lower refer to one above and one below power of 0, respectively
     */
    if ( upper->first == 0 ) 
    {
        out += upper->second * P { 1 };
        ++upper;
    }

    /* iterate through powers, using x to keep track of powers */
    P x { p };
    for ( unsigned i = 1; upper != constants.end () || lower != constants.rend (); ++i )
    {
        /* if the power is currently the same as upper or lower, add that term */
        if ( upper != constants.end () && upper->first == i )
        {
            out += upper->second * x;
            ++upper;
        }
        if ( lower != constants.rend () && lower->first == -i )
        {
            out += lower->second * ( P { 1 } / x );
            ++lower;
        }

        /* multiply x by p */
        x *= p;
    }

    /* return out */
    return out;
}

/* get/set_constant
 *
 * get/set the constant at the specified power of x
 */
template<class Rt, class P, class K> inline
K glh::math::binomial_function<Rt, P, K>::get_constant ( const int n )
{
    /* find entry */
    auto it = constants.find ( n );

    /* if not found, return 0 constant */
    if ( it == constants.end () ) return K { 0 };

    /* else return the constant */
    else return it->second;
}
template<class Rt, class P, class K> inline
void glh::math::binomial_function<Rt, P, K>::set_constant ( const int n, const K& k )
{
    /* find entry */
    auto it = constants.find ( n );

    /* if not found, add the entry */
    if ( it == constants.end () ) constants.insert ( std::make_pair ( n, k ) );

    /* otherwise, set the new value */
    else it->second = k;
}



/* #ifndef GLHELPER_FUNCTION_HPP_INCLUDED */
#endif