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

    public:
      virtual DataType Sample(const DataType::Array & paramValues, Rng * pRng) const;
      virtual Scalar LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const;

      static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
    };

}

#endif /* BIIPS_DBIN_HPP_ */
