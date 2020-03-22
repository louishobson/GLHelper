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
#include <initializer_list>
#include <iostream>
#include <cmath>

/* include glhelper_core.hpp */
#include <glhelper/glhelper_core.hpp>

/* include glhelper_exception.hpp */
#include <glhelper/glhelper_exception.hpp>



/* NAMESPACE FORWARD DECLARATIONS */

namespace glh
{
    /* class matrix
     *
     * a class representing a matrix of a size mxn
     */
    template<unsigned M, unsigned N = M> class matrix;

    /* typedef matrix<M, M> matM
     *
     * default matrix sizes
     */
    typedef matrix<1, 1> mat1;
    typedef matrix<2, 2> mat2;
    typedef matrix<3, 3> mat3;
    typedef matrix<4, 4> mat4;

    /* class matrix_exception : exception
     *
     * for exceptions related to matrices
     */
    class matrix_exception;
}



/* MATRIX DECLARATION */

/* class matrix
 *
 * a class representing a matrix of a size mxn
 */
template<unsigned M, unsigned N> class glh::matrix
{
public:

    /* zero-parameter constructor
     *
     * a matrix gets initialised to a zero matrix by default
     */
    matrix ()
        : data { 0. }
    {}

    /* initialiser list constructor
     *
     * constructs data from initialiser list
     */
    matrix ( std::initializer_list<float> init_list )
        : data { init_list }
    {}

    /* default copy constructor
     *
     * copy a matrix of the same size
     */
    matrix ( const matrix<M, N>& other ) = default;

    /* default move copy constructor */
    matrix ( matrix<M, N>&& other ) = default;

    /* default copy assignment operator */
    matrix& operator= ( const matrix<M, N>& other ) = default;

    /* default destructor */
    ~matrix () = default;



    /* at
     *
     * gets values from the matrix
     * 
     * i,j or i: the row/column coordinate, or the index
     */
    float& at ( const unsigned i, const unsigned j );
    const float& at ( const unsigned i, const unsigned j ) const;
    float& at ( const unsigned i );
    const float& at ( const unsigned i ) const;

    /* is_square
     *
     * return: boolean for if the matrix is square or not
     */
    bool is_square () const { return ( M == N ); }

    /* transpose
     *
     * return: the transpose of the matrix
     */
    matrix<N, M> transpose () const;

    /* submatrix
     *
     * i,j or i: the row/column to remove, or the index of which to remove its row and collumn
     * 
     * return: a submatrix found by removing a row and column
     */
    matrix<M - 1, N - 1> submatrix ( const unsigned i, const unsigned j ) const;
    matrix<M - 1, N - 1> submatrix ( const unsigned i ) const;

    /* minor
     *
     * i,j or i: the row/collumn, or the index to find the minor of
     * 
     * return: the minor of the index specified
     */
    float minor ( const unsigned i, const unsigned j ) const { return submatrix ( i, j ).determinant (); }
    float minor ( const unsigned i ) const { return submatrix ( i ).determinant (); }

    /* determinant
     *
     * throws if the matrix is not square
     * 
     * return: the determinant of the matrix
     */
    float determinant () const;

    /* is_singular
     *
     * throws if the matrix is not square
     * 
     * return: boolean for if the matrix is singular or not
     */
    bool is_singluar () const { return ( determinant () == 0 ); }

    /* inverse
     *
     * throws if not a square matrix (where M==N)
     * 
     * return: the inverse of the matrix
     */
    matrix<M, N> inverse () const;



    /* internal_data
     *
     * return: reference to data
     */
    std::array<float, M * N>& internal_data () { return data; }
    const std::array<float, M * N>& internal_data () const { return data; }

    /* internal_ptr
     *
     * return: pointer to the internal array of data
     */
    unsigned * internal_ptr () { return data.data (); }
    const unsigned * internal_ptr () const { return data.data (); }

private:

    /* array data
     *
     * the actual data of the matrix
     */
    std::array<float, M * N> data;

};



/* MATRIX OPERATORS DECLARATIONS */

/* operator+
 *
 * addition operations on matrices
 */
template<unsigned M, unsigned N> glh::matrix<M, N> operator+ ( const glh::matrix<M, N>& lhs, const glh::matrix<M, N>& rhs );
template<unsigned M, unsigned N> glh::matrix<M, N> operator+ ( const glh::matrix<M, N>& lhs, const float& rhs );

