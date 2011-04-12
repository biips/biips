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
  inline Scalar sqrtScalar(Scalar s) { return std::sqrt(s); };
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

  ScalarHistogram::ScalarHistogram(const HistogramType & hist) : BaseType(hist.begin(), hist.end())
  {
  }

  Types<Scalar>::Array ScalarHistogram::GetPositions() const
  {
    Types<Scalar>::Array positions(size());
    for (Size i = 0; i<size(); ++i)
      positions[i] = operator[](i).first;
    return positions;
  }

  Types<Scalar>::Array ScalarHistogram::GetFrequencies() const
  {
    Types<Scalar>::Array freq(size());
    for (Size i = 0; i<size(); ++i)
      freq[i] = operator[](i).second;
    return freq;
  }

  Scalar DiscreteScalarHistogram::MaxFreq() const
  {
    const_iterator it_max_value = std::max_element(begin(), end(), ScalarPairCompare());
    return it_max_value->first;
  }

  void DiscreteScalarHistogram::Push(Scalar position, Scalar value)
  {
    Size u_position = roundSize(position);
    if ( empty() )
    {
      // insert intermediate 0
      for (Size pos = 1; pos < u_position; pos ++)
        push_back(PairType(Scalar(pos), 0.0));
      // push new value
      push_back(PairType(Scalar(u_position), value));
      normConst_ += value;
    }
    else
    {
      Size u_back_pos = roundSize(back().first);
      if ( u_position > u_back_pos )
      {
        // insert intermediate 0
        for (Size pos = u_back_pos + 1; pos < u_position; pos ++)
          push_back(PairType(Scalar(pos), 0.0));
        // push new value
        push_back(PairType(Scalar(u_position), value));
        normConst_ += value;
      }
      else
      {
        Size u_front_pos = roundSize(front().first);
        if ( (u_position >= u_front_pos) )
        {
          iterator it_position = std::find_if(begin(), end(), ScalarPairEqualPredicate(u_position));
          // increment correct value
          it_position->second += value;
          normConst_ += value;
        }
        // TODO else throw exception
      }
    }
  }

  void DiscreteScalarHistogram::Normalize()
  {
    if (normConst_ != 1.0 && normConst_ != 0.0)
      for (Size i =0; i<size(); ++i)
        operator[](i).second /= normConst_;
    normConst_ = 1.0;
  }



  ScalarAccumulator::ScalarAccumulator()
  : pdfCacheSize_(1), pdfNumBins_(1), cdfNumCells_(1),
    acc_(Tags::Pdf::cache_size = pdfCacheSize_, Tags::Pdf::num_bins = pdfNumBins_, Tags::Quantiles::probabilities = quantileProbs_, Tags::Cdf::num_cells = cdfNumCells_)
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
    switch(tag)
    {
      // FIXME: never drop COUNT: problem when the only feature is PDF or CDF or QUANTILES
      case SUM_OF_WEIGHTS:
        acc_.drop<Tags::SumOfWeights>();
        break;
      case SUM:
        acc_.drop<Tags::Sum>();
        break;
      case MEAN:
        acc_.drop<Tags::Mean>();
        break;
      case VARIANCE:
        acc_.drop<Tags::Variance>();
        break;
      case MOMENT2:
        acc_.drop<Tags::Moment2>();
        break;
      case MOMENT3:
        acc_.drop<Tags::Moment3>();
        break;
      case MOMENT4:
        acc_.drop<Tags::Moment4>();
        break;
      case MOMENT5:
        acc_.drop<Tags::Moment5>();
        break;
      case SKEWNESS:
        acc_.drop<Tags::Skewness>();
        break;
      case KURTOSIS:
        acc_.drop<Tags::Kurtosis>();
        break;
      case MIN:
        acc_.drop<Tags::Min>();
        break;
      case MAX:
        acc_.drop<Tags::Max>();
        break;
      case PDF:
        acc_.drop<Tags::Pdf>();
        break;
      case QUANTILES:
        acc_.drop<Tags::Quantiles>();
        break;
      case CDF:
        acc_.drop<Tags::Cdf>();
        break;
      default:
        break;
    }
  }


  void ScalarAccumulator::ClearFeatures()
  {
    for (std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat != featuresMap_.end(); ++it_feat )
      it_feat->second = false;
  }


  void ScalarAccumulator::Init()
  {
    if (!quantileProbs_.empty())
    {
      quantileProbs_.sort();
      quantileProbs_.unique();
    }
    acc_ = AccType(Tags::Pdf::cache_size = pdfCacheSize_, Tags::Pdf::num_bins = pdfNumBins_, Tags::Quantiles::probabilities = quantileProbs_, Tags::Cdf::num_cells = cdfNumCells_);
    for ( std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat != featuresMap_.end(); ++it_feat )
      if ( ! it_feat->second )
        drop(it_feat->first);
  }


  ElementAccumulator::ElementAccumulator()
  : dimDefined_(false)
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
    switch(tag)
    {
      case SUM_OF_WEIGHTS:
        acc_.drop<Tags::SumOfWeights>();
        break;
      case SUM:
        acc_.drop<Tags::Sum>();
        break;
      case MEAN:
        acc_.drop<Tags::Mean>();
        break;
      case VARIANCE:
        acc_.drop<Tags::Variance>();
        break;
      case MOMENT2:
        acc_.drop<Tags::Moment2>();
        break;
      case MOMENT3:
        acc_.drop<Tags::Moment3>();
        break;
      case MOMENT4:
        acc_.drop<Tags::Moment4>();
        break;
      case MOMENT5:
        acc_.drop<Tags::Moment5>();
        break;
      case SKEWNESS:
        acc_.drop<Tags::Skewness>();
        break;
      case KURTOSIS:
        acc_.drop<Tags::Kurtosis>();
        break;
      default:
        break;
    }
  }


  void ElementAccumulator::ClearFeatures()
  {
    for (std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat != featuresMap_.end(); ++it_feat )
      it_feat->second = false;
  }


  void ElementAccumulator::Init(const DimArray::Ptr & pDim)
  {
    pDim_ = pDim;
    dimDefined_ = true;
    acc_ = AccType(acc::sample = StorageType(pDim->Length(), 0.));
    for ( std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat != featuresMap_.end(); ++it_feat )
      if ( ! it_feat->second )
        drop(it_feat->first);
  }


  DiscreteScalarAccumulator::DiscreteScalarAccumulator()
  {
    featuresMap_[SUM_OF_WEIGHTS] = false;
    featuresMap_[MIN] = false;
    featuresMap_[MAX] = false;
    featuresMap_[PDF] = false;
    featuresMap_[CDF] = false;
    featuresMap_[MAX_PDF] = false;

    droppableFeatures_.insert(SUM_OF_WEIGHTS);
    droppableFeatures_.insert(MIN);
    droppableFeatures_.insert(MAX);
  }


  void DiscreteScalarAccumulator::ClearFeatures()
  {
    for (std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat != featuresMap_.end(); ++it_feat )
      it_feat->second = false;
    hist_ = DiscreteScalarHistogram();
  }


  void DiscreteScalarAccumulator::drop(StatsTag tag)
   {
     switch(tag)
     {
       case SUM_OF_WEIGHTS:
         acc_.drop<Tags::SumOfWeights>();
         break;
       case MIN:
         acc_.drop<Tags::Min>();
         break;
       case MAX:
         acc_.drop<Tags::Max>();
         break;
       default:
         break;
     }
   }


  void DiscreteScalarAccumulator::AddFeature(StatsTag feat)
  {
    if (featuresMap_.find(feat) != featuresMap_.end())
      featuresMap_[feat] = true;
    if (feat == CDF || feat == MAX_PDF)
      featuresMap_[PDF] = true;
  }


  void DiscreteScalarAccumulator::RemoveFeature(StatsTag feat)
  {
    if ( feat == PDF )
    {
      if ( (!featuresMap_[CDF]) && (!featuresMap_[MAX_PDF]) )
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
    for ( std::map<StatsTag, Bool>::iterator it_feat = featuresMap_.begin(); it_feat != featuresMap_.end(); ++it_feat )
      if ( (!it_feat->second) && std::binary_search(droppableFeatures_.begin(), droppableFeatures_.end(), it_feat->first) )
        drop(it_feat->first);
  }

  void DiscreteScalarAccumulator::Push(Scalar value, Scalar weight)
  {
    acc_(value, acc::weight = weight);
    if ( featuresMap_[PDF] )
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
    for (Size i=1; i<cdf_hist.size(); ++i)
      cdf_hist.Push(cdf_hist[i].first, cdf_hist[i-1].second);
    return cdf_hist;
  } // TODO throw exception
}
