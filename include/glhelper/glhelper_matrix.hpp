/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_matrix.hpp
 * 
 * implements matrix mathematics
 * notable constructs include:
 * 
 * 
 * 
 * CLASS GLH::MATH::MATRIX
 *
 * template class to represent a matrix of any given size
 * the template parameters M and N form a matrix of size MxN (M rows, N columns)
 * IMPORTANT: matrix access through the at (...) methods is ROW-MAJOR
 *            however, the actual storage of the matrices is COLUMN
 * 
 * 
 * 
 * MATRIX NON-MEMBER FUNCTIONS
 * 
 * non-member functions include (all in namespace glh::math):
 * 
 * OPERATORS*+-/: for matrix-matrix and matrix-scalar combinations
 * TRANSPOSE: transpose a matrix of any size
 * SUBMATRIX: get a submatrix by removing the row and column of a specific element
 * DET: get the determinant of a square matrix via the adjugate method
 * MINOR: get the minor of an element of a square matrix
 * INVERSE: get the inverse matrix of a square matrix
 * 
 * 
 * 
 * CLASS GLH::EXCEPTION::MATRIX_EXCEPTION
 * 
 * thrown when an error occurs in one of the matrix methods or non-member functions (e.g. attempting to get the inverse of a singular matrix)
 *   
 */



/* HEADER GUARD */
#ifndef GLHELPER_MATRIX_HPP_INCLUDED
#define GLHELPER_MATRIX_HPP_INCLUDED



/* INCLUDES */

/* include core headers */
#include <array>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>



/* NAMESPACE DECLARATIONS */

namespace glh
{
    namespace math
    {
        /* TYPES AND CLASSESS */

        /* class matrix
        *
        * a class representing a matrix of a size MxN
        */
        template<unsigned M, unsigned N, class T> class matrix;

        /* default matrix types */
        template<unsigned M, unsigned N> using fmatrix = matrix<M, N, float>;
        template<unsigned M, unsigned N> using dmatrix = matrix<M, N, double>;
        template<unsigned M, unsigned N> using imatrix = matrix<M, N, int>;
        using fmat2 = fmatrix<2, 2>; using fmat2x3 = fmatrix<2, 3>; using fmat2x4 = fmatrix<2, 4>;
        using fmat3 = fmatrix<3, 3>; using fmat3x2 = fmatrix<3, 2>; using fmat3x4 = fmatrix<3, 4>;
        using fmat4 = fmatrix<4, 4>; using fmat4x2 = fmatrix<4, 2>; using fmat4x3 = fmatrix<4, 3>;
        using dmat2 = dmatrix<2, 2>; using dmat2x3 = dmatrix<2, 3>; using dmat2x4 = dmatrix<2, 4>;
        using dmat3 = dmatrix<3, 3>; using dmat3x2 = dmatrix<3, 2>; using dmat3x4 = dmatrix<3, 4>;
        using dmat4 = dmatrix<4, 4>; using dmat4x2 = dmatrix<4, 2>; using dmat4x3 = dmatrix<4, 3>;
        using imat2 = imatrix<2, 2>; using imat2x3 = imatrix<2, 3>; using imat2x4 = imatrix<2, 4>;
        using imat3 = imatrix<3, 3>; using imat3x2 = imatrix<3, 2>; using imat3x4 = imatrix<3, 4>;
        using imat4 = imatrix<4, 4>; using imat4x2 = imatrix<4, 2>; using imat4x3 = imatrix<4, 3>;
        using mat2 = dmat2; using mat2x3 = dmat2x3; using mat2x4 = dmat2x4;
        using mat3 = dmat3; using mat3x2 = dmat3x2; using mat3x4 = dmat3x4;
        using mat4 = dmat4; using mat4x2 = dmat4x2; using mat4x3 = dmat4x3;
        

        /* MATRIX MODIFIER FUNCTIONS DECLARATIONS */

