//                                               -*- C++ -*-
/*! \file ModelTest.hpp
* \brief
*
* $LastChangedBy$
* $LastChangedDate$
* $LastChangedRevision$
* $Id$
*/

#ifndef BIIPS_MODELTEST_HPP_
#define BIIPS_MODELTEST_HPP_

#include "graph/Graph.hpp"
#include "sampler/SMCSampler.hpp"
#include "sampler/Accumulator.hpp"

namespace Biips
{
  class ModelTest
  {
  protected:
    int argc_;
    char** argv_;
    String name_;
    Size verbose_;
    Bool showFinalPlot_;
    Bool showAllPlot_;
    mutable std::ostream & os_;

    Graph::Ptr pDataGraph_;
    Graph::Ptr pModelGraph_;
    SMCSampler::Ptr pSampler_;
    Types<NodeSamplerFactory::Ptr>::Array nodeSamplerFactoryInvOrder_;
    ScalarAccumulator scalarAcc_;
    DiscreteScalarAccumulator discreteScalarAcc_;
    ElementAccumulator elementAcc_;

    std::map<String, Size> sizeParamMap_;
    std::map<String, Scalar> scalarParamMap_;
    std::map<String, DataType> dataTypeParamMap_;

    Types<String>::Array inDataVarNames_;
    Types<String>::Array obsVarNames_;
    Types<String>::Array inBenchFilterVarNames_;
    Types<String>::Array inBenchSmoothVarNames_;
    Types<String>::Array printResultsVarNames_;

    std::map<String, DimArray::Ptr> dimArrayMap_;

    std::map<String, Types<NodeId>::Array> dataNodeIdMap_;
    std::map<String, Types<NodeId>::Array> modelNodeIdMap_;

    std::map<String, DataType::Array> dataValuesMap_;
    std::map<String, DataType::Array> benchFilterValuesMap_;
    std::map<String, DataType::Array> benchSmoothValuesMap_;
    std::map<String, DataType::Array> smcFilterValuesMap_;
    std::map<String, DataType::Array> smcSmoothValuesMap_;


    template<typename InputIterator>
    void printValues(std::ostream & os, const String & name, InputIterator first, const InputIterator & last, char separator = ' ') const;

    void printValues(std::ostream & os, const String & name, const DataType::Array & dataArray, Size len = 1, char separator = ' ') const;

    void printLine(std::ostream & os, const DataType::Array & dataArray, Size dim = 0, char separator = ' ') const;

//    void inputValues(DataType::Array & values, Size varLen, const DimArray::Ptr varDimPtr, std::istream & is) const;

    void setDataArrayMap(const std::map<String, std::vector<DataType> > & from, std::map<String, DataType::Array> & to);

    void setObsValues();

    Bool error(Scalar & error, const Types<String>::Array & varNames,
        const std::map<String, DataType::Array> & smcValuesMap,
        const std::map<String, DataType::Array> & benchValuesMap) const;

    virtual void initFilterAccumulators() = 0;
    virtual void initSmoothAccumulators() = 0;

    virtual void filterAccumulate(Size t) = 0;
    virtual void smoothAccumulate(Size t) = 0;

    void setName(const String & name) { name_ = name; };

  public:
    ModelTest(int argc, char** argv, const String & name, Size verbose = 1, Bool showFinal = false, Bool showAll = false, std::ostream & os = std::cout)
    : argc_(argc), argv_(argv), name_(name), verbose_(verbose), showFinalPlot_(showFinal), showAllPlot_(showAll), os_(os) {};

    virtual const String & Name() const { return name_; };

    virtual void PrintIntro() const = 0;

//    virtual void InputModelParam(std::istream & is = std::cin) = 0;
    void SetModelParam(const std::map<String, DataType> & model_param_map);
    void SetDimensions(const std::map<String, DimArray::Ptr> & dim_map);
    void SetData(const std::map<String, std::vector<DataType> > & data_map) { setDataArrayMap(data_map, dataValuesMap_); }
    void SetBenchFilter(const std::map<String, std::vector<DataType> > & bench_filter_map) { setDataArrayMap(bench_filter_map, benchFilterValuesMap_); }
    void SetBenchSmooth(const std::map<String, std::vector<DataType> > & bench_smooth_map) { setDataArrayMap(bench_smooth_map, benchSmoothValuesMap_); }

    virtual void BuildDataGraph();

    Size DataGraphSize() const { return pDataGraph_->GetSize(); };
    const std::map<String, Size> & DataNodesSummary() const { return pDataGraph_->NodesSummary(); };
    const std::map<String, Size> & DataUnobsNodesSummary() const { return pDataGraph_->UnobsNodesSummary(); };

    void PrintDataGraph() { pDataGraph_->PrintGraph(os_); };

    void SampleData(Size rngSeed);

//    void OutputData(std::ostream & os = std::cout) const;

//    void InputData(std::istream & is = std::cin);

    virtual void RunBench();

//    void OutputBench(std::ostream & os = std::cout) const;

//    void InputBench(std::istream & is = std::cin);

    virtual void BuildModelGraph() = 0;

    Size ModelGraphSize() const { return pModelGraph_->GetSize(); };
    const std::map<String, Size> & ModelNodesSummary() const { return pModelGraph_->NodesSummary(); };
    const std::map<String, Size> & ModelUnobsNodesSummary() const { return pModelGraph_->UnobsNodesSummary(); };

    void PrintModelGraph() { pModelGraph_->PrintGraph(os_); };

    void ClearSMC();
    virtual void RunSMC(Size nParticles, Size rngSeed, Bool prior = false, Scalar essThreshold = 0.5, ResampleType rsType = SMC_RESAMPLE_STRATIFIED, Bool showProgress = true);

    virtual void PlotResults(const String & plotFileName = "") const = 0;

    Bool ErrorFilter(Scalar & errorFilter) const { return error(errorFilter, inBenchFilterVarNames_, smcFilterValuesMap_, benchFilterValuesMap_); };
    Bool ErrorSmooth(Scalar & errorSmooth) const { return error(errorSmooth, inBenchFilterVarNames_, smcSmoothValuesMap_, benchSmoothValuesMap_); };

  };


  template<typename InputIterator>
  void ModelTest::printValues(std::ostream & os, const String & name, InputIterator first, const InputIterator & last, char separator) const
  {
    os << name << " = ";
    while(first != last)
    {
      os << (*first) << separator;
      ++first;
    }
    os << std::endl;
  }

}

#endif /* BIIPS_MODELTEST_HPP_ */
