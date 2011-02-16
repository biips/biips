//                                               -*- C++ -*-
/*! \file DNorm.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DNORM_HPP_
#define BIIPS_DNORM_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{
  class DNorm : public Distribution
  {
  protected:
    typedef DNorm SelfType;

    DNorm() : Distribution("dnorm", 2) {};

  public:
    virtual DataType Sample(const DataType::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };

} /* namespace Biips */

#endif /* BIIPS_DNORM_HPP_ */
