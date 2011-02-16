//                                               -*- C++ -*-
/*! \file BiipsBase.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPSBASE_HPP_
#define BIIPSBASE_HPP_

#include "samplers/ConjugateNormal.hpp"
#include "samplers/ConjugateNormalLinear.hpp"
#include "samplers/ConjugateNormalVar.hpp"
#include "samplers/ConjugateNormalVarLinear.hpp"
#include "samplers/ConjugateMNormal.hpp"
#include "samplers/ConjugateMNormalLinear.hpp"
#include "samplers/ConjugateMNormalVar.hpp"
#include "samplers/ConjugateMNormalVarLinear.hpp"
#include "samplers/ConjugateBeta.hpp"
#include "samplers/DiscreteBruteOptimal.hpp"


namespace Biips
{

  void loadBaseModule();

}

#endif /* BIIPSBASE_HPP_ */
