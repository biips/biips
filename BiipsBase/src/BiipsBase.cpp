//                                               -*- C++ -*-
/*! \file BiipsBase.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "BiipsBase.hpp"
#include "function/FunctionTable.hpp"
#include "distribution/DistributionTable.hpp"
#include "sampler/SMCSampler.hpp"

#include "functions/Multiply.hpp"
#include "functions/Add.hpp"
#include "functions/MatMult.hpp"
#include "functions/Subtract.hpp"
#include "functions/Divide.hpp"
#include "functions/Neg.hpp"
#include "functions/Pow.hpp"
#include "functions/Cos.hpp"
#include "functions/Sqrt.hpp"
#include "functions/Transpose.hpp"
#include "functions/Sum.hpp"
#include "functions/Equal.hpp"

#include "distributions/DNorm.hpp"
#include "distributions/DNormVar.hpp"
#include "distributions/DMNorm.hpp"
#include "distributions/DMNormVar.hpp"
#include "distributions/DBeta.hpp"
#include "distributions/DBin.hpp"
#include "distributions/DCat.hpp"

#include "samplers/DiscreteOptimal.hpp"
#include "samplers/ConjugateNormalVar.hpp"
#include "samplers/ConjugateNormal.hpp"
#include "samplers/ConjugateBeta.hpp"
#include "samplers/ConjugateMNormalVar.hpp"
#include "samplers/ConjugateMNormal.hpp"
#include "samplers/ConjugateNormalVarLinear.hpp"
#include "samplers/ConjugateNormalLinear.hpp"
#include "samplers/ConjugateMNormalVarLinear.hpp"
#include "samplers/ConjugateMNormalLinear.hpp"

namespace Biips
{

  void loadBaseModule(FunctionTable & funcTab, DistributionTable & distTab)
  {
    funcTab.Insert(Multiply::Instance());
    funcTab.Insert(Add::Instance());
    funcTab.Insert(MatMult::Instance());
    funcTab.Insert(Subtract::Instance());
    funcTab.Insert(Divide::Instance());
    funcTab.Insert(Neg::Instance());
    funcTab.Insert(Pow::Instance());
    funcTab.Insert(Cos::Instance());
    funcTab.Insert(Sqrt::Instance());
    funcTab.Insert(Transpose::Instance());
    funcTab.Insert(Sum::Instance());
    funcTab.Insert(Equal::Instance());

    distTab.Insert(DNorm::Instance());
    distTab.Insert(DNormVar::Instance());
    distTab.Insert(DMNorm::Instance());
    distTab.Insert(DMNormVar::Instance());
    distTab.Insert(DBeta::Instance());
    distTab.Insert(DBin::Instance());
    distTab.Insert(DCat::Instance());

    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(ConjugateMNormalLinearFactory::Instance(), true));
    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(ConjugateMNormalVarLinearFactory::Instance(), true));
    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(ConjugateNormalLinearFactory::Instance(), true));
    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(ConjugateNormalVarLinearFactory::Instance(), true));

    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(ConjugateMNormalFactory::Instance(), true));
    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(ConjugateMNormalVarFactory::Instance(), true));
    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(ConjugateBetaFactory::Instance(), true));
    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(ConjugateNormalFactory::Instance(), true));
    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(ConjugateNormalVarFactory::Instance(), true));

    SMCSampler::NodeSamplerFactories().push_front(
        std::make_pair(DiscreteOptimalFactory::Instance(), true));
  }

}
