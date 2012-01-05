//                                               -*- C++ -*-
/*! \file Accumulator.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "sampler/Accumulator.hpp"
#include "common/Integer.hpp"

namespace Biips
{
  inline Scalar sqrtScalar(Scalar s)
  {
    return std::sqrt(s);
  }
}

namespace std
{
  Biips::ValArray sqrt(const Biips::ValArray & val)
  {
    return val.Apply(&Biips::sqrtScalar);
  }
}

namespace Biips
{

  ScalarHistogram::ScalarHistogram(const HistogramType & hist) :
    BaseType(hist.begin(), hist.end())
  {
  }

  Types<Scalar>::Array ScalarHistogram::GetPositions() const
  {
    Types<Scalar>::Array positions(size());
    const_iterator it = begin();
    for (Size i = 0; it != end(); ++it, ++i)
      positions[i] = it->first;
    return positions;
  }

  Types<Scalar>::Array ScalarHistogram::GetFrequencies() const
  {
    Types<Scalar>::Array freq(size());
    const_iterator it = begin();
    for (Size i = 0; it != end(); ++it, ++i)
      freq[i] = it->second;
    return freq;
  }

  struct ScalarPairSecondCompare
  {
    typedef ScalarHistogram::value_type PairType;
    Bool operator()(const PairType & lhs, const PairType & rhs) const
    {
      return lhs.second < rhs.second;
    }
  };

  Scalar DiscreteScalarHistogram::MaxFreq() const
  {
    const_iterator it_max_value = std::max_element(begin(),
                                                   end(),
                                                   ScalarPairSecondCompare());
    return it_max_value->first;
  }

  void DiscreteScalarHistogram::Push(Scalar position, Scalar value)
  {
    Size u_position = roundSize(position);
    operator[](Scalar(u_position)) += value;
    normConst_ += value;
  }

  void DiscreteScalarHistogram::Normalize()
  {
    if (normConst_ == 1.0 && normConst_ == 0.0)
      return;
    for (iterator it = begin(); it != end(); ++it)
      it->second /= normConst_;
    normConst_ = 1.0;
  }

  ScalarAccumulator::ScalarAccumulator() :
    pdfCacheSize_(1), pdfNumBins_(1), cdfNumCells_(1),
        acc_(Tags::Pdf::cache_size = pdfCacheSize_,
             Tags::Pdf::num_bins = pdfNumBins_,
             Tags::Quantiles::probabilities = quantileProbs_,
             Tags::Cdf::num_cells = cdfNumCells_)
  {
    featuresMap_[SUM_OF_WEIGHTS] = false;
    featuresMap_[SUM] = false;
    featuresMap_[MEAN] = false;
    featuresMap_[VARIANCE] = false;
    featuresMap_[MOMENT2] = false;
    featuresMap_[MOMENT3] = false;
    featuresMap_[MOMENT4] = false;
    featuresMap_[MOMENT5] = false;
    featuresMap_[SKEWNESS] = false;
    featuresMap_[KURTOSIS] = false;
    featuresMap_[MIN] = false;
    featuresMap_[MAX] = false;
    featuresMap_[PDF] = false;
    featuresMap_[QUANTILES] = false;
    featuresMap_[CDF] = false;
  }

  void ScalarAccumulator::drop(StatsTag tag)
  {
    switch (tag)
    {
      // FIXME: never drop COUNT: problem when the only feature is PDF or CDF or QUANTILES
      case SUM_OF_WEIGHTS:
        acc_.drop<Tags::SumOfWeights> ();
        break;
      case SUM:
        acc_.drop<Tags::Sum> ();
        break;
      case MEAN:
        acc_.drop<Tags::Mean> ();
        break;
      case VARIANCE:
        acc_.drop<Tags::Variance> ();
        break;
      case MOMENT2:
        acc_.drop<Tags::Moment2> ();
        break;
      case MOMENT3:
        acc_.drop<Tags::Moment3> ();
        break;
      case MOMENT4:
        acc_.drop<Tags::Moment4> ();
        break;
      case MOMENT5:
        acc_.drop<Tags::Moment5> ();
        break;
      case SKEWNESS:
        acc_.drop<Tags::Skewness> ();
        break;
      case KURTOSIS:
        acc_.drop<Tags::Kurtosis> ();
        break;
      case MIN:
        acc_.drop<Tags::Min> ();
        break;
      case MAX:
        acc_.drop<Tags::Max> ();
        break;
      case PDF:
        acc_.drop<Tags::Pdf> ();
        break;
      case QUANTILES:
        acc_.drop<Tags::Quantiles> ();
        break;
      case CDF:
        acc_.drop<Tags::Cdf> ();
        break;
      default:
        break;
    }
  }

  void ScalarAccumulator::ClearFeatures()
  {
    for (std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat
        != featuresMap_.end(); ++it_feat)
      it_feat->second = false;
  }

  void ScalarAccumulator::Init()
  {
    if (!quantileProbs_.empty())
    {
      quantileProbs_.sort();
      quantileProbs_.unique();
    }
    acc_ = AccType(Tags::Pdf::cache_size = pdfCacheSize_,
                   Tags::Pdf::num_bins = pdfNumBins_,
                   Tags::Quantiles::probabilities = quantileProbs_,
                   Tags::Cdf::num_cells = cdfNumCells_);
    for (std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat
        != featuresMap_.end(); ++it_feat)
      if (!it_feat->second)
        drop(it_feat->first);
  }

  DiscreteScalarAccumulator::DiscreteScalarAccumulator()
  {
    featuresMap_[SUM_OF_WEIGHTS] = false;
    featuresMap_[MIN] = false;
    featuresMap_[MAX] = false;
    featuresMap_[PDF] = false;
    featuresMap_[CDF] = false;
    featuresMap_[MODE] = false;

    droppableFeatures_.insert(SUM_OF_WEIGHTS);
    droppableFeatures_.insert(MIN);
    droppableFeatures_.insert(MAX);
  }

  void DiscreteScalarAccumulator::ClearFeatures()
  {
    for (std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat
        != featuresMap_.end(); ++it_feat)
      it_feat->second = false;
    hist_ = DiscreteScalarHistogram();
  }

  void DiscreteScalarAccumulator::drop(StatsTag tag)
  {
    switch (tag)
    {
      case SUM_OF_WEIGHTS:
        acc_.drop<Tags::SumOfWeights> ();
        break;
      case MIN:
        acc_.drop<Tags::Min> ();
        break;
      case MAX:
        acc_.drop<Tags::Max> ();
        break;
      default:
        break;
    }
  }

  void DiscreteScalarAccumulator::AddFeature(StatsTag feat)
  {
    if (featuresMap_.find(feat) != featuresMap_.end())
      featuresMap_[feat] = true;
    if (feat == CDF || feat == MODE)
      featuresMap_[PDF] = true;
  }

  void DiscreteScalarAccumulator::RemoveFeature(StatsTag feat)
  {
    if (feat == PDF)
    {
      if ((!featuresMap_[CDF]) && (!featuresMap_[MODE]))
      {
        featuresMap_[PDF] = false;
        hist_ = DiscreteScalarHistogram();
      }
    }
    else if (featuresMap_.find(feat) != featuresMap_.end())
      featuresMap_[feat] = false;
  }

  void DiscreteScalarAccumulator::Init()
  {
    acc_ = AccType();
    hist_ = DiscreteScalarHistogram();
    for (std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat
        != featuresMap_.end(); ++it_feat)
      if ((!it_feat->second) && std::binary_search(droppableFeatures_.begin(),
                                                   droppableFeatures_.end(),
                                                   it_feat->first))
        drop(it_feat->first);
  }

  void DiscreteScalarAccumulator::Push(Scalar value, Scalar weight)
  {
    acc_(value, acc::weight = weight);
    if (featuresMap_[PDF])
    {
      hist_.Push(value, weight);
    }
  }

  const DiscreteScalarHistogram & DiscreteScalarAccumulator::Pdf()
  {
    hist_.Normalize();
    return hist_;
  } // TODO throw exception


  DiscreteScalarHistogram DiscreteScalarAccumulator::Cdf()
  {
    DiscreteScalarHistogram cdf_hist = Pdf();
    DiscreteScalarHistogram::iterator it = cdf_hist.begin();
    for (Size i = 0; it != cdf_hist.end(); ++it, ++i)
      cdf_hist.Push(it->first, it->second);
    return cdf_hist;
  } // TODO throw exception


  ElementAccumulator::ElementAccumulator() :
    dimDefined_(false)
  {
    featuresMap_[SUM_OF_WEIGHTS] = false;
    featuresMap_[SUM] = false;
    featuresMap_[MEAN] = false;
    featuresMap_[VARIANCE] = false;
    featuresMap_[MOMENT2] = false;
    featuresMap_[MOMENT3] = false;
    featuresMap_[MOMENT4] = false;
    featuresMap_[MOMENT5] = false;
    featuresMap_[SKEWNESS] = false;
    featuresMap_[KURTOSIS] = false;
  }

  void ElementAccumulator::drop(StatsTag tag)
  {
    switch (tag)
    {
      case SUM_OF_WEIGHTS:
        acc_.drop<Tags::SumOfWeights> ();
        break;
      case SUM:
        acc_.drop<Tags::Sum> ();
        break;
      case MEAN:
        acc_.drop<Tags::Mean> ();
        break;
      case VARIANCE:
        acc_.drop<Tags::Variance> ();
        break;
      case MOMENT2:
        acc_.drop<Tags::Moment2> ();
        break;
      case MOMENT3:
        acc_.drop<Tags::Moment3> ();
        break;
      case MOMENT4:
        acc_.drop<Tags::Moment4> ();
        break;
      case MOMENT5:
        acc_.drop<Tags::Moment5> ();
        break;
      case SKEWNESS:
        acc_.drop<Tags::Skewness> ();
        break;
      case KURTOSIS:
        acc_.drop<Tags::Kurtosis> ();
        break;
      default:
        break;
    }
  }

  void ElementAccumulator::ClearFeatures()
  {
    for (std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat
        != featuresMap_.end(); ++it_feat)
      it_feat->second = false;
  }

  void ElementAccumulator::Init(const DimArray::Ptr & pDim)
  {
    pDim_ = pDim;
    dimDefined_ = true;
    acc_ = AccType(acc::sample = StorageType(pDim->Length(), 0.));
    for (std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat
        != featuresMap_.end(); ++it_feat)
      if (!it_feat->second)
        drop(it_feat->first);
  }

  MultiArray ElementAccumulator::Sum() const
  {
    ValArray::Ptr p_val(new ValArray(acc::weighted_sum(acc_)));
    return MultiArray(pDim_, p_val);
  } // TODO check dimDefined_  // TODO throw exception
  MultiArray ElementAccumulator::Mean() const
  {
    ValArray::Ptr p_val(new ValArray(acc::weighted_mean(acc_)));
    return MultiArray(pDim_, p_val);
  } // TODO check dimDefined_ // TODO throw exception
  MultiArray ElementAccumulator::Variance() const
  {
    ValArray::Ptr p_val(new ValArray(acc::weighted_variance(acc_)));
    return MultiArray(pDim_, p_val);
  } // TODO check dimDefined_ // TODO throw exception
  MultiArray ElementAccumulator::Skewness() const
  {
    ValArray::Ptr p_val(new ValArray(acc::weighted_skewness(acc_)));
    return MultiArray(pDim_, p_val);
  } // TODO check dimDefined_ // TODO throw exception
  MultiArray ElementAccumulator::Kurtosis() const
  {
    ValArray::Ptr p_val(new ValArray(acc::weighted_kurtosis(acc_)));
    return MultiArray(pDim_, p_val);
  } // TODO check dimDefined_ // TODO throw exception
}
