//                                               -*- C++ -*-
/*! \file Accumulator.hpp
* \brief
*
* \author  $LastChangedBy$
* \date    $LastChangedDate$
* \version $LastChangedRevision$
* Id:      $Id$
*/

#ifndef BIIPS_ACCUMULATOR_HPP_
#define BIIPS_ACCUMULATOR_HPP_

#include <map>
#include <list>
#include <set>

#include "common/Common.hpp"

namespace std
{
  Biips::ValArray sqrt(const Biips::ValArray & val);
}

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/weighted_sum.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>
#include <boost/accumulators/statistics/weighted_variance.hpp>
#include <boost/accumulators/statistics/weighted_moment.hpp>
#include <boost/accumulators/statistics/weighted_skewness.hpp>
#include <boost/accumulators/statistics/weighted_kurtosis.hpp>
#include <boost/accumulators/statistics/weighted_extended_p_square.hpp>
#include <boost/accumulators/statistics/weighted_p_square_cumulative_distribution.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/weighted_density.hpp>


namespace acc = boost::accumulators;


namespace Biips
{

  struct Tags
  {
    typedef acc::droppable<acc::tag::count>                  Count;
    typedef acc::droppable<acc::tag::sum_of_weights>         SumOfWeights;
    typedef acc::droppable<acc::tag::weighted_sum>           Sum;
    typedef acc::droppable<acc::tag::weighted_mean>          Mean;
    typedef acc::droppable<acc::tag::lazy_weighted_variance> Variance;
    typedef acc::droppable<acc::tag::weighted_moment<2> >    Moment2;
    typedef acc::droppable<acc::tag::weighted_moment<3> >    Moment3;
    typedef acc::droppable<acc::tag::weighted_moment<4> >    Moment4;
    typedef acc::droppable<acc::tag::weighted_moment<5> >    Moment5;
    typedef acc::droppable<acc::tag::weighted_skewness>      Skewness;
    typedef acc::droppable<acc::tag::weighted_kurtosis>      Kurtosis;
    typedef acc::droppable<acc::tag::min>                    Min;
    typedef acc::droppable<acc::tag::max>                    Max;
    typedef acc::droppable<acc::tag::weighted_density>       Pdf;
    typedef acc::droppable<acc::tag::weighted_extended_p_square> Quantiles;
    typedef acc::droppable<acc::tag::weighted_p_square_cumulative_distribution> Cdf;
  };


  enum StatsTag
  {
    COUNT,
    SUM_OF_WEIGHTS,
    SUM,
    MEAN,
    VARIANCE,
    MOMENT2,
    MOMENT3,
    MOMENT4,
    MOMENT5,
    SKEWNESS,
    KURTOSIS,
    MIN,
    MAX,
    PDF,
    QUANTILES,
    CDF,
    MODE
  };


  class ScalarHistogram : public std::map<Scalar, Scalar>
  {
  protected:
    typedef std::map<Scalar, Scalar> BaseType;
    typedef boost::iterator_range<Types<Types<Scalar>::Pair >::Iterator> HistogramType;

  public:
    ScalarHistogram() {};
    ScalarHistogram(const HistogramType & hist);

    Types<Scalar>::Array GetPositions() const;
    Types<Scalar>::Array GetFrequencies() const;
    Scalar Min() const { return begin()->first; }
    Scalar Max() const { return (--end())->first; }
  };


  class DiscreteScalarHistogram : public ScalarHistogram
  {
  public:
    typedef ScalarHistogram BaseType;

  protected:
    Scalar normConst_;

  public:
    DiscreteScalarHistogram() {};
    Scalar MaxFreq() const;

    void Push(Scalar position, Scalar value);
    void Normalize();
  };


  class ScalarAccumulator
  {
  public:
    typedef ScalarAccumulator SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef acc::accumulator_set<Scalar,
    acc::features<Tags::Count, Tags::Sum, Tags::SumOfWeights,
    Tags::Mean, Tags::Variance, Tags::Skewness, Tags::Kurtosis,
    Tags::Moment2, Tags::Moment3, Tags::Moment4, Tags::Moment5,
    Tags::Min, Tags::Max, Tags::Pdf, Tags::Quantiles, Tags::Cdf>,
    Scalar> AccType;

  protected:
    Size pdfCacheSize_;
    Size pdfNumBins_;
    std::list<Scalar> quantileProbs_;
    Size cdfNumCells_;

    std::map<StatsTag, Bool> featuresMap_;

    AccType acc_;

    void drop(StatsTag tag);

  public:
    ScalarAccumulator();

    void ClearFeatures();
    template<typename InputIterator>
    void AddFeatures(InputIterator first, InputIterator last)
    {
      for (InputIterator it_feat = first; it_feat != last; ++it_feat)
        AddFeature(*it_feat);
    }
    void AddFeature(StatsTag feat) { if (featuresMap_.find(feat) != featuresMap_.end()) featuresMap_[feat] = true; }
    void RemoveFeature(StatsTag feat) { if (featuresMap_.find(feat) != featuresMap_.end()) featuresMap_[feat] = false; }

    void SetPdfParam(Size cacheSize, Size numBins) { pdfCacheSize_ = cacheSize; pdfNumBins_ = numBins; }

    template<typename InputIterator>
    void SetQuantileProbs(InputIterator first, InputIterator last) { quantileProbs_.assign(first, last); }
    void ClearQuantileProbs() { quantileProbs_.clear(); }
    void AddQuantileProb(Scalar prob) { quantileProbs_.push_back(prob); }
    void RemoveQuantileProb(Scalar prob)  { quantileProbs_.remove(prob); }

    void SetCdfParam(Size numCells) { cdfNumCells_ = numCells; }

    void Init();

