//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file NumArray.hpp
 * \brief A n-dimensional data object class
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_NUMARRAY_HPP_
#define BIIPS_NUMARRAY_HPP_

#include "common/Types.hpp"
#include "common/DimArray.hpp"
#include "common/ValArray.hpp"
#include "common/Error.hpp"
#include <sstream>

namespace Biips
{
  // -----------------------------------------------------------------------------
  // NumArray
  // -----------------------------------------------------------------------------

  class NumArrayArray;
  class NumArrayPair;

  class MultiArray;

  class NumArray
  {
  public:
    //! The Values storage Type is ValArray
    typedef ValArray StorageType;
    typedef StorageType::value_type ValueType;
    typedef StorageOrder StorageOrderType;

    typedef NumArray SelfType;
    //! An array of NumArray
    typedef NumArrayArray Array;
    //! A pair of NumArray
    typedef NumArrayPair Pair;

  protected:
    //! pointer of the dimensions array of the data
    DimArray * pDim_;
    //! pointer of the values array of the data
    StorageType * pValues_;

    void checkDimPtr() const
    {
      if (!pDim_)
        throw LogicError("Can not access dimension of NumArray: null pointer.");
    }
    void checkValuesPtr() const
    {
      if (!pValues_)
        throw LogicError("Can not access values of NumArray: null pointer.");
    }

  public:
    NumArray() :
      pDim_(NULL), pValues_(NULL)
    {
    }
    explicit NumArray(DimArray * pDim, StorageType * pVal) :
      pDim_(pDim), pValues_(pVal)
    {
      }
      explicit NumArray(const MultiArray & marray);

    //! Number of dimensions accessor
    /*!
     * @return the size of the DimArray
     */
    Size NDim() const
    {
      checkDimPtr();
      return pDim_->size();
    }

    //! Length of the DatType
    /*!
     * Computes the product of the sizes of each dimension
     * @return the number of elements of the NumArray
     */
    Size Length() const
    {
      checkDimPtr();
      return pDim_->Length();
    }

    /*!
     * Indicates whether the NumArray corresponds to a scalar object,
     * i.e. containing only one element.
     */
    Bool IsScalar() const
    {
      checkDimPtr();
      return pDim_->IsScalar();
    }
    /*!
     * Indicates whether the NumArray corresponds to a vector object,
     * i.e. only one dimension.
     */
    Bool IsVector() const
    {
      checkDimPtr();
      return pDim_->IsVector();
    }
    /*!
     * Indicates whether the NumArray corresponds to a matrix object,
     * i.e. exactly two dimensions.
     */
    Bool IsMatrix() const
    {
      checkDimPtr();
      return pDim_->IsMatrix();
    }
    /*!
     * Indicates whether the NumArray corresponds to a squared matrix.
     */
    Bool IsSquared() const
    {
      checkDimPtr();
      return pDim_->IsSquared();
    }

    //! DimArray accessor.
    /*!
     * @return the dimension array
     */
    const DimArray & Dim() const
    {
      checkDimPtr();
      return *pDim_;
    }

    //! DimArray accessor.
    /*!
     * @return a reference to the dimension array
     */
    DimArray & Dim()
    {
      checkDimPtr();
      return *pDim_;
    }

    //! DimArray pointer accessor.
    /*!
     * @return the dimension array  pointer
     */
    DimArray * DimPtr()
    {
      return pDim_;
    }

    //! DimArray pointer accessor.
    /*!
     * @return the dimension array pointer
     */
    const DimArray * DimPtr() const
    {
      return pDim_;
    }

    //! ValArray accessor.
    /*!
     * @return the values array
     */
    const StorageType & Values() const
    {
      checkValuesPtr();
      return *pValues_;
    }
    //! ValArray accessor.
    /*!
     * @return a reference to the values array
     */
    StorageType & Values()
    {
      checkValuesPtr();
      return *pValues_;
    }
    //! ValArray accessor.
    /*!
     * @return the values array pointer
     */
    StorageType * ValuesPtr()
    {
      return pValues_;
    }

