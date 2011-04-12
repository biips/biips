//                                               -*- C++ -*-
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
#include "smc-exception.hh"

#include "common/Types.hpp"

//! A macro defining the catch instruction of biips errors
#define BIIPS_CATCH_ERRORS                     \
  catch(smc::exception & e)                    \
  {                                            \
    std::cerr << "SMCTC ERROR: " << e;         \
  }                                            \
  catch (RuntimeError & e)                     \
  {                                            \
    std::cerr << "RUNTIME ERROR: " << e.what();\
  }                                            \
  catch(const Biips::NodeError & e)            \
  {                                            \
    std::cerr << "LOGIC ERROR: in node "       \
      << e.nodeId_ << "\n"                     \
      << e.what();                             \
  }                                            \
  catch(const Biips::LogicError & e)           \
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


  class NodeError : public LogicError
  {
  public:
    typedef LogicError BaseType;

    NodeId nodeId_;

    NodeError(NodeId nodeId, const String & msg) : BaseType(msg), nodeId_(nodeId) {}

    virtual ~NodeError() throw() {}
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

}

#endif /* BIIPS_ERROR_HPP_ */
