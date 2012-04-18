//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
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

/*! \file ModelTest.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MODELTEST_HPP_
#define BIIPS_MODELTEST_HPP_

#include "graph/Graph.hpp"
#include "sampler/ForwardSampler.hpp"
#include "common/Accumulator.hpp"
#include "common/ArrayAccumulator.hpp"
#include "function/Function.hpp"
#include "distribution/Distribution.hpp"

namespace Biips
{
  class ModelTest
  {
  protected:
    int argc_;
    char** argv_;
    String name_;
    Size verbose_;
    Size showMode_;
    std::ostream & os_;

    Graph::Ptr pDataGraph_;
    Graph::Ptr pModelGraph_;
    ForwardSampler::Ptr pSampler_;
    Types<NodeSamplerFactory::Ptr>::Array nodeSamplerFactoryInvOrder_;
    Accumulator scalarAcc_;
    DensityAccumulator densAcc_;
    QuantileAccumulator quantAcc_;
    DiscreteAccumulator discreteScalarAcc_;
    ArrayAccumulator arrayAcc_;
    std::map<String, Size> sizeParamMap_;
    std::map<String, Scalar> scalarParamMap_;
    std::map<String, MultiArray> dataTypeParamMap_;

    Types<String>::Array inDataVarNames_;
    Types<String>::Array obsVarNames_;
    Types<String>::Array inBenchFilterVarNames_;
    Types<String>::Array inBenchSmoothVarNames_;
    Types<String>::Array printResultsVarNames_;

    std::map<String, DimArray::Ptr> dimArrayMap_;

    std::map<String, Types<NodeId>::Array> dataNodeIdMap_;
    std::map<String, Types<NodeId>::Array> modelNodeIdMap_;

    std::map<String, MultiArray::Array> dataValuesMap_;
    std::map<String, MultiArray::Array> benchFilterValuesMap_;
    std::map<String, MultiArray::Array> benchSmoothValuesMap_;
    std::map<String, MultiArray::Array> smcFilterValuesMap_;
    std::map<String, MultiArray::Array> smcSmoothValuesMap_;

    FunctionTable funcTab_;
    DistributionTable distTab_;

    template<typename InputIterator>
    void printValues(std::ostream & os,
                     const String & name,
                     InputIterator first,
                     const InputIterator & last,
                     char separator = ' ') const;

    void printValues(std::ostream & os,
                     const String & name,
                     const MultiArray::Array & dataArray,
                     Size len = 1,
                     char separator = ' ') const;

    void printLine(std::ostream & os,
                   const MultiArray::Array & dataArray,
                   Size dim = 0,
                   char separator = ' ') const;

    void
    setDataArrayMap(const std::map<String, std::vector<MultiArray> > & from,
                    std::map<String, MultiArray::Array> & to);

    void setObsValues();

    Bool error(Scalar & error,
               const Types<String>::Array & varNames,
               const std::map<String, MultiArray::Array> & smcValuesMap,
               const std::map<String, MultiArray::Array> & benchValuesMap) const;

    virtual void initFilterAccumulators(Size nParticles, Size numBins) = 0;
    virtual void initSmoothAccumulators(Size nParticles, Size numBins) = 0;

    virtual void filterAccumulate(Size t) = 0;
    virtual void smoothAccumulate(Size t) = 0;

    void setName(const String & name)
    {
      name_ = name;
    }

    void loadBase();

  public:
    ModelTest(int argc,
              char** argv,
              const String & name,
              Size verbose = 1,
              Size showMode = 0,
              std::ostream & os = std::cout) :
      argc_(argc), argv_(argv), name_(name), verbose_(verbose),
          showMode_(showMode), os_(os)
    {
    }

    virtual const String & Name() const
    {
      return name_;
    }

    virtual void PrintIntro() = 0;

    //    virtual void InputModelParam(std::istream & is = std::cin) = 0;
    void SetModelParam(const std::map<String, MultiArray> & model_param_map);
    void SetDimensions(const std::map<String, DimArray::Ptr> & dim_map);
    void SetData(const std::map<String, std::vector<MultiArray> > & data_map)
    {
      setDataArrayMap(data_map, dataValuesMap_);
    }
    void SetBenchFilter(const std::map<String, std::vector<MultiArray> > & bench_filter_map)
    {
      setDataArrayMap(bench_filter_map, benchFilterValuesMap_);
    }
    void SetBenchSmooth(const std::map<String, std::vector<MultiArray> > & bench_smooth_map)
    {
      setDataArrayMap(bench_smooth_map, benchSmoothValuesMap_);
    }

    virtual void BuildDataGraph();

    Size DataGraphSize() const
    {
      return pDataGraph_->GetSize();
    }
    const std::map<String, Size> & DataNodesSummary() const
    {
      return pDataGraph_->NodesSummary();
    }
    const std::map<String, Size> & DataUnobsNodesSummary() const
    {
      return pDataGraph_->UnobsNodesSummary();
    }

    void PrintDataGraph()
    {
      pDataGraph_->PrintGraph(os_);
    }
    void PrintDataGraphviz()
    {
      pDataGraph_->PrintGraphviz(os_);
    }

    void SampleData(Size rngSeed);

    //    void OutputData(std::ostream & os = std::cout) const;

    //    void InputData(std::istream & is = std::cin);

    virtual void RunBench();

    //    void OutputBench(std::ostream & os = std::cout) const;

    //    void InputBench(std::istream & is = std::cin);

    virtual void BuildModelGraph() = 0;

    Size ModelGraphSize() const
    {
      return pModelGraph_->GetSize();
    }
    const std::map<String, Size> & ModelNodesSummary() const
    {
      return pModelGraph_->NodesSummary();
    }
    const std::map<String, Size> & ModelUnobsNodesSummary() const
    {
      return pModelGraph_->UnobsNodesSummary();
    }

    void PrintModelGraph()
    {
      pModelGraph_->PrintGraph(os_);
    }
    void PrintModelGraphviz()
    {
      pModelGraph_->PrintGraphviz(os_);
    }

    void ClearSMC();
    virtual void BuildSMC(Bool prior = false);
    virtual void RunSMC(Size nParticles,
                        Size rngSeed,
                        const String & rsType = "stratified",
                        Scalar essThreshold = 0.5,
                        Bool showProgress = true,
                        Size numBins = 40);

    virtual void PlotResults(const String & plotFileName = "") const = 0;

    Bool ErrorFilter(Scalar & errorFilter) const
    {
      return error(errorFilter,
                   inBenchFilterVarNames_,
                   smcFilterValuesMap_,
                   benchFilterValuesMap_);
    }

    Bool ErrorSmooth(Scalar & errorSmooth) const
    {
      return error(errorSmooth,
                   inBenchFilterVarNames_,
                   smcSmoothValuesMap_,
                   benchSmoothValuesMap_);
    }

    Scalar LogNormConst() const
    {
      return pSampler_->LogNormConst();
    }

  };

  template<typename InputIterator>
  void ModelTest::printValues(std::ostream & os,
                              const String & name,
                              InputIterator first,
                              const InputIterator & last,
                              char separator) const
  {
    os << name << " = ";
    while (first != last)
    {
      os << (*first) << separator;
      ++first;
    }
    os << std::endl;
  }

}

#endif /* BIIPS_MODELTEST_HPP_ */
