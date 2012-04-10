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

/*! \file BUGSModel.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_BUGSMODEL_HPP_
#define BIIPS_BUGSMODEL_HPP_

#include "model/Model.hpp"
#include "model/NodeArray.hpp"
#include "model/SymbolTable.hpp"
#include "common/Accumulator.hpp"
#include "model/NodeArrayMonitor.hpp"

#include <map>

namespace Biips
{

  class BUGSModel: public Model
  {
  public:
    typedef Model BaseType;
    typedef BUGSModel SelfType;
    typedef Types<SelfType>::Ptr Ptr;

  protected:
    SymbolTable symbolTable_;
    Types<String>::Array filterMonitorsNames_;
    Types<IndexRange>::Array filterMonitorsRanges_;
    Types<String>::Array smoothTreeMonitorsNames_;
    Types<IndexRange>::Array smoothTreeMonitorsRanges_;
    Types<String>::Array smoothMonitorsNames_;
    Types<IndexRange>::Array smoothMonitorsRanges_;

  public:
    BUGSModel() :
      symbolTable_(*this)
    {
    }

    std::map<String, MultiArray> Sample(Rng * pRng) const;

    SymbolTable & GetSymbolTable()
    {
      return symbolTable_;
    }

    Bool SetFilterMonitor(const String & name, const IndexRange & range = NULL_RANGE);
    Bool SetSmoothTreeMonitor(const String & name, const IndexRange & range =
        NULL_RANGE);
    Bool SetSmoothMonitor(const String & name, const IndexRange & range = NULL_RANGE);

    Bool
    IsFilterMonitored(const String & name, IndexRange range = NULL_RANGE) const;
    Bool IsSmoothTreeMonitored(const String & name, IndexRange range =
        NULL_RANGE) const;
    Bool
    IsSmoothMonitored(const String & name, IndexRange range = NULL_RANGE) const;

    void PrintSamplersSequence(std::ostream & out) const;

    void PrintGraphviz(std::ostream & out) const;

    // TODO manage multi statFeature
    Bool ExtractFilterStat(String name, StatTag statFeature, std::map<
        IndexRange, MultiArray> & statMap) const;
    Bool ExtractSmoothTreeStat(String name, StatTag statFeature, std::map<
        IndexRange, MultiArray> & statMap) const;
    Bool ExtractSmoothStat(String name, StatTag statFeature, std::map<
        IndexRange, MultiArray> & statMap) const;

    Bool ExtractFilterPdf(String name,
                          std::map<IndexRange, Histogram> & pdfMap,
                          Size numBins = 40,
                          Scalar cacheFraction = 0.25) const;
    Bool ExtractSmoothTreePdf(String name,
                              std::map<IndexRange, Histogram> & pdfMap,
                              Size numBins = 40,
                              Scalar cacheFraction = 0.25) const;
    Bool ExtractSmoothPdf(String name,
                          std::map<IndexRange, Histogram> & pdfMap,
                          Size numBins = 40,
                          Scalar cacheFraction = 0.25) const;

    Bool DumpData(std::map<String, MultiArray> & dataMap) const;
    Bool ChangeData(const std::map<String, MultiArray> & dataMap);
    Bool GetLogPriorDensity(Scalar & prior,
                         const String & variable,
                         IndexRange range = NULL_RANGE) const;

    Bool
        DumpFilterMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const;
    Bool
        DumpSmoothTreeMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const;
    Bool
        DumpSmoothMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const;

    Bool SampleSmoothTreeParticle(const Rng::Ptr & pRng, std::map<
        String, MultiArray> & sampledValues) const;

    void virtual ClearFilterMonitors();
    void virtual ClearSmoothTreeMonitors();
    void virtual ClearSmoothMonitors();
  };

}

#endif /* BIIPS_BUGSMODEL_HPP_ */
