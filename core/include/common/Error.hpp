//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file Error.hpp
 * \brief
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_ERROR_HPP_
#define BIIPS_ERROR_HPP_

#include <exception>
#include "common/Types.hpp"

//! A macro defining the catch instruction of biips errors
#define BIIPS_CATCH_ERRORS                     \
  catch(Biips::NodeError & e)                  \
  {                                            \
    std::cerr << "Error in node "              \
      << e.GetNodeId() << "\n"                 \
      << e.what();                             \
  }                                            \
  catch (Biips::RuntimeError & e)              \
  {                                            \
    std::cerr << "RUNTIME ERROR: " << e.what();\
  }                                            \
  catch(Biips::LogicError & e)                 \
  {                                            \
    std::cerr << "LOGIC ERROR: " << e.what();  \
  }/*                                            \
  catch(const std::exception & e)              \
  {                                            \
    std::cerr << "STD ERROR: " << e.what();    \
  }*/

namespace Biips
{

  class LogicError : public std::exception
  {
  protected:
    String msg_;

  public:
    typedef std::exception BaseType;

    LogicError(const String & msg) : BaseType(), msg_(msg) {}

    virtual const char* what() const throw() { return msg_.c_str(); }

    virtual ~LogicError() throw() {}
  };


  class RuntimeError : public std::exception
  {
  protected:
    String msg_;

  public:
    typedef std::exception BaseType;

    RuntimeError(const String & msg) : BaseType(), msg_(msg) {}

    virtual const char* what() const throw() { return msg_.c_str(); }

    virtual ~RuntimeError() throw() {}
  };


  class NumericalError : public RuntimeError
  {
  public:
    typedef RuntimeError BaseType;

    NumericalError(const String & msg) : BaseType(msg + "\nA numerical problem has appeared. If this problem persits, consider the data may be poorly adapted to the model or vice versa.") {}

    virtual ~NumericalError() throw() {}
  };


  class NodeError : public RuntimeError
  {
  protected:
    NodeId nodeId_;

  public:
    typedef RuntimeError BaseType;

    NodeError(NodeId nodeId, const String & msg) : BaseType(msg), nodeId_(nodeId) {}

    NodeId GetNodeId() const { return nodeId_; }

    virtual ~NodeError() throw() {}
  };
}

#endif /* BIIPS_ERROR_HPP_ */