/* operator-
 *
 * subtraction operations on matrices
 */
template<unsigned M, unsigned N> glh::matrix<M, N> operator- ( const glh::matrix<M, N>& lhs, const glh::matrix<M, N>& rhs );
template<unsigned M, unsigned N> glh::matrix<M, N> operator- ( const glh::matrix<M, N>& lhs, const float& rhs );

/* operator*
 * 
 * multiplication operations on matrices
 */
template<unsigned M1, unsigned N1M2, unsigned N2> glh::matrix<M1, N2> operator* ( const glh::matrix<M1, N1M2>& lhs, const glh::matrix<N1M2, N2>& rhs );
template<unsigned M, unsigned N> glh::matrix<M, N> operator* ( const glh::matrix<M, N>& lhs, const float& rhs );

/* operator/
 *
 * division operations on matrices
 */
template<unsigned M1, unsigned N1M2, unsigned N2> glh::matrix<M1, N2> operator/ ( const glh::matrix<M1, N1M2>& lhs, const glh::matrix<N1M2, N2>& rhs );
template<unsigned M, unsigned N> glh::matrix<M, N> operator/ ( const glh::matrix<M, N>& lhs, const float& rhs );

/* unary plus operator */
template<unsigned M, unsigned N> glh::matrix<M, N> operator+ ( const glh::matrix<N, M>& lhs );
/* unary minus operator */
template<unsigned M, unsigned N> glh::matrix<M, N> operator- ( const glh::matrix<N, M>& lhs );

/* overload of std::pow
 *
 * raise a matrix to a power
 */
template<unsigned M, unsigned N> glh::matrix<M, N> std::pow ( const glh::matrix<M, N> base, const int exp );

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M, unsigned N> std::ostream& operator<< ( std::ostream& os, const glh::matrix<M, N>& obj );



/* MATRIX_EXCEPTION DECLARATION */

/* class matrix_exception : exception
 *
 * for exceptions related to matrices
 */
class glh::matrix_exception : public exception
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
    explicit matrix_exception () = default;

    /* default everything else and inherits what () function */

};



/* MATRIX IMPLEMENTATION */

/* at
 *
 * gets values from the matrix
 * 
 * i,j or i: the row/collum coordinate, or the index
 */
template<unsigned M, unsigned N> float& glh::matrix<M, N>::at ( const unsigned i, const unsigned j )
{
    /* check bounds then return if valid */
    if ( i >= M && j >= N ) throw matrix_exception { "matrix indices are out of bounds" };
    return data.at ( ( i * N ) + j );
}
template<unsigned M, unsigned N> const float& glh::matrix<M, N>::at ( const unsigned i, const unsigned j ) const
{
    /* check bounds then return if valid */
    if ( i >= M && j >= N ) throw matrix_exception { "matrix indices are out of bounds" };
    return data.at ( ( i * N ) + j );
}
template<unsigned M, unsigned N> float& glh::matrix<M, N>::at ( const unsigned i )
{
    /* check bounds then return if valid */
    if ( i >= M * N ) throw matrix_exception { "matrix indices are out of bounds" };
    return data.at ( i );
}
template<unsigned M, unsigned N> const float& glh::matrix<M, N>::at ( const unsigned i ) const
{
    /* check bounds then return if valid */
    if ( i >= M * N ) throw matrix_exception { "matrix indices are out of bounds" };
    return data.at ( i );
}

/* transpose
 *
 * return: the transpose of the matrix
 */
template<unsigned M, unsigned N> glh::matrix<N, M> glh::matrix<M, N>::transpose () const
{
    /* create the new matrix */
    matrix<N, M> transp;

    /* double loop to set new values */
    for ( unsigned iti = 0; iti < M; ++iti ) for ( unsigned itj = 0; itj < N; ++itj ) transp.at ( iti, itj ) = data.at ( itj, iti );

    /* return new matrix */
    return transp;
}

/* submatrix
 *
 * i,j or i: the row/column to remove, or the index of which to remove its row and collumn
 * 
 * return: a submatrix found by removing a row and column
 */
