//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
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

/*! \file Table.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
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
