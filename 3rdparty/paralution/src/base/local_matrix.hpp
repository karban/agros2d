// *************************************************************************
//
//    PARALUTION   www.paralution.com
//
//    Copyright (C) 2012-2013 Dimitar Lukarski
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// *************************************************************************

#ifndef PARALUTION_LOCAL_MATRIX_HPP_
#define PARALUTION_LOCAL_MATRIX_HPP_

#include "operator.hpp"
#include "base_matrix.hpp"
#include "backend_manager.hpp"

namespace paralution {

template <typename ValueType>
class LocalVector;
template <typename ValueType>
class GlobalVector;

template <typename ValueType>
class GlobalMatrix;

// Local Matrix
template <typename ValueType>
class LocalMatrix : public Operator<ValueType> {
  
public:

  LocalMatrix();
  virtual ~LocalMatrix();

  virtual void info(void) const;

  /// Return the matrix format id (see matrix_formats.hpp)
  unsigned int get_format(void) const { return this->matrix_->get_mat_format(); }

  /// Return the number of rows in the matrix
  virtual int get_nrow(void) const { return this->matrix_->get_nrow(); }
  /// Return the number of columns in the matrix
  virtual int get_ncol(void) const { return this->matrix_->get_ncol(); }
  /// Return the number of non-zeros in the matrix
  int get_nnz(void) const { return this->matrix_->get_nnz(); }

  /// Allocate CSR Matrix
  void AllocateCSR(const std::string name, const int nnz, const int nrow, const int ncol);
  /// Allocate BCSR Matrix - not implemented
  void AllocateBCSR(void) {};
  /// Allocate MCSR Matrix
  void AllocateMCSR(const std::string name, const int nnz, const int nrow, const int ncol);
  /// Allocate COO Matrix
  void AllocateCOO(const std::string name, const int nnz, const int nrow, const int ncol);
  /// Allocate DIA Matrix
  void AllocateDIA(const std::string name, const int nnz, const int nrow, const int ncol, const int ndiag);
  /// Allocate ELL Matrix
  void AllocateELL(const std::string name, const int nnz, const int nrow, const int ncol, const int max_row);
  /// Allocate HYB Matrix
  void AllocateHYB(const std::string name, const int ell_nnz, const int coo_nnz, const int ell_max_row,
                   const int nrow, const int ncol);
  /// Allocate DENSE Matrix
  void AllocateDENSE(const std::string name, const int nrow, const int ncol);

  /// Initialize a COO matrix on the Host with externally allocated data  
  void SetDataPtrCOO(int **row, int **col, ValueType **val,
                     std::string name, const int nnz, const int nrow, const int ncol);

  /// Initialize a CSR matrix on the Host with externally allocated data
  void SetDataPtrCSR(int **row_offset, int **col, ValueType **val,
                     std::string name, const int nnz, const int nrow, const int ncol);

  /// Clear (free) the matrix
  void Clear(void);

  /// Set all the values to zero
  void Zeros(void);

  /// Scale all the values in the matrix 
  void Scale(const ValueType alpha);
  /// Scale the diagonal entries of the matrix with alpha
  void ScaleDiagonal(const ValueType alpha);
  /// Scale the off-diagonal entries of the matrix with alpha
  void ScaleOffDiagonal(const ValueType alpha);
 
  /// Add a scalar to all values
  void AddScalar(const ValueType alpha);
  /// Add alpha to the diagonal entries of the matrix
  void AddScalarDiagonal(const ValueType alpha);
  /// Add alpha to the off-diagonal entries of the matrix
  void AddScalarOffDiagonal(const ValueType alpha);

  /// Extract a sub-matrix with row/col_offset and row/col_size
  void ExtractSubMatrix(const int row_offset,
                        const int col_offset,
                        const int row_size,
                        const int col_size,
                        LocalMatrix<ValueType> *mat) const;

  /// Extract array of non-overlapping sub-matrices (row/col_num_blocks define the 
  /// blocks for rows/columns; row/col_offset have sizes col/row_num_blocks+1, where [i+1]-[i] 
  /// defines the i-th size of the sub-matrix)
  void ExtractSubMatrices(const int row_num_blocks,
                          const int col_num_blocks,
                          const int *row_offset,
                          const int *col_offset,
                          LocalMatrix<ValueType> ***mat) const;