        /* promote_matrix
         *
         * promte a matrix to the preferred type give another type
         */
        template<unsigned M, unsigned N, class T0, class T1> std::conditional_t<std::is_same<T0, std::common_type_t<T0, T1>>::value, const matrix<M, N, T0>&, matrix<M, N, std::common_type_t<T0, T1>>> promote_matrix ( const matrix<M, N, T0>& lhs );

        /* transpose
         * 
         * _matrix: the matrix to transpose
         * 
         * return: the new matrix
         */
        template<unsigned M, unsigned N, class T> matrix<N, M, T> transpose ( const matrix<M, N, T>& _matrix );

        /* submatrix
         *
         * _matrix: the original matrix
         * i,j or i: row/column to remove, or the index of which to remove its row and column
         * 
         * return: the new transformed matrix
         */
        template<unsigned M, unsigned N, class T> matrix<M - 1, N - 1, T> submatrix ( const matrix<M, N, T>& _matrix, const unsigned i, const unsigned j );

        /* det
         *
         * _matrix: the matrix to find the determinant of
         *
         * return: the determinant
         */
        template<unsigned M, class T> std::enable_if_t<( M > 1 ), T> det ( const matrix<M, M, T>& _matrix );
        template<unsigned M, class T> std::enable_if_t<M == 1, T> det ( const matrix<M, M, T>& _matrix );

        /* minor
         *
         * _matrix: the matrix to find a minor of
         * i,j or i: the row/column, or the index to find the minor of
         * 
         * return: the minor of the element requested
         */
        template<unsigned M, class T> T minor ( const matrix<M, M, T>& _matrix, const unsigned i, const unsigned j );

        /* inverse
         *
         * _matrix: the matrix to find the inverse of
         * 
         * return: the inverse matrix
         */
        template<unsigned M, class T> std::enable_if_t<( M > 1 ), matrix<M, M, T>> inverse ( const matrix<M, M, T>& _matrix );
        template<unsigned M, class T> std::enable_if_t<M == 1, matrix<M, M, T>> inverse ( const matrix<M, M, T>& _matrix );
    
        /* pow
         *
         * raise a matrix to a power
         * a negative power uses the inverse as the base
         */
        template<unsigned M, class T> matrix<M, M, T> pow ( const matrix<M, M, T>& base, const int exp );
    
    }

    namespace meta
    {
        /* struct is_matrix
         *
         * is_matrix::value is true if the type supplied is a matrix
         */
        template<class T> struct is_matrix;
    }

    namespace exception
    {
        /* class matrix_exception : exception
         *
         * for exceptions related to matrices
         */
        class matrix_exception;
    }
}



/* MATRIX OPERATORS DECLARATIONS */

/* operator== and operator!=
 *
 * compares two matrices of the same size value by value
 */
template<unsigned M, unsigned N, class T0, class T1> bool operator== ( const glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs );
template<unsigned M, unsigned N, class T0, class T1> bool operator!= ( const glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs );

/* operator+(=)
 *
 * addition operations on matrices include:
 * 
 * matrix + matrix
 * matrix + scalar == scalar + matrix
 * matrix += matrix
 * matrix += scalar
 */
template<unsigned M, unsigned N, class T0, class T1> glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator+ ( const glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs );
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator+ ( const glh::math::matrix<M, N, T0>& lhs, const T1& rhs );
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T0>::value>...> glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator+ ( const T0& lhs,const glh::math::matrix<M, N, T1>& rhs );
template<unsigned M, unsigned N, class T0, class T1> glh::math::matrix<M, N, T0>& operator+= ( glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs );
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> glh::math::matrix<M, N, T0>& operator+= ( glh::math::matrix<M, N, T0>& lhs, const T1& rhs );

/* operator-(=)
 *
 * subtraction operations on matrices include:
 * 
 * matrix - matrix
 * matrix - scalar
 * matrix -= matrix
 * matrix -= scalar
 */
