//                                               -*- C++ -*-
/*! \file DBeta.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_DBETA_HPP_
#define BIIPS_DBETA_HPP_

#include "distribution/Distribution.hpp"

namespace Biips
{

  class DBeta : public Distribution
  {
  protected:
    typedef DBeta SelfType;

    DBeta() : Distribution("dbeta", 2){};

    virtual Bool checkParamDims(const Types<DimArray::Ptr>::Array & paramDims) const;
    virtual DimArray dim(const Types<DimArray::Ptr>::Array & paramDims) const;

  public:
    virtual MultiArray Sample(const MultiArray::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogPdf(const MultiArray & x, const MultiArray::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_DBETA_HPP_ */