template<unsigned M, unsigned N> glh::matrix<M - 1, N - 1> glh::matrix<M, N>::submatrix ( const unsigned i, const unsigned j ) const
{
    /* check is within bounds */
    if ( i >= M && j >= N ) throw matrix_exception { "matrix indices are out of bounds" };

    /* create the new matrix */
    matrix<M - 1, N - 1> submat;

    /* subiti and subitj definition */
    unsigned subiti = 0, subitj = 0;

    /* i loop */
    for ( unsigned iti = 0; iti < M; ++iti )
    {
        /* if is the row we need to ignore, continue without incramenting subiti */
        if ( iti == i ) continue;
        /* j loop */
        for ( unsigned itj = 0; itj < N; ++itj )
        {
            /* if is the collumn we need to ignore, continue without incramenting subitj */
            if ( itj == j ) continue;
            /* otherwise set the new value */
            submat.at ( subiti, subitj ) = data.at ( iti, itj );
            /* incrament subitij */
            ++subitj;
        }
        /* incrament subiti */
        ++subiti;
    }

    /* return submatrux */
    return submat;
}
template<unsigned M, unsigned N> glh::matrix<M - 1, N - 1> glh::matrix<M, N>::submatrix ( const unsigned i ) const
{
    /* find the row and column and use the other overload */
    return submatrix ( ( unsigned ) i / N, i % N );
}

/* determinant
 *
 * throws if the matrix is not square
 * 
 * return: the determinant of the matrix
 */
template<unsigned M, unsigned N> float glh::matrix<M, N>::determinant () const
{
    /* throw if is not square */
    if ( !is_square () ) throw matrix_exception { "cannot find the determinant of a non-square matrix" };
    /* throw if is 0x0 matrix */
    if ( M == 0 ) throw matrix_exception ( "cannot find the determinant of a 0x0 matrix" );

    /* if is 1x1, return the only value */
    if ( M == 1 ) return data.at ( 0 );

    /* store the running determinant */
    float det = 0;
    /* multiplier for working out the determinant
     * starts as 1, and is multiplied by -1 for each value on the top row
     * this allows for the positive negative pattern to occur
     */
    float det_mult = 1.;
    
    /* loop for each value in the top row */
    for ( unsigned iti = 0; iti < N; ++iti )
    {
        /* add to the determinant */
        det += ( data.at ( iti ) * minor ( iti ) * det_mult );
        /* multiply det_mult by -1 */
        det_mult *= -1.;
    }

    /* return the determinant */
    return det;
}

/* inverse
 *
 * throws if not a square matrix (where M==N)
 * 
 * return: the inverse of the matrix
 */
template<unsigned M, unsigned N> glh::matrix<M, N> glh::matrix<M, N>::inverse () const
{
    /* create the new matrix */
    matrix<M, N> inv;

    /* multiplier for cofactors */
    float mult = 1.;

    /* loop for each value in the new matrix */
    for ( unsigned iti = 0; iti < M; ++iti )
    {
        /* replace it with its minor */
        inv.at ( iti ) = minor ( iti );
        /* replace it with its cofactor value */
        inv.at ( iti ) *= mult;
        /* multiply mult by -1 to keep up sign-swapping pattern*/
        mult *= -1.;
    }

    /* return the transpose of the new matrix divided the determinant of the original matrix */
    return inv.transpose () / determinant ();
}



/* MATRIX OPERATORS IMPLEMENTATION */

/* operator+
 *
 * addition operations on matrices
 */
template<unsigned M, unsigned N> glh::matrix<M, N> operator+ ( const glh::matrix<M, N>& lhs, const glh::matrix<M, N>& rhs )
{
    /* create the new matrix */
    glh::matrix<M, N> result;

    /* set its values */
    for ( unsigned iti = 0; iti < M * N; ++iti ) result.at ( iti ) = lhs.at ( iti ) + rhs.at ( iti );

    /* return the result */
    return result;
}
template<unsigned M, unsigned N> glh::matrix<M, N> operator+ ( const glh::matrix<M, N>& lhs, const float& rhs )
{
    /* create the new matrix */
    glh::matrix<M, N> result;

    /* set its values */
    for ( unsigned iti = 0; iti < M * N; ++iti ) result.at ( iti ) = lhs.at ( iti ) + rhs;

    /* return the result */
    return result;
}

/* operator-
 *
 * subtraction operations on matrices
 */