template<unsigned M, unsigned N, class T0, class T1> glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator- ( const glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs );
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator- ( const glh::math::matrix<M, N, T0>& lhs, const T1& rhs );
template<unsigned M, unsigned N, class T0, class T1> glh::math::matrix<M, N, T0>& operator-= ( glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs );
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> glh::math::matrix<M, N, T0>& operator-= ( glh::math::matrix<M, N, T0>& lhs, const T1& rhs );

/* operator*(=)
 * 
 * multiplication operations on matrices include
 * 
 * matrix * matrix (of compatible sizes)
 * matrix * scalar == scalar * matrix
 * matrix *= matrix (SEE BELOW)
 * matrix *= scalar
 * 
 * NOTE: mat1 *= mat2 is equivalent to mat1 = MAT2 * MAT1
 *       this is for the purpose of adding transformations
 */
template<unsigned M0, unsigned N0M1, unsigned N1, class T0, class T1> glh::math::matrix<M0, N1, std::common_type_t<T0, T1>> operator* ( const glh::math::matrix<M0, N0M1, T0>& lhs, const glh::math::matrix<N0M1, N1, T1>& rhs );
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator* ( const glh::math::matrix<M, N, T0>& lhs, const T1& rhs );
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T0>::value>...> glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator* ( const T0& lhs, const glh::math::matrix<M, N, T1>& rhs );
template<unsigned M0, unsigned N0M1, unsigned N1, class T0, class T1> glh::math::matrix<N0M1, N1, T0>& operator*= ( glh::math::matrix<N0M1, N1, T0>& lhs, const glh::math::matrix<M0, N0M1, T1>& rhs );
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> glh::math::matrix<M, N, T0>& operator*= ( glh::math::matrix<M, N, T0>& lhs, const T1& rhs );

/* operator/(=)
 *
 * division operations on matrices include:
 * 
 * matrix / scalar
 * matrix /= scalar
 * 
 * to get matrix division, use multiplication with inverse matrices
 */
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator/ ( const glh::math::matrix<M, N, T0>& lhs, const T1& rhs );
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> glh::math::matrix<M, N, T0>& operator/= ( glh::math::matrix<M, N, T0>& lhs, const T1& rhs );

/* unary plus operator */
template<unsigned M, unsigned N, class T> glh::math::matrix<M, N, T>& operator+ ( const glh::math::matrix<M, N, T>& lhs );
/* unary minus operator */
template<unsigned M, unsigned N, class T> glh::math::matrix<M, N, T>& operator- ( const glh::math::matrix<M, N, T>& lhs );

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M, unsigned N, class T> std::ostream& operator<< ( std::ostream& os, const glh::math::matrix<M, N, T>& _matrix );



/* IS_MATRIX DEFINITION */

/* struct is_matrix
 *
 * is_matrix::value is true if the type supplied is a matrix
 */
template<class T> struct glh::meta::is_matrix : std::false_type {};
template<> template<unsigned M, unsigned N, class T> struct glh::meta::is_matrix<glh::math::matrix<M, N, T>> : std::true_type {};



/* MATRIX DEFINITION */

/* class matrix
 *
 * a class representing a matrix of a size MxN
 */
