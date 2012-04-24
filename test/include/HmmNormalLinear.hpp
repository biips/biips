//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
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

/*! \file HmmNormalLinear.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_HMMNORMALLINEAR_HPP_
#define BIIPS_HMMNORMALLINEAR_HPP_

#include "ModelTest.hpp"

namespace Biips
{

  class HmmNormalLinear: public ModelTest
  {
  protected:
    Bool precFlag_;

    static const String NAME_;

    void initAccumulators(Size nParticles, Size numBins, std::map<String,
        MultiArray::Array> & statsValuesMap);

    virtual void initFilterAccumulators(Size nParticles, Size numBins);
    virtual void initSmoothAccumulators(Size nParticles, Size numBins);

    void accumulate(Size t,
                    std::map<String, MultiArray::Array> & statsValuesMap,
                    const String & title);

    virtual void filterAccumulate(Size t);
    virtual void smoothAccumulate(Size t);

  public:
    typedef ModelTest BaseType;

    HmmNormalLinear(int argc,
                    char** argv,
                    Size verbose = 1,
                    Size showMode = 0,
                    Bool precFlag = false,
                    std::ostream & os = std::cout);

    virtual void PrintIntro();

    //    virtual void InputModelParam(std::istream & is = std::cin);

    virtual void RunBench();

    virtual void BuildModelGraph();

    virtual void PlotResults(const String & plotFileName = "") const;

  };

}

#endif /* BIIPS_HMMNORMALLINEAR_HPP_ */
