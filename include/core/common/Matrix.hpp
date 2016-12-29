#ifndef BIIPS_MATRIX_HPP_
#define BIIPS_MATRIX_HPP_

#include "common/NumArray.hpp"
#include "common/MultiArray.hpp"
#include <boost/numeric/ublas/matrix.hpp>

namespace Biips
{

  class Matrix: public ublas::matrix<NumArray::ValueType,
      NumArray::StorageOrderType, NumArray::StorageType>
  {
  public:
    typedef ublas::matrix<NumArray::ValueType, NumArray::StorageOrderType,
        NumArray::StorageType> BaseType;

    Matrix()
    {
    }
    Matrix(size_type sz1, size_type sz2)
        : BaseType(sz1, sz2)
    {
    }
    Matrix(size_type sz1, size_type sz2, value_type val)
        : BaseType(sz1, sz2, array_type(sz1 * sz2, val))
    {
    }
    Matrix(size_type sz1, size_type sz2, const array_type & value)
        : BaseType(sz1, sz2, value)
    {
    }
    explicit Matrix(const NumArray & data);
    explicit Matrix(const MultiArray & data);
    template<class AE>
    Matrix(const ublas::matrix_expression<AE> &ae)
        : BaseType(ae)
    {
    }

    template<class AE>
    Matrix & operator =(const ublas::matrix_expression<AE> & ae)
    {
      BaseType::operator=(ae);
      return *this;
    }
  };


  class MatrixRef: public ublas::matrix<NumArray::ValueType,
  NumArray::StorageOrderType, NumArray::StorageType>
  {
  public:
    typedef NumArray::ValueType ValueType;
    typedef NumArray::StorageOrderType StorageOrder;
    typedef NumArray::StorageType StorageType;
    typedef ublas::matrix<ValueType, StorageOrder, StorageType> BaseType;

  protected:
    DimArray * pDim_;
    ValArray * pValues_;
    mutable Bool released_;

    using BaseType::swap;

  public:
    explicit MatrixRef(NumArray & dat) : BaseType(dat.Dim()[0],
        dat.IsVector() ? 1 : dat.Dim()[1], array_type()),
        pDim_(dat.DimPtr()),
        pValues_(dat.ValuesPtr()),
        released_(false)
    {
      BaseType::data().swap(dat.Values());
    }
    explicit MatrixRef(MultiArray & dat) : BaseType(dat.Dim()[0],
        dat.IsVector() ? 1 : dat.Dim()[1], array_type()),
        pDim_(dat.DimPtr().get()),
        pValues_(dat.ValuesPtr().get()),
        released_(false)
    {
      BaseType::data().swap(dat.Values());
    }

    MatrixRef(MatrixRef & mat_ref);

    MatrixRef(const MatrixRef & mat_ref);

    template<class AE>
    MatrixRef & operator = (const ublas::matrix_expression<AE> & ae)
    {
      BaseType::operator=(ae);
      return *this;
    }

    virtual ~MatrixRef()
    {
      Release();
    }

    void Release();

    Bool Released() const
    {
      return released_;
    }
  };

}

#endif /* BIIPS_MATRIX_HPP_ */