template<unsigned M, unsigned N, class T> class glh::math::matrix
{

    /* static assert that M > 0 && N > 0 */
    static_assert ( M > 0 && N > 0, "a matrix cannot have a 0 dimension" );

    /* static assert that T is arithmetic */
    static_assert ( std::is_arithmetic<T>::value, "a matrix cannot be instantiated from a non-arithmetic type" );

public:

    /* default/value constructor
     *
     * sets all elements to the value provided
     */
    explicit matrix ( const T& val = 0.0 ) { data.fill ( val ); }

    /* initializer list constructor
     *
     * constructs data from initializer list
     */
    explicit matrix ( const std::initializer_list<T> init_list );

    /* copy constructor
     *
     * copy a matrix of the same size of any type
     */
    template<class _T> matrix ( const matrix<M, N, _T>& other );

    /* copy assignment operator
     *
     * copy assign from a matrix of the same size of any type
     */
    template<class _T> matrix<M, N, T>& operator= ( const matrix<M, N, _T>& other );

    /* default destructor */
    ~matrix () = default;



    /* width/height
     *
     * width and height of the matrix
     */
    static const unsigned height = M;
    static const unsigned width = N;

    /* the type of the matrix */
    typedef T value_type;



    /* at
     *
     * gets values from the matrix
     * 
     * i,j: the row/column coordinate
     */
    T& at ( const unsigned i, const unsigned j );
    const T& at ( const unsigned i, const unsigned j ) const;



    /* __at
     *
     * accesses the internal array
     * the internal array is in column-major order, hence the '__'
     */
    T& __at ( const unsigned i );
    const T& __at ( const unsigned i ) const;



    /* internal_ptr
     *
     * return: pointer to the internal array of data
     */
    T * internal_ptr () { return data.data (); }
    const T * internal_ptr () const { return data.data (); }

    /* format_str
     *
     * format the matrix to a string
     */
    std::string format_str () const;



private:

    /* array data
     *
     * the actual data of the matrix
     */
    std::array<T, M * N> data;

};



/* MATRIX_EXCEPTION DEFINITION */

/* class matrix_exception : exception
 *
 * for exceptions related to matrices
 */
class glh::exception::matrix_exception : public exception
{
public:

    /* full constructor
     *
     * __what: description of the exception
     */
    explicit matrix_exception ( const std::string& __what )
        : exception { __what }
    {}

    /* default zero-parameter constructor
     *
     * construct matrix_exception with no descrption
     */
    matrix_exception () = default;

    /* default everything else and inherits what () function */

};



/* MATRIX IMPLEMENTATION */

/* copy constructor
 *
 * copy a matrix of the same size of any type
 */
template<unsigned M, unsigned N, class T>
template<class _T> inline glh::math::matrix<M, N, T>::matrix ( const matrix<M, N, _T>& other )
{
    /* copy values from other to this */
    for ( unsigned i = 0; i < M * N; ++i ) __at ( i ) = other.__at ( i );
}

/* initializer list constructor
 *
 * constructs data from initializer list
 */
template<unsigned M, unsigned N, class T> inline glh::math::matrix<M, N, T>::matrix ( const std::initializer_list<T> init_list )
{
    /* check the size of the list */
    if ( init_list.size () != M * N ) throw exception::matrix_exception { "matrix initializer list is invalid" };
    
    /* set the values */
    unsigned i = 0;
    for ( const T& v: init_list ) 
    { 
        __at ( ( ( i % N ) * M ) + ( i / N ) ) = v;
        ++i;
    }
}



/* copy assignment operator
 *
 * copy assign from a matrix of the same size of any type
 */
template<unsigned M, unsigned N, class T>
template<class _T> inline glh::math::matrix<M, N, T>& glh::math::matrix<M, N, T>::operator= ( const matrix<M, N, _T>& other )
{
    /* copy values from other to this */
    for ( unsigned i = 0; i < M * N; ++i ) __at ( i ) = other.__at ( i );

    /* return * this */
    return * this;
}



/* at
 *
 * gets values from the matrix
 * 
 * i,j or i: the row/column coordinate, or the index
 */
template<unsigned M, unsigned N, class T> inline T& glh::math::matrix<M, N, T>::at ( const unsigned i, const unsigned j )
{
    /* check bounds then return if valid */
    if ( i >= M || j >= N ) throw exception::matrix_exception { "matrix indices are out of bounds" };
    return data.at ( ( j * N ) + i );
}
template<unsigned M, unsigned N, class T> inline const T& glh::math::matrix<M, N, T>::at ( const unsigned i, const unsigned j ) const
{
    /* check bounds then return if valid */
    if ( i >= M || j >= N ) throw exception::matrix_exception { "matrix indices are out of bounds" };
    return data.at ( ( j * N ) + i );
}



