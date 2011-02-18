//                                               -*- C++ -*-
/*! \file DataType.hpp
 * \brief A n-dimensional data object class
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DATATYPE_HPP_
#define BIIPS_DATATYPE_HPP_

#include "common/Types.hpp"
#include "common/DimArray.hpp"
#include "common/ValArray.hpp"
#include "common/IndexRange.hpp"

#include <cassert>

namespace Biips
{
  class Vector;
  class Matrix;

  class VectorRef;
  class MatrixRef;

  class DataArray;

  //! A n-dimensional data object class
  /*!
   * DataType represents a wholly defined n-dimensional data object,
   * aggregating its dimension and its values.
   * Those two members are aggregated with shared pointers, so DataType
   * objects are light weight objects providing a convenient interface.
   */
  class DataType
  {
  public:
    //! The Values storage Type is ValArray
    typedef ValArray StorageType;
    typedef StorageType::value_type ValueType;
    typedef StorageType::StorageOrder StorageOrder;

    typedef DataType SelfType;
    //! An array of DataType
    typedef DataArray Array;

  protected:
    //! Shared pointer of the dimensions array of the data
    Types<DimArray>::Ptr pDim_;
    //! Shared pointer of the values array of the data
    Types<StorageType>::Ptr pValue_;

    void printRecursion(std::ostream & os, const IndexRange & range, IndexRange::SizeType dim, IndexRange::Indices & indices) const;

  public:
    DataType() {}
    /*!
     * DataType affectation operator.
     * Copies DimArray and ValArray shared pointers.
     */
    DataType & operator = (const DataType & dat);
    /*!
     * Creates a DataType with DimArray pointed by pDim.
     * Dynamically allocates a ValArray with size corresponding to
     * the DimArray length.
     * @param pDim dimension array shared pointer
     */
    explicit DataType(const DimArray::Ptr & pDim) : pDim_(pDim), pValue_(new StorageType(pDim->Length())) {}
    /*!
     * Creates a DataType with DimArray dim.
     * Dynamically allocates a copy of dim, and a ValArray
     * with size corresponding to the DimArray length.
     * @param dim dimension array
     */
    explicit DataType(const DimArray & dim) : pDim_(new DimArray(dim)), pValue_(new StorageType(dim.Length())) {}
    /*!
     * Creates a DataType with DimArray pointed by pDim.
     * Dynamically allocates a ValArray with size corresponding to
     * the DimArray length and values set to val.
     * @param pDim dimension array shared pointer
     * @param val value of the elements
     */
    DataType(const DimArray::Ptr & pDim, ValueType val) : pDim_(pDim), pValue_(new StorageType(pDim->Length(), val)) {}
    /*!
     * Creates a DataType with DimArray dim.
     * Dynamically allocates a copy of dim, and a ValArray
     * with size corresponding to the DimArray length and values set to val.
     * @param dim dimension array
     * @param val value of the elements
     */
    DataType(const DimArray & dim, ValueType val) : pDim_(new DimArray(dim)), pValue_(new StorageType(dim.Length(), val)) {}
    /*!
     * Creates a DataType with DimArray dim
     * and ValArray pointed by pValue.
     * Dynamically allocates a copy of dim.
     * @param dim dimension array
     * @param pValue values array shared pointer
     */
    DataType(const DimArray & dim, const Types<StorageType>::Ptr & pValue) : pDim_(new DimArray(dim)), pValue_(pValue) {} // TODO check dims
    /*!
     * Creates a DataType with DimArray pointed by pDim
     * and ValArray value.
     * Dynamically allocates a copy of value.
     * @param pDim dimension array shared pointer
     * @param value values array
     */
    DataType(const DimArray::Ptr & pDim, const StorageType & value) : pDim_(pDim), pValue_(new StorageType(value)) {} // TODO check dims
    /*!
     * Creates a DataType with DimArray dim and ValArray value.
     * Dynamically allocates copies of dim and value.
     * @param dim dimension array
     * @param value values array
     */
    DataType(const DimArray & dim, const StorageType & value) : pDim_(new DimArray(dim)), pValue_(new StorageType(value)) {} // TODO check dims
    /*!
     * Creates a DataType with DimArray pointed by pDim
     * and ValArray pointed by pValue.
     * @param pDim dimension array shared pointer
     * @param pValue values array shared pointer
     */
    DataType(const DimArray::Ptr & pDim, const Types<StorageType>::Ptr & pValue) : pDim_(pDim), pValue_(pValue) {} // TODO check dims
    /*!
     * Creates a DataType from a Scalar.
     * Dynamically allocates its DimArray, set to (1). i.e. a one element vector containing 1,
     * and its ValArray, containing one element set to val.
     * @param val scalar value.
     */
    explicit DataType(Scalar val) : pDim_(new DimArray(1, 1)), pValue_(new StorageType(1, val)) {} // FIXME 2 dimensions ?
    /*!
     * Creates a DataType from a Vector.
     * Dynamically allocates its Dimarray, and its ValArray copying the vector values.
     */
    explicit DataType(const Vector & vec);
    /*!
     * Creates a DataType from a Matrix.
     * Dynamically allocates its Dimarray, and its ValArray copying the matrix values.
     */
    explicit DataType(const Matrix & mat);

    //! Number of dimensions accessor
    /*!
     * @return the size of the DimArray
     */
    Size NDim() const { assert(pDim_); return pDim_->size(); };
    //! Length of the DatType
    /*!
     * Computes the product of the sizes of each dimension
     * @return the number of elements of the DataType
     */
    Size Length() const { assert(pDim_); return pDim_->Length(); };
    /*!
     * Indicates whether the DataType corresponds to a scalar object,
     * i.e. containing only one element.
     */
    Bool IsScalar() const { assert(pDim_); return pDim_->IsScalar(); }
    /*!
     * Indicates whether the DataType corresponds to a vector object,
     * i.e. only one dimension.
     */
    Bool IsVector() const { assert(pDim_); return pDim_->IsVector(); }
    /*!
     * Indicates whether the DataType corresponds to a matrix object,
     * i.e. exactly two dimensions.
     */
    Bool IsMatrix() const { assert(pDim_); return pDim_->IsMatrix(); }
    /*!
     * Indicates whether the DataType corresponds to a squared matrix.
     */
    Bool IsSquared() const { assert(pDim_); return pDim_->IsSquared(); }

    //! DimArray accessor.
    /*!
     * @return the dimension array
     */
    const DimArray & Dim() const { assert(pDim_); return *pDim_; };
    //! DimArray accessor.
    /*!
     * @return a reference to the dimension array
     */
    DimArray & Dim() { assert(pDim_); return *pDim_; };
    //! DimArray shared pointer accessor.
    /*!
     * @return the dimension array shared pointer
     */
    const DimArray::Ptr & DimPtr() const { return pDim_; };

    /*!
     * @return an IndexRange of the DataType
     */
    const IndexRange Range() const { assert(pDim_); return IndexRange(*pDim_); }

    //! ValArray accessor.
    /*!
     * @return the values array
     */
    const StorageType & Values() const { assert(pValue_); return *pValue_; };
    //! ValArray accessor.
    /*!
     * @return a reference to the values array
     */
    StorageType & Values() { assert(pValue_); return *pValue_; };
    //! ValArray accessor.
    /*!
     * @return the values array shared pointer
     */
    const Types<StorageType>::Ptr & ValuesPtr() const { return pValue_; };

    /*!
     * Most of the DataType objects will contain one scalar value.
     * This method gives a convenient Scalar handle of the ValArray.
     * @return The first value of the ValArray if size is 1
     */
    Scalar ScalarView() const { assert( IsScalar() && pValue_ && (pValue_->size() == 1) ); return (*pValue_)[0]; } // TODO throw exception
    /*!
     * Most of the DataType objects will contain one scalar value.
     * This method gives a convenient Scalar handle of the ValArray.
     * @return  A reference to the first value of the ValArray if size is 1
     */
    Scalar & ScalarView() { assert( IsScalar() && pValue_ && (pValue_->size() == 1) ); return (*pValue_)[0]; } // TODO throw exception
    /*!
     * If the DataType is a vector, this method gives a convenient
     * vector interface to it.
     * @return A vector proxy object.
     * @warning The ValArray values are swapped with the VectorRef one
     * until its destruction or call to its Release method. During this time,
     * the DataType's ValArray is empty.
     */
    VectorRef VectorView();
    /*!
     * If the DataType is a matrix, this method gives a convenient
     * matrix interface to it.
     * @return A matrix proxy object
     * @warning The ValArray values are swapped with the MatrixRef ones
     * until its destruction or call to its Release method. During this time,
     * the DataType's ValArray is empty.
     */
    MatrixRef MatrixView();

    /*!
     * Prints the elements of the DataType according to its dimensions.
     * @param os output stream receiving the print
     */
    void Print(std::ostream & os) const;


    DataType & Alloc(const DataType & from)
    {
      pDim_ = DimArray::Ptr(new DimArray(from.Dim()));
      pValue_ = StorageType::Ptr(new StorageType(from.Values()));
      return *this;
    } // TODO check dim
    DataType & Alloc(Scalar scal)
    {
      pDim_ = DimArray::Ptr(new DimArray(1,1));
      pValue_ = StorageType::Ptr(new StorageType(1,scal));
      return *this;
    }
    DataType & Alloc(const Vector & vec);
    DataType & Alloc(const Matrix & mat);
    DataType & Alloc(const DimArray & dim)
    {
      pDim_ = DimArray::Ptr(new DimArray(dim));
      return *this;
    } // TODO check dim
    DataType & Alloc(const StorageType & val)
    {
      pValue_ = StorageType::Ptr(new StorageType(val));
      return *this;
    } // TODO check dim
    DataType & Alloc(const DimArray & dim, const StorageType & val)
    {
      pDim_ = DimArray::Ptr(new DimArray(dim));
      pValue_ = StorageType::Ptr(new StorageType(val));
      return *this;
    } // TODO check dim
    DataType & AllocValue(Scalar scal = 0.0)
    {
      if (pDim_)
        pValue_ = StorageType::Ptr(new StorageType(Length(), scal));
      return *this;
    }

    DataType & SetPtr(const DataType & from)
    {
      *this = from;
      return *this;
    } // TODO check dim
    DataType & SetPtr(const DimArray::Ptr & pDim)
    {
      pDim_ = pDim;
      return *this;
    } // TODO check dim
    DataType & SetPtr(const StorageType::Ptr & pVal)
    {
      pValue_ = pVal;
      return *this;
    } // TODO check dim
    DataType & SetPtr(const DimArray::Ptr & pDim, const StorageType::Ptr & pVal)
    {
      pDim_ = pDim;
      pValue_ = pVal;
      return *this;
    } // TODO check dim
  };

  //! DataType ostream operator
  std::ostream & operator << (std::ostream & os, const DataType & dat);



  class DataArray : public std::vector<DataType>
  {
  public:
    typedef DataArray SelfType;
    typedef std::vector<DataType> BaseType;

  protected:
    // forbid assign
    DataArray & operator= (const DataArray & rhs);

    BaseType::assign;
    BaseType::insert;
    BaseType::push_back;
    BaseType::resize;

  public:
    DataArray() {}
    DataArray(Size s) : BaseType(s) {}

    DataArray & Alloc(const DataArray & from)
    {
      resize(from.size());
      for (Size i = 0; i<size(); ++i)
        (*this)[i].Alloc(from[i]);
      return *this;
    }

    template<typename InputIterator>
    DataArray & Alloc(InputIterator first, InputIterator last)
    {
      resize(std::distance(first, last));
      for (Size i = 0; i<size(); ++i)
      {
        (*this)[i].Alloc(*first);
        ++first;
      }
      return *this;
    }

    DataArray & SetPtr(const DataArray & from)
    {
      BaseType::operator=(from);
      return *this;
    }

    template<typename InputIterator>
    DataArray & SetPtr(InputIterator first, InputIterator last)
    {
      BaseType::assign(first, last);
      return *this;
    }
  };


  template<>
  struct Types<DataType>
  {
    typedef DataType SelfType; //!< The template type itself

    //! Smart pointer type defined with a boost::shared_ptr<>
    typedef boost::shared_ptr<SelfType> Ptr;

    typedef DataArray Array; //!< Array type
    typedef Array::iterator Iterator; //!< Iterator type of the Array type
    typedef Array::reverse_iterator ReverseIterator; //!< Reverse iterator type of the Array type
    typedef Array::const_iterator ConstIterator; //!< Const iterator type of the Array type
    typedef Array::const_reverse_iterator ConstReverseIterator; //!< Const reverse iterator type of the Array type

    typedef std::vector<Ptr> PtrArray; //!< Array of pointers defined with a std::vector<boost::shared_ptr<> >

    typedef std::pair<SelfType, SelfType> Pair;
    typedef std::pair<Iterator, Iterator> IteratorPair;
    typedef std::pair<ConstIterator, ConstIterator> ConstIteratorPair; //!< Pair of ConstIterators type, used to define a range
  };

}

#endif /* BIIPS_DATATYPE_HPP_ */
