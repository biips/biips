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
#include "graph/Graph.hpp"

#include "functions/Multiply.hpp"
#include "functions/Add.hpp"
#include "functions/MatMult.hpp"
#include "functions/Substract.hpp"
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

namespace Biips
{

  // TODO add a insert NodeSamplerFactory function

  void insertFunction(const Function::Ptr & func)
  {
    static Graph::FunctionTab & funcTab = Graph::FuncTab();
    funcTab[func->Name()] = func;
  }


  void insertDistribution(const Distribution::Ptr & dist)
  {
    static Graph::DistributionTab & distTab = Graph::DistTab();
    distTab[dist->Name()] = dist;
  }

  void loadBaseModule()
  {
    insertFunction(Multiply::Instance());
    insertFunction(Add::Instance());
    insertFunction(MatMult::Instance());
    insertFunction(Substract::Instance());
    insertFunction(Divide::Instance());
    insertFunction(Neg::Instance());
    insertFunction(Pow::Instance());
    insertFunction(Cos::Instance());
    insertFunction(Sqrt::Instance());
    insertFunction(Transpose::Instance());
    insertFunction(Sum::Instance());
    insertFunction(Equal::Instance());

    insertDistribution(DNorm::Instance());
    insertDistribution(DNormVar::Instance());
    insertDistribution(DMNorm::Instance());
    insertDistribution(DMNormVar::Instance());
    insertDistribution(DBeta::Instance());
    insertDistribution(DBin::Instance());
    insertDistribution(DCat::Instance());
  }

}
