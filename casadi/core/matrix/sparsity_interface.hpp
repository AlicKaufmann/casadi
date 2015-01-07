/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef CASADI_SPARSITY_INTERFACE_HPP
#define CASADI_SPARSITY_INTERFACE_HPP

#include "../std_vector_tools.hpp"

namespace casadi {
  /** \brief Sparsity interface class

      This is a common base class for GenericMatrix (i.e. MX and Matrix<>) and Sparsity, introducing a
      uniform syntax and implementing common functionality using the curiously recurring template pattern
      (CRTP) idiom.\n

      \author Joel Andersson
      \date 2014
  */
  template<typename MatType>
  class CASADI_EXPORT SparsityInterface {
  public:
    std::vector< std::vector< MatType > >
      zz_blocksplit(const std::vector<int>& vert_offset, const std::vector<int>& horz_offset) const;
    static MatType zz_veccat(const std::vector< MatType >& x);
    static MatType zz_vecNZcat(const std::vector< MatType >& x);
    MatType zz_vec() const;
#ifndef SWIG
    /** \brief Concatenate a list of matrices horizontally
     * Alternative terminology: horizontal stack, hstack, horizontal append, [a b]
     *
     *   horzcat(horzsplit(x, ...)) = x
     */
    inline friend MatType horzcat(const std::vector<MatType> &v) {
      return MatType::zz_horzcat(v);
    }

    /** \brief Concatenate horizontally, two matrices */
    inline friend MatType horzcat(const MatType &x, const MatType &y) {
      std::vector<MatType> v(2);
      v[0] = x;
      v[1] = y;
      return horzcat(v);
    }

    /** \brief Concatenate a list of matrices vertically
     * Alternative terminology: vertical stack, vstack, vertical append, [a;b]
     *
     *   vertcat(vertsplit(x, ...)) = x
     */
    inline friend MatType vertcat(const std::vector<MatType> &v) {
      return MatType::zz_vertcat(v);
    }

    /** \brief Concatenate vertically, two matrices */
    inline friend MatType vertcat(const MatType &x, const MatType &y) {
      std::vector<MatType> v(2);
      v[0] = x;
      v[1] = y;
      return vertcat(v);
    }

    /** \brief  split horizontally, retaining groups of columns
     * \param offset List of all start columns for each group
     *      the last column group will run to the end.
     *
     *   horzcat(horzsplit(x, ...)) = x
     */
    inline friend std::vector<MatType > horzsplit(const MatType &v,
                                                  const std::vector<int>& offset) {
      return v.zz_horzsplit(offset);
    }

    /** \brief  split horizontally, retaining fixed-sized groups of columns
     * \param incr Size of each group of columns
     *
     *   horzcat(horzsplit(x, ...)) = x
     */
    inline friend std::vector<MatType > horzsplit(const MatType &v, int incr=1) {
      casadi_assert(incr>=1);
      int sz2 = static_cast<const MatType&>(v).size2();
      std::vector<int> offset2 = range(0, sz2, incr);
      offset2.push_back(sz2);
      return horzsplit(v, offset2);
    }

    /** \brief  split vertically, retaining groups of rows
     * \param output_offset List of all start rows for each group
     *      the last row group will run to the end.
     *
     *   vertcat(vertsplit(x, ...)) = x
     */
    inline friend std::vector<MatType > vertsplit(const MatType &v,
                                                  const std::vector<int>& offset) {
      return v.zz_vertsplit(offset);
    }

    /** \brief  split vertically, retaining fixed-sized groups of rows
     * \param incr Size of each group of rows
     *
     *   vertcat(vertsplit(x, ...)) = x
     */
    inline friend std::vector<MatType > vertsplit(const MatType &v, int incr=1) {
      casadi_assert(incr>=1);
      int sz1 = static_cast<const MatType&>(v).size1();
      std::vector<int> offset1 = range(0, sz1, incr);
      offset1.push_back(sz1);
      return vertsplit(v, offset1);
    }

    /** \brief Construct a matrix from a list of list of blocks.
     */
    inline friend MatType blockcat(const std::vector< std::vector<MatType > > &v) {
      return MatType::zz_blockcat(v);
    }

    /** \brief Construct a matrix from 4 blocks
     */
    inline friend MatType blockcat(const MatType &A, const MatType &B,
                                   const MatType &C, const MatType &D) {
      return vertcat(horzcat(A, B), horzcat(C, D));
    }

    /** \brief  chop up into blocks
     * \param vert_offset Defines the boundaries of the block rows
     * \param horz_offset Defines the boundaries of the block columns
     *
     *   blockcat(blocksplit(x,..., ...)) = x
     */
    inline friend
      std::vector< std::vector< MatType > > blocksplit(const MatType& x,
                                                       const std::vector<int>& vert_offset,
                                                       const std::vector<int>& horz_offset) {
      return x.zz_blocksplit(vert_offset, horz_offset);
    }

