//                                               -*- C++ -*-
/*! \file sandbox.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "sampler/Accumulator.hpp"
#include "model/Monitor.hpp"

int main(int argc, char ** argv)
{
  using namespace Biips;

  ScalarAccumulator acc;
  acc.AddFeature(PDF);
  acc.SetPdfParam(10, 10);

  acc.Init();

  for(Size i=0; i<100; ++i)
  {
    acc.Push(Scalar(i), 1.0);
  }

  return 0;
}
