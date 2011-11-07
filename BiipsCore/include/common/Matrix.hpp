//                                               -*- C++ -*-
/*! \file Matrix.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MATRIX_HPP_
#define BIIPS_MATRIX_HPP_

#include "common/MultiArray.hpp"

#include <boost/numeric/ublas/matrix.hpp>

namespace Biips
{

  class Matrix : public ublas::matrix<MultiArray::ValueType, MultiArray::StorageOrderType, MultiArray::StorageType>
  {
  public:
    typedef ublas::matrix<MultiArray::ValueType, MultiArray::StorageOrderType, MultiArray::StorageType> BaseType;

    Matrix() {};
    Matrix(size_type sz1, size_type sz2) : BaseType(sz1, sz2) {}
    Matrix(size_type sz1, size_type sz2, value_type val) : BaseType(sz1, sz2, array_type(sz1*sz2, val)) {}
    Matrix(size_type sz1, size_type sz2, const array_type & value) : BaseType(sz1, sz2, value) {}
    explicit Matrix(const MultiArray & data) : BaseType(data.Dim()[0], data.Dim()[1], data.Values()) {}
    template<class AE>
    Matrix(const ublas::matrix_expression<AE> &ae) : BaseType(ae) {}

    template<class AE>
    Matrix & operator = (const ublas::matrix_expression<AE> & ae) { BaseType::operator=(ae); return *this; }
  };



  class MatrixRef : public ublas::matrix<MultiArray::ValueType, MultiArray::StorageOrderType, MultiArray::StorageType>
  {
  public:
    typedef MultiArray::ValueType ValueType;
    typedef MultiArray::StorageOrderType StorageOrder;
    typedef MultiArray::StorageType StorageType;
    typedef ublas::matrix<ValueType, StorageOrder, StorageType> BaseType;

  protected:
    MultiArray * pData_;
    mutable Bool released_;

    BaseType::swap;


  public:
    explicit MatrixRef(MultiArray & dat) : BaseType(dat.Dim()[0], dat.Dim()[1], array_type()), pData_(&dat), released_(false) { BaseType::data().swap(pData_->Values()); }

    MatrixRef(MatrixRef & mat_ref) : BaseType(mat_ref.size1(), mat_ref.size2(), array_type()), pData_(mat_ref.pData_), released_(mat_ref.released_)
    {
      if ( !released_ )
      {
        swap(mat_ref);
        mat_ref.released_ = true;
      }
    }

    MatrixRef(const MatrixRef & mat_ref) : BaseType(mat_ref), pData_(mat_ref.pData_), released_(mat_ref.released_)
    {
      if ( !released_ )
        mat_ref.released_ = true;
    }

    template<class AE>
    MatrixRef & operator = (const ublas::matrix_expression<AE> & ae) { BaseType::operator=(ae); pData_->Dim()[0] = size1(); pData_->Dim()[1] = size2(); return *this; }

    // TODO put in cpp
    void resize (size_type size_1, size_type size_2, bool preserve = true) { BaseType::resize(size_1, size_2, preserve); pData_->Dim()[0] = size_1; pData_->Dim()[1] = size_2; }

    virtual ~MatrixRef() { if (! released_) data().swap(pData_->Values()); }

    void Release() { if (! released_) { data().swap(pData_->Values()); released_ = true;} }

    Bool Released() const { return released_; }
  };

}

#endif /* BIIPS_MATRIX_HPP_ */
