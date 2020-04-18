/*
 * Copyright (C) 2020 Louis Hobson <louis-hobson@hotmail.co.uk>. All Rights Reserved.
 * 
 * Distributed under MIT licence as a part of the GLHelper C++ library.
 * For details, see: https://github.com/louishobson/GLHelper/blob/master/LICENSE
 * 
 * include/glhelper/glhelper_matrix.hpp
 * 
 * implements matrix mathematics
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

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    namespace math
    {
        /* TYPES AND CLASSESS */

        /* class matrix
        *
        * a class representing a matrix of a size MxN
        */
        template<unsigned M, unsigned N = M> class matrix;

        /* typedef matrix<M, M> matM
        *
        * default matrix sizes
        */
        typedef matrix<1> mat1;
        typedef matrix<2> mat2;
        typedef matrix<3> mat3;
        typedef matrix<4> mat4;

        

        /* MATRIX MODIFIER FUNCTIONS DECLARATIONS */

        /* transpose
         * 
         * _matrix: the matrix to transpose
         * 
         * return: the new matrix
         */
        template<unsigned M, unsigned N> matrix<N, M> transpose ( const matrix<M, N>& _matrix );

        /* submatrix
         *
         * _matrix: the original matrix
         * i,j or i: row/column to remove, or the index of which to remove its row and column
         * 
         * return: the new transformed matrix
         */
        template<unsigned M, unsigned N> matrix<M - 1, N - 1> submatrix ( const matrix<M, N>& _matrix, const unsigned i, const unsigned j );
        template<unsigned M, unsigned N> matrix<M - 1, N - 1> submatrix ( const matrix<M, N>& _matrix, const unsigned i );

        /* det
         *
         * _matrix: the matrix to find the determinant of
         *
         * return: the determinant
         */
        template<unsigned M> double det ( const matrix<M>& _matrix );
        template<> double det<1> ( const matrix<1>& _matrix );

        /* minor
         *
         * _matrix: the matrix to find a minor of
         * i,j or i: the row/column, or the index to find the minor of
         * 
         * return: the minor of the element requested
         */
        template<unsigned M> double minor ( const matrix<M>& _matrix, const unsigned i, const unsigned j );
        template<unsigned M> double minor ( const matrix<M>& _matrix, const unsigned i );

        /* inverse
         *
         * _matrix: the matrix to find the inverse of
         * 
         * return: the inverse matrix
         */
        template<unsigned M> matrix<M> inverse ( const matrix<M>& _matrix );
        template<> matrix<1> inverse<1> ( const matrix<1>& _matrix );
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



/* MATRIX DEFINITION */

/* class matrix
 *
 * a class representing a matrix of a size MxN
 */
template<unsigned M, unsigned N> class glh::math::matrix
{

    /* static assert that M > 0 && N > 0 */
    static_assert ( M > 0 && N > 0, "matrix cannot have a 0 dimension" );

public:

    /* zero-parameter constructor
     *
     * a matrix gets initialised to a zero matrix by default
     */
    matrix () { data.fill ( 0.0 ); }

    /* double constructor
     *
     * sets all values to the double provided
     */
    explicit matrix ( const double val ) { data.fill ( val ); }

    /* initialiser list constructor
     *
     * constructs data from initialiser list
     */
    explicit matrix ( const std::initializer_list<double> init_list );

    /* default copy constructor
     *
     * copy a matrix of the same size
     */
    matrix ( const matrix<M, N>& other ) = default;

    /* default copy assignment operator */
    matrix<M, N>& operator= ( const matrix<M, N>& other ) = default;

    /* default destructor */
    ~matrix () = default;



    /* at
     *
     * gets values from the matrix
     * 
     * i,j or i: the row/column coordinate, or the index
     */
    double& at ( const unsigned i, const unsigned j );
    const double& at ( const unsigned i, const unsigned j ) const;
    double& at ( const unsigned i );
    const double& at ( const unsigned i ) const;



    /* internal_data
     *
     * return: reference to data
     */
    std::array<double, M * N>& internal_data () { return data; }
    const std::array<double, M * N>& internal_data () const { return data; }

    /* internal_ptr
     *
     * return: pointer to the internal array of data
     */
    double * internal_ptr () { return data.data (); }
    const double * internal_ptr () const { return data.data (); }

    /* export_data
     *
     * return: an std::array which can be used with OpenGL
     */
    std::array<float, M * N> export_data () const;

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
    std::array<double, M * N> data;

};



