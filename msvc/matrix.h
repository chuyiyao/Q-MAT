/*
Copyright 2011. All rights reserved.
Institute of Measurement and Control Systems
Karlsruhe Institute of Technology, Germany

Authors: Andreas Geiger

matrix is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or any later version.

matrix is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
matrix; if not, write to the Free Software Foundation, Inc., 51 Franklin
Street, Fifth Floor, Boston, MA 02110-1301, USA 
*/

#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>


#define endll endl << endl // double end line definition
//typedef double double;      // double precision
//typedef double  double;    // single precision
namespace ICPL {
class Matrix {

public:

  // constructor / deconstructor
  Matrix ();                                                  // init empty 0x0 matrix
  Matrix (const int m,const int n);                   // init empty mxn matrix
  Matrix (const int m,const int n,const double* val_); // init mxn matrix with values from array 'val'
  Matrix (const Matrix &M);                                   // creates deepcopy of M
  ~Matrix ();

  // assignment operator, copies contents of M
  Matrix& operator= (const Matrix &M);

  // copies submatrix of M into array 'val', default values copy whole row/column/matrix
  void getData(double* val_,int i1=0,int j1=0,int i2=-1,int j2=-1);

  // set or get submatrices of current matrix
  Matrix getMat(int i1,int j1,int i2=-1,int j2=-1);
  void   setMat(const Matrix &M,const int i,const int j);

  // set sub-matrix to scalar (default 0), -1 as end replaces whole row/column/matrix
  void setVal(double s,int i1=0,int j1=0,int i2=-1,int j2=-1);

  // set (part of) diagonal to scalar, -1 as end replaces whole diagonal
  void setDiag(double s,int i1=0,int i2=-1);

  // clear matrix
  void zero();
  
  // extract columns with given index
  Matrix extractCols (std::vector<int> idx);

  // create identity matrix
  static Matrix eye (const int m);
  void          eye ();

  // create matrix with ones
  static Matrix ones(const int m,const int n);

  static Matrix zeros(const int m, const int n);
  // create diagonal matrix with nx1 or 1xn matrix M as elements
  static Matrix diag(const Matrix &M);
  
  // returns the m-by-n matrix whose elements are taken column-wise from M
  static Matrix reshape(const Matrix &M,int m,int n);

  // create 3x3 rotation matrices (convention: http://en.wikipedia.org/wiki/Rotation_matrix)
  static Matrix rotMatX(const double &angle);
  static Matrix rotMatY(const double &angle);
  static Matrix rotMatZ(const double &angle);

  // simple arithmetic operations
  Matrix  operator+ (const Matrix &M); // add matrix
  Matrix  operator- (const Matrix &M); // subtract matrix
  Matrix  operator* (const Matrix &M); // multiply with matrix
  Matrix  operator* (const double &s);  // multiply with scalar
  Matrix  operator/ (const Matrix &M); // divide elementwise by matrix (or vector)
  Matrix  operator/ (const double &s);  // divide by scalar
  Matrix &operator += (Matrix &M);
  Matrix  operator- ();                // negative matrix
  Matrix  operator~ ();                // transpose
  double   l2norm ();                   // euclidean norm (vectors) / frobenius norm (matrices)
  double   mean ();                     // mean of all elements in matrix

  // complex arithmetic operations
  static Matrix cross (const Matrix &a, const Matrix &b);    // cross product of two vectors
  static Matrix inv (const Matrix &M);                       // invert matrix M
  bool   inv ();                                             // invert this matrix
  double  det ();                                             // returns determinant of matrix
  bool   solve (const Matrix &M,double eps=1e-20);            // solve linear system M*x=B, replaces *this and M
  bool   lu(int *idx, double &d, double eps=1e-20);        // replace *this by lower upper decomposition
  void   svd(Matrix &U,Matrix &W,Matrix &V);                 // singular value decomposition *this = U*diag(W)*V^T

  // print matrix to stream
#if 0

  friend std::ostream& operator<< (std::ostream& out,const Matrix& M);

#endif
  // direct data access
  double   **val;
  int   m,n;

private:

  void allocateMemory (const int m_,const int n_);
  void releaseMemory ();
  inline double pythag(double a,double b);

};

}

std::ostream &operator << (std::ostream &os, const ICPL::Matrix &Ma);


#endif // MATRIX_H