  /// Extract the diagonal values of the matrix into a LocalVector
  void ExtractDiagonal(LocalVector<ValueType> *vec_diag) const;

  /// Extract the inverse (reciprocal) diagonal values of the matrix into a LocalVector
  void ExtractInverseDiagonal(LocalVector<ValueType> *vec_inv_diag) const;

  /// Perform (forward) permutation of the matrix
  void Permute(const LocalVector<int> &permutation);

  /// Perform (backward) permutation of the matrix
  void PermuteBackward(const LocalVector<int> &permutation);

  /// Perform multi-coloring decomposition of the matrix; Returns 
  /// number of colors, the corresponding sizes (the array is allocated in 
  /// the function) and the permutation
  void MultiColoring(int &num_colors,
                     int **size_colors,
                     LocalVector<int> *permutation) const;

  /// Perform maximal independent set decomposition of the matrix; 
  /// Returns the size of the maximal independent set and the 
  /// corresponding permutation
  void MaximalIndependentSet(int &size,
                             LocalVector<int> *permutation) const;

  /// Perform ILU(0) factorization
  void ILU0Factorize(void);

  /// Perform ILU(t,m) factorization based on threshold and maximum
  /// number of elements per row
  void ILUTFactorize(const ValueType t, const int maxrow);

  /// Perform ILU(p) factorization based on power (see power(q)-pattern method, 
  /// D. Lukarski "Parallel Sparse Linear Algebra for Multi-core and Many-core 
  /// Platforms - Parallel Solvers and Preconditioners", PhD Thesis, 2012, KIT);
  /// level==true build the structure based on levels; level==false build the
  /// structure only based on the power(p+1)
  void ILUpFactorize(const int p, const bool level=true);   
  /// Analyse the structure (level-scheduling)
  void LUAnalyse(void);
  /// Delete the analysed data (see LUAnalyse)
  void LUAnalyseClear(void);
  /// Solve LU out = in; if level-scheduling algorithm is provided then the 
  /// graph traversing is performed in parallel
  void LUSolve(const LocalVector<ValueType> &in, LocalVector<ValueType> *out) const; 

  /// Perform IC(0) factorization
  void IC0Factorize(void);  

  /// Analyse the structure (level-scheduling)
  void LLAnalyse(void);
  /// Delete the analysed data (see LLAnalyse)
  void LLAnalyseClear(void);
  /// Solve LL^T out = in; if level-scheduling algorithm is provided then the 
  /// graph traversing is performed in parallel
  void LLSolve(const LocalVector<ValueType> &in, LocalVector<ValueType> *out) const; 

  /// Analyse the structure (level-scheduling) L-part
  /// diag_unit == true the diag is 1;
  /// diag_unit == false the diag is 0;
  void LAnalyse(const bool diag_unit=false);
  /// Delete the analysed data (see LAnalyse) L-party
  void LAnalyseClear(void);
  /// Solve L out = in; if level-scheduling algorithm is provided then the 
  /// graph traversing is performed in parallel
  void LSolve(const LocalVector<ValueType> &in, LocalVector<ValueType> *out) const; 

  /// Analyse the structure (level-scheduling) U-part;
  /// diag_unit == true the diag is 1;
  /// diag_unit == false the diag is 0;
  void UAnalyse(const bool diag_unit=false);
  /// Delete the analysed data (see UAnalyse) U-party
  void UAnalyseClear(void);
  /// Solve U out = in; if level-scheduling algorithm is provided then the 
  /// graph traversing is performed in parallel
  void USolve(const LocalVector<ValueType> &in, LocalVector<ValueType> *out) const; 


  /// Read matrix from MTX (Matrix Market Format) file
  void ReadFileMTX(const std::string filename);
  /// Write matrix to MTX (Matrix Market Format) file
  void WriteFileMTX(const std::string filename) const;
  
  virtual void MoveToAccelerator(void);
  virtual void MoveToHost(void);

  /// Copy matrix (values and structure) from another LocalMatrix
  void CopyFrom(const LocalMatrix<ValueType> &src);

  /// Clone the entire matrix (values,structure+backend descr) 
  /// from another LocalMatrix
  void CloneFrom(const LocalMatrix<ValueType> &src);

  /// Copy (import) CSR matrix described in three arrays (offsets, columns, values). 
  /// The object data has to be allocated (call AllocateCSR first)
  void CopyFromCSR(const int *row_offsets, const int *col, const ValueType *val);

