/***************************************************************************
 *   Copyright (C) 2007-2013 by the resistivity.net development team       *
 *   Carsten Rücker carsten@resistivity.net                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _GIMLI_MATRIX__H
#define _GIMLI_MATRIX__H

#include "gimli.h"
#include "pos.h"
#include "vector.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <cerrno>

#ifdef USE_THREADS
    #if USE_BOOST_THREAD
        #include <boost/thread.hpp>
    #endif // USE_BOOST_THREAD
#endif // USE_THREADS

namespace GIMLI{

template < class ValueType > class DLLEXPORT Matrix3 {
public:
    ValueType mat_[9];
    
    Matrix3()
        : valid_(false){}
        
    Matrix3(const Matrix3 < ValueType > & m ){
        mat_[0] = m.mat_[0]; mat_[1] = m.mat_[1]; mat_[2] = m.mat_[2];
        mat_[3] = m.mat_[3]; mat_[4] = m.mat_[4]; mat_[5] = m.mat_[5];
        mat_[6] = m.mat_[6]; mat_[7] = m.mat_[7]; mat_[8] = m.mat_[8];
    }
     
    inline ValueType & operator [](Index i){ return mat_[i];}
    inline const ValueType & operator [](Index i) const { return mat_[i];}
    
    #define DEFINE_UNARY_MOD_OPERATOR__(OP, NAME) \
    inline Matrix3 < ValueType > & operator OP##= (const ValueType & val) { \
        for (register Index i = 0; i < 9; i += 3) {\
            mat_[i] OP##= val; mat_[i+1] OP##= val; mat_[i+2] OP##= val; } return *this; \
    }

    DEFINE_UNARY_MOD_OPERATOR__(+, PLUS)
    DEFINE_UNARY_MOD_OPERATOR__(-, MINUS)
    DEFINE_UNARY_MOD_OPERATOR__(/, DIVID)
    DEFINE_UNARY_MOD_OPERATOR__(*, MULT)

    #undef DEFINE_UNARY_MOD_OPERATOR__
    
    void clear(){
        mat_[0] = 0.0; mat_[1] = 0.0; mat_[2] = 0.0;
        mat_[3] = 0.0; mat_[4] = 0.0; mat_[5] = 0.0;
        mat_[6] = 0.0; mat_[7] = 0.0; mat_[8] = 0.0;
    }
    
    inline const Vector < ValueType > col(Index i) const { 
        Vector < ValueType >ret(3);
        ret[0] = mat_[i]; ret[1] = mat_[3 + i]; ret[2] = mat_[6 + i];
        return ret;
    }
    
    inline const Vector < ValueType > row(Index i) const { 
        Vector < ValueType >ret(3);
        ret[0] = mat_[i * 3]; ret[1] = mat_[i * 3 + 1]; ret[2] = mat_[i * 3 + 2];
        return ret;
    }
    
    inline void setVal(RVector v, Index i){ 
        mat_[i * 3] = v[0]; mat_[i * 3 + 1] = v[1]; mat_[i * 3 + 2] = v[2];
    }
    
    inline void setValid(bool v){valid_ = v;}
    
    inline bool valid() const {return valid_;}

protected:
    
    bool valid_;
};

template < class ValueType > 
std::ostream & operator << (std::ostream & str, const Matrix3 < ValueType > & vec){
    for (Index i = 0; i < 3; i ++) str << vec[i] << " "; str << std::endl;
    for (Index i = 0; i < 3; i ++) str << vec[3 + i] << " "; str << std::endl;
    for (Index i = 0; i < 3; i ++) str << vec[6 + i] << " "; str << std::endl;
    return str;
}



template < class ValueType > \
Pos< ValueType > operator * (const Matrix3 < ValueType > & A, const Pos < ValueType > & b) { 
    return Pos< ValueType > (A[0] * b[0] + A[1] * b[1] + A[2] * b[2], 
                             A[3] * b[0] + A[4] * b[1] + A[5] * b[2],
                             A[6] * b[0] + A[7] * b[1] + A[8] * b[2]);
}

//! Interface class for matrices.
/*! Pure virtual interface class for matrices.
 * If you want your own Jacobian matrix to be used in \ref Inversion or \ref ModellingBase
 you have to derive your matrix from this class and implement all necessary members. */
   
class DLLEXPORT MatrixBase{
public:

    /*! Default constructor. */
    MatrixBase(){}

