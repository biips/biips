//                                               -*- C++ -*-
/*! \file MultiArray.hpp
 * \brief A n-dimensional data object class
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MULTIARRAY_HPP_
#define BIIPS_MULTIARRAY_HPP_

#include "common/Types.hpp"
#include "common/DimArray.hpp"
#include "common/ValArray.hpp"
#include "common/Error.hpp"

namespace Biips
{
  class Vector;
  class Matrix;

  class VectorRef;
  class MatrixRef;

  class MultiArrayArray;
  class MultiArrayPair;

  //! A n-dimensional data object class
  /*!
   * MultiArray represents a wholly defined n-dimensional data object,
   * aggregating its dimension and its values.
   * Those two members are aggregated with shared pointers, so MultiArray
   * objects are light weight objects providing a convenient interface.
   */
  class MultiArray
  {
  public:
    //! The Values storage Type is ValArray
    typedef ValArray StorageType;
    typedef StorageType::value_type ValueType;
    typedef StorageOrder StorageOrderType;

    typedef MultiArray SelfType;
    //! An array of MultiArray
    typedef MultiArrayArray Array;
    //! A pair of MultiArray
    typedef MultiArrayPair Pair;

  protected:
    //! Shared pointer of the dimensions array of the data
    Types<DimArray>::Ptr pDim_;
    //! Shared pointer of the values array of the data
    Types<StorageType>::Ptr pValues_;

    void checkDimPtr() const { if(!pDim_) throw LogicError("Can not access dimension of MultiArray: null pointer."); }
    void checkValuesPtr() const { if(!pValues_) throw LogicError("Can not access values of MultiArray: null pointer."); }

  public:
    MultiArray() {}
    /*!
     * MultiArray affectation operator.
     * Copies DimArray and ValArray shared pointers.
     */
    MultiArray & operator = (const MultiArray & dat);
    /*!
     * Creates a MultiArray with DimArray pointed by pDim.
     * Dynamically allocates a ValArray with size corresponding to
     * the DimArray length.
     * @param pDim dimension array shared pointer
     */
    explicit MultiArray(const DimArray::Ptr & pDim) : pDim_(pDim), pValues_(new StorageType(pDim->Length())) {}
    /*!
     * Creates a MultiArray with DimArray dim.
     * Dynamically allocates a copy of dim, and a ValArray
     * with size corresponding to the DimArray length.
     * @param dim dimension array
     */
    explicit MultiArray(const DimArray & dim) : pDim_(new DimArray(dim)), pValues_(new StorageType(dim.Length())) {}
    /*!
     * Creates a MultiArray with DimArray pointed by pDim.
     * Dynamically allocates a ValArray with size corresponding to
     * the DimArray length and values set to val.
     * @param pDim dimension array shared pointer
     * @param val value of the elements
     */
    MultiArray(const DimArray::Ptr & pDim, ValueType val) : pDim_(pDim), pValues_(new StorageType(pDim->Length(), val)) {}
    /*!
     * Creates a MultiArray with DimArray dim.
     * Dynamically allocates a copy of dim, and a ValArray
     * with size corresponding to the DimArray length and values set to val.
     * @param dim dimension array
     * @param val value of the elements
     */
    MultiArray(const DimArray & dim, ValueType val) : pDim_(new DimArray(dim)), pValues_(new StorageType(dim.Length(), val)) {}
    /*!
     * Creates a MultiArray with DimArray dim
     * and ValArray pointed by pValue.
     * Dynamically allocates a copy of dim.
     * @param dim dimension array
     * @param pValue values array shared pointer
     */
    MultiArray(const DimArray & dim, const Types<StorageType>::Ptr & pValue) : pDim_(new DimArray(dim)), pValues_(pValue) {} // TODO check dims
    /*!
     * Creates a MultiArray with DimArray pointed by pDim
     * and ValArray value.
     * Dynamically allocates a copy of value.
     * @param pDim dimension array shared pointer
     * @param value values array
     */
    MultiArray(const DimArray::Ptr & pDim, const StorageType & value) : pDim_(pDim), pValues_(new StorageType(value)) {} // TODO check dims
    /*!
     * Creates a MultiArray with DimArray dim and ValArray value.
     * Dynamically allocates copies of dim and value.
     * @param dim dimension array
     * @param value values array
     */
    MultiArray(const DimArray & dim, const StorageType & value) : pDim_(new DimArray(dim)), pValues_(new StorageType(value)) {} // TODO check dims
    /*!
     * Creates a MultiArray with DimArray pointed by pDim
     * and ValArray pointed by pValue.
     * @param pDim dimension array shared pointer
     * @param pValue values array shared pointer
     */
    MultiArray(const DimArray::Ptr & pDim, const Types<StorageType>::Ptr & pValue) : pDim_(pDim), pValues_(pValue) {} // TODO check dims
    /*!
     * Creates a MultiArray from a Scalar.
     * Dynamically allocates its DimArray, set to (1). i.e. a one element vector containing 1,
     * and its ValArray, containing one element set to val.
     * @param val scalar value.
     */
    explicit MultiArray(Scalar val) : pDim_(new DimArray(1, 1)), pValues_(new StorageType(1, val)) {} // FIXME 2 dimensions ?
    /*!
     * Creates a MultiArray from a Vector.
     * Dynamically allocates its Dimarray, and its ValArray copying the vector values.
     */
    explicit MultiArray(const Vector & vec);
    /*!
     * Creates a MultiArray from a Matrix.
     * Dynamically allocates its Dimarray, and its ValArray copying the matrix values.
     */
    explicit MultiArray(const Matrix & mat);

    //! Number of dimensions accessor
    /*!
     * @return the size of the DimArray
     */
    Size NDim() const { checkDimPtr(); return pDim_->size(); };
    //! Length of the DatType
    /*!
     * Computes the product of the sizes of each dimension
     * @return the number of elements of the MultiArray
     */
    Size Length() const { checkDimPtr(); return pDim_->Length(); };
    /*!
     * Indicates whether the MultiArray corresponds to a scalar object,
     * i.e. containing only one element.
     */
    Bool IsScalar() const { checkDimPtr(); return pDim_->IsScalar(); }
    /*!
     * Indicates whether the MultiArray corresponds to a vector object,
     * i.e. only one dimension.
     */
    Bool IsVector() const { checkDimPtr(); return pDim_->IsVector(); }
    /*!
     * Indicates whether the MultiArray corresponds to a matrix object,
     * i.e. exactly two dimensions.
     */
    Bool IsMatrix() const { checkDimPtr(); return pDim_->IsMatrix(); }
    /*!
     * Indicates whether the MultiArray corresponds to a squared matrix.
     */
    Bool IsSquared() const { checkDimPtr(); return pDim_->IsSquared(); }

    //! DimArray accessor.
    /*!
     * @return the dimension array
     */
    const DimArray & Dim() const { checkDimPtr(); return *pDim_; };
    //! DimArray accessor.
    /*!
     * @return a reference to the dimension array
     */
    DimArray & Dim() { checkDimPtr(); return *pDim_; };
    //! DimArray shared pointer accessor.
    /*!
     * @return the dimension array shared pointer
     */
    const DimArray::Ptr & DimPtr() const { return pDim_; };

    //! ValArray accessor.
    /*!
     * @return the values array
     */
    const StorageType & Values() const { checkValuesPtr(); return *pValues_; };
    //! ValArray accessor.
    /*!
     * @return a reference to the values array
     */
    StorageType & Values() { checkValuesPtr(); return *pValues_; };
    //! ValArray accessor.
    /*!
     * @return the values array shared pointer
     */
    const Types<StorageType>::Ptr & ValuesPtr() const { return pValues_; };

    /*!
     * Most of the MultiArray objects will contain one scalar value.
     * This method gives a convenient Scalar handle of the ValArray.
     * @return The first value of the ValArray if size is 1
     */
    Scalar ScalarView() const { return (*pValues_)[0]; } // TODO throw exception
    /*!
     * Most of the MultiArray objects will contain one scalar value.
     * This method gives a convenient Scalar handle of the ValArray.
     * @return  A reference to the first value of the ValArray if size is 1
     */
    Scalar & ScalarView() { return (*pValues_)[0]; } // TODO throw exception


    MultiArray & Alloc(const MultiArray & from)
    {
      pDim_ = DimArray::Ptr(new DimArray(from.Dim()));
      pValues_ = StorageType::Ptr(new StorageType(from.Values()));
      return *this;
    } // TODO check dim
    MultiArray & Alloc(Scalar scal)
    {
      pDim_ = DimArray::Ptr(new DimArray(1,1));
      pValues_ = StorageType::Ptr(new StorageType(1,scal));
      return *this;
    }
    MultiArray & Alloc(const Vector & vec);
    MultiArray & Alloc(const Matrix & mat);
    MultiArray & Alloc(const DimArray & dim)
    {
      pDim_ = DimArray::Ptr(new DimArray(dim));
      return *this;
    } // TODO check dim
    MultiArray & Alloc(const StorageType & val)
    {
      pValues_ = StorageType::Ptr(new StorageType(val));
      return *this;
    } // TODO check dim
    MultiArray & Alloc(const DimArray & dim, const StorageType & val)
    {
      pDim_ = DimArray::Ptr(new DimArray(dim));
      pValues_ = StorageType::Ptr(new StorageType(val));
      return *this;
    } // TODO check dim
    MultiArray & AllocValue(Scalar scal = 0.0)
    {
      if (pDim_)
        pValues_ = StorageType::Ptr(new StorageType(Length(), scal));
      return *this;
    }

    MultiArray & SetPtr(const MultiArray & from)
    {
      *this = from;
      return *this;
    } // TODO check dim
    MultiArray & SetPtr(const DimArray::Ptr & pDim)
    {
      pDim_ = pDim;
      return *this;
    } // TODO check dim
    MultiArray & SetPtr(const StorageType::Ptr & pVal)
    {
      pValues_ = pVal;
      return *this;
    } // TODO check dim
    MultiArray & SetPtr(const DimArray::Ptr & pDim, const StorageType::Ptr & pVal)
    {
      pDim_ = pDim;
      pValues_ = pVal;
      return *this;
    } // TODO check dim

    Bool IsNULL() const { return !(pDim_ || pValues_); }
  };


  // -----------------------------------------------------------------------------
  // NULL_MULTIARRAY
  // -----------------------------------------------------------------------------

  const MultiArray NULL_MULTIARRAY = MultiArray();


  // -----------------------------------------------------------------------------
  // MultiArrayArray
  // -----------------------------------------------------------------------------

  class MultiArrayArray : public std::vector<MultiArray>
  {
  public:
    typedef MultiArrayArray SelfType;
    typedef std::vector<MultiArray> BaseType;

  protected:
    // forbid assign
    MultiArrayArray & operator= (const MultiArrayArray & rhs);

    BaseType::assign;
    BaseType::insert;
    BaseType::push_back;
    BaseType::resize;

  public:
    MultiArrayArray() {}
    MultiArrayArray(Size s) : BaseType(s) {}

    MultiArrayArray & Alloc(const MultiArrayArray & from)
    {
      resize(from.size());
      for (Size i = 0; i<size(); ++i)
        (*this)[i].Alloc(from[i]);
      return *this;
    }

    template<typename InputIterator>
    MultiArrayArray & Alloc(InputIterator first, InputIterator last)
    {
      resize(std::distance(first, last));
      for (Size i = 0; i<size(); ++i)
      {
        (*this)[i].Alloc(*first);
        ++first;
      }
      return *this;
    }

    MultiArrayArray & SetPtr(const MultiArrayArray & from)
    {
      BaseType::operator=(from);
      return *this;
    }

    template<typename InputIterator>
    MultiArrayArray & SetPtr(InputIterator first, InputIterator last)
    {
      BaseType::assign(first, last);
      return *this;
    }
  };


  // -----------------------------------------------------------------------------
  // MultiArrayPair
  // -----------------------------------------------------------------------------

  class MultiArrayPair : public std::pair<MultiArray, MultiArray>
  {
  public:
    typedef MultiArrayPair SelfType;
    typedef std::pair<MultiArray, MultiArray> BaseType;

  protected:
    // forbid assign
    MultiArrayPair & operator= (const MultiArrayPair & rhs);

  public:
    MultiArrayPair() {}

    MultiArrayPair & Alloc(const MultiArrayPair & from)
    {
      first.Alloc(from.first);
      second.Alloc(from.second);
      return *this;
    }

    MultiArrayPair & SetPtr(const MultiArrayPair & from)
    {
      BaseType::operator=(from);
      return *this;
    }
  };

  const MultiArrayPair NULL_MULTIARRAYPAIR;

  // -----------------------------------------------------------------------------
  // MultiArray Types
  // -----------------------------------------------------------------------------

  template<>
  struct Types<MultiArray>
  {
    typedef MultiArray SelfType; //!< The template type itself

    //! Smart pointer type defined with a boost::shared_ptr<>
    typedef boost::shared_ptr<SelfType> Ptr;

    typedef MultiArrayArray Array; //!< Array type
    typedef Array::iterator Iterator; //!< Iterator type of the Array type
    typedef Array::reverse_iterator ReverseIterator; //!< Reverse iterator type of the Array type
    typedef Array::const_iterator ConstIterator; //!< Const iterator type of the Array type
    typedef Array::const_reverse_iterator ConstReverseIterator; //!< Const reverse iterator type of the Array type

    typedef std::vector<Ptr> PtrArray; //!< Array of pointers defined with a std::vector<boost::shared_ptr<> >

    typedef MultiArrayPair Pair;
    typedef std::pair<Iterator, Iterator> IteratorPair;
    typedef std::pair<ConstIterator, ConstIterator> ConstIteratorPair; //!< Pair of ConstIterators type, used to define a range
  };

}

#endif /* BIIPS_MULTIARRAY_HPP_ */
