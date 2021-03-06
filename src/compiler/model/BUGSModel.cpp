#include "model/BUGSModel.hpp"
#include "common/IndexRangeIterator.hpp"
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/variate_generator.hpp>

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
      for (IndexRangeIterator it_range(range); !it_range.AtEnd();
          it_range.Next(), ++k)
      {
        // compute the offset
        Size offset = node_array.Range().GetOffset(it_range);
        // copy the value
        data_table.at(var_name).Values()[offset] =
            (*sampled_values[node_id])[k];
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
      if (!range_valid.Overlaps(it->right))
        continue;

      BaseType::SetFilterMonitor(it->left);
    }


    filterMonitorsNames_.push_back(name);
    filterMonitorsRanges_.push_back(range);

    return true;
  }

  Bool BUGSModel::SetGenTreeSmoothMonitor(const String & name,
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
      if (!range_valid.Overlaps(it->right))
        continue;

      BaseType::SetGenTreeSmoothMonitor(it->left);
    }

    genTreeSmoothMonitorsNames_.push_back(name);
    genTreeSmoothMonitorsRanges_.push_back(range);

    return true;
  }

  Bool BUGSModel::SetBackwardSmoothMonitor(const String & name,
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
      if (!range_valid.Overlaps(it->right))
        continue;

      BaseType::SetBackwardSmoothMonitor(it->left);
    }

    backwardSmoothMonitorsNames_.push_back(name);
    backwardSmoothMonitorsRanges_.push_back(range);

    return true;
  }

  Bool BUGSModel::IsFilterMonitored(const String & name, IndexRange range, Bool check_released) const
  {
    if (!symbolTable_.Contains(name))
      return false;

    if (range.IsNull())
      range = symbolTable_.GetNodeArray(name).Range();
    else if (!symbolTable_.GetNodeArray(name).Range().Contains(range))
      throw LogicError(String("IsFilterMonitored: range ") + print(range)
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

      if (!check_released)
        continue;

      Size iter = pSampler_->GetNodeSamplingIteration(it->left);
      if (filterMonitors_.size() < iter + 1 || !filterMonitors_[iter]
                                                                || !filterMonitors_[iter]->Contains(it->left))
        return false;
    }

    return true;
  }

  Bool BUGSModel::IsGenTreeSmoothMonitored(const String & name,
                                           IndexRange range, Bool check_released) const
  {
    if (!symbolTable_.Contains(name))
      return false;

    if (range.IsNull())
      range = symbolTable_.GetNodeArray(name).Range();
    else if (!symbolTable_.GetNodeArray(name).Range().Contains(range))
      throw LogicError(String("IsGenTreeSmoothMonitored: range ") + print(range)
                       + " is not contained in variable " + name);

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it =
        node_id_range_bimap.begin(); it != node_id_range_bimap.end(); ++it)
    {
      if (!range.Overlaps(it->right))
        continue;
      if (!genTreeSmoothMonitoredNodeIds_.count(it->left))
        return false;

      if (!check_released)
        continue;

      if (!pGenTreeSmoothMonitor_ || pGenTreeSmoothMonitor_->Contains(it->left))
        return false;
    }

    return true;
  }

  Bool BUGSModel::IsBackwardSmoothMonitored(const String & name, IndexRange range, Bool check_released) const
  {
    if (!symbolTable_.Contains(name))
      return false;

    if (range.IsNull())
      range = symbolTable_.GetNodeArray(name).Range();
    else if (!symbolTable_.GetNodeArray(name).Range().Contains(range))
      throw LogicError(String("IsBackwardSmoothMonitored: range ") + print(range)
                       + " is not contained in variable " + name);

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::const_iterator it =
        node_id_range_bimap.begin(); it != node_id_range_bimap.end(); ++it)
    {
      if (!range.Overlaps(it->right))
        continue;
      if (!backwardSmoothMonitorsMap_.count(it->left))
        return false;

      if (!check_released)
        continue;

      Size iter = pSampler_->NIterations() - 1
          - pSampler_->GetNodeSamplingIteration(it->left);
      if (backwardSmoothMonitors_.size() < iter + 1 || (!backwardSmoothMonitors_[iter]
                                                                                 && !backwardSmoothMonitors_[iter]->Contains(it->left)))
        return false;
    }

    return true;
  }

  Bool BUGSModel::ExtractFilterStat(String name,
                                    StatTag statFeature,
                                    std::map<IndexRange, MultiArray> & statMap) const
  {
    if (!statMap.empty())
      throw LogicError("Can not extract filter statistic: statistics map is not empty.");

    if (!IsFilterMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      MultiArray stat_marray(BaseType::ExtractFilterStat(node_id, statFeature));
      statMap.insert(std::make_pair(index_range, stat_marray));
    }

    return true;
  }

  Bool BUGSModel::ExtractGenTreeSmoothStat(String name,
                                           StatTag statFeature,
                                           std::map<IndexRange, MultiArray> & statMap) const
  {
    if (!statMap.empty())
      throw LogicError("Can not extract smooth statistic: statistics map is not empty.");

    if (!IsGenTreeSmoothMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      MultiArray stat_marray(BaseType::ExtractGenTreeSmoothStat(node_id,
                                                                statFeature));
      statMap.insert(std::make_pair(index_range, stat_marray));
    }

    return true;
  }

  Bool BUGSModel::ExtractBackwardSmoothStat(String name,
                                            StatTag statFeature,
                                            std::map<IndexRange, MultiArray> & statMap) const
  {
    if (!statMap.empty())
      throw LogicError("Can not extract filter statistic: statistics map is not empty.");

    if (!IsBackwardSmoothMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      MultiArray stat_marray(BaseType::ExtractBackwardSmoothStat(node_id, statFeature));
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
        node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      if (index_range.Length() != 1)
        return false;
    }

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
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

  Bool BUGSModel::ExtractGenTreeSmoothPdf(String name,
                                          std::map<IndexRange, Histogram> & pdfMap,
                                          Size numBins,
                                          Scalar cacheFraction) const
  {
    if (!pdfMap.empty())
      throw LogicError("Can not extract smooth pdf: pdf map is not empty.");

    if (!IsGenTreeSmoothMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    // check that all the nodes are scalar

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      if (index_range.Length() != 1)
        return false;
    }

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      Histogram pdf_hist = BaseType::ExtractGenTreeSmoothPdf(node_id,
                                                             numBins,
                                                             cacheFraction);
      pdfMap.insert(std::make_pair(index_range, pdf_hist));
    }

    return true;
  }

  Bool BUGSModel::ExtractBackwardSmoothPdf(String name,
                                           std::map<IndexRange, Histogram> & pdfMap,
                                           Size numBins,
                                           Scalar cacheFraction) const
  {
    if (!pdfMap.empty())
      throw LogicError("Can not extract backward smooth pdf: pdf map is not empty.");

    if (!IsBackwardSmoothMonitored(name, NULL_RANGE))
      return false;

    const boost::bimap<NodeId, IndexRange> & node_id_range_bimap =
        symbolTable_.GetNodeArray(name).NodeIdRangeBimap();

    // check that all the nodes are scalar

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      if (index_range.Length() != 1)
        return false;
    }

    for (boost::bimap<NodeId, IndexRange>::right_const_iterator it =
        node_id_range_bimap.right.begin();
        it != node_id_range_bimap.right.end(); ++it)
    {
      const IndexRange & index_range = it->first;
      NodeId node_id = it->second;
      Histogram pdf_hist = BaseType::ExtractBackwardSmoothPdf(node_id,
                                                              numBins,
                                                              cacheFraction);
      pdfMap.insert(std::make_pair(index_range, pdf_hist));
    }

    return true;
  }

  Bool BUGSModel::ChangeData(const String & variable,
                             const IndexRange & range,
                             const MultiArray & data,
                             Bool & rebuildSampler,
                             Bool mcmc)
  {
    Bool ok = symbolTable_.ChangeData(variable, range, data, rebuildSampler, mcmc);

    return ok;
  }

  Bool BUGSModel::SampleData(const String & variable,
                             const IndexRange & range,
                             MultiArray & data,
                             Rng * pRng)
  {
    symbolTable_.SampleData(variable, range, data, pRng);

    return true;
  }

  Bool BUGSModel::RemoveData(const String & variable, const IndexRange & range)
  {
    symbolTable_.RemoveData(variable, range);

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
                                                         *pGraph_, symbolTable_)));
    }

    return true;
  }

  Bool BUGSModel::DumpGenTreeSmoothMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;

    for (Size i = 0; i < genTreeSmoothMonitorsNames_.size(); ++i)
    {
      const String & var_name = genTreeSmoothMonitorsNames_[i];
      if (!symbolTable_.Contains(var_name))
        throw LogicError(String("Monitored array ") + var_name
                         + " does not exist in the symbol table.");

      String name = var_name;
      IndexRange range = genTreeSmoothMonitorsRanges_[i];
      if (range.IsNull())
        range = symbolTable_.GetNodeArray(name).Range();
      else
        name.append(print(range));

      monitorsMap.insert(std::make_pair(name,
                                        NodeArrayMonitor(symbolTable_.GetNodeArray(var_name),
                                                         range,
                                                         pGenTreeSmoothMonitor_.get(),
                                                         pSampler_->NParticles(),
                                                         *pGraph_,
                                                         symbolTable_)));
    }

    return true;
  }

  Bool BUGSModel::SampleGenTreeSmoothParticle(Rng * pRng,
                                              std::map<String, MultiArray> & sampledValues) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;
    if (!pGenTreeSmoothMonitor_)
      return false;

    // sample one particle according to the weights
    typedef boost::random::discrete_distribution<Int, Scalar> CategoricalDist;
    CategoricalDist dist(pGenTreeSmoothMonitor_->GetUnnormWeights().begin(),
                         pGenTreeSmoothMonitor_->GetUnnormWeights().end());
    typedef boost::random::variate_generator<Rng::GenType&, CategoricalDist> CategoricalGen;
    CategoricalGen gen(pRng->GetGen(), dist);
    Size chosen_particle = gen();

    for (Size i = 0; i < genTreeSmoothMonitorsNames_.size(); ++i)
    {
      const String & var_name = genTreeSmoothMonitorsNames_[i];
      if (!symbolTable_.Contains(var_name))
        throw LogicError(String("Monitored array ") + var_name
                         + " does not exist in the symbol table.");

      String name = var_name;
      IndexRange range = genTreeSmoothMonitorsRanges_[i];
      if (range.IsNull())
        range = symbolTable_.GetNodeArray(name).Range();
      else
        name.append(print(range));

      sampledValues[name] = NodeArrayValue(symbolTable_.GetNodeArray(var_name),
                                           range,
                                           pGenTreeSmoothMonitor_.get(),
                                           chosen_particle,
                                           *pGraph_).GetValue();
    }

    return true;
  }

  Bool BUGSModel::DumpBackwardSmoothMonitors(std::map<String, NodeArrayMonitor> & monitorsMap) const
  {
    if (!pSampler_)
      return false;
    if (!pSampler_->AtEnd())
      return false;

    for (Size i = 0; i < backwardSmoothMonitorsNames_.size(); ++i)
    {
      const String & var_name = backwardSmoothMonitorsNames_[i];
      if (!symbolTable_.Contains(var_name))
        throw LogicError(String("Monitored array ") + var_name
                         + " does not exist in the symbol table.");

      String name = var_name;
      IndexRange range = backwardSmoothMonitorsRanges_[i];
      if (range.IsNull())
        range = symbolTable_.GetNodeArray(name).Range();
      else
        name.append(print(range));

      monitorsMap.insert(std::make_pair(name,
                                        NodeArrayMonitor(symbolTable_.GetNodeArray(var_name),
                                                         range,
                                                         backwardSmoothMonitorsMap_,
                                                         pSampler_->NParticles(),
                                                         *pGraph_,
                                                         symbolTable_,
                                                         pSmoother_->ConditionalNodes())));
    }

    return true;
  }

  //  void BUGSModel::PrintSamplersSequence(std::ostream & out) const
  //  {
  //    Types<std::pair<NodeId, String> >::Array node_id_samplers_seq =
  //        pSampler_->GetSamplersSequence();
  //    for (Size i = 0; i < node_id_samplers_seq.size(); ++i)
  //    {
  //      NodeId node_id = node_id_samplers_seq[i].first;
  //      const String & name = node_id_samplers_seq[i].second;
  //      out << i + 1 << ": " << symbolTable_.GetName(node_id) << " (id="
  //          << node_id << "), " << name << std::endl;
  //    }
  //  }

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

  Bool BUGSModel::GetFixedSupport(ValArray & lower, ValArray & upper,
                                  const String & variable,
                                  IndexRange range) const
  {
    if (!symbolTable_.Contains(variable))
      throw RuntimeError(String("Can not get fixed support: variable ")
                         + variable + " not found.");

    const NodeArray & array = symbolTable_.GetNodeArray(variable);
    if (range.IsNull())
      range = array.Range();
    NodeId node_id = array.GetNode(range);
    if (node_id == NULL_NODEID)
      throw RuntimeError(String("Can not get fixed support: invalid range: ")
                         + variable + print(range));

    Types<ValArray>::Pair support = BaseType::GetFixedSupport(node_id);
    lower = support.first;
    upper = support.second;

    return true;
  }

  void BUGSModel::ClearFilterMonitors(Bool release_only)
  {
    BaseType::ClearFilterMonitors(release_only);
    if (release_only)
      return;
    filterMonitorsNames_.clear();
    filterMonitorsRanges_.clear();
  }

  void BUGSModel::ClearGenTreeSmoothMonitors(Bool release_only)
  {
    BaseType::ClearGenTreeSmoothMonitors(release_only);
    if (release_only)
      return;
    genTreeSmoothMonitorsNames_.clear();
    genTreeSmoothMonitorsRanges_.clear();
  }

  void BUGSModel::ClearBackwardSmoothMonitors(Bool release_only)
  {
    BaseType::ClearBackwardSmoothMonitors(release_only);
    if (release_only)
      return;
    backwardSmoothMonitorsNames_.clear();
    backwardSmoothMonitorsRanges_.clear();
  }
}
