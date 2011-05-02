//                                               -*- C++ -*-
/*! \file Distribution.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DISTRIBUTION_HPP_
#define BIIPS_DISTRIBUTION_HPP_

#include "common/Common.hpp"

namespace Biips
{

  const String UNDEF_DIST = "undef";

  //! Distribution abstract class
  /*!
   * Distribution objects are associated to StochasticNode objects.
   * The implementations follow the singleton design pattern,
   * as it is only a utility class.
   *
   * Distribution objects have a name member corresponding to
   * the associated BUGS language keyword.
   * e.g. "dnorm" for normal distribution
   *
   * The nParam_ member is the number of parameters of the distribution.
   * It corresponds to the number of parents of a SchasticNode defined
   * by this Distribution.
   * e.g. For DNorm, i.e. normal distribution
   * nParam_ = 2 (mean and variance)
   */
  class Distribution
  {
  protected:
    //! name of the distribution in BUGS language
    const String name_;
    //! number of parameters of the distribution
    const Size nParam_;

    /*!
     * Constructs a Distribution object from a name and number
     * of parameters.
     * @param name name of the distribution in BUGS language
     * @param nParam number of parameters
     * This constructor is protected according to the singleton
     * design pattern.
     */
    Distribution(const String & name, Size nParam) : name_(name), nParam_(nParam) {};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const = 0;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const = 0;

  public:
    typedef Distribution SelfType;
    //! A Distribution shared pointer
    typedef Types<SelfType>::Ptr Ptr;

    //! Name member accessor
    String const & Name() const { return name_; };
    //! Number of parameters member accessor
    Size NParam() const { return nParam_; };

    Bool CheckNParam(Size n) const { return n == nParam_; }

    /*!
    * Checks that dimensions of the parameters are correct.
    *
    * This function only needs to be run once for each parameter
    * vector. Thereafter, the values of the parameters will change,
    * but the dimensions will not.
    */
    Bool CheckParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;

    /*!
    * Calculates what the dimension of the distribution should be,
    * based on the dimensions of its parameters.
    */
    DimArray Dim(const Types<DimArray::Ptr>::Array & paramDims) const;

    //! Sampling method
    /*!
     * Samples according to the distribution, given MultiArray values of the parameters
     * and a RNG object.
     * \param paramValues values array of the parameters.
     * The MultiArray values must ordered according
     * to the parameters order of the distribution.
     * \param pRng RNG pointer
     * \return a random sample
     */
    virtual MultiArray Sample(const MultiArray::Array & paramValues, Rng * pRng) const = 0;
    //! Logarithmic unnormalized probability density function
    /*!
     * Evaluates the log unnormalized pdf at point x, given the distribution
     * parameters values in paramValues.
     * Computation of the particles weights doesn't involve normalizing constant
     * evaluation.
     * @param x point of evaluation
     * @param paramValues values array of the parameters.
     * The MultiArray values must ordered according
     * to the parameters order of the distribution.
     */
    virtual Scalar LogPdf(const MultiArray & x, const MultiArray::Array & paramValues) const = 0;

    virtual ~Distribution() {};

  };


} /* namespace Biips */

#endif /* BIIPS_DISTRIBUTION_HPP_ */