    //! ValArray accessor.
    /*!
     * @return the values array pointer
     */
    const StorageType * ValuesPtr() const
    {
      return pValues_;
    }

    SelfType & SetPtr(DimArray * pDim)
    {
      pDim_ = pDim;
      return *this;
    }
    SelfType & SetPtr(StorageType * pVal)
    {
      pValues_ = pVal;
      return *this;
    }
    SelfType & SetPtr(DimArray * pDim, StorageType * pVal)
    {
      pDim_ = pDim;
      pValues_ = pVal;
      return *this;
    }

    /*!
     * Most of the NumArray objects will contain one scalar value.
     * This method gives a convenient Scalar handle of the ValArray.
     * @return The first value of the ValArray if size is 1
     */
    Scalar ScalarView() const
    {
      return (*pValues_)[0];
    } // TODO throw exception
    /*!
     * Most of the NumArray objects will contain one scalar value.
     * This method gives a convenient Scalar handle of the ValArray.
     * @return  A reference to the first value of the ValArray if size is 1
     */
    Scalar & ScalarView()
    {
      return (*pValues_)[0];
    } // TODO throw exception

    Bool IsNULL() const
    {
      return !pValues_;
    }
  };

  // -----------------------------------------------------------------------------
  // NULL_NUMARRAY
  // -----------------------------------------------------------------------------

  const NumArray NULL_NUMARRAY = NumArray();

  // -----------------------------------------------------------------------------
  // NumArrayArray
  // -----------------------------------------------------------------------------

  class MultiArrayArray;

  class NumArrayArray: public std::vector<NumArray>
  {
  public:
    typedef NumArrayArray SelfType;
    typedef std::vector<NumArray> BaseType;

    NumArrayArray()
    {
    }
    NumArrayArray(Size s) :
      BaseType(s)
    {
    }
    explicit NumArrayArray(const MultiArrayArray & marray);
  };

  // -----------------------------------------------------------------------------
  // NumArrayPair
  // -----------------------------------------------------------------------------

  class NumArrayPair: public std::pair<NumArray, NumArray>
  {
  public:
    typedef NumArrayPair SelfType;
    typedef std::pair<NumArray, NumArray> BaseType;

    NumArrayPair()
    {
    }
  };

  const NumArrayPair NULL_NUMARRAYPAIR;

  Bool allMissing(const NumArray & marray);
  Bool anyMissing(const NumArray & marray);

  /*!
   * Prints the elements of the NumArray according to its dimensions.
   */
  std::ostream & operator << (std::ostream & os, const NumArray & dat);

  // -----------------------------------------------------------------------------
  // NumArray Types
  // -----------------------------------------------------------------------------

  template<>
  struct Types<NumArray>
  {
    typedef NumArray SelfType; //!< The template type itself

    //! Smart pointer type defined with a boost::shared_ptr<>
    typedef boost::shared_ptr<SelfType> Ptr;

    typedef NumArrayArray Array; //!< Array type
    typedef Array::iterator Iterator; //!< Iterator type of the Array type
    typedef Array::reverse_iterator ReverseIterator; //!< Reverse iterator type of the Array type
    typedef Array::const_iterator ConstIterator; //!< Const iterator type of the Array type
    typedef Array::const_reverse_iterator ConstReverseIterator; //!< Const reverse iterator type of the Array type

    typedef std::vector<Ptr> PtrArray; //!< Array of pointers defined with a std::vector<boost::shared_ptr<> >

    typedef NumArrayPair Pair;
    typedef std::pair<Iterator, Iterator> IteratorPair;
    typedef std::pair<ConstIterator, ConstIterator> ConstIteratorPair; //!< Pair of ConstIterators type, used to define a range
  };

}

#endif /* BIIPS_NUMARRAY_HPP_ */
