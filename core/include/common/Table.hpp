//                                               -*- C++ -*-
/*! \file Table.hpp
 * \brief
 *
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_TABLE_HPP_
#define BIIPS_TABLE_HPP_

#include "common/Types.hpp"
#include <map>

namespace Biips
{

  template<typename T>
  class Table
  {
  public:
    typedef T StoredType;
    typedef typename Types<StoredType>::Ptr StoredPtr;

  protected:
    std::map<String, StoredPtr> map_;
    std::map<String, StoredPtr> aliasMap_;

    static const StoredPtr nullPtr_;

  public:
    Table() {}

    Bool Insert(const StoredPtr & ptr);

    Bool Contains(const String & name) const;

    const StoredPtr & GetPtr(const String & name) const;
    const StoredPtr & operator [] (const String & name) const { return GetPtr(name); }

    virtual ~Table() {}
  };


  template<typename T>
  const typename Table<T>::StoredPtr Table<T>::nullPtr_;


  template<typename T>
  Bool Table<T>::Insert(const StoredPtr & ptr)
  {
    if (!ptr)
      throw LogicError("Can not Insert in Table: ptr is NULL.");

    if (Contains(ptr->Name()))
      return false;

    map_.insert(std::make_pair(ptr->Name(), ptr));

    if (!ptr->Alias().empty())
      aliasMap_.insert(std::make_pair(ptr->Alias(), ptr));

    return true;
  }


  template<typename T>
  Bool Table<T>::Contains(const String & name) const
  {
    return map_.find(name) != map_.end() || aliasMap_.find(name) != aliasMap_.end();
  }


  template<typename T>
  const typename Table<T>::StoredPtr & Table<T>::GetPtr(const String & name) const
  {
    if (map_.find(name) != map_.end())
      return map_.at(name);
    else if (aliasMap_.find(name) != aliasMap_.end())
      return aliasMap_.at(name);
    else
      return nullPtr_;
  }

}

#endif /* BIIPS_TABLE_HPP_ */