template<unsigned M, unsigned N> glh::matrix<M, N> operator- ( const glh::matrix<M, N>& lhs, const glh::matrix<M, N>& rhs )
{
    /* return the addition of lhs and -rhs */
    return lhs + -lhs;
}
template<unsigned M, unsigned N> glh::matrix<M, N> operator- ( const glh::matrix<M, N>& lhs, const float& rhs )
{
    /* return the addition of lhs and -rhs */
    return lhs + -rhs;
}

/* operator*
 * 
 * multiplication operations on matrices
 */
template<unsigned M1, unsigned N1M2, unsigned N2> glh::matrix<M1, N2> operator* ( const glh::matrix<M1, N1M2>& lhs, const glh::matrix<N1M2, N2>& rhs )
{
    /* create the new matrix */
    glh::matrix<M1, N2> result;

    /* double loop for each row/column of result */
    for ( unsigned iti = 0; iti < M1; ++iti ) for ( unsigned itj = 0; itj < N2; ++itj )
    {
        /* another loop to add up the product of the corresponding values of lhs and rhs*/
        for ( unsigned itk = 0; itk < N1M2; ++itk ) result.at ( iti, itj ) += lhs.at ( iti, itk ) * rhs.at ( itj, itk );
    }

    /* return result */
    return result;
}
template<unsigned M, unsigned N> glh::matrix<M, N> operator* ( const glh::matrix<M, N>& lhs, const float& rhs )
{
    /* create the new matrix */
    glh::matrix<M, N> result;

    /* set its values */
    for ( unsigned iti = 0; iti < M * N; ++iti ) result.at ( iti ) = lhs.at ( iti ) * rhs;

    /* return result */
    return result;
}

/* operator/
 *
 * division operations on matrices
 */
template<unsigned M1, unsigned N1M2, unsigned N2> glh::matrix<M1, N2> operator/ ( const glh::matrix<M1, N1M2>& lhs, const glh::matrix<N1M2, N2>& rhs )
{
    /* return the multiplication of lhs and the inverse of rhs */
    return lhs * rhs.inverse ();
}
template<unsigned M, unsigned N> glh::matrix<M, N> operator/ ( const glh::matrix<M, N>& lhs, const float& rhs )
{
    /* return the multiplication of lhs and 1/rhs */
    return lhs * ( 1 / rhs );
}

/* unary plus operator */
template<unsigned M, unsigned N> glh::matrix<M, N> operator+ ( const glh::matrix<N, M>& lhs )
{
    /* return the same matrix */
    return lhs;
}
/* unary minus operator */
template<unsigned M, unsigned N> glh::matrix<M, N> operator- ( const glh::matrix<N, M>& lhs )
{
    /* create the new matrix */
    glh::matrix<M, N> result;

    /* set its values */
    for ( unsigned iti = 0; iti < M * N; ++iti ) result.at ( iti ) = -lhs.at ( iti );

    /* return result */
    return result;
}

/* overload of std::pow
 *
 * raise a matrix to a power
 */
template<unsigned M, unsigned N> glh::matrix<M, N> std::pow ( const glh::matrix<M, N> base, const int exp )
{
    /* throw if is not square */
    if ( !base.is_square () ) throw glh::matrix_exception { "cannot raise non-square matrix to a power" };

    /* produce new base based on sign of exp */
    glh::matrix<M, N> newbase { ( exp >= 0 ? base : base.inverse () ) };

    /* make exp positive */
    unsigned newexp = std::abs ( exp );

    /* produce result matrix
     * not going to include glh_transform.hpp just for the identity matrix
     */
    glh::matrix<M, N> result;
    for ( unsigned iti = 0; iti < M * N; ++iti ) result.at ( iti, iti ) = 1.;

    /* apply power */
    for ( unsigned itexp = 0; itexp < newexp; ++itexp ) result *= newbase;

    /* return result */
    return result;
}

/* operator<<
 *
 * format as a one-line string
 */
template<unsigned M, unsigned N> std::ostream& operator<< ( std::ostream& os, const glh::matrix<M, N>& obj )
{
    /* stream intro into ostream */
    os << "glh::matrix<" << M << "," << N << ">{";
    /* loop for each value */
    for ( unsigned iti = 0; iti < M * N; ++iti )
    {
        /* stream the value */
        os << obj.at ( iti );
        /* if not end of stream, stream comma */
        if ( iti + 1 < M * N ) os << ",";
    }
    /* stream closing } */
    os << "}";
    /* return os */
    return os;
}


/* #ifndef GLHELPER_MATRIX_HPP_INCLUDED */
#endif