/* __at
 *
 * accesses the internal array
 * the internal array is in column-major order, hence the '__'
 */
template<unsigned M, unsigned N, class T>
T& glh::math::matrix<M, N, T>::__at ( const unsigned i )
{
    /* check bounds then return if valid */
    if ( i >= M * N ) throw exception::matrix_exception { "matrix indices are out of bounds" };
    return data.at ( i );
}
template<unsigned M, unsigned N, class T>
const T& glh::math::matrix<M, N, T>::__at ( const unsigned i ) const
{
    /* check bounds then return if valid */
    if ( i >= M * N ) throw exception::matrix_exception { "matrix indices are out of bounds" };
    return data.at ( i );
}



/* format_str
 *
 * format the matrix to a string
 */
template<unsigned M, unsigned N, class T> inline std::string glh::math::matrix<M, N, T>::format_str () const
{
    /* create stringstream to stream into */
    std::stringstream ss;
    /* double loop for rows ans columns */
    for ( unsigned i = 0; i < M; ++i ) 
    {
        for ( unsigned j = 0; j < N; ++j )
        {
            /* print value */
            ss << at ( i, j ) << ", ";
        }
        /* print end line */
        ss << std::endl;
    }
    /* return string */
    return ss.str ();
}



/* MATRIX MODIFIER FUNCTIONS IMPLEMENTATIONS */

/* promote_matrix
 *
 * promte a matrix to the preferred type give another type
 */
template<unsigned M, unsigned N, class T0, class T1> inline std::conditional_t<std::is_same<T0, std::common_type_t<T0, T1>>::value, const glh::math::matrix<M, N, T0>&, glh::math::matrix<M, N, std::common_type_t<T0, T1>>> glh::math::promote_matrix ( const matrix<M, N, T0>& lhs )
{
    return lhs;
}


/* transpose
 * 
 * _matrix: the matrix to transpose
 * 
 * return: the new matrix
 */
template<unsigned M, unsigned N, class T> inline glh::math::matrix<N, M, T> glh::math::transpose ( const matrix<M, N, T>& _matrix )
{
    /* create the new matrix */
    matrix<N, M, T> transp;

    /* double loop to set new values */
    for ( unsigned i = 0; i < M; ++i ) for ( unsigned j = 0; j < N; ++j ) transp.at ( i, j ) = _matrix.at ( j, i );

    /* return new matrix */
    return transp;
}

/* submatrix
 *
 * _matrix: the original matrix
 * i,j or i: row/column to remove, or the index of which to remove its row and column
 * 
 * return: the new transformed matrix
 */
template<unsigned M, unsigned N, class T> inline glh::math::matrix<M - 1, N - 1, T> glh::math::submatrix ( const matrix<M, N, T>& _matrix, const unsigned i, const unsigned j )
{
    /* check is within bounds */
    if ( i >= M || j >= N ) throw exception::matrix_exception { "matrix indices are out of bounds" };

    /* create the new matrix */
    matrix<M - 1, N - 1, T> submat;

    /* i loop */
    for ( unsigned iti = 0, subiti = 0; iti < M; ++iti )
    {
        /* if is the row we need to ignore, continue without incramenting subiti */
        if ( iti == i ) continue;
        /* j loop */
        for ( unsigned itj = 0, subitj = 0; itj < N; ++itj )
        {
            /* if is the columnn we need to ignore, continue without incramenting subitj */
            if ( itj == j ) continue;
            /* otherwise set the new value */
            submat.at ( subiti, subitj ) = _matrix.at ( iti, itj );
            /* incrament subitj */
            ++subitj;
        }
        /* incrament subiti */
        ++subiti;
    }

    /* return submatrux */
    return submat;
}