    /*! Default destructor. */
    virtual ~MatrixBase(){}

    /*! Return entity rtti value. */
    virtual uint rtti() const { return GIMLI_MATRIXBASE_RTTI; }
    
    /*! Return number of cols */
    virtual Index rows() const {
       THROW_TO_IMPL
       return 0;
    }

    /*! Return number of colums */
    virtual Index cols() const {
        THROW_TO_IMPL
        return 0;
    }

//     /*! Resize this matrix to rows, cols */
//     virtual void resize(Index rows, Index cols) = 0;

    /*! Clear the data, set size to zero and frees memory. */
    virtual void clear() {
        THROW_TO_IMPL
    }

    /*! Return this * a  */
    virtual RVector mult(const RVector & a) const {
       THROW_TO_IMPL
       return RVector(rows());
    }

    /*! Return this.T * a */
    virtual RVector transMult(const RVector & a) const {
        THROW_TO_IMPL
        return RVector(cols());
    }

    /*! Save this matrix into the file filename given. */
    virtual void save(const std::string & filename) const {
        THROW_TO_IMPL
    }

protected:
    
    
};

//! Identity matrix: derived from matrixBase
class DLLEXPORT IdentityMatrix : public MatrixBase {
public:
    /*! Default constructor (empty matrix). */
    IdentityMatrix() : nrows_(0), val_(0.0){}

    /*! Constructor with number of rows/colums. */
    IdentityMatrix(Index nrows, double val = 1.0) : nrows_(nrows), val_(val){}

    /*! Default destructor. */
    virtual ~IdentityMatrix(){}

    /*! Return number of cols */
    virtual Index rows() const { return nrows_; }

    /*! Return number of cols */
    virtual Index cols() const { return nrows_; }

    /*! Return this * a  */
    virtual RVector mult(const RVector & a) const { 
        if (a.size() != nrows_) { 
            throwLengthError(1, WHERE_AM_I + " vector/matrix lengths do not match " +
                                  toStr(nrows_) + " " + toStr(a.size()));
        }
        return a * val_;
    }

    /*! Return this.T * a */
    virtual RVector transMult(const RVector & a) const { 
        if (a.size() != nrows_) {
            throwLengthError(1, WHERE_AM_I + " matrix/vector lengths do not match " +
                                 toStr(a.size()) + " " + toStr(nrows_));
        }
        return a * val_;
    }

protected:
    Index nrows_;
    double val_;
};

