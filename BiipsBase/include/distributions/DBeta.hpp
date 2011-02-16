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

  public:
    virtual DataType Sample(const DataType::Array & paramValues, Rng * pRng) const;
    virtual Scalar LogUnnormPdf(const DataType & x, const DataType::Array & paramValues) const;

    static Distribution::Ptr Instance() { static Distribution::Ptr p_instance(new SelfType()); return p_instance; };
  };

}

#endif /* BIIPS_DBETA_HPP_ */
