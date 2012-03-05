//                                               -*- C++ -*-
/*! \file weighted_statistics.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "RBiipsCommon.h"
#include "sampler/Accumulator.hpp"

using namespace Biips;
using std::endl;

using namespace Biips;

template <typename FeatureIterator>
static ScalarAccumulator accumulate(SEXP values, SEXP weights, FeatureIterator firstFeat, FeatureIterator LastFeat)
{
  Rcpp::NumericVector values_vec(values);
  Rcpp::NumericVector weights_vec(weights);
  if (values_vec.size() != weights_vec.size())
    throw LogicError("values and weights must have same length.");

  ScalarAccumulator accu;
  accu.AddFeatures(firstFeat, LastFeat);
  accu.Init();

  for (int i = 0; i<values_vec.size(); ++i)
    accu.Push(values_vec[i], weights_vec[i]);

  return accu;
}


RcppExport SEXP weighted_mean (SEXP values, SEXP weights)
{
  BEGIN_RBIIPS

  static StatsTag features[] = {MEAN};
  ScalarAccumulator accu = accumulate(values, weights, features, features + sizeof(features)/sizeof(StatsTag));

  Rcpp::List stats;
  stats["Mean"] = Rcpp::wrap(accu.Mean());

  return stats;
  END_RBIIPS
}


RcppExport SEXP weighted_var (SEXP values, SEXP weights)
{
  BEGIN_RBIIPS

  static StatsTag features[] = {MEAN, VARIANCE};
  ScalarAccumulator accu = accumulate(values, weights, features, features + sizeof(features)/sizeof(StatsTag));

  Rcpp::List stats;
  stats["Mean"] = Rcpp::wrap(accu.Mean());
  stats["2nd mt."] = Rcpp::wrap(accu.Moment<2>());
  stats["Var."] = Rcpp::wrap(accu.Variance());
  stats["SD"] = Rcpp::wrap(std::sqrt(accu.Variance()));

  return stats;
  END_RBIIPS
}


RcppExport SEXP weighted_skew (SEXP values, SEXP weights)
{
  BEGIN_RBIIPS

  static StatsTag features[] = {MEAN, VARIANCE, SKEWNESS};
  ScalarAccumulator accu = accumulate(values, weights, features, features + sizeof(features)/sizeof(StatsTag));

  Rcpp::List stats;
  stats["Mean"] = Rcpp::wrap(accu.Mean());
  stats["2nd mt."] = Rcpp::wrap(accu.Moment<2>());
  stats["Var."] = Rcpp::wrap(accu.Variance());
  stats["SD"] = Rcpp::wrap(std::sqrt(accu.Variance()));
  stats["3rd mt."] = Rcpp::wrap(accu.Moment<3>());
  stats["Skew."] = Rcpp::wrap(accu.Skewness());

  return stats;
  END_RBIIPS
}


RcppExport SEXP weighted_kurt (SEXP values, SEXP weights)
{
  BEGIN_RBIIPS

  static StatsTag features[] = {MEAN, VARIANCE, SKEWNESS, KURTOSIS};
  ScalarAccumulator accu = accumulate(values, weights, features, features + sizeof(features)/sizeof(StatsTag));

  Rcpp::List stats;
  stats["Mean"] = Rcpp::wrap(accu.Mean());
  stats["2nd mt."] = Rcpp::wrap(accu.Moment<2>());
  stats["Var."] = Rcpp::wrap(accu.Variance());
  stats["SD"] = Rcpp::wrap(std::sqrt(accu.Variance()));
  stats["3rd mt."] = Rcpp::wrap(accu.Moment<3>());
  stats["Skew."] = Rcpp::wrap(accu.Skewness());
  stats["4th mt."] = Rcpp::wrap(accu.Moment<4>());
  stats["Kurt."] = Rcpp::wrap(accu.Kurtosis());

  return stats;
  END_RBIIPS
}


RcppExport SEXP weighted_median (SEXP values, SEXP weights)
{
  BEGIN_RBIIPS

  Rcpp::NumericVector values_vec(values);
  Rcpp::NumericVector weights_vec(weights);
  if (values_vec.size() != weights_vec.size())
    throw LogicError("values and weights must have same length.");

  ScalarAccumulator accu;
  accu.AddFeature(QUANTILES);
  accu.AddQuantileProb(0.5);
  accu.Init();

  for (int i = 0; i<values_vec.size(); ++i)
    accu.Push(values_vec[i], weights_vec[i]);

  Rcpp::List stats;
  stats["Median"] = Rcpp::wrap(accu.Quantile(0));

  return stats;
  END_RBIIPS
}


RcppExport SEXP weighted_quantiles (SEXP values, SEXP weights, SEXP probs)
{
  BEGIN_RBIIPS

  Rcpp::NumericVector values_vec(values);
  Rcpp::NumericVector weights_vec(weights);
  if (values_vec.size() != weights_vec.size())
    throw LogicError("values and weights must have same length.");
  Rcpp::NumericVector probs_vec(probs);

  ScalarAccumulator accu;
  accu.AddFeature(QUANTILES);
  accu.SetQuantileProbs(probs_vec.begin(), probs_vec.end());
  accu.Init();

  for (int i = 0; i<values_vec.size(); ++i)
    accu.Push(values_vec[i], weights_vec[i]);

  Rcpp::List stats;
  for (int i = 0; i<probs_vec.size(); ++i)
  {
    if (probs_vec[i] == 0.5)
      stats["Median"] = Rcpp::wrap(accu.Quantile(i));
    else
      stats[String("Qu. ")+print(probs_vec[i])] = Rcpp::wrap(accu.Quantile(i));
  }

  return stats;
  END_RBIIPS
}


RcppExport SEXP weighted_table(SEXP values, SEXP weights)
{
  BEGIN_RBIIPS

  Rcpp::NumericVector values_vec(values);
  Rcpp::NumericVector weights_vec(weights);
  if (values_vec.size() != weights_vec.size())
    throw LogicError("values and weights must have same length.");

  DiscreteScalarAccumulator accu;
  accu.AddFeature(PDF);
  accu.Init();

  for (int i = 0; i<values_vec.size(); ++i)
    accu.Push(values_vec[i], weights_vec[i]);

  Rcpp::List stats;
  const DiscreteScalarHistogram & hist = accu.Pdf();
  Types<Scalar>::Array vec = hist.GetPositions();
  Rcpp::IntegerVector x(vec.begin(), vec.end());
  vec = hist.GetFrequencies();
  Rcpp::NumericVector y(vec.begin(), vec.end());
  Rcpp::List table;
  table["x"] = x;
  table["y"] = y;
  stats["Table"] = table;

  return stats;
  END_RBIIPS
}


RcppExport SEXP weighted_mode(SEXP values, SEXP weights)
{
  BEGIN_RBIIPS

  Rcpp::NumericVector values_vec(values);
  Rcpp::NumericVector weights_vec(weights);
  if (values_vec.size() != weights_vec.size())
    throw LogicError("values and weights must have same length.");

  DiscreteScalarAccumulator accu;
  accu.AddFeature(MODE);
  accu.Init();

  for (int i = 0; i<values_vec.size(); ++i)
    accu.Push(values_vec[i], weights_vec[i]);

  Rcpp::List stats;
  const DiscreteScalarHistogram & hist = accu.Pdf();
  Types<Scalar>::Array vec = hist.GetPositions();
  Rcpp::IntegerVector x(vec.begin(), vec.end());
  vec = hist.GetFrequencies();
  Rcpp::NumericVector proba(vec.begin(), vec.end());
  Rcpp::List table;
  table["x"] = x;
  table["proba."] = proba;
  stats["Table"] = table;
  stats["Mode"] = Rcpp::wrap(accu.Mode());

  return stats;
  END_RBIIPS
}
