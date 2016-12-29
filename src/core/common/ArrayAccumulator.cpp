#include "common/ArrayAccumulator.hpp"

namespace Biips
{

  ArrayAccumulator::ArrayAccumulator()
  {
    featuresMap_[SUM] = false;
    featuresMap_[MEAN] = false;
    featuresMap_[MOMENT2] = false;
    featuresMap_[VARIANCE] = false;
    featuresMap_[MOMENT3] = false;
    featuresMap_[SKEWNESS] = false;
    featuresMap_[MOMENT4] = false;
    featuresMap_[KURTOSIS] = false;
  }

  void ArrayAccumulator::drop(StatTag tag)
  {
    switch (tag)
    {
      case SUM:
        acc_.drop<AccTags::Sum> ();
        break;
      case MEAN:
        acc_.drop<AccTags::Mean> ();
        break;
      case VARIANCE:
        acc_.drop<AccTags::Variance> ();
        break;
      case MOMENT2:
        acc_.drop<AccTags::Moment2> ();
        break;
      case MOMENT3:
        acc_.drop<AccTags::Moment3> ();
        break;
      case MOMENT4:
        acc_.drop<AccTags::Moment4> ();
        break;
      case SKEWNESS:
        acc_.drop<AccTags::Skewness> ();
        break;
      case KURTOSIS:
        acc_.drop<AccTags::Kurtosis> ();
        break;
      default:
        throw LogicError("Invalid StatTag in ArrayAccumulator.");
        break;
    }
  }

  void ArrayAccumulator::ClearFeatures()
  {
    for (std::map<StatTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat
        != featuresMap_.end(); ++it_feat)
      it_feat->second = false;
  }

  void ArrayAccumulator::AddFeature(StatTag feat)
  {
    if (featuresMap_.find(feat) != featuresMap_.end())
      featuresMap_[feat] = true;
  }

  void ArrayAccumulator::RemoveFeature(StatTag feat)
  {
    if (featuresMap_.find(feat) != featuresMap_.end())
      featuresMap_[feat] = false;
  }

  void ArrayAccumulator::Init(const DimArray::Ptr & pDim)
  {
    pDim_ = pDim;
    acc_ = AccType(acc::sample = StorageType(pDim->Length(), 0.));
    for (std::map<StatTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat
        != featuresMap_.end(); ++it_feat)
      if (!it_feat->second)
        drop(it_feat->first);
  }

  void ArrayAccumulator::Push(const StorageType & value, Scalar weight)
  {
    acc_(value, acc::weight = weight);
  }

  Size ArrayAccumulator::Count() const
  {
    return acc::count(acc_);
  }

  Scalar ArrayAccumulator::SumOfWeights() const
  {
    return acc::sum_of_weights(acc_);
  }

  MultiArray ArrayAccumulator::Sum() const
  {
    if (!pDim_)
      throw LogicError("Can not extract statistic: dim pointer is null.");
    if (!featuresMap_.at(SUM))
      throw LogicError("Invalid statistic extraction.");
    ValArray::Ptr p_val(new ValArray(acc::weighted_sum(acc_)));
    return MultiArray(pDim_, p_val);
  }

  MultiArray ArrayAccumulator::Mean() const
  {
    if (!pDim_)
      throw LogicError("Can not extract statistic: dim pointer is null.");
    if (!featuresMap_.at(MEAN))
      throw LogicError("Invalid statistic extraction.");
    ValArray::Ptr p_val(new ValArray(acc::weighted_mean(acc_)));
    return MultiArray(pDim_, p_val);
  }

  MultiArray ArrayAccumulator::Variance() const
  {
    if (!pDim_)
      throw LogicError("Can not extract statistic: dim pointer is null.");
    if (!featuresMap_.at(VARIANCE))
      throw LogicError("Invalid statistic extraction.");
    ValArray::Ptr p_val(new ValArray(acc::weighted_variance(acc_)));
    return MultiArray(pDim_, p_val);
  }

  MultiArray ArrayAccumulator::Skewness() const
  {
    if (!pDim_)
      throw LogicError("Can not extract statistic: dim pointer is null.");
    if (!featuresMap_.at(SKEWNESS))
      throw LogicError("Invalid statistic extraction.");
    ValArray::Ptr p_val(new ValArray(acc::weighted_skewness(acc_)));
    return MultiArray(pDim_, p_val);
  }

  MultiArray ArrayAccumulator::Kurtosis() const
  {
    if (!pDim_)
      throw LogicError("Can not extract statistic: dim pointer is null.");
    if (!featuresMap_.at(KURTOSIS))
      throw LogicError("Invalid statistic extraction.");
    ValArray::Ptr p_val(new ValArray(acc::weighted_kurtosis(acc_)));
    return MultiArray(pDim_, p_val);
  }
}