/* det
 *
 * _matrix: the matrix to find the determinant of
 *
 * return: the determinant
 */
template<unsigned M, class T> inline std::enable_if_t<( M > 1 ), T> glh::math::det ( const matrix<M, M, T>& _matrix )
{
    /* store the running determinant */
    T determinant = 0;

    /* multiplier for working out the determinant
     * starts as 1, and is multiplied by -1 for each value on the top row
     * this allows for the positive negative pattern to occur
     */
    int det_mult = 1;
    
    /* loop for each value in the top row */
    for ( unsigned i = 0; i < M; ++i )
    {
        /* add to the determinant */
        determinant += ( _matrix.at ( 0, i ) * minor ( _matrix, 0, i ) * det_mult );
        /* multiply det_mult by -1 */
        det_mult *= -1;
    }

    /* return the determinant */
    return determinant;
}
template<unsigned M, class T> inline std::enable_if_t<M == 1, T> glh::math::det ( const matrix<M, M, T>& _matrix )
{
    /* return the only value in the matrix */
    return _matrix.at ( 0, 0 );
}

/* minor
 *
 * _matrix: the matrix to find a minor of
 * i,j or i: the row/column, or the index to find the minor of
 * 
 * return: the minor of the element requested
 */
template<unsigned M, class T> inline T glh::math::minor ( const matrix<M, M, T>& _matrix, const unsigned i, const unsigned j )
{
    /* return the determinant of the submatrix given by i and j */
    return det ( submatrix ( _matrix, i, j ) );
}

/* inverse
 *
 * _matrix: the matrix to find the inverse of
 * 
 * return: the inverse matrix
 */
template<unsigned M, class T> inline std::enable_if_t<( M > 1 ), glh::math::matrix<M, M, T>> glh::math::inverse ( const matrix<M, M, T>& _matrix )
{
    /* get the determinant */
    const T determinant = det ( _matrix );

    /* if the determinant is 0, throw */
    if ( determinant == 0.0 ) throw exception::matrix_exception { "cannot find inverse of a singular matrix" };

    /* create the new matrix */
    matrix<M, M, T> cof;

    /* loop for each value in the new matrix */
    for ( unsigned i = 0; i < M; ++i ) for ( unsigned j = 0; j < M; ++j )
    {
        /* replace it with its cofactor value
         * this is found by multiplying the minor of the element by (-1)^i+j
         */
        cof.at ( i, j ) = minor ( _matrix, i, j ) * std::pow ( -1.0, i + j );
    }

    /* return the transpose of the new matrix divided the determinant of the original matrix */
    return transpose ( cof ) / determinant;
}
template<unsigned M, class T> inline std::enable_if_t<M == 1, glh::math::matrix<M, M, T>> glh::math::inverse ( const matrix<M, M, T>& _matrix )
{
    /* if only element is 0, throw */
    if ( _matrix.at ( 0 ) == 0 ) throw exception::matrix_exception { "cannot find inverse of a singular matrix" };
    /* return the only value in the matrix */
    return matrix<1, 1, T> { 1.0 / _matrix.at ( 0 ) };
}

/* pow
 *
 * raise a matrix to a power
 * a negative power uses the inverse as the base
 */
template<unsigned M, class T> inline glh::math::matrix<M, M, T> glh::math::pow ( const matrix<M, M, T>& lhs, const int rhs )
{
    /* produce new base based on sign of exp */
    matrix<M, M, T> base { ( exp >= 0 ? lhs : inverse ( lhs ) ) };

    /* make exp positive */
    unsigned exp = std::abs ( rhs );

    /* produce result matrix
     * not going to include glh_transform.hpp just for the identity matrix
     */
    matrix<M, M, T> result { 0 };
    for ( unsigned i = 0; i < M; ++i ) result.at ( i, i ) = 1.0;

    /* apply power */
    for ( unsigned i = 0; i < exp; ++i ) result = result * base;

    /* return result */
    return result;
}