  /// Copy (export) CSR matrix described in three arrays 
  /// (offsets, columns, values). The output arrays have the allocated
  void CopyToCSR(int *row_offsets, int *col, ValueType *val) const;

  /// Copy (import) COO matrix described in three arrays (rows, columns, values). 
  /// The object data has to be allocated (call AllocateCOO first)
  void CopyFromCOO(const int *row, const int *col, const ValueType *val);

  /// Create a restriction matrix operator based on an int vector map
  void CreateFromMap(const LocalVector<int> &map, const int n, const int m);

  /// Convert the matrix to CSR structure
  void ConvertToCSR(void);
  /// Convert the matrix to MCSR structure
  void ConvertToMCSR(void);
  /// Convert the matrix to BCSR structure
  void ConvertToBCSR(void);
  /// Convert the matrix to COO structure
  void ConvertToCOO(void);
  /// Convert the matrix to ELL structure
  void ConvertToELL(void);
  /// Convert the matrix to DIA structure
  void ConvertToDIA(void);
  /// Convert the matrix to HYB structure
  void ConvertToHYB(void);
  /// Convert the matrix to DENSE structure
  void ConvertToDENSE(void);
  /// Convert the matrix to specified matrix ID format
  void ConvertTo(const unsigned int matrix_format);
  
  virtual void Apply(const LocalVector<ValueType> &in, LocalVector<ValueType> *out) const; 
  virtual void ApplyAdd(const LocalVector<ValueType> &in, const ValueType scalar, 
                        LocalVector<ValueType> *out) const; 

  /// Perform symbolic computation (structure only) of |this|^p
  void SymbolicPower(const int p);

  // Perform matrix addition, this = alpha*this + beta*mat; 
  /// if structure==false the structure of the matrix is not changed; 
  /// if structure==true new data structure is computed
  void MatrixAdd(const LocalMatrix<ValueType> &mat, const ValueType alpha=ValueType(1.0), 
                 const ValueType beta=ValueType(1.0), const bool structure=false);

  /// Multiply two matrices, this = A * B
  void MatrixMult(const LocalMatrix<ValueType> &A, const LocalMatrix<ValueType> &B);

  /// Multiply the matrix with diagonal matrix (stored in LocalVector), 
  /// this=this*diag
  void DiagonalMatrixMult(const LocalVector<ValueType> &diag);

  /// Compute the spectrum approximation with Gershgorin circles theorem
  void Gershgorin(ValueType &lambda_min,
                  ValueType &lambda_max) const;

  /// Delete all entries in the matrix which abs(a_ij) <= drop_off;
  /// the diagonal elements are never deleted;  
  void Compress(const ValueType drop_off);

  /// Transpose the matrix
  void Transpose(void);

  /// Strong couplings for aggregation-based AMG
  void AMGConnect(const ValueType eps, LocalVector<int> *connections) const;
  /// Plain aggregation - Modification of a greedy aggregation scheme from Vanek (1996)
  void AMGAggregate(const LocalVector<int> &connections, LocalVector<int> *aggregates) const;
  /// Interpolation scheme based on smoothed aggregation from Vanek (1996)
  void AMGSmoothedAggregation(const ValueType relax,
                              const LocalVector<int> &aggregates,
                              const LocalVector<int> &connections,
                                    LocalMatrix<ValueType> *prolong,
                                    LocalMatrix<ValueType> *restrict) const;
  /// Aggregation-based interpolation scheme
  void AMGAggregation(const LocalVector<int> &aggregates,
                            LocalMatrix<ValueType> *prolong,
                            LocalMatrix<ValueType> *restrict) const;

protected:

  virtual bool is_host(void) const;
  virtual bool is_accel(void) const;

private:

  /// Pointer from the base matrix class to the current 
  /// allocated matrix (host_ or accel_)
  BaseMatrix<ValueType> *matrix_;

  /// Host Matrix
  HostMatrix<ValueType> *matrix_host_;

  /// Accelerator Matrix
  AcceleratorMatrix<ValueType> *matrix_accel_;

  friend class LocalVector<ValueType>;  
  friend class GlobalVector<ValueType>;  
  friend class GlobalMatrix<ValueType>;  
  
};


};

#endif // PARALUTION_LOCAL_MATRIX_HPP_