/* MATRIX OPERATORS DECLARATIONS */

/* operator== and operator!=
 *
 * compares two matrices of the same size value by value
 */
template<unsigned M, unsigned N> bool operator== ( const glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs );
template<unsigned M, unsigned N> bool operator!= ( const glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs );

/* operator+(=)
 *
 * addition operations on matrices include:
 * 
 * matrix + matrix
 * matrix + scalar == scalar + matrix
 * matrix += matrix
 * matrix += scalar
 */
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator+ ( const glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator+ ( const glh::math::matrix<M, N>& lhs, const double rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator+ ( const double lhs,const glh::math::matrix<M, N>& rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N>& operator+= ( glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N>& operator+= ( glh::math::matrix<M, N>& lhs, const double rhs );

/* operator-(=)
 *
 * subtraction operations on matrices include:
 * 
 * matrix - matrix
 * matrix - scalar
 * matrix -= matrix
 * matrix -= scalar
 */
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator- ( const glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator- ( const glh::math::matrix<M, N>& lhs, const double rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N>& operator-= ( glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N>& operator-= ( glh::math::matrix<M, N>& lhs, const double rhs );

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
template<unsigned M0, unsigned N0M1, unsigned N1> glh::math::matrix<M0, N1> operator* ( const glh::math::matrix<M0, N0M1>& lhs, const glh::math::matrix<N0M1, N1>& rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator* ( const glh::math::matrix<M, N>& lhs, const double rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator* ( const double lhs, const glh::math::matrix<M, N>& rhs );
template<unsigned M0, unsigned N0M1, unsigned N1> glh::math::matrix<M0, N1>& operator*= ( glh::math::matrix<M0, N0M1>& lhs, const glh::math::matrix<N0M1, N1>& rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N>& operator*= ( glh::math::matrix<M, N>& lhs, const double rhs );

/* operator/(=)
 *
 * division operations on matrices include:
 * 
 * matrix / scalar
 * matrix /= scalar
 * 
 * to get matrix division, use multiplication with inverse matrices
 */
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator/ ( const glh::math::matrix<M, N>& lhs, const double rhs );
template<unsigned M, unsigned N> glh::math::matrix<M, N>& operator/= ( glh::math::matrix<M, N>& lhs, const double rhs );

/* unary plus operator */
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator+ ( const glh::math::matrix<N, M>& lhs );
/* unary minus operator */
template<unsigned M, unsigned N> glh::math::matrix<M, N> operator- ( const glh::math::matrix<N, M>& lhs );

/* overload of std::pow
 *
 * raise a matrix to a power
 * a negative power uses the determinant as the base
 */
template<unsigned M> glh::math::matrix<M> std::pow ( const glh::math::matrix<M>& base, const int exp );

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M, unsigned N> std::ostream& operator<< ( std::ostream& os, const glh::math::matrix<M, N>& _matrix );



/* MATRIX_EXCEPTION DECLARATION */

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
        : exception ( __what )
    {}

    /* default zero-parameter constructor
     *
     * construct matrix_exception with no descrption
     */
    matrix_exception () = default;

    /* default everything else and inherits what () function */

};



/* MATRIX IMPLEMENTATION */

/* initialiser list constructor
 *
 * constructs data from initialiser list
 */
template<unsigned M, unsigned N> inline glh::math::matrix<M, N>::matrix ( const std::initializer_list<double> init_list )
{
    /* check the size of the list */
    if ( init_list.size () != M * N ) throw exception::matrix_exception { "matrix initialiser list is invalid" };
    /* set the values */
    for ( auto it = init_list.begin (); it < init_list.end (); ++it ) at ( it - init_list.begin () ) = * it;
}

/* at
 *
 * gets values from the matrix
 * 
 * i,j or i: the row/collum coordinate, or the index
 */
template<unsigned M, unsigned N> inline double& glh::math::matrix<M, N>::at ( const unsigned i, const unsigned j )
{
    /* check bounds then return if valid */
    if ( i >= M || j >= N ) throw exception::matrix_exception { "matrix indices are out of bounds" };
    return data.at ( ( i * N ) + j );
}
template<unsigned M, unsigned N> inline const double& glh::math::matrix<M, N>::at ( const unsigned i, const unsigned j ) const
{
    /* check bounds then return if valid */
    if ( i >= M || j >= N ) throw exception::matrix_exception { "matrix indices are out of bounds" };
    return data.at ( ( i * N ) + j );
}
template<unsigned M, unsigned N> inline double& glh::math::matrix<M, N>::at ( const unsigned i )
{
    /* check bounds then return if valid */
    if ( i >= M * N ) throw exception::matrix_exception { "matrix indices are out of bounds" };
    return data.at ( i );
}
template<unsigned M, unsigned N> inline const double& glh::math::matrix<M, N>::at ( const unsigned i ) const
{
    /* check bounds then return if valid */
    if ( i >= M * N ) throw exception::matrix_exception { "matrix indices are out of bounds" };
    return data.at ( i );
}

/* export_data
 *
 * return: an std::array which can be used with OpenGL
 */
template<unsigned M, unsigned N> std::array<float, M * N> glh::math::matrix<M, N>::export_data () const
{   
    /* create the new array */
    std::array<float, M * N> new_data;

    /* copy elements over, but transpose them at the same time (as thats what OpenGL expects) */
    for ( unsigned i = 0; i < M; ++i ) for ( unsigned j = 0; j < N; ++j ) 
    {
        new_data.at ( ( i * N ) + j ) = at ( j, i );
    }
    /* return the float data */
    return new_data;
}

/* format_str
 *
 * format the matrix to a string
 */
template<unsigned M, unsigned N> inline std::string glh::math::matrix<M, N>::format_str () const
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

/* transpose
 * 
 * _matrix: the matrix to transpose
 * 
 * return: the new matrix
 */
template<unsigned M, unsigned N> inline glh::math::matrix<N, M> glh::math::transpose ( const matrix<M, N>& _matrix )
{
    /* create the new matrix */
    matrix<N, M> transp;

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
template<unsigned M, unsigned N> inline glh::math::matrix<M - 1, N - 1> glh::math::submatrix ( const matrix<M, N>& _matrix, const unsigned i, const unsigned j )
{
    /* check is within bounds */
    if ( i >= M || j >= N ) throw exception::matrix_exception { "matrix indices are out of bounds" };

    /* create the new matrix */
    matrix<M - 1, N - 1> submat;

    /* i loop */
    for ( unsigned iti = 0, subiti = 0; iti < M; ++iti )
    {
        /* if is the row we need to ignore, continue without incramenting subiti */
        if ( iti == i ) continue;
        /* j loop */
        for ( unsigned itj = 0, subitj = 0; itj < N; ++itj )
        {
            /* if is the collumn we need to ignore, continue without incramenting subitj */
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
template<unsigned M, unsigned N> inline glh::math::matrix<M - 1, N - 1> glh::math::submatrix ( const matrix<M, N>& _matrix, const unsigned i )
{
    /* return the submatrix given by expanding i to i and j */
    return submatrix ( _matrix, ( unsigned ) i / N, i % N );
}

/* det
 *
 * _matrix: the matrix to find the determinant of
 *
 * return: the determinant
 */
template<unsigned M> inline double glh::math::det ( const matrix<M>& _matrix )
{
    /* store the running determinant */
    double det = 0.0;

    /* multiplier for working out the determinant
     * starts as 1, and is multiplied by -1 for each value on the top row
     * this allows for the positive negative pattern to occur
     */
    double det_mult = 1.0;
    
    /* loop for each value in the top row */
    for ( unsigned i = 0; i < M; ++i )
    {
        /* add to the determinant */
        det += ( _matrix.at ( i ) * minor ( _matrix, i ) * det_mult );
        /* multiply det_mult by -1 */
        det_mult *= -1.0;
    }

    /* return the determinant */
    return det;
}
template<> inline double glh::math::det<1> ( const matrix<1>& _matrix )
{
    /* return the only value in the matrix */
    return _matrix.at ( 0 );
}

/* minor
 *
 * _matrix: the matrix to find a minor of
 * i,j or i: the row/column, or the index to find the minor of
 * 
 * return: the minor of the element requested
 */
template<unsigned M> inline double glh::math::minor ( const matrix<M>& _matrix, const unsigned i, const unsigned j )
{
    /* return the determinant of the submatrix given by i and j */
    return det ( submatrix ( _matrix, i, j ) );
}
template<unsigned M> inline double glh::math::minor ( const matrix<M>& _matrix, const unsigned i )
{
    /* return the determinant of the submatrix given by i */
    return det ( submatrix ( _matrix, i ) );
}

/* inverse
 *
 * _matrix: the matrix to find the inverse of
 * 
 * return: the inverse matrix
 */
template<unsigned M> inline glh::math::matrix<M> glh::math::inverse ( const matrix<M>& _matrix )
{
    /* get the determinant */
    const double determinant = det ( _matrix );

    /* if the determinant is 0, throw */
    if ( determinant == 0.0 ) throw exception::matrix_exception { "cannot find inverse of a singular matrix" };

    /* create the new matrix */
    matrix<M> cof;

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
template<> inline glh::math::matrix<1> glh::math::inverse<1> ( const matrix<1>& _matrix )
{
    /* if only element is 0, throw */
    if ( _matrix.at ( 0 ) == 0.0 ) throw exception::matrix_exception { "cannot find inverse of a singular matrix" };
    /* return the only value in the matrix */
    return matrix<1> { 1.0 / _matrix.at ( 0 ) };
}



/* MATRIX OPERATORS IMPLEMENTATION */

/* operator== and operator!=
 *
 * compares two matrices of the same size value by value
 */
template<unsigned M, unsigned N> inline bool operator== ( const glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs )
{
    /* return false if any elements differ */
    for ( unsigned i = 0; i < M * N; ++i ) if ( lhs.at ( i ) != rhs.at ( i ) ) return false;

    /* else return true */
    return true;
}
template<unsigned M, unsigned N> inline bool operator!= ( const glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs )
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
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator+ ( const glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = lhs.at ( i ) + rhs.at ( i );

    /* return the result */
    return result;
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator+ ( const glh::math::matrix<M, N>& lhs, const double rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = lhs.at ( i ) + rhs;

    /* return the result */
    return result;
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator+ ( const double lhs,const glh::math::matrix<M, N>& rhs )
{
    /* equivalent to matrix + scalar */
    return rhs + lhs;
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N>& operator+= ( glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs )
{
    /* set lhs to the addition of lhs and rhs */
    return ( lhs = lhs + rhs );
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N>& operator+= ( glh::math::matrix<M, N>& lhs, const double rhs )
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
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator- ( const glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = lhs.at ( i ) - rhs.at ( i );

    /* return the result */
    return result;
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator- ( const glh::math::matrix<M, N>& lhs, const double rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = lhs.at ( i ) - rhs;

    /* return the result */
    return result;
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N>& operator-= ( glh::math::matrix<M, N>& lhs, const glh::math::matrix<M, N>& rhs )
{
    /* set lhs to the subtraction of lhs and rhs */
    return ( lhs = lhs - rhs );
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N>& operator-= ( glh::math::matrix<M, N>& lhs, const double rhs )
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
template<unsigned M0, unsigned N0M1, unsigned N1> inline glh::math::matrix<M0, N1> operator* ( const glh::math::matrix<M0, N0M1>& lhs, const glh::math::matrix<N0M1, N1>& rhs )
{
    /* create the new matrix */
    glh::math::matrix<M0, N1> result;

    /* double loop for each row/column of result */
    for ( unsigned i = 0; i < M0; ++i ) for ( unsigned j = 0; j < N1; ++j )
    {
        /* another loop to add up the product of the corresponding values of lhs and rhs*/
        for ( unsigned k = 0; k < N0M1; ++k ) result.at ( i, j ) += lhs.at ( i, k ) * rhs.at ( k, j );
    }

    /* return result */
    return result;
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator* ( const glh::math::matrix<M, N>& lhs, const double rhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = lhs.at ( i ) * rhs;

    /* return result */
    return result;
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator* ( const double lhs, const glh::math::matrix<M, N>& rhs )
{
    /* equivalent to matrix * scalar */
    return rhs * lhs;
}
template<unsigned M0, unsigned N0M1, unsigned N1> inline glh::math::matrix<M0, N1>& operator*= ( glh::math::matrix<M0, N0M1>& lhs, const glh::math::matrix<N0M1, N1>& rhs )
{
    /* set lhs to the multiplication of rhs * lhs */
    return ( lhs = rhs * lhs );
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N>& operator*= ( glh::math::matrix<M, N>& lhs, const double rhs )
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
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator/ ( const glh::math::matrix<M, N>& lhs, const double rhs )
{
    /* return the multiplication of lhs and 1/rhs */
    return lhs * ( 1.0 / rhs );
}
template<unsigned M, unsigned N> inline glh::math::matrix<M, N>& operator/= ( glh::math::matrix<M, N>& lhs, const double rhs )
{
    /* set lhs to the division of lhs / rhs */
    return ( lhs = lhs / rhs );
}

/* unary plus operator */
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator+ ( const glh::math::matrix<N, M>& lhs )
{
    /* return the same matrix */
    return lhs;
}
/* unary minus operator */
template<unsigned M, unsigned N> inline glh::math::matrix<M, N> operator- ( const glh::math::matrix<N, M>& lhs )
{
    /* create the new matrix */
    glh::math::matrix<M, N> result;

    /* set its values */
    for ( unsigned i = 0; i < M * N; ++i ) result.at ( i ) = -lhs.at ( i );

    /* return result */
    return result;
}

/* overload of std::pow
 *
 * raise a matrix to a power
 * a negative power uses the determinant as the base
 */
template<unsigned M> inline glh::math::matrix<M> std::pow ( const glh::math::matrix<M>& base, const int exp )
{
    /* produce new base based on sign of exp */
    glh::math::matrix<M> newbase { ( exp >= 0 ? base : inverse ( base ) ) };

    /* make exp positive */
    unsigned newexp = std::abs ( exp );

    /* produce result matrix
     * not going to include glh_transform.hpp just for the identity matrix
     */
    glh::math::matrix<M> result;
    for ( unsigned i = 0; i < M; ++i ) result.at ( i, i ) = 1.0;

    /* apply power */
    for ( unsigned i = 0; i < newexp; ++i ) result = result * newbase;

    /* return result */
    return result;
}

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M, unsigned N> inline std::ostream& operator<< ( std::ostream& os, const glh::math::matrix<M, N>& _matrix )
{
    /* stream intro into ostream */
    os << "glh::matrix<" << M << "," << N << ">{";
    /* loop for each value */
    for ( unsigned i = 0; i < M * N; ++i )
    {
        /* stream the value */
        os << _matrix.at ( i );
        /* if not end of stream, stream comma */
        if ( i + 1 < M * N ) os << ",";
    }
    /* stream closing } */
    os << "}";
    /* return os */
    return os;
}


/* #ifndef GLHELPER_MATRIX_HPP_INCLUDED */
#endif