    virtual void Push(Scalar value, Scalar weight) { acc_(value, acc::weight = weight); }

    Size Count() const { return acc::count(acc_); }; // TODO throw exception
    Scalar SumOfWeights() const { return acc::sum_of_weights(acc_); }; // TODO throw exception
    Scalar Sum() const { return acc::weighted_sum(acc_); }; // TODO throw exception
    Scalar Mean() const { return acc::weighted_mean(acc_); }; // TODO throw exception
    Scalar Variance() const { return acc::weighted_variance(acc_); }; // TODO throw exception
    template<Size Order>
    Scalar Moment() const { return acc::weighted_moment<Order>(acc_); }; // TODO throw exception
    Scalar Skewness() const { return acc::weighted_skewness(acc_); }; // TODO throw exception
    Scalar Kurtosis() const { return acc::weighted_kurtosis(acc_); }; // TODO throw exception
    Scalar Min() const { return acc::min(acc_); }; // TODO throw exception
    Scalar Max() const { return acc::max(acc_); }; // TODO throw exception
    ScalarHistogram Pdf() const { return ScalarHistogram(acc::weighted_density(acc_)); }; // TODO throw exception
    Scalar Quantile(Size i) const { return acc::weighted_extended_p_square(acc_)[i]; }; // TODO throw exception
    ScalarHistogram Cdf() const { return ScalarHistogram(acc::weighted_p_square_cumulative_distribution(acc_)); }; // TODO throw exception
  };


  class DiscreteScalarAccumulator
  {
  public:
    typedef DiscreteScalarAccumulator SelfType;
    typedef Types<SelfType>::Ptr Ptr;

    typedef acc::accumulator_set<Scalar,
    acc::features<Tags::Count, Tags::SumOfWeights,
    Tags::Min, Tags::Max>,
    Scalar> AccType;

  protected:
    std::map<StatsTag, Bool> featuresMap_;
    std::set<StatsTag> droppableFeatures_;

    AccType acc_;
    DiscreteScalarHistogram hist_;

    void drop(StatsTag tag);

  public:
    DiscreteScalarAccumulator();

    void ClearFeatures();
    template<typename InputIterator>
    void AddFeatures(InputIterator first, InputIterator last)
    {
      for (InputIterator it_feat = first; it_feat != last; ++it_feat)
        AddFeature(*it_feat);
    }
    void AddFeature(StatsTag feat);
    void RemoveFeature(StatsTag feat);

    void Init();

    void Push(Scalar value, Scalar weight);

    Size Count() const { return acc::count(acc_); }; // TODO throw exception
    Scalar SumOfWeights() const { return acc::sum_of_weights(acc_); }; // TODO throw exception
    Scalar Min() const { return acc::min(acc_); }; // TODO throw exception
    Scalar Max() const { return acc::max(acc_); }; // TODO throw exception
    const DiscreteScalarHistogram & Pdf(); // TODO throw exception
    DiscreteScalarHistogram Cdf(); // TODO throw exception
    Scalar Mode() const { return hist_.MaxFreq(); }; // TODO throw exception
  };


  class ElementAccumulator
  {
  public:
    typedef MultiArray::StorageType StorageType;

    typedef ElementAccumulator SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef acc::accumulator_set<StorageType,
    acc::features<Tags::Count, Tags::Sum, Tags::SumOfWeights,
    Tags::Mean, Tags::Variance, Tags::Skewness, Tags::Kurtosis,
    Tags::Moment2, Tags::Moment3, Tags::Moment4, Tags::Moment5>,
    Scalar> AccType;

  protected:
    AccType acc_;
    DimArray::Ptr pDim_;
    Bool dimDefined_;

    std::map<StatsTag, Bool> featuresMap_;

    void drop(StatsTag tag);

  public:
    ElementAccumulator();

    void ClearFeatures();
    template<typename InputIterator>
    void AddFeatures(InputIterator first, InputIterator last)
    {
      for (InputIterator it_feat = first; it_feat != last; ++it_feat)
        AddFeature(*it_feat);
    }
    void AddFeature(StatsTag feat) { if (featuresMap_.find(feat) != featuresMap_.end()) featuresMap_[feat] = true; }
    void RemoveFeature(StatsTag feat) { if (featuresMap_.find(feat) != featuresMap_.end()) featuresMap_[feat] = false; }

    void Init(const DimArray::Ptr & pDim);

    void Push(const StorageType & value, Scalar weight) { acc_(value, acc::weight = weight); }

    Size Count() const { return acc::count(acc_); }; // TODO throw exception
    Scalar SumOfWeights() const { return acc::sum_of_weights(acc_); }; // TODO throw exception
    MultiArray Sum() const { return MultiArray(pDim_, acc::weighted_sum(acc_)); }; // TODO check dimDefined_  // TODO throw exception
    MultiArray Mean() const { return MultiArray(pDim_, acc::weighted_mean(acc_)); }; // TODO check dimDefined_ // TODO throw exception
    MultiArray Variance() const { return MultiArray(pDim_, acc::weighted_variance(acc_)); }; // TODO check dimDefined_ // TODO throw exception
    template<Size Order>
    MultiArray Moment() const { return MultiArray(pDim_, acc::weighted_moment<Order>(acc_)); }; // TODO check dimDefined_ // TODO throw exception
    MultiArray Skewness() const { return MultiArray(pDim_, acc::weighted_skewness(acc_)); }; // TODO check dimDefined_ // TODO throw exception
    MultiArray Kurtosis() const { return MultiArray(pDim_, acc::weighted_kurtosis(acc_)); }; // TODO check dimDefined_ // TODO throw exception
  };

}

#endif /* BIIPS_ACCUMULATOR_HPP_ */
