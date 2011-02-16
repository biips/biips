//                                               -*- C++ -*-
/*! \file Vector.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_VECTOR_HPP_
#define BIIPS_VECTOR_HPP_

#include "common/DataType.hpp"

#include <boost/numeric/ublas/vector.hpp>
//#include <boost/numeric/ublas/vector_proxy.hpp>

namespace Biips
{

  class Vector : public ublas::vector<DataType::ValueType, DataType::StorageType>
  {
  public:
    typedef ublas::vector<DataType::ValueType, DataType::StorageType> BaseType;

    Vector() {};
    explicit Vector(size_type sz) : BaseType(sz) {}
    Vector(size_type sz, value_type val) : BaseType(sz, array_type(sz, val)) {}
    Vector(size_type sz, const array_type & value) : BaseType(sz, value) {}
    explicit Vector(const DataType & data) : BaseType(data.Length(), data.Value()) {}
    template<class AE>
    Vector(const ublas::vector_expression<AE> &ae) : BaseType(ae) {}

    template<class AE>
    Vector & operator = (const ublas::vector_expression<AE> & ae) { operator=(Vector(ae)); return *this; }
  };


  class VectorRef : public ublas::vector<DataType::ValueType, DataType::StorageType>
  {
  public:
    typedef DataType::ValueType ValueType;
    typedef DataType::StorageType StorageType;
    typedef ublas::vector<ValueType, StorageType> BaseType;

  protected:
    DataType * pData_;
    mutable Bool released_;

    BaseType::swap;

  public:
    explicit VectorRef(DataType & dat) : BaseType(dat.Length(), array_type()), pData_(&dat), released_(false) { BaseType::data().swap(pData_->Value()); }

    VectorRef(VectorRef & vec_ref) : BaseType(vec_ref.size(), array_type()), pData_(vec_ref.pData_), released_(vec_ref.released_)
    {
      if ( !released_ )
      {
        swap(vec_ref);
        vec_ref.released_ = true;
      }
    }

    VectorRef(const VectorRef & vec_ref) : BaseType(vec_ref), pData_(vec_ref.pData_), released_(vec_ref.released_)
    {
      if ( !released_ )
        vec_ref.released_ = true;
    }

    // TODO put in cpp
    template<class AE>
    VectorRef & operator = (const ublas::vector_expression<AE> & ae) { BaseType::operator=(ae); pData_->Dim()[0] = size(); return *this; }

    // TODO put in cpp
    void resize (size_type s, bool preserve = true) { BaseType::resize(s, preserve); pData_->Dim()[0] = s; }

    virtual ~VectorRef() { if (! released_) data().swap(pData_->Value()); }

    void Release() { if (! released_) { data().swap(pData_->Value()); released_ = true;} }

    Bool Released() const { return released_; }
  };

}

#endif /* BIIPS_VECTOR_HPP_ */
