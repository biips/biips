#ifndef BIIPS_ARRAYACCUMULATOR_HPP_
#define BIIPS_ARRAYACCUMULATOR_HPP_

#include "common/MultiArray.hpp"
#include "common/Accumulator.hpp"

namespace Biips
{

  class ArrayAccumulator
  {
  public:
    typedef MultiArray::StorageType StorageType;

    typedef ArrayAccumulator SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef acc::accumulator_set<StorageType, acc::features<AccTags::Sum,
        AccTags::Mean, AccTags::Moment2, AccTags::Variance, AccTags::Moment3,
        AccTags::Skewness, AccTags::Moment4, AccTags::Kurtosis>, Scalar>
        AccType;

  protected:
    AccType acc_;
    DimArray::Ptr pDim_;

    std::map<StatTag, Bool> featuresMap_;

    void drop(StatTag tag);

  public:
    ArrayAccumulator();

    void ClearFeatures();
    void AddFeature(StatTag feat);
    void RemoveFeature(StatTag feat);

    void Init(const DimArray::Ptr & pDim);

    void Push(const StorageType & value, Scalar weight);

    Size Count() const;
    Scalar SumOfWeights() const;
    MultiArray Sum() const;
    MultiArray Mean() const;
    MultiArray Variance() const;
    MultiArray Skewness() const;
    MultiArray Kurtosis() const;
    template<Size Order>
    MultiArray Moment() const
    {
      switch (Order)
      {
        case 2:
          if (!featuresMap_.at(MOMENT2))
            throw LogicError("Invalid statistic extraction.");
          break;
        case 3:
          if (!featuresMap_.at(MOMENT3))
            throw LogicError("Invalid statistic extraction.");
          break;
        case 4:
          if (!featuresMap_.at(MOMENT4))
            throw LogicError("Invalid statistic extraction.");
          break;
        default:
          throw LogicError("Invalid moment order.");
          break;
      }
      if (!pDim_)
        throw LogicError("Can not extract statistic: dim pointer is null.");
      ValArray::Ptr p_val(new ValArray(acc::weighted_moment<Order>(acc_)));
      return MultiArray(pDim_, p_val);
    }
  };
}

#endif /* BIIPS_ARRAYACCUMULATOR_HPP_ */