/* MATRIX OPERATORS IMPLEMENTATION */

/* operator== and operator!=
 *
 * compares two matrices of the same size value by value
 */
template<unsigned M, unsigned N, class T0, class T1> inline bool operator== ( const glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs )
{
    /* return false if any elements differ */
    for ( unsigned i = 0; i < M * N; ++i ) if ( lhs.at ( i ) != rhs.at ( i ) ) return false;

    /* else return true */
    return true;
}
template<unsigned M, unsigned N, class T0, class T1> inline bool operator!= ( const glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs )
{
    /* return true if any elements differ */
    for ( unsigned i = 0; i < M * N; ++i ) if ( lhs.at ( i ) != rhs.at ( i ) ) return true;

    /* else return false */
    return false;
}

/* operator+(=)
 *
 * addition operations on matrices include:
 * 
 * matrix + matrix
 * matrix + scalar == scalar + matrix
 * matrix += matrix
 * matrix += scalar
 */
template<unsigned M, unsigned N, class T0, class T1> inline glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator+ ( const glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N, std::common_type_t<T0, T1>> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = lhs.at ( i ) + rhs.at ( i );

    /* return the result */
    return result;
}
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> inline glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator+ ( const glh::math::matrix<M, N, T0>& lhs, const T1& rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N, std::common_type_t<T0, T1>> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = lhs.at ( i ) + rhs;

    /* return the result */
    return result;
}
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T0>::value>...> inline glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator+ ( const T0& lhs,const glh::math::matrix<M, N, T1>& rhs )
{
    /* equivalent to matrix + scalar */
    return rhs + lhs;
}
template<unsigned M, unsigned N, class T0, class T1> inline glh::math::matrix<M, N, T0>& operator+= ( glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs )
{
    /* set lhs to the addition of lhs and rhs */
    return ( lhs = lhs + rhs );
}
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> inline glh::math::matrix<M, N, T0>& operator+= ( glh::math::matrix<M, N, T0>& lhs, const T1& rhs )
{
    /* set lhs to the addition of lhs and rhs */
    return ( lhs = lhs + rhs );
}

/* operator-(=)
 *
 * subtraction operations on matrices include:
 * 
 * matrix - matrix
 * matrix - scalar
 * matrix -= matrix
 * matrix -= scalar
 */
template<unsigned M, unsigned N, class T0, class T1> inline glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator- ( const glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N, std::common_type_t<T0, T1>> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = lhs.at ( i ) - rhs.at ( i );

    /* return the result */
    return result;
}
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> inline glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator- ( const glh::math::matrix<M, N, T0>& lhs, const T1& rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N, std::common_type_t<T0, T1>> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = lhs.at ( i ) - rhs;

    /* return the result */
    return result;
}
template<unsigned M, unsigned N, class T0, class T1> inline glh::math::matrix<M, N, T0>& operator-= ( glh::math::matrix<M, N, T0>& lhs, const glh::math::matrix<M, N, T1>& rhs )
{
    /* set lhs to the subtraction of lhs and rhs */
    return ( lhs = lhs - rhs );
}
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> inline glh::math::matrix<M, N, T0>& operator-= ( glh::math::matrix<M, N, T0>& lhs, const T1& rhs )
{
    /* set lhs to the subtraction of lhs and rhs */
    return ( lhs = lhs - rhs );
}

/* operator*(=)
 * 
 * multiplication operations on matrices include
 * 
 * matrix * matrix (of compatible sizes)
 * matrix * scalar == scalar * matrix
 * matrix *= matrix (SEE BELOW)
 * matrix *= scalar
 * 
 * NOTE: mat1 *= mat2 is equivalent to mat1 = MAT2 * MAT1
 *       this is for the purpose of adding transformations
 */