    /** \brief  chop up into blocks
     * \param vert_incr Defines the increment for block boundaries in row dimension
     * \param horz_incr Defines the increment for block boundaries in column dimension
     *
     *   blockcat(blocksplit(x,..., ...)) = x
     */
    inline friend std::vector< std::vector< MatType > >
      blocksplit(const MatType& x, int vert_incr=1, int horz_incr=1) {
      casadi_assert(horz_incr>=1);
      casadi_assert(vert_incr>=1);
      int sz1 = static_cast<const MatType&>(x).size1();
      std::vector<int> offset1 = range(0, sz1, vert_incr);
      offset1.push_back(sz1);
      int sz2 = static_cast<const MatType&>(x).size2();
      std::vector<int> offset2 = range(0, sz2, horz_incr);
      offset2.push_back(sz2);
      return blocksplit(x, offset1, offset2);
    }

    /** \brief Construct a matrix with given block on the diagonal */
    inline friend MatType blkdiag(const std::vector<MatType> &A) {
      return MatType::zz_blkdiag(A);
    }

    /** \brief Concatenate along diagonal, two matrices */
    inline friend MatType blkdiag(const MatType &x, const MatType &y) {
      std::vector<MatType> v(2);
      v[0] = x;
      v[1] = y;
      return blkdiag(v);
    }

    /** \brief  concatenate vertically while vectorizing all arguments with vec */
    inline friend MatType veccat(const std::vector< MatType >& x) {
      return MatType::zz_veccat(x);
    }

    /** \brief  concatenate vertically while vectorizing all arguments with vecNZ */
    inline friend MatType vecNZcat(const std::vector< MatType >& x) {
      return MatType::zz_vecNZcat(x);
    }

    /** \brief Matrix product of two matrices:  */
    inline friend MatType mul(const MatType &X, const MatType &Y) {
      return X.zz_mtimes(Y);
    }

    /** \brief Matrix product and addition
        Matrix product of two matrices (X and Y), adding the result to
        a third matrix Z. The result has the same sparsity pattern as
        C meaning that other entries of (X*Y) are ignored.
        The operation is equivalent to: Z+mul(X,Y).setSparse(Z.sparsity()).
    */
    inline friend MatType mul(const MatType &X, const MatType &Y, const MatType &Z) {
      return X.zz_mtimes(Y, Z);
    }

    /** \brief Matrix product of n matrices */
    inline friend MatType mul(const std::vector<MatType> &args) {
      casadi_assert_message(args.size()>=1,
                            "mul(std::vector<MatType> &args): "
                            "supplied list must not be empty.");
      MatType ret = args[0];
      for (int i=1; i<args.size(); ++i) ret = mul(ret, args[i]);
      return ret;
    }

    /** \brief Transpose */
    inline friend MatType transpose(const MatType& X) {
      return X.T();
    }

    /** \brief  make a vector
        Reshapes/vectorizes the matrix such that the shape becomes (expr.numel(), 1).
        Columns are stacked on top of each other.
        Same as reshape(expr, expr.numel(), 1)

        a c \n
        b d \n

        turns into

        a \n
        b \n
        c \n
        d \n

    */
    inline friend MatType vec(const MatType& a) { return a.zz_vec();}

    /** \brief Returns a flattened version of the matrix, preserving only nonzeros
     */
    inline friend MatType vecNZ(const MatType& a) { return a.zz_vecNZ();}
#endif // SWIG
  };

#ifndef SWIG
  template<typename MatType>
  MatType SparsityInterface<MatType>::zz_vec() const {
    const MatType& x = static_cast<const MatType&>(*this);
    return reshape(x, x.numel(), 1);
  }

  template<typename MatType>
  std::vector< std::vector< MatType > >
  SparsityInterface<MatType>::zz_blocksplit(const std::vector<int>& vert_offset,
                                            const std::vector<int>& horz_offset) const {
    std::vector< MatType > rows = vertsplit(static_cast<const MatType&>(*this), vert_offset);
    std::vector< std::vector< MatType > > ret;
    for (int i=0;i<rows.size();++i) {
      ret.push_back(horzsplit(rows[i], horz_offset));
    }
    return ret;
  }

  template<typename MatType>
  MatType SparsityInterface<MatType>::zz_veccat(const std::vector< MatType >& x) {
    std::vector< MatType > x_vec = x;
    for (typename std::vector< MatType >::iterator it=x_vec.begin();
         it!=x_vec.end(); ++it) {
      *it = vec(*it);
    }
    return vertcat(x_vec);
  }

  template<typename MatType>
  MatType SparsityInterface<MatType>::zz_vecNZcat(const std::vector< MatType >& x) {
    std::vector< MatType > x_vec = x;
    for (typename std::vector< MatType >::iterator it=x_vec.begin();
         it!=x_vec.end(); ++it) {
      *it = vecNZ(*it);
    }
    return vertcat(x_vec);
  }
#endif // SWIG

} // namespace casadi

#endif // CASADI_SPARSITY_INTERFACE_HPP