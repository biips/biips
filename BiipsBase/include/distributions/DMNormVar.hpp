//                                               -*- C++ -*-
/*! \file DMNormVar.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#ifndef BIIPS_DMNORMVAR_HPP_
#define BIIPS_DMNORMVAR_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{
  class DMNormVar : public Distribution
  {
  protected:
    typedef DMNormVar SelfType;

    DMNormVar() : Distribution("dmnorm.var", 2) {};

  public:
    virtual DataType Sample(const DataType::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };

} /* namespace Biips */


#endif /* BIIPS_DMNORMVAR_HPP_ */