//! Simple row-based dense matrix based on \ref Vector
/*! Simple row-based dense matrix based on \ref Vector */
template < class ValueType > class DLLEXPORT Matrix : public MatrixBase {
public:
    /*! Constructs an empty matrix with the dimension rows x cols. Content of the matrix is zero. */
    Matrix(Index rows=0, Index cols=0){
        resize(rows, cols);
    }

    /*! Copy constructor */
    Matrix(const std::vector < Vector< ValueType > > & mat){ copy_(mat); }

    /*! Constructor, read matrix from file see \ref load(Matrix < ValueType > & A, const std::string & filename). */
    Matrix(const std::string & filename){ load(*this, filename); }

    /*! Copyconstructor */
    Matrix(const Matrix < ValueType > & mat){ copy_(mat); }

    /*! Assignment operator */
    Matrix < ValueType > & operator = (const Matrix< ValueType > & mat){
        if (this != & mat){
            copy_(mat);
        } return *this;
    }

    /*! Destruct matrix and free memory. */
    virtual ~Matrix(){}

    /*! Return entity rtti value. */
    virtual uint rtti() const { return GIMLI_MATRIX_RTTI; }
    
    #define DEFINE_UNARY_MOD_OPERATOR__(OP, NAME) \
    inline Matrix < ValueType > & operator OP##= (const Matrix < ValueType > & A) { \
        for (register Index i = 0; i < mat_.size(); i ++) mat_[i] OP##= A[i]; return *this; } \
    inline Matrix < ValueType > & operator OP##= (const ValueType & val) { \
        for (register Index i = 0; i < mat_.size(); i ++) mat_[i] OP##= val; return *this; } \

    DEFINE_UNARY_MOD_OPERATOR__(+, PLUS)
    DEFINE_UNARY_MOD_OPERATOR__(-, MINUS)
    DEFINE_UNARY_MOD_OPERATOR__(/, DIVID)
    DEFINE_UNARY_MOD_OPERATOR__(*, MULT)

    #undef DEFINE_UNARY_MOD_OPERATOR__

//     Index col = cols();
//         for (register Index i = 0; i < mat_.size(); i ++) {
//             ValueType * Aj = &mat_[i][0];
//             ValueType * Aje = &mat_[i][col];
//             for (; Aj != Aje;) *Aj++ OP##= val;
//         }   return *this; }

    /*! Index operator for write operations without boundary check. */
    Vector< ValueType > & operator [] (Index i) {
// //         if (i < 0 || i > mat_.size()-1) {
//             throwLengthError(1, WHERE_AM_I + " row bounds out of range " +
//                                 toStr(i) + " " + toStr(this->rows())) ;
//         }
        return mat_[i];
    }

    /*! Read only C style index operator, without boundary check. */
    const Vector< ValueType > & operator [] (Index i) const { return mat_[i]; }

    /*! Implicite type converter. */
    template < class T > operator Matrix< T >(){
        Matrix< T > f(this->rows());
        for (uint i = 0; i < this->rows(); i ++){ f[i] = Vector < T >(mat_[i]); }
        return f;
    }

    /*! Resize the matrix to rows x cols. */
    virtual void resize(Index rows, Index cols){ allocate_(rows, cols); }

    /*! Clear the matrix and free memory. */
    inline void clear() { mat_.clear(); }

    /*! Return number of rows. */
    inline Index rows() const { return mat_.size(); }

    /*! Return number of colums. */
    inline Index cols() const { if (mat_.size() > 0) return mat_[0].size(); return 0; }

    /*! Set a value. Throws out of range exception if index check fails. */
    inline void setVal(const Vector < ValueType > & val, Index i) {
        if (i >= 0 && i < mat_.size()) {
            mat_[i] = val;
        } else {
            throwRangeError(1, WHERE_AM_I, i, 0, this->rows());
        }
    }

    /*! Readonly getter. */
    inline const Vector < ValueType > & getVal(Index i) const {
        if (i < 0 || i > mat_.size()-1) {
            throwLengthError(1, WHERE_AM_I + " row bounds out of range " +
                                toStr(i) + " " + toStr(this->rows())) ;
        }
        return mat_[i];
    }

    /*! Return reference to row. Used for pygimli. */
    inline Vector < ValueType > & rowR(Index i) {
        if (i < 0 || i > mat_.size()-1) {
            throwLengthError(1, WHERE_AM_I + " row bounds out of range " +
                                toStr(i) + " " + toStr(this->rows())) ;
        }
        return mat_[i];
    }

    /*! Readonly row of matrix, with boundary check.*/
    const Vector< ValueType > & row(Index i) const { return getVal(i); }

    /*! Readonly col of matrix, with boundary check. Probably slow.*/
    Vector< ValueType > col(Index i) const {
        if (i < 0 || i > this->cols()-1) {
            throwLengthError(1, WHERE_AM_I + " col bounds out of range " +
                                toStr(i) + " " + toStr(this->cols())) ;
        }
        Vector < ValueType > col(this->rows());
        for (Index j = 0, jmax = rows(); j < jmax; j ++) col[j] = mat_[j][i];
        return col;
    }

    /*! Add another row vector add the end. */
    inline void push_back(const Vector < ValueType > & vec) {
        //**!!! length check necessary
        mat_.push_back(vec);
        rowFlag_.resize(rowFlag_.size() + 1);
    }

    /*! Return last row vector. */
    inline Vector< ValueType > & back() { return mat_.back(); }

    /*! Set one specific column */
    inline void setCol(uint col, const Vector < ValueType > & v){
        if (col < 0 || col > this->cols()-1) {
            throwLengthError(1, WHERE_AM_I + " col bounds out of range " +
                                toStr(col) + " " + toStr(this->cols())) ;
        }
        if (v.size() > this->rows()) {
            throwLengthError(1, WHERE_AM_I + " rows bounds out of range " +
                                toStr(v.size()) + " " + toStr(this->rows())) ;
        }
        for (uint i = 0; i < v.size(); i ++) mat_[i][col] = v[i];
    }

    /*! Return reference to row flag vector. Maybee you can check if the rows are valid. Size is set automatic to the amount of rows. */
    BVector & rowFlag(){ return rowFlag_; }

    /*! Multiplication (A*b) with a vector of the same value type. */
    Vector < ValueType > mult(const Vector < ValueType > & b) const {
        Index cols = this->cols();
        Index rows = this->rows();

        Vector < ValueType > ret(rows, 0.0);

        //register ValueType tmpval = 0;
        if (b.size() == cols){
            for (register Index i = 0; i < rows; ++i){
                ret[i] = sum((*this)[i] * b);
            }
        } else {
            throwLengthError(1, WHERE_AM_I + " " + toStr(cols) + " != " + toStr(b.size()));
        }
        return ret;
    }

    /*! Multiplication (A*b) with a part of a vector between two defined indices. */
    Vector < ValueType > mult(const Vector < ValueType > & b, Index startI, Index endI) const {
        Index cols = this->cols();
        Index rows = this->rows();
        Index bsize = Index(endI - startI);

        if (bsize != cols) {
            throwLengthError(1, WHERE_AM_I + " " + toStr(cols) + " < " + toStr(endI) + "-" + toStr(startI));
        }
        Vector < ValueType > ret(rows, 0.0);
        for (register Index i = 0; i < rows; ++i){
            for (register Index j = startI; j < endI; j++) {
                ret[i] += (*this)[i][j] * b[j];
            }
        }
        return ret;
    }

    /*! Transpose multiplication (A^T*b) with a vector of the same value type. */
    Vector< ValueType > transMult(const Vector < ValueType > & b) const {
        Index cols = this->cols();
        Index rows = this->rows();

        Vector < ValueType > ret(cols, 0.0);

        //register ValueType tmpval = 0;

        if (b.size() == rows){
            for(Index i = 0; i < rows; i++){
                for(Index j = 0; j < cols; j++){
                    ret[j] +=  (*this)[i][j] * b[i];
                }
            }
        } else {
            throwLengthError(1, WHERE_AM_I + " " + toStr(rows) + " != " + toStr(b.size()));
        }
        return ret;
    }

    /*! Save matrix to file. */
    virtual void save(const std::string & filename) const {
        saveMatrix(*this, filename);
    }

    /*! Round each matrix element to a given tolerance. */
    void round(const ValueType & tolerance){
        for (Index i = 0; i < mat_.size(); i ++) mat_[i].round(tolerance);
        // ??? std::for_each(mat_.begin, mat_.end, boost::bind(&Vector< ValueType >::round, tolerance));
    }
    
protected:

    void allocate_(Index rows, Index cols){
        if (mat_.size() != rows) mat_.resize(rows);
        for (Index i = 0; i < mat_.size(); i ++) mat_[i].resize(cols);
        rowFlag_.resize(rows);
    }

    void copy_(const Matrix < ValueType > & mat){
        allocate_(mat.rows(), mat.cols());
        for (Index i = 0; i < mat_.size(); i ++) mat_[i] = mat[i];
    }

	std::vector < Vector< ValueType > > mat_;

    /*! BVector flag(rows) for free use, e.g., check if rows are set valid. */
    BVector rowFlag_;
};

