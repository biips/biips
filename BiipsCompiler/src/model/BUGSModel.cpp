//                                               -*- C++ -*-
/*! \file BUGSModel.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "model/BUGSModel.hpp"
#include "common/IndexRangeIterator.hpp"
#include "iostream/outStream.hpp"

namespace Biips
{

  std::map<String, MultiArray> BUGSModel::Sample(const Rng::Ptr & pRng) const
  {
    // Sample values
    NodeValues sampled_values = pGraph_->SampleValues(pRng);

    // create an empty data table
    std::map<String, MultiArray> data_table;

    // Put sampled values into a data map
    for (NodeId node_id=0; node_id <sampled_values.size(); ++node_id)
    {
      // check if the node is named, i.e. is in the symbol table
      if(!symbolTable_.Contains(node_id))
        continue;

      // check if the node is not constant
      if(pGraph_->GetNode(node_id).GetType() == CONSTANT)
        continue;

      // search the name of the node corresponding to the id
      // in the symbol table
      const String & var_name = symbolTable_.GetVariableName(node_id);

      // if the var_name key does not exist
      // create a MultiArray of the corresponding size
      // initialized with BIIPS_REALNA
      const NodeArray & node_array = symbolTable_.GetNodeArray(var_name);
      if (!data_table.count(var_name))
        data_table[var_name] = MultiArray(node_array.Range().Dim(), BIIPS_REALNA);

      // get the subrange in the NodeArray
      IndexRange range = node_array.GetRange(node_id);

      if (range.Length() != sampled_values[node_id]->size())
        throw RuntimeError(String("Dimension of sampled values for Node ") + print(node_id) + " mismatch.");

      // then copy values into the corresponding range of the MultiArray
      Size k = 0;
      for (IndexRangeIterator it_range(range); !it_range.AtEnd(); it_range.Next(), ++k)
      {
        // compute the offset
        Size offset = node_array.Range().GetOffset(it_range);
        // copy the value
        data_table.at(var_name).Values()[offset] = (*sampled_values[node_id])[k];
      }
    }

    return data_table;
  }


  Bool BUGSModel::SetFilterMonitor(const String & name)
  {
    // TODO use Monitor Factory

    if (!symbolTable_.Contains(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it = node_id_range_bimap.begin();
        it != node_id_range_bimap.end(); ++it)
    {
      BaseType::SetFilterMonitor(it->left);
    }

    filterMonitorsNames_.push_back(name);

    return true;
  }


  Bool BUGSModel::SetSmoothTreeMonitor(const String & name)
  {
    // TODO use Monitor Factory

    if (!symbolTable_.Contains(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it = node_id_range_bimap.begin();
        it != node_id_range_bimap.end(); ++it)
    {
      BaseType::SetSmoothTreeMonitor(it->left);
    }

    smoothTreeMonitorsNames_.push_back(name);

    return true;
  }


  Bool BUGSModel::SetSmoothMonitor(const String & name)
  {
    // TODO use Monitor Factory

    if (!symbolTable_.Contains(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it = node_id_range_bimap.begin();
        it != node_id_range_bimap.end(); ++it)
    {
      BaseType::SetSmoothMonitor(it->left);
    }

    smoothMonitorsNames_.push_back(name);

    return true;
  }


  Bool BUGSModel::IsFilterMonitored(const String & name) const
  {
    if (!symbolTable_.Contains(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for(boost::bimap<NodeId, IndexRange>::const_iterator it = node_id_range_bimap.begin();
        it != node_id_range_bimap.end(); ++it)
    {
      if (!filterMonitorsMap_.count(it->left))
        return false;
      Size iter = pSampler_->GetNodeSamplingIteration(it->left);
      if (filterMonitors_.size()<iter+1)
        return false;
      if (!filterMonitors_[iter])
        return false;
      if (!filterMonitors_[iter]->Contains(it->left))
        return false;
    }

    return true;
  }


  Bool BUGSModel::IsSmoothTreeMonitored(const String & name) const
  {
    if (!symbolTable_.Contains(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for(boost::bimap<NodeId, IndexRange>::const_iterator it = node_id_range_bimap.begin();
        it != node_id_range_bimap.end(); ++it)
    {
      if (!smoothTreeMonitoredNodeIds_.count(it->left))
        return false;
      Size iter = pSampler_->NIterations();
      if (filterMonitors_.size()<iter+1)
        return false;
      if (!filterMonitors_[iter])
        return false;
      if (!filterMonitors_[iter]->Contains(it->left))
        return false;
    }

    return true;
  }


  Bool BUGSModel::IsSmoothMonitored(const String & name) const
  {
    if (!symbolTable_.Contains(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for(boost::bimap<NodeId, IndexRange>::const_iterator it = node_id_range_bimap.begin();
        it != node_id_range_bimap.end(); ++it)
    {
      if (!smoothMonitorsMap_.count(it->left))
        return false;
      Size iter = pSampler_->NIterations() - 1 - pSampler_->GetNodeSamplingIteration(it->left);
      if (smoothMonitors_.size()<iter+1)
        return false;
      if (!smoothMonitors_[iter])
        return false;
      if (!smoothMonitors_[iter]->Contains(it->left))
        return false;
    }

    return true;
  }


  Bool BUGSModel::ExtractFilterStat(String name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap) const
  {
    if (!statMap.empty())
      throw LogicError("Can not extract filter statistic: statistics map is not empty.");

    if (!IsFilterMonitored(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for(boost::bimap<NodeId, IndexRange>::right_const_iterator it = node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      MultiArray stat_marray(BaseType::ExtractFilterStat(node_id, statFeature));
      statMap.insert(std::make_pair(index_range, stat_marray));
    }

    return true;
  }


  Bool BUGSModel::ExtractSmoothTreeStat(String name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap) const
  {
    if (!statMap.empty())
      throw LogicError("Can not extract smooth tree statistic: statistics map is not empty.");

    if (!IsSmoothTreeMonitored(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for(boost::bimap<NodeId, IndexRange>::right_const_iterator it = node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      MultiArray stat_marray(BaseType::ExtractSmoothTreeStat(node_id, statFeature));
      statMap.insert(std::make_pair(index_range, stat_marray));
    }

    return true;
  }


  Bool BUGSModel::ExtractSmoothStat(String name, StatsTag statFeature, std::map<IndexRange, MultiArray> & statMap) const
  {
    if (!statMap.empty())
      throw LogicError("Can not extract filter statistic: statistics map is not empty.");

    if (!IsSmoothMonitored(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for(boost::bimap<NodeId, IndexRange>::right_const_iterator it = node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      MultiArray stat_marray(BaseType::ExtractSmoothStat(node_id, statFeature));
      statMap.insert(std::make_pair(index_range, stat_marray));
    }

    return true;
  }


  Bool BUGSModel::ExtractFilterPdf(String name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins, Scalar cacheFraction) const
  {
    if (!pdfMap.empty())
      throw LogicError("Can not extract filter pdf: pdf map is not empty.");

    if (!IsFilterMonitored(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    // check that all the nodes are scalar

    for(boost::bimap<NodeId, IndexRange>::right_const_iterator it = node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      if (index_range.Length() != 1)
        return false;
    }

    for(boost::bimap<NodeId, IndexRange>::right_const_iterator it = node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      ScalarHistogram pdf_hist = BaseType::ExtractFilterPdf(node_id, numBins, cacheFraction);
      pdfMap.insert(std::make_pair(index_range, pdf_hist));
    }

    return true;
  }


  Bool BUGSModel::ExtractSmoothTreePdf(String name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins, Scalar cacheFraction) const
  {
    if (!pdfMap.empty())
      throw LogicError("Can not extract smooth tree pdf: pdf map is not empty.");

    if (!IsSmoothTreeMonitored(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    // check that all the nodes are scalar

    for(boost::bimap<NodeId, IndexRange>::right_const_iterator it = node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      if (index_range.Length() != 1)
        return false;
    }

    for(boost::bimap<NodeId, IndexRange>::right_const_iterator it = node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      ScalarHistogram pdf_hist = BaseType::ExtractSmoothTreePdf(node_id, numBins, cacheFraction);
      pdfMap.insert(std::make_pair(index_range, pdf_hist));
    }

    return true;
  }


  Bool BUGSModel::ExtractSmoothPdf(String name, std::map<IndexRange, ScalarHistogram> & pdfMap, Size numBins, Scalar cacheFraction) const
  {
    if (!pdfMap.empty())
      throw LogicError("Can not extract filter pdf: pdf map is not empty.");

    if (!IsSmoothMonitored(name))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap
    = symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    // check that all the nodes are scalar

    for(boost::bimap<NodeId, IndexRange>::right_const_iterator it = node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      if (index_range.Length() != 1)
        return false;
    }

    for(boost::bimap<NodeId, IndexRange>::right_const_iterator it = node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      ScalarHistogram pdf_hist = BaseType::ExtractSmoothPdf(node_id, numBins, cacheFraction);
      pdfMap.insert(std::make_pair(index_range, pdf_hist));
    }

    return true;
  }


  Bool BUGSModel::DumpFilterMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;

    for (Size i=0; i<filterMonitorsNames_.size(); ++i)
    {
      const String & name = filterMonitorsNames_[i];
      if (!symbolTable_.Contains(name))
        throw LogicError(String("Monitored array ") + name + " does not exist in the symbol table.");

      monitorsMap.insert(std::make_pair(name,
          NodeArrayMonitor(symbolTable_.GetNodeArray(name), filterMonitorsMap_)
      ));
    }

    return true;
  }


  Bool BUGSModel::DumpSmoothTreeMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;

    for (Size i=0; i<smoothTreeMonitorsNames_.size(); ++i)
    {
      const String & name = smoothTreeMonitorsNames_[i];
      if (!symbolTable_.Contains(name))
        throw LogicError(String("Monitored array ") + name + " does not exist in the symbol table.");

      monitorsMap.insert(std::make_pair(name,
          NodeArrayMonitor(symbolTable_.GetNodeArray(name), pSmoothTreeMonitor_)
      ));
    }

    return true;
  }


  Bool BUGSModel::DumpSmoothMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;

    for (Size i=0; i<smoothMonitorsNames_.size(); ++i)
    {
      const String & name = smoothMonitorsNames_[i];
      if (!symbolTable_.Contains(name))
        throw LogicError(String("Monitored array ") + name + " does not exist in the symbol table.");

      monitorsMap.insert(std::make_pair(name,
          NodeArrayMonitor(symbolTable_.GetNodeArray(name), smoothMonitorsMap_)
      ));
    }

    return true;
  }


  void BUGSModel::PrintSamplersSequence(std::ostream & out) const
  {
    Types<std::pair<NodeId, String> >::Array node_id_samplers_seq = pSampler_->GetSamplersSequence();
    for (Size i=0; i<node_id_samplers_seq.size(); ++i)
    {
      NodeId node_id = node_id_samplers_seq[i].first;
      const String & name = node_id_samplers_seq[i].second;
      out << i+1 << ": " << symbolTable_.GetName(node_id) << " (id=" << node_id << "), " << name << std::endl;
    }
  }



  class VarNamePropertyWriter: public VertexPropertyWriter
  {
  public:
    typedef VertexPropertyWriter BaseType;
    typedef VarNamePropertyWriter SelfType;

    VarNamePropertyWriter(const Graph & graph, const SymbolTable & symTab) : BaseType(graph), symTab_(symTab) {}

  protected:
    const SymbolTable & symTab_;

    virtual String label(NodeId id) const
    {
      String ans;
      if (symTab_.Contains(id))
      {
        ans += symTab_.GetName(id);
        ans += "\\n";
      }

      ans += BaseType::label(id);

      return ans;
    }
  };


  void BUGSModel::PrintGraphviz(std::ostream & out) const
  {
    VarNamePropertyWriter vnpw(*pGraph_, symbolTable_);
    pGraph_->PrintGraphviz(out, vnpw);
  }

}
