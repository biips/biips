//                                               -*- C++ -*-
/*! \file DMNorm.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DMNORM_HPP_
#define BIIPS_DMNORM_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{
  class DMNorm : public Distribution
  {
  protected:
    typedef DMNorm SelfType;

    DMNorm() : Distribution("dmnorm", 2) {};

  public:
    virtual DataType Sample(const DataType::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };

} /* namespace Biips */

#endif /* BIIPS_DMNORM_HPP_ */