template<unsigned M0, unsigned N0M1, unsigned N1, class T0, class T1> inline glh::math::matrix<M0, N1, std::common_type_t<T0, T1>> operator* ( const glh::math::matrix<M0, N0M1, T0>& lhs, const glh::math::matrix<N0M1, N1, T1>& rhs )
{
    /* create the new matrix */
    glh::math::matrix<M0, N1, std::common_type_t<T0, T1>> result;

    /* double loop for each row/column of result */
    for ( unsigned i = 0; i < M0; ++i ) for ( unsigned j = 0; j < N1; ++j )
    {
        /* another loop to add up the product of the corresponding values of lhs and rhs*/
        for ( unsigned k = 0; k < N0M1; ++k ) result.at ( i, j ) += lhs.at ( i, k ) * rhs.at ( k, j );
    }

    /* return result */
    return result;
}
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> inline glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator* ( const glh::math::matrix<M, N, T0>& lhs, const T1& rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N, std::common_type_t<T0, T1>> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.__at ( i ) = lhs.__at ( i ) * rhs;

    /* return result */
    return result;
}
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T0>::value>...> inline glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator* ( const T0& lhs, const glh::math::matrix<M, N, T1>& rhs )
{
    /* equivalent to matrix * scalar */
    return rhs * lhs;
}
template<unsigned M0, unsigned N0M1, unsigned N1, class T0, class T1> inline glh::math::matrix<N0M1, N1, T0>& operator*= ( glh::math::matrix<N0M1, N1, T0>& lhs, const glh::math::matrix<M0, N0M1, T1>& rhs )
{
    /* set lhs to the multiplication of rhs * lhs */
    return ( lhs = rhs * lhs );
}
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> inline glh::math::matrix<M, N, T0>& operator*= ( glh::math::matrix<M, N, T0>& lhs, const T1& rhs )
{
    /* set lhs to the multiplication of lhs * rhs */
    return ( lhs = lhs * rhs );
}

/* operator/(=)
 *
 * division operations on matrices include:
 * 
 * matrix / scalar
 * matrix /= scalar
 * 
 * to get matrix division, use multiplication with inverse matrices
 */
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> inline glh::math::matrix<M, N, std::common_type_t<T0, T1>> operator/ ( const glh::math::matrix<M, N, T0>& lhs, const T1& rhs )
{
    /* return the multiplication of lhs and 1/rhs */
    return lhs * ( 1.0 / rhs );
}
template<unsigned M, unsigned N, class T0, class T1, std::enable_if_t<std::is_arithmetic<T1>::value>...> inline glh::math::matrix<M, N, T0>& operator/= ( glh::math::matrix<M, N, T0>& lhs, const T1& rhs )
{
    /* set lhs to the division of lhs / rhs */
    return ( lhs = lhs / rhs );
}

/* unary plus operator */
template<unsigned M, unsigned N, class T> inline glh::math::matrix<M, N, T>& operator+ ( const glh::math::matrix<M, N, T>& lhs )
{
    /* return the same matrix */
    return lhs;
}
/* unary minus operator */
template<unsigned M, unsigned N, class T> inline glh::math::matrix<M, N, T>& operator- ( const glh::math::matrix<M, N, T>& lhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N, T> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = -lhs.at ( i );

    /* return result */
    return result;
}

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M, unsigned N, class T> inline std::ostream& operator<< ( std::ostream& os, const glh::math::matrix<M, N, T>& _matrix )
{
    /* stream intro into ostream */
    os << "matrix<" << M << "," << N << ">{";
    /* loop for each value */
    for ( unsigned i = 0; i < M; ++i ) for ( unsigned j = 0; j < N; ++j )
    {
        /* stream the value */
        os << _matrix.at ( i, j );
        /* if not end of stream, output comma */
        if ( i * N + j < M * N ) os << ",";
    }
    /* stream closing } */
    os << "}";
    /* return os */
    return os;
}


/* #ifndef GLHELPER_MATRIX_HPP_INCLUDED */
#endif