#define DEFINE_BINARY_OPERATOR__(OP, NAME) \
template < class ValueType > \
Matrix < ValueType > operator OP (const Matrix < ValueType > & A, const Matrix < ValueType > & B) { \
Matrix < ValueType > tmp(A); \
return tmp OP##= B; } \
template < class ValueType > \
Matrix < ValueType > operator OP (const Matrix < ValueType > & A, const ValueType & v) { \
Matrix < ValueType > tmp(A); \
return tmp OP##= v; }

DEFINE_BINARY_OPERATOR__(+, PLUS)
DEFINE_BINARY_OPERATOR__(-, MINUS)
DEFINE_BINARY_OPERATOR__(/, DIVID)
DEFINE_BINARY_OPERATOR__(*, MULT)

#undef DEFINE_BINARY_OPERATOR__

template< class ValueType > class DLLEXPORT Mult{
public:
    Mult(Vector< ValueType > & x, const Vector< ValueType > & b, const Matrix < ValueType > & A, Index start, Index end) :
        x_(&x), b_(&b), A_(&A), start_(start), end_(end){
    }
    void operator()() {
        for (register Index i = start_; i < end_; i++) (*x_)[i] = sum((*A_)[i] * *b_);
    }

    Vector< ValueType > * x_;
    const Vector< ValueType > * b_;
    const Matrix< ValueType > * A_;
    Index start_;
    Index end_;
};

template < class ValueType >
Vector < ValueType > multMT(const Matrix < ValueType > & A, const Vector < ValueType > & b){
#ifdef USE_THREADS
    Index cols = A.cols();
    Index rows = A.rows();

    Vector < ValueType > ret(rows);
    boost::thread_group threads;
    Index nThreads = 2;
    Index singleCalcCount = Index(ceil((double)rows / (double)nThreads));
 //   CycleCounter cc;

    for (Index i = 0; i < nThreads; i ++){
// 	Vector < ValueType > *start = &A[singleCalcCount * i];
//         Vector < ValueType > *end   = &A[singleCalcCount * (i + 1)];
        Index start = singleCalcCount * i;
        Index end   = singleCalcCount * (i + 1);
	if (i == nThreads -1) end = A.rows();
//	cc.tic();
        threads.create_thread(Mult< ValueType >(ret, b, A, start, end));
  //      std::cout << cc.toc() << std::endl;
    }
    threads.join_all();
#else
    return mult(A, b);
#endif
}


template < class ValueType >
bool operator == (const Matrix< ValueType > & A, const Matrix< ValueType > & B){
    if (A.rows() != B.rows() || A.cols() != B.cols()) return false;
    for (Index i = 0; i < A.rows(); i ++){
        if (A[i] != B[i]) return false;
    }
    return true;
}

template < class ValueType >
void scaleMatrix(Matrix < ValueType >& A,
                  const Vector < ValueType > & l, const Vector < ValueType > & r){
    Index rows = A.rows();
    Index cols = A.cols();
    if (rows != l.size()){
        throwLengthError(1, WHERE_AM_I + " " + toStr(rows) + " != " + toStr(l.size()));
    };
    if (cols != r.size()){
        throwLengthError(1, WHERE_AM_I + " " + toStr(cols) + " != " + toStr(r.size()));
    }

    for (Index i = 0 ; i < rows ; i++) {
        //for (Index j = 0 ; j < cols ; j++) A[i][j] *= (l[i] * r[j]);
        A[i] *= r * l[i];
    }
}

template < class ValueType >
void rank1Update(Matrix < ValueType > & A,
                  const Vector < ValueType > & u, const Vector < ValueType > & v) {
    Index rows = A.rows();
    Index cols = A.cols();
    if (rows != u.size()){
        throwLengthError(1, WHERE_AM_I + " " + toStr(rows) + " != " + toStr(u.size()));
    };

    if (cols != v.size()){
        throwLengthError(1, WHERE_AM_I + " " + toStr(cols) + " != " + toStr(v.size()));
    }

    for (Index i = 0 ; i < rows ; i++) {
        //for (Index j = 0 ; j < ncols ; j++) A[i][j] += (u[i] * v[j]);
        A[i] += v * u[i];
    }
    return;
}

template < class ValueType >
Matrix < ValueType > fliplr(const Matrix< ValueType > & m){
    Matrix < ValueType > n;
    for (Index i = 0; i < m.rows(); i ++) n.push_back(fliplr(m[i]));
    return n;
}

//********************* MATRIX I/O *******************************

/*! Save matrix into a file (Binary).
    File suffix ($MATRIXBINSUFFIX) will be append if none given.
    Format: rows(uint32) cols(uint32) vals(rows*cols(ValueType))
    If IOFormat == Ascii matrix will be saved in Ascii format, See: \ref saveMatrixRow
*/
template < class ValueType >
bool saveMatrix(const Matrix < ValueType > & A, const std::string & filename, IOFormat format = Binary){
    if (format == Ascii) return saveMatrixRow(A, filename);
    std::string fname(filename);
    if (fname.rfind('.') == std::string::npos) fname += MATRIXBINSUFFIX;

    FILE *file; file = fopen(fname.c_str(), "w+b");
	
    if (!file){
		std::cerr << fname << ": " << strerror(errno) << " " << errno << std::endl;
        return false;
    }

    uint32 rows = A.rows();
    uint ret = fwrite(& rows, sizeof(uint32), 1, file);
    if (ret == 0) return false;
    uint32 cols = A.cols();
    ret = fwrite(& cols, sizeof(uint32), 1, file);

    for (uint i = 0; i < rows; i ++){
        for (uint j = 0; j < cols; j ++){
            ret = fwrite(&A[i][j], sizeof(ValueType), 1, file);
        }
    }
    fclose(file);
    return true;
}

/*! Load matrix from a single or multiple files (Binary).
    File suffix (\ref MATRIXBINSUFFIX, ".matrix", ".mat") given or not -- loads single datafile, else try to load matrix from multiple binary vector files.
    Single format: see \ref save(const Matrix < ValueType > & A, const std::string & filename)
*/
template < class ValueType >
bool load(Matrix < ValueType > & A, const std::string & filename){

    //!* First check if filename suffix is ".matrix", ".mat", \ref MATRIXBINSUFFIX;
    if (filename.rfind(".matrix") != std::string::npos ||
         filename.rfind(".mat") != std::string::npos ||
         filename.rfind(MATRIXBINSUFFIX) != std::string::npos) {
        //!** yes, load \ref loadMatrixSingleBin(filename)
        return loadMatrixSingleBin(A, filename);
    }

    //!* no: check if filename is expandable with suffix ".matrix" or ".mat";
    if (fileExist(filename + ".matrix")) return loadMatrixSingleBin(A, filename + ".matrix");
    if (fileExist(filename + ".mat"))    return loadMatrixSingleBin(A, filename + ".mat");
    if (fileExist(filename + MATRIXBINSUFFIX))
        //!** yes , load \ref loadMatrixSingleBin(filename + \ref MATRIXBINSUFFIX)
        return loadMatrixSingleBin(A, filename + MATRIXBINSUFFIX);

    //!* no: try to load matrix from multiple binary vectors;
    return loadMatrixVectorsBin(A, filename);
}

/*! Force to load single matrix binary file.
    Format: see \ref save(const Matrix < ValueType > & A, const std::string & filename). */
template < class ValueType >
bool loadMatrixSingleBin(Matrix < ValueType > & A,
                          const std::string & filename){

    FILE *file; file = fopen(filename.c_str(), "r+b");
    
    if (!file) {
        throwError(EXIT_OPEN_FILE, WHERE_AM_I + " " + 
                   filename + ": " + strerror(errno));
    }

    uint32 rows = 0; fread(&rows, sizeof(uint32), 1, file);
    uint32 cols = 0; fread(&cols, sizeof(uint32), 1, file);

    A.resize(rows, cols);
    for (uint32 i = 0; i < rows; i ++){
        for (uint32 j = 0; j < cols; j ++){
            fread((char*)&A[i][j], sizeof(ValueType), 1, file);
        }
    }
    fclose(file);
    A.rowFlag().fill(1);
    return true;
}

/*! Force to load multiple binary vector files into one matrix (row-based). File name will be determined from filenamebody + successive increased number (read while files exist). \n
e.g. read "filename.0.* ... filename.n.* -> Matrix[0--n)[0..vector.size())\n
kCount can be given to use as subcounter. \n
e.g. read "filename.0_0.* ... filename.n_0.* ... filename.0_kCount-1.* ... filename.n_kCount-1.* ->
Matrix[0--n*kCount)[0..vector.size())
*/
template < class ValueType >
bool loadMatrixVectorsBin(Matrix < ValueType > & A,
                            const std::string & filenameBody, uint kCount = 1){

    A.clear();
    Vector < ValueType > tmp;
    std::string filename;

    for (uint i = 0; i < kCount; i++){
        uint count = 0;
        while (1){ // load as long as posible
            if (kCount > 1){
                filename = filenameBody + "." + toStr(count) + "_" + toStr(i) + ".pot";
            } else {
                filename = filenameBody + "." + toStr(count) + ".pot";
            }

            if (!fileExist(filename)){
                filename = filenameBody + "." + toStr(count);
                if (!fileExist(filename)){
                    if (count == 0) {
	               std::cerr << " can not found: " << filename << std::endl;
                    }
                    break;
                }
            }
            if (load(tmp, filename, Binary )) A.push_back(tmp);
            count ++;
        } // while files exist
    } // for each k count
    return true;
}

/*! Save Matrix into Ascii File (column based). */
template < class ValueType >
bool saveMatrixCol(const Matrix < ValueType > & A, const std::string & filename){
    return saveMatrixCol(A, filename, "");
}

/*! Save Matrix into Ascii File (column based)  with optional comments header line. */
template < class ValueType >
bool saveMatrixCol(const Matrix < ValueType > & A, const std::string & filename,
                    const std::string & comments){
    std::fstream file; openOutFile(filename, & file, true);
    if (comments.length() > 0){
        file << "#" << comments << std::endl;
    }

    for (uint i = 0; i < A.cols(); i ++){
        for (uint j = 0; j < A.rows(); j ++){
            file << A[j][i] << "\t";
        }
        file << std::endl;
    }
    file.close();
    return true;
}

/*! Load Matrix from Ascii File (column based). */
template < class ValueType >
bool loadMatrixCol(Matrix < ValueType > & A, const std::string & filename){
    std::vector < std::string > comments;
    return loadMatrixCol(A, filename, comments);
}

/*! Load Matrix from Ascii File (column based), with optional comments header line. */
template < class ValueType >
bool loadMatrixCol(Matrix < ValueType > & A, const std::string & filename,
                    std::vector < std::string > & comments){

    uint commentCount = 0;
    uint cols = countColumnsInFile(filename, commentCount);
//     Index rows = countRowsInFile(filename);
//     // get length of file:
//     std::fstream file; openInFile(filename, & file, true);
//     Index length = fileLength(file);
//
//     // allocate memory:
//     char * buffer = new char[length];
//     file.read(buffer, length);
//     file.close();
//
//     delete buffer;
//     return true;

    std::vector < double > tmp;
    Vector < ValueType > row(cols);
    std::fstream file; openInFile(filename, & file, true);
    for (uint i = 0; i < commentCount; i ++) {
        std::string str;
        getline(file, str);
        comments = getSubstrings(str.substr(str.find('#'), -1));
    }

    double val;
    while(file >> val) tmp.push_back(val);

    file.close();
    Index rows = tmp.size() / cols ;
    A.resize(cols, rows);

    for (uint i = 0; i < rows; i ++){
        for (uint j = 0; j < cols; j ++){
            A[j][i] = tmp[i * cols + j];
        }
    }
    return true;
}

/*! Save Matrix into Ascii File (row based). */
template < class ValueType >
bool saveMatrixRow(const Matrix < ValueType > & A, const std::string & filename){
    return saveMatrixRow(A, filename, "");
}

/*! Save Matrix into Ascii File (row based)  with optional comments header line. */
template < class ValueType >
bool saveMatrixRow(const Matrix < ValueType > & A, const std::string & filename,
                    const std::string & comments){
    std::fstream file; openOutFile(filename, & file, true);
    if (comments.length() > 0){
        file << "#" << comments << std::endl;
    }

    for (uint i = 0; i < A.rows(); i ++){
        for (uint j = 0; j < A.cols(); j ++){
            file << A[i][j] << "\t";
        }
        file << std::endl;
    }
    file.close();
    return true;
}

/*! Load Matrix from Ascii File (row based). */
template < class ValueType >
bool loadMatrixRow(Matrix < ValueType > & A, const std::string & filename){

    std::vector < std::string > comments;
    return loadMatrixRow(A, filename, comments);
}

/*! Load Matrix from Ascii File (row based), with optional comments header line. */
template < class ValueType >
bool loadMatrixRow(Matrix < ValueType > & A,
                    const std::string & filename,
                    std::vector < std::string > & comments){

    uint commentCount = 0;
    uint cols = countColumnsInFile(filename, commentCount);

    Vector < ValueType > row(cols);
    std::fstream file; openInFile(filename, & file, true);
    for (uint i = 0; i < commentCount; i ++) {
        std::string str;
        getline(file, str);
        comments = getSubstrings(str.substr(str.find('#'), -1));
    }

    double val;
    std::vector < double > tmp;
    while(file >> val) tmp.push_back(val);

    file.close();
    Index rows = tmp.size() / cols ;
    A.resize(rows, cols);

    for (uint i = 0; i < rows; i ++){
        for (uint j = 0; j < cols; j ++){
            A[i][j] = tmp[i * cols + j];
        }
    }
    return true;
}

/*! Return determinant for Matrix(2 x 2). */
template < class T > inline T det(const T & a, const T & b, const T & c, const T & d){
    return a * d - b * c;
}

template < class ValueType > inline ValueType det(const Matrix3 < ValueType > & A){
    return A[0] * (A[4] * A[8] - A[5] * A[7]) -
           A[1] * (A[3] * A[8] - A[5] * A[6]) +
           A[2] * (A[3] * A[7] - A[4] * A[6]);
}
    
/*! Return determinant for Matrix A. This function is a stub. Only Matrix dimensions of 2 and 3 are considered. */
template < class Matrix > double det(const Matrix & A){
    //** das geht viel schoener, aber nicht mehr heute.;
    double det = 0.0;
    switch (A.rows()){
        case 2: det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
            break;
        case 3:
            det = A[0][0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1]) -
                  A[0][1] * (A[1][0] * A[2][2] - A[1][2] * A[2][0]) +
                  A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
            break;
        default:
            std::cerr << WHERE_AM_I << " matrix determinant of dim not yet implemented -- dim: " << A.rows() << std::endl;
            break;
    }
    return det;
}


/*! Return the inverse of Matrix A3. This function is a stub. Only Matrix dimensions of 2 and 3 are considered. */
template < class ValueType > inline Matrix3<ValueType> inv(const Matrix3< ValueType > & A){
    Matrix3< ValueType > I;
    inv(A, I);
    return I;
}

/*! Return the inverse of Matrix3 A. */
template < class ValueType > inline void inv(const Matrix3< ValueType > & A,
                                             Matrix3< ValueType > & I){
//     __M
//     std::cout << A << std::endl;
    I[0] =  (A[4] * A[8] - A[5] * A[7]);
    I[3] = -(A[3] * A[8] - A[5] * A[6]);
    I[6] =  (A[3] * A[7] - A[4] * A[6]);
    I[1] = -(A[1] * A[8] - A[2] * A[7]);
    I[4] =  (A[0] * A[8] - A[2] * A[6]);
    I[7] = -(A[0] * A[7] - A[1] * A[6]);
    I[2] =  (A[1] * A[5] - A[2] * A[4]);
    I[5] = -(A[0] * A[5] - A[2] * A[3]);
    I[8] =  (A[0] * A[4] - A[1] * A[3]);
//     std::cout << I << std::endl;
//     std::cout << (A[0] * I[0] + A[1] * I[3] + A[2] * I[6]) << std::endl;
//     std::cout << det(A) << std::endl;
    I /= (A[0] * I[0] + A[1] * I[3] + A[2] * I[6]);
}

/*! Return the inverse of Matrix A. This function is a stub. Only Matrix dimensions of 2 and 3 are considered. */
template < class Matrix > Matrix inv(const Matrix & A){
    //** das geht viel schoener, aber nicht mehr heute.; Wie?
    Matrix I(A.rows(), A.cols());
    inv(A, I);
    return I;
}


/*! Return the inverse of Matrix A. This function is a stub. Only Matrix dimensions of 2 and 3 are considered. */
template < class Matrix > void inv(const Matrix & A, Matrix & I){
    //** das geht viel schoener, aber nicht mehr heute.; Wie?
    
    switch (I.rows()){
        case 2: 
            I[0][0] =  A[1][1];
            I[1][0] = -A[1][0];
            I[0][1] = -A[0][1];
            I[1][1] =  A[0][0];            
            break;
        case 3:
            I[0][0] =  (A[1][1] * A[2][2] - A[1][2] * A[2][1]);
            I[1][0] = -(A[1][0] * A[2][2] - A[1][2] * A[2][0]);
            I[2][0] =  (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
            I[0][1] = -(A[0][1] * A[2][2] - A[0][2] * A[2][1]);
            I[1][1] =  (A[0][0] * A[2][2] - A[0][2] * A[2][0]);
            I[2][1] = -(A[0][0] * A[2][1] - A[0][1] * A[2][0]);
            I[0][2] =  (A[0][1] * A[1][2] - A[0][2] * A[1][1]);
            I[1][2] = -(A[0][0] * A[1][2] - A[0][2] * A[1][0]);
            I[2][2] =  (A[0][0] * A[1][1] - A[0][1] * A[1][0]);
            break;
        default:
            std::cerr << WHERE_AM_I << " matrix determinant of dim not yet implemented -- dim: " << A.rows() << std::endl;
            break;
    }
    I /= det(A);
}

inline void save(const MatrixBase & A, const std::string & filename){
    A.save(filename);
}

inline void save(MatrixBase & A, const std::string & filename){
    A.save(filename);
}

inline RVector operator * (const MatrixBase & A, const RVector & b){
    return A.mult(b);
}

inline RVector transMult(const MatrixBase & A, const RVector & b){
    return A.transMult(b);
}

inline RVector operator * (const RMatrix & A, const RVector & b){
    return A.mult(b);
}

inline RVector transMult(const RMatrix & A, const RVector & b){
    return A.transMult(b);
}


} //namespace GIMLI

#endif // _GIMLI_MATRIX__H

