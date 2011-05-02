//                                               -*- C++ -*-
/*! \file DBin.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DBIN_HPP_
#define BIIPS_DBIN_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{
  
  class DBin : public Distribution
    {
    protected:
      typedef DBin SelfType;

      DBin() : Distribution("dbin", 2) {};

      virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
      virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

    public:
      virtual MultiArray Sample(const MultiArray::Array & paramValues, Rng * pRng) const;
      virtual Scalar LogPdf(const MultiArray & x, const MultiArray::Array & paramValues) const;

      static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
    };

}

#endif /* BIIPS_DBIN_HPP_ */
