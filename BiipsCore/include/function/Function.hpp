//                                            -*- C++ -*-
/*! \file Function.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_FUNCTION_HPP_
#define BIIPS_FUNCTION_HPP_

#include "common/Common.hpp"

namespace Biips
{

  //! Function abstract class
  /*!
   * Function objects are associated to LogicalNode objects.
   * The implementations follow the singleton design pattern,
   * as it is only a utility class.
   *
   * Function objects have a name_ member corresponding to
   * the associated BUGS language keyword.
   * e.g. "+" for binary node addition operator.
   * "exp" for exponential elementwise function.
   *
   * The nParam_ member is the number of parameters of the function.
   * It corresponds to the number of parents of a LogicalNode defined
   * by this Function.
   * e.g. For Add concrete Function
   * nParam_ = 2 (left and right operands)
   * For Exp concrete Function
   * nParam_ = 1
   */
  class Function
  {
  protected:
     //! name of the function in BUGS language
    const String name_;
    //! number of parameters of the function
    const Size nParam_;

    /*!
     * Constructs a Function object from a name and number
     * of parameters.
     * @param name name of the function in BUGS language
     * @param nParam number of parameters
     * This constructor is protected according to the singleton
     * design pattern.
     */
    Function(const String & name, Size nParam) : name_(name), nParam_(nParam) {};

  public:
    typedef Function SelfType;
    //! A Function shared pointer
    typedef Types<SelfType>::Ptr Ptr;

    //! Name member accessor
    String const & Name() const { return name_; };
    //! Number of parameters member accessor
    Size NParam() const { return nParam_; };

    //! Evaluation of the function
    /*!
     * Evaluates the function given its parameters values in paramValues.
     * @param paramValues values array of the parameters.
     * The DataType values must ordered according
     * to the parameters order of the function.
     */
    virtual DataType Eval(const DataType::Array & paramValues) const = 0;

    virtual ~Function() {};
  };

}

#endif /* BIIPS_FUNCTION_HPP_ */
