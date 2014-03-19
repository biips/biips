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

/*! \file ModelTest.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "ModelTest.hpp"
#include "BiipsBase.hpp"
#include "TestIO.hpp"
#include "common/cholesky.hpp"
#include <boost/progress.hpp>

namespace Biips
{

  void ModelTest::printValues(std::ostream & os,
                              const String & name,
                              const MultiArray::Array & dataArray,
                              Size len,
                              char separator) const
  {
    for (Size dim = 0; dim < len; ++dim)
    {
      os << name;
      if (len > 1)
        os << "." << dim + 1;
      os << " = ";
      printLine(os, dataArray, dim, separator);
    }
  }

  void ModelTest::printLine(std::ostream & os,
                            const MultiArray::Array & dataArray,
                            Size dim,
                            char separator) const
  {
    for (Size k = 0; k < dataArray.size(); ++k)
      os << dataArray[k].Values()[dim] << separator;
    os << std::endl;
  }

  void ModelTest::setObsValues()
  {
    NodeValues obs_values(pModelGraph_->GetSize());
    for (Size i_var = 0; i_var < obsVarNames_.size(); ++i_var)
    {
      const String & var_name = obsVarNames_[i_var];
      MultiArray::Array & gen_val = dataValuesMap_[var_name];
      for (Size k = 0; k < gen_val.size(); ++k)
        obs_values[modelNodeIdMap_[var_name][k]] = gen_val[k].ValuesPtr();
    }
    pModelGraph_->SetObsValues(obs_values);
  }

  void ModelTest::SetModelParam(const std::map<String, MultiArray> & model_param_map)
  {
    String var;

    std::map<String, MultiArray>::const_iterator it_param_map =
        model_param_map.begin();
    while (it_param_map != model_param_map.end())
    {
      String var = it_param_map->first;
      if (sizeParamMap_.count(var))
        sizeParamMap_[var] = roundSize(it_param_map->second.ScalarView());
      else if (scalarParamMap_.count(var))
        scalarParamMap_[var] = it_param_map->second.ScalarView();
      else
      {
        DimArray::Ptr p_dim(new DimArray(it_param_map->second.Dim()));
        ValArray::Ptr p_val(new ValArray(it_param_map->second.Values()));
        dataTypeParamMap_[var].SetPtr(p_dim, p_val);
      }
      ++it_param_map;
    }
  }

  void ModelTest::SetDimensions(const std::map<String, DimArray::Ptr> & dim_map)
  {
    dimArrayMap_ = dim_map;
  }

  void ModelTest::setDataArrayMap(const std::map<String,
                                      std::vector<MultiArray> > & from,
                                  std::map<String, MultiArray::Array> & to)
  {
    std::map<String, std::vector<MultiArray> >::const_iterator it_from =
        from.begin();
    while (it_from != from.end())
    {
      to[it_from->first].assign(it_from->second.begin(), it_from->second.end());
      ++it_from;
    }
  }

  void ModelTest::loadBase()
  {
    loadBaseModule(funcTab_, distTab_);
  }

  void ModelTest::BuildDataGraph()
  {
    BuildModelGraph();
    pDataGraph_ = pModelGraph_;
    dataNodeIdMap_ = modelNodeIdMap_;
  }

  void ModelTest::SampleData(Size rngSeed)
  {
    Rng::Ptr p_rng(new Rng(rngSeed));

    NodeValues gen_node_values = pDataGraph_->SampleValues(p_rng.get());

    for (Size i_var = 0; i_var < inDataVarNames_.size(); ++i_var)
    {
      const String & var_name = inDataVarNames_[i_var];
      Size var_len = dataNodeIdMap_[var_name].size();
      dataValuesMap_[var_name] = MultiArray::Array(var_len);
      MultiArray::Array & gen_values = dataValuesMap_[var_name];
      for (Size k = 0; k < gen_values.size(); ++k)
        gen_values[k]
            = MultiArray(dimArrayMap_[var_name],
                         gen_node_values[dataNodeIdMap_[var_name][k]]);
    }
    if (verbose_ >= 2)
    {
      os_ << "Generated values: " << std::endl;

      for (Size i_var = 0; i_var < inDataVarNames_.size(); ++i_var)
      {
        const String & var_name = inDataVarNames_[i_var];
        printValues(os_,
                    var_name,
                    dataValuesMap_[var_name],
                    dimArrayMap_.at(var_name)->Length());
      }
      os_ << std::endl;
    }
  }

  //  void ModelTest::OutputData(std::ostream & os) const
  //  {
  //    for (Size i_var=0; i_var<inDataVarNames_.size(); ++i_var)
  //    {
  //      const String & var_name = inDataVarNames_[i_var];
  //      const MultiArray::Array & gen_values = dataValuesMap_.at(var_name);
  //      printValues(os, var_name, gen_values, dimArrayMap_.at(var_name)->Length());
  //    }
  //  }


  //  void ModelTest::InputData(std::istream & is)
  //  {
  //    for (Size i_var=0; i_var<inDataVarNames_.size(); ++i_var)
  //    {
  //      const String & var_name = inDataVarNames_[i_var];
  //      Size var_len = dataNodeIdMap_[var_name].size();
  //      inputValues(genValuesMap_[var_name], var_len, dimArrayMap_[var_name], is);
  //    }
  //
  //    if (verbose_)
  //    {
  //      os_ << "Input values: " << std::endl;
  //
  //      for (Size i_var=0; i_var<inDataVarNames_.size(); ++i_var)
  //      {
  //        const String & var_name = inDataVarNames_[i_var];
  //        printValues(os_, var_name, genValuesMap_[var_name], dimArrayMap_[var_name]->Length());
  //      }
  //    }
  //  }


  void ModelTest::RunBench()
  {
    os_ << "WARNING: no bench implemented." << std::endl;
  }
  ;

  //  void ModelTest::OutputBench(std::ostream & os) const
  //  {
  //    for (Size i_var=0; i_var<inBenchFilterVarNames_.size(); ++i_var)
  //    {
  //      const String & var_name = inBenchFilterVarNames_[i_var];
  //      printValues(os, var_name, benchFilterValuesMap_.at(var_name), dimArrayMap_.at(var_name)->Length());
  //    }
  //    for (Size i_var=0; i_var<inBenchSmoothVarNames_.size(); ++i_var)
  //    {
  //      const String & var_name = inBenchSmoothVarNames_[i_var];
  //      printValues(os, var_name, benchSmoothValuesMap_.at(var_name), dimArrayMap_.at(var_name)->Length());
  //    }
  //  }


  //  void ModelTest::InputBench(std::istream & is)
  //  {
  //    for (Size i_var=0; i_var<inBenchFilterVarNames_.size(); ++i_var)
  //    {
  //      const String & var_name = inBenchFilterVarNames_[i_var];
  //      Size var_len = modelNodeIdMap_[var_name].size();
  //      inputValues(benchFilterValuesMap_[var_name], var_len, dimArrayMap_[var_name], is);
  //    }
  //    for (Size i_var=0; i_var<inBenchSmoothVarNames_.size(); ++i_var)
  //    {
  //      const String & var_name = inBenchSmoothVarNames_[i_var];
  //      Size var_len = modelNodeIdMap_[var_name].size();
  //      inputValues(benchSmoothValuesMap_[var_name], var_len, dimArrayMap_[var_name], is);
  //    }
  //
  //    if (verbose_)
  //    {
  //      if (! inBenchFilterVarNames_.empty() )
  //        os_ << "Input Filtering benchmark values: " << std::endl;
  //
  //      for (Size i_var=0; i_var<inBenchFilterVarNames_.size(); ++i_var)
  //      {
  //        const String & var_name = inBenchFilterVarNames_[i_var];
  //        printValues(os_, var_name, benchFilterValuesMap_[var_name], dimArrayMap_[var_name]->Length());
  //      }
  //
  //      if (! inBenchSmoothVarNames_.empty() )
  //        os_ << "Input Smoothing benchmark values: " << std::endl;
  //
  //      for (Size i_var=0; i_var<inBenchSmoothVarNames_.size(); ++i_var)
  //      {
  //        const String & var_name = inBenchSmoothVarNames_[i_var];
  //        printValues(os_, var_name, benchSmoothValuesMap_[var_name], dimArrayMap_[var_name]->Length());
  //      }
  //    }
  //  }


  void ModelTest::ClearSMC()
  {
    smcFilterValuesMap_.clear();
    smcSmoothValuesMap_.clear();
  }

  void ModelTest::BuildSMC(Bool prior)
  {
    // sampler
    //--------
    pSampler_ = ForwardSampler::Ptr(new ForwardSampler(*pModelGraph_));

    std::list<std::pair<NodeSamplerFactory::Ptr, Bool> >::iterator
        it_sampler_fact = ForwardSampler::NodeSamplerFactories().begin();
    for (; it_sampler_fact != ForwardSampler::NodeSamplerFactories().end(); ++it_sampler_fact)
    {
      if (prior)
      {
        it_sampler_fact->second = false;
        continue;
      }
      Bool active = std::find(nodeSamplerFactoryInvOrder_.begin(),
                              nodeSamplerFactoryInvOrder_.end(),
                              it_sampler_fact->first)
          != nodeSamplerFactoryInvOrder_.end();
      it_sampler_fact->second = active;
    }

    pSampler_->Build();
  }

  void ModelTest::RunSMC(Size nParticles,
                         Size rngSeed,
                         const String & rsType,
                         Scalar essThreshold,
                         Bool showProgress,
                         Size numBins)
  {
    boost::scoped_ptr<Rng> p_rng(new Rng(rngSeed));

    // TODO
//    if (verbose_ >= 2)
//    {
//      os_ << "Node sampler's sequence: " << std::endl;
//      Types<std::pair<NodeId, String> >::Array samplers_seq =
//          pSampler_->GetSamplersSequence();
//      for (Size i = 0; i < samplers_seq.size(); ++i)
//      {
//        NodeId node_id = samplers_seq[i].first;
//        const String & name = samplers_seq[i].second;
//        os_ << i << ": node " << node_id << ", " << name << std::endl;
//      }
//      os_ << std::endl;
//    }

    Types<boost::progress_display>::Ptr p_show_progress;
    if (verbose_ == 1 && showProgress)
      p_show_progress
          = Types<boost::progress_display>::Ptr(new boost::progress_display(pSampler_->NIterations(),
                                                                            os_,
                                                                            ""));
    else if (verbose_ >= 2)
      os_ << "SMC sampler's progress: " << std::endl;

    // filtering
    pSampler_->Initialize(nParticles, p_rng.get(), rsType, essThreshold);

    initFilterAccumulators(nParticles, numBins);
    filterAccumulate(0);

    if (verbose_ >= 1 && showProgress)
      ++(*p_show_progress);
    // TODO
//    else if (verbose_ >= 2)
//      printSamplerState(*pSampler_, os_);

    Size t_max = pSampler_->NIterations();

    for (Size t = 1; t < t_max; ++t)
    {
      pSampler_->Iterate();
      if (verbose_ >= 1 && showProgress)
        ++(*p_show_progress);
      // TODO
//      else if (verbose_ >= 2)
//        printSamplerState(*pSampler_, os_);

      filterAccumulate(t);
    }

    if (verbose_ >= 2)
    {
      os_ << std::endl;
      os_ << "Particle Filter estimates: " << std::endl;
      for (Size i_var = 0; i_var < printResultsVarNames_.size(); ++i_var)
      {
        const String & var_name = printResultsVarNames_[i_var];
        printValues(os_,
                    var_name,
                    smcFilterValuesMap_[var_name],
                    dimArrayMap_[var_name]->Length());
      }
      os_ << std::endl;
    }

    initSmoothAccumulators(nParticles, numBins);

    // smoothing
    for (Size t = 0; t < t_max; ++t)
    {
      smoothAccumulate(t);
    }

    if (verbose_ >= 2)
    {
      os_ << "Particle Smoother estimates: " << std::endl;
      for (Size i_var = 0; i_var < printResultsVarNames_.size(); ++i_var)
      {
        const String & var_name = printResultsVarNames_[i_var];
        printValues(os_,
                    var_name,
                    smcSmoothValuesMap_[var_name],
                    dimArrayMap_[var_name]->Length());
      }
      os_ << std::endl;
    }

  }

  Bool ModelTest::error(Scalar & error,
                        const Types<String>::Array & varNames,
                        const std::map<String, MultiArray::Array> & smcValuesMap,
                        const std::map<String, MultiArray::Array> & benchValuesMap) const
  {
    Bool valid = false;
    error = 0.0;

    for (Size i_var = 0; i_var < varNames.size(); ++i_var)
    {
      const String & var_name = varNames[i_var];

      if (smcValuesMap.count(var_name) && benchValuesMap.count(var_name)
          && benchValuesMap.count("var." + var_name))
      {
        const MultiArray::Array & smc_values =
            smcValuesMap.find(var_name)->second;
        const MultiArray::Array & bench_values =
            benchValuesMap.find(var_name)->second;
        const MultiArray::Array & bench_var_values = benchValuesMap.find("var."
            + var_name)->second;

        for (Size k = 0; k < smc_values.size(); ++k)
        {
          Size dim = smc_values[k].Length(); // TODO check dim
          Vector diff_vec(dim, smc_values[k].Values()
              - bench_values[k].Values());
          Matrix var_chol(dim, dim, bench_var_values[k].Values());
          if (!ublas::cholesky_factorize(var_chol))
            throw RuntimeError("ModelTest::error: matrix is not positive-semidefinite.");
          ublas::inplace_solve(var_chol, diff_vec, ublas::lower_tag());
          error += ublas::inner_prod(diff_vec, diff_vec);
        }
        valid = true;
      }
      else
      {
        valid = false;
        break;
      }
    }

    return valid;
  }

}
