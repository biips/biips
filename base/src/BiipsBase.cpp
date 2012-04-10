//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file BiipsBase.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

extern "C" {
  void BiipsBase_is_present(void) {}
}

#include "BiipsBase.hpp"
#include "sampler/ForwardSampler.hpp"

#include "functions/LogicalOperators.hpp"
#include "functions/ComparisonOperators.hpp"
#include "functions/ArithmeticOperators.hpp"
#include "functions/MatMult.hpp"
#include "functions/Pow.hpp"
#include "functions/IfElse.hpp"
#include "functions/TrigonometricFunctions.hpp"
#include "functions/UsualFunctions.hpp"
#include "functions/Step.hpp"
#include "functions/Transpose.hpp"
#include "functions/Sum.hpp"
#include "functions/Inverse.hpp"
#include "functions/Max.hpp"
#include "functions/Min.hpp"
#include "functions/Prod.hpp"
#include "functions/Equals.hpp"

#include "distributions/DNorm.hpp"
#include "distributions/DNormVar.hpp"
#include "distributions/DMNorm.hpp"
#include "distributions/DMNormVar.hpp"
#include "distributions/DBeta.hpp"
#include "distributions/DBin.hpp"
#include "distributions/DCat.hpp"
#include "distributions/DUnif.hpp"
#include "distributions/DBern.hpp"
#include "distributions/DExp.hpp"
#include "distributions/DLnorm.hpp"
#include "distributions/DChisqr.hpp"
#include "distributions/DGamma.hpp"
#include "distributions/DT.hpp"
#include "distributions/DWeib.hpp"
//#include "distributions/DPar.hpp"
#include "distributions/DPois.hpp"
#include "distributions/DF.hpp"
//#include "distributions/DDexp.hpp"
//#include "distributions/DGenGamma.hpp"

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
    // TODO check returned boolean
    funcTab.Insert(And::Instance());
    funcTab.Insert(Or::Instance());
    funcTab.Insert(Not::Instance());

    funcTab.Insert(GreaterThan::Instance());
    funcTab.Insert(GreaterOrEqual::Instance());
    funcTab.Insert(LessThan::Instance());
    funcTab.Insert(LessOrEqual::Instance());
    funcTab.Insert(Equal::Instance());
    funcTab.Insert(NotEqual::Instance());

    funcTab.Insert(Add::Instance());
    funcTab.Insert(Subtract::Instance());
    funcTab.Insert(Multiply::Instance());
    funcTab.Insert(Divide::Instance());
    funcTab.Insert(MatMult::Instance());
    funcTab.Insert(Neg::Instance());

    funcTab.Insert(PowInfix::Instance());
    funcTab.Insert(Pow::Instance());

    funcTab.Insert(Equals::Instance());
    funcTab.Insert(IfElse::Instance());

    funcTab.Insert(Cos::Instance());
    funcTab.Insert(Sin::Instance());
    funcTab.Insert(Tan::Instance());
    funcTab.Insert(ArcCos::Instance());
    funcTab.Insert(ArcSin::Instance());
    funcTab.Insert(ArcTan::Instance());
    funcTab.Insert(Cosh::Instance());
    funcTab.Insert(Sinh::Instance());
    funcTab.Insert(Tanh::Instance());
    funcTab.Insert(ArcCosh::Instance());
    funcTab.Insert(ArcSinh::Instance());
    funcTab.Insert(ArcTanh::Instance());

    funcTab.Insert(Abs::Instance());
    funcTab.Insert(Sqrt::Instance());
    funcTab.Insert(Exp::Instance());
    funcTab.Insert(Log::Instance());
    funcTab.Insert(Round::Instance());
    funcTab.Insert(Trunc::Instance());

    funcTab.Insert(Step::Instance());

    funcTab.Insert(Transpose::Instance());
    funcTab.Insert(Sum::Instance());
    funcTab.Insert(Inverse::Instance());
    funcTab.Insert(Max::Instance());
    funcTab.Insert(Min::Instance());
    funcTab.Insert(Prod::Instance());

    // TODO check returned boolean
    distTab.Insert(DNorm::Instance());
    distTab.Insert(DNormVar::Instance());
    distTab.Insert(DMNorm::Instance());
    distTab.Insert(DMNormVar::Instance());
    distTab.Insert(DBeta::Instance());
    distTab.Insert(DBin::Instance());
    distTab.Insert(DCat::Instance());
    distTab.Insert(DUnif::Instance());
    distTab.Insert(DBern::Instance());
    distTab.Insert(DExp::Instance());
    distTab.Insert(DLnorm::Instance());
    distTab.Insert(DChisqr::Instance());
    distTab.Insert(DGamma::Instance());
    distTab.Insert(DT::Instance());
    distTab.Insert(DWeib::Instance());
//    distTab.Insert(DPar::Instance());
    distTab.Insert(DPois::Instance());
    distTab.Insert(DF::Instance());
//    distTab.Insert(DDexp::Instance());
//    distTab.Insert(DGenGamma::Instance());

    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(ConjugateMNormalLinearFactory::Instance(),
                                                                     true));
    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(ConjugateMNormalVarLinearFactory::Instance(),
                                                                     true));
    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(ConjugateNormalLinearFactory::Instance(),
                                                                     true));
    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(ConjugateNormalVarLinearFactory::Instance(),
                                                                     true));

    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(ConjugateMNormalFactory::Instance(),
                                                                     true));
    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(ConjugateMNormalVarFactory::Instance(),
                                                                     true));
    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(ConjugateBetaFactory::Instance(),
                                                                     true));
    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(ConjugateNormalFactory::Instance(),
                                                                     true));
    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(ConjugateNormalVarFactory::Instance(),
                                                                     true));

    ForwardSampler::NodeSamplerFactories().push_front(std::make_pair(DiscreteOptimalFactory::Instance(),
                                                                     true));
  }

}
