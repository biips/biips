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

/*! \file BUGSModel.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "model/BUGSModel.hpp"
#include "common/IndexRangeIterator.hpp"
#include "iostream/outStream.hpp"
#include <boost/random/discrete_distribution.hpp>

namespace Biips
{

  std::map<String, MultiArray> BUGSModel::Sample(Rng * pRng) const
  {
    // Sample values
    NodeValues sampled_values = pGraph_->SampleValues(pRng);

    // create an empty data table
    std::map<String, MultiArray> data_table;

    // Put sampled values into a data map
    for (NodeId node_id = 0; node_id < sampled_values.size(); ++node_id)
    {
      // check if the node is named, i.e. is in the symbol table
      if (!symbolTable_.Contains(node_id))
        continue;

      // check if the node is not constant
      if (pGraph_->GetNode(node_id).GetType() == CONSTANT)
        continue;

      // search the name of the node corresponding to the id
      // in the symbol table
      const String & var_name = symbolTable_.GetVariableName(node_id);

      // if the var_name key does not exist
      // create a MultiArray of the corresponding size
      // initialized with BIIPS_REALNA
      const NodeArray & node_array = symbolTable_.GetNodeArray(var_name);
      if (!data_table.count(var_name))
      {
        DimArray::Ptr p_dim = node_array.Range().DimPtr();
        ValArray::Ptr p_val(new ValArray(p_dim->Length(), BIIPS_REALNA));
        data_table[var_name] = MultiArray(p_dim, p_val);
      }

      // get the subrange in the NodeArray
      IndexRange range = node_array.GetRange(node_id);

      if (range.Length() != sampled_values[node_id]->size())
        throw RuntimeError(String("Dimension of sampled values for Node ")
            + print(node_id) + " mismatch.");

      // then copy values into the corresponding range of the MultiArray
      Size k = 0;
      for (IndexRangeIterator it_range(range); !it_range.AtEnd(); it_range.Next(), ++k)
      {
        // compute the offset
        Size offset = node_array.Range().GetOffset(it_range);
        // copy the value
        data_table.at(var_name).Values()[offset]
            = (*sampled_values[node_id])[k];
      }
    }
    return data_table;
  }

  Bool BUGSModel::SetFilterMonitor(const String & name,
                                   const IndexRange & range)
  {
    // TODO use Monitor Factory

    if (!symbolTable_.Contains(name))
      return false;

    IndexRange range_valid;
    if (range.IsNull())
      range_valid = symbolTable_.GetNodeArray(name).Range();
    else
    {
      if (!symbolTable_.GetNodeArray(name).Range().Contains(range))
        return false;
      range_valid = range;
    }

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it =
        node_id_range_bimap.begin(); it != node_id_range_bimap.end(); ++it)
    {
      if (range_valid.Overlaps(it->right))
        BaseType::SetFilterMonitor(it->left);
    }

    filterMonitorsNames_.push_back(name);
    filterMonitorsRanges_.push_back(range);

    return true;
  }

  Bool BUGSModel::SetSmoothTreeMonitor(const String & name,
                                       const IndexRange & range)
  {
    // TODO use Monitor Factory

    if (!symbolTable_.Contains(name))
      return false;

    IndexRange range_valid;
    if (range.IsNull())
      range_valid = symbolTable_.GetNodeArray(name).Range();
    else
    {
      if (!symbolTable_.GetNodeArray(name).Range().Contains(range))
        return false;
      range_valid = range;
    }

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it =
        node_id_range_bimap.begin(); it != node_id_range_bimap.end(); ++it)
    {
      if (range_valid.Overlaps(it->right))
        BaseType::SetSmoothTreeMonitor(it->left);
    }

    smoothTreeMonitorsNames_.push_back(name);
    smoothTreeMonitorsRanges_.push_back(range);

    return true;
  }

  Bool BUGSModel::SetSmoothMonitor(const String & name,
                                   const IndexRange & range)
  {
    // TODO use Monitor Factory

    if (!symbolTable_.Contains(name))
      return false;

    IndexRange range_valid;
    if (range.IsNull())
      range_valid = symbolTable_.GetNodeArray(name).Range();
    else
    {
      if (!symbolTable_.GetNodeArray(name).Range().Contains(range))
        return false;
      range_valid = range;
    }

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it =
        node_id_range_bimap.begin(); it != node_id_range_bimap.end(); ++it)
    {
      if (range_valid.Overlaps(it->right))
        BaseType::SetSmoothMonitor(it->left);
    }

    smoothMonitorsNames_.push_back(name);
    smoothMonitorsRanges_.push_back(range);

    return true;
  }

  Bool BUGSModel::IsFilterMonitored(const String & name, IndexRange range) const
  {
    if (!symbolTable_.Contains(name))
      return false;

    if (range.IsNull())
      range = symbolTable_.GetNodeArray(name).Range();
    else if (!symbolTable_.GetNodeArray(name).Range().Contains(range))
      throw LogicError(String("SetSmoothMonitor: range ") + print(range)
          + " is not contained in variable " + name);

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it =
        node_id_range_bimap.begin(); it != node_id_range_bimap.end(); ++it)
    {
      if (!range.Overlaps(it->right))
        continue;
      if (!filterMonitorsMap_.count(it->left))
        return false;
      Size iter = pSampler_->GetNodeSamplingIteration(it->left);
      if (filterMonitors_.size() < iter + 1 && !filterMonitors_[iter]
          && !filterMonitors_[iter]->Contains(it->left))
        return false;
    }

    return true;
  }

  Bool BUGSModel::IsSmoothTreeMonitored(const String & name, IndexRange range) const
  {
    if (!symbolTable_.Contains(name))
      return false;

    if (range.IsNull())
      range = symbolTable_.GetNodeArray(name).Range();
    else if (!symbolTable_.GetNodeArray(name).Range().Contains(range))
      throw LogicError(String("SetSmoothMonitor: range ") + print(range)
          + " is not contained in variable " + name);

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it =
        node_id_range_bimap.begin(); it != node_id_range_bimap.end(); ++it)
    {
      if (!range.Overlaps(it->right))
        continue;
      if (!smoothTreeMonitoredNodeIds_.count(it->left))
        return false;
      if (!pSmoothTreeMonitor_ && pSmoothTreeMonitor_->Contains(it->left))
        return false;
    }

    return true;
  }

  Bool BUGSModel::IsSmoothMonitored(const String & name, IndexRange range) const
  {
    if (!symbolTable_.Contains(name))
      return false;

    if (range.IsNull())
      range = symbolTable_.GetNodeArray(name).Range();
    else if (!symbolTable_.GetNodeArray(name).Range().Contains(range))
      throw LogicError(String("SetSmoothMonitor: range ") + print(range)
          + " is not contained in variable " + name);

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it =
        node_id_range_bimap.begin(); it != node_id_range_bimap.end(); ++it)
    {
      if (!range.Overlaps(it->right))
        continue;
      if (!smoothMonitorsMap_.count(it->left))
        return false;
      Size iter = pSampler_->NIterations() - 1
          - pSampler_->GetNodeSamplingIteration(it->left);
      if (smoothMonitors_.size() < iter + 1 && !smoothMonitors_[iter]
          && !smoothMonitors_[iter]->Contains(it->left))
        return false;
    }

    return true;
  }

  Bool BUGSModel::ExtractFilterStat(String name, StatTag statFeature, std::map<
      IndexRange, MultiArray> & statMap) const
  {
    if (!statMap.empty())
      throw LogicError("Can not extract filter statistic: statistics map is not empty.");

    if (!IsFilterMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin(); it
        != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      MultiArray stat_marray(BaseType::ExtractFilterStat(node_id, statFeature));
      statMap.insert(std::make_pair(index_range, stat_marray));
    }

    return true;
  }

  Bool BUGSModel::ExtractSmoothTreeStat(String name,
                                        StatTag statFeature,
                                        std::map<IndexRange, MultiArray> & statMap) const
  {
    if (!statMap.empty())
      throw LogicError("Can not extract smooth tree statistic: statistics map is not empty.");

    if (!IsSmoothTreeMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin(); it
        != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      MultiArray stat_marray(BaseType::ExtractSmoothTreeStat(node_id,
                                                             statFeature));
      statMap.insert(std::make_pair(index_range, stat_marray));
    }

    return true;
  }

  Bool BUGSModel::ExtractSmoothStat(String name, StatTag statFeature, std::map<
      IndexRange, MultiArray> & statMap) const
  {
    if (!statMap.empty())
      throw LogicError("Can not extract filter statistic: statistics map is not empty.");

    if (!IsSmoothMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin(); it
        != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      MultiArray stat_marray(BaseType::ExtractSmoothStat(node_id, statFeature));
      statMap.insert(std::make_pair(index_range, stat_marray));
    }

    return true;
  }

  Bool BUGSModel::ExtractFilterPdf(String name,
                                   std::map<IndexRange, Histogram> & pdfMap,
                                   Size numBins,
                                   Scalar cacheFraction) const
  {
    if (!pdfMap.empty())
      throw LogicError("Can not extract filter pdf: pdf map is not empty.");

    if (!IsFilterMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    // check that all the nodes are scalar

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin(); it
        != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      if (index_range.Length() != 1)
        return false;
    }

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin(); it
        != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      Histogram pdf_hist = BaseType::ExtractFilterPdf(node_id,
                                                      numBins,
                                                      cacheFraction);
      pdfMap.insert(std::make_pair(index_range, pdf_hist));
    }

    return true;
  }

  Bool BUGSModel::ExtractSmoothTreePdf(String name, std::map<IndexRange,
      Histogram> & pdfMap, Size numBins, Scalar cacheFraction) const
  {
    if (!pdfMap.empty())
      throw LogicError("Can not extract smooth tree pdf: pdf map is not empty.");

    if (!IsSmoothTreeMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    // check that all the nodes are scalar

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin(); it
        != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      if (index_range.Length() != 1)
        return false;
    }

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin(); it
        != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      Histogram pdf_hist = BaseType::ExtractSmoothTreePdf(node_id,
                                                          numBins,
                                                          cacheFraction);
      pdfMap.insert(std::make_pair(index_range, pdf_hist));
    }

    return true;
  }

  Bool BUGSModel::ExtractSmoothPdf(String name,
                                   std::map<IndexRange, Histogram> & pdfMap,
                                   Size numBins,
                                   Scalar cacheFraction) const
  {
    if (!pdfMap.empty())
      throw LogicError("Can not extract filter pdf: pdf map is not empty.");

    if (!IsSmoothMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    // check that all the nodes are scalar

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin(); it
        != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      if (index_range.Length() != 1)
        return false;
    }

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin(); it
        != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      Histogram pdf_hist = BaseType::ExtractSmoothPdf(node_id,
                                                      numBins,
                                                      cacheFraction);
      pdfMap.insert(std::make_pair(index_range, pdf_hist));
    }

    return true;
  }

  Bool BUGSModel::ChangeData(const std::map<String, MultiArray> & dataMap,
                             Bool & rebuildSampler)
  {
    rebuildSampler = symbolTable_.ChangeData(dataMap);

    return true;
  }

  Bool BUGSModel::SampleData(const Types<String>::Array & variableNames,
                             Rng * pRng,
                             std::map<String, MultiArray> & dataMap,
                             Bool & rebuildSampler)
  {
    rebuildSampler = symbolTable_.SampleData(variableNames, pRng, dataMap);

    return true;
  }

  Bool BUGSModel::DumpData(std::map<String, MultiArray> & dataMap) const
  {
    symbolTable_.ReadData(dataMap);

    return true;
  }

  Bool BUGSModel::DumpFilterMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;

    for (Size i = 0; i < filterMonitorsNames_.size(); ++i)
    {
      const String & var_name = filterMonitorsNames_[i];
      if (!symbolTable_.Contains(var_name))
        throw LogicError(String("Monitored array ") + var_name
            + " does not exist in the symbol table.");

      String name = var_name;
      IndexRange range = filterMonitorsRanges_[i];
      if (range.IsNull())
        range = symbolTable_.GetNodeArray(name).Range();
      else
        name.append(print(range));

      monitorsMap.insert(std::make_pair(name,
                                        NodeArrayMonitor(symbolTable_.GetNodeArray(var_name),
                                                         range,
                                                         filterMonitorsMap_,
                                                         pSampler_->NParticles(),
                                                         *pGraph_)));
    }

    return true;
  }

  Bool BUGSModel::DumpSmoothTreeMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;

    for (Size i = 0; i < smoothTreeMonitorsNames_.size(); ++i)
    {
      const String & var_name = smoothTreeMonitorsNames_[i];
      if (!symbolTable_.Contains(var_name))
        throw LogicError(String("Monitored array ") + var_name
            + " does not exist in the symbol table.");

      String name = var_name;
      IndexRange range = smoothTreeMonitorsRanges_[i];
      if (range.IsNull())
        range = symbolTable_.GetNodeArray(name).Range();
      else
        name.append(print(range));

      monitorsMap.insert(std::make_pair(name,
                                        NodeArrayMonitor(symbolTable_.GetNodeArray(var_name),
                                                         range,
                                                         pSmoothTreeMonitor_,
                                                         pSampler_->NParticles(),
                                                         *pGraph_)));
    }

    return true;
  }

  Bool BUGSModel::SampleSmoothTreeParticle(const Rng::Ptr & pRng, std::map<
      String, MultiArray> & sampledValues) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;

    // sample one particle according to the weights
    typedef boost::random::discrete_distribution<Int, Scalar> CategoricalDist;
    CategoricalDist dist(pSmoothTreeMonitor_->GetUnnormWeights().begin(),
                         pSmoothTreeMonitor_->GetUnnormWeights().end());
    typedef boost::variate_generator<Rng::GenType&, CategoricalDist>
        CategoricalGen;
    CategoricalGen gen(pRng->GetGen(), dist);
    Size chosen_particle = gen();

    for (Size i = 0; i < smoothTreeMonitorsNames_.size(); ++i)
    {
      const String & var_name = smoothTreeMonitorsNames_[i];
      if (!symbolTable_.Contains(var_name))
        throw LogicError(String("Monitored array ") + var_name
            + " does not exist in the symbol table.");

      String name = var_name;
      IndexRange range = smoothTreeMonitorsRanges_[i];
      if (range.IsNull())
        range = symbolTable_.GetNodeArray(name).Range();
      else
        name.append(print(range));

      sampledValues[name] = NodeArrayValue(symbolTable_.GetNodeArray(var_name),
                                           range,
                                           pSmoothTreeMonitor_,
                                           chosen_particle,
                                           *pGraph_).GetValue();
    }

    return true;
  }

  Bool BUGSModel::DumpSmoothMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;

    for (Size i = 0; i < smoothMonitorsNames_.size(); ++i)
    {
      const String & var_name = smoothMonitorsNames_[i];
      if (!symbolTable_.Contains(var_name))
        throw LogicError(String("Monitored array ") + var_name
            + " does not exist in the symbol table.");

      String name = var_name;
      IndexRange range = smoothMonitorsRanges_[i];
      if (range.IsNull())
        range = symbolTable_.GetNodeArray(name).Range();
      else
        name.append(print(range));

      monitorsMap.insert(std::make_pair(name,
                                        NodeArrayMonitor(symbolTable_.GetNodeArray(var_name),
                                                         range,
                                                         smoothMonitorsMap_,
                                                         pSampler_->NParticles(),
                                                         *pGraph_)));
    }

    return true;
  }

  void BUGSModel::PrintSamplersSequence(std::ostream & out) const
  {
    Types<std::pair<NodeId, String> >::Array node_id_samplers_seq =
        pSampler_->GetSamplersSequence();
    for (Size i = 0; i < node_id_samplers_seq.size(); ++i)
    {
      NodeId node_id = node_id_samplers_seq[i].first;
      const String & name = node_id_samplers_seq[i].second;
      out << i + 1 << ": " << symbolTable_.GetName(node_id) << " (id="
          << node_id << "), " << name << std::endl;
    }
  }

  class VarNamePropertyWriter: public VertexPropertyWriter
  {
  public:
    typedef VertexPropertyWriter BaseType;
    typedef VarNamePropertyWriter SelfType;

    VarNamePropertyWriter(const Graph & graph, const SymbolTable & symTab) :
      BaseType(graph), symTab_(symTab)
    {
    }

  protected:
    const SymbolTable & symTab_;

    virtual String label(NodeId id) const
    {
      String ans;
      if (symTab_.Contains(id))
      {
        ans += symTab_.GetName(id);
        ans += "\\n";
        ans += BaseType::label(id);
      }
      else
      {
        ans += symTab_.GetName(id);
        ans += "\\n";
        ans += print(id);
      }

      return ans;
    }
  };

  void BUGSModel::PrintGraphviz(std::ostream & out) const
  {
    VarNamePropertyWriter vnpw(*pGraph_, symbolTable_);
    pGraph_->PrintGraphviz(out, vnpw);
  }

  Bool BUGSModel::GetLogPriorDensity(Scalar & prior,
                                     const String & variable,
                                     IndexRange range) const
  {
    if (!symbolTable_.Contains(variable))
      throw RuntimeError(String("Can not get prior density: variable ")
          + variable + " not found.");

    const NodeArray & array = symbolTable_.GetNodeArray(variable);
    if (range.IsNull())
      range = array.Range();
    NodeId node_id = array.GetNode(range);
    if (node_id == NULL_NODEID)
      throw RuntimeError(String("Can not get prior density: invalid range: ")
          + variable + print(range));

    prior = BaseType::GetLogPriorDensity(node_id);

    return true;
  }

  void BUGSModel::ClearFilterMonitors()
  {
    filterMonitorsNames_.clear();
    filterMonitorsRanges_.clear();
    BaseType::ClearFilterMonitors();
  }

  void BUGSModel::ClearSmoothTreeMonitors()
  {
    smoothTreeMonitorsNames_.clear();
    smoothTreeMonitorsRanges_.clear();
    BaseType::ClearSmoothTreeMonitors();
  }

  void BUGSModel::ClearSmoothMonitors()
  {
    smoothMonitorsNames_.clear();
    smoothMonitorsRanges_.clear();
    BaseType::ClearSmoothMonitors();
  }
}