//                                               -*- C++ -*-
/*
 * RBiips package for GNU R is an interface to BiiPS C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * RBiips is derived software based on:
 * BiiPS, Copyright (C) Inria, 2012
 * rjags, Copyright (C) Martyn Plummer, 2002-2010
 * Rcpp, Copyright (C) Dirk Eddelbuettel and Romain Francois, 2009-2011
 *
 * This file is part of RBiips.
 *
 * RBiips is free software: you can redistribute it and/or modify
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
/*! \file weighted_statistics.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "RBiipsCommon.h"
#include "common/Accumulator.hpp"
#include "common/Utility.hpp"

using namespace Biips;
using std::endl;

using namespace Biips;

template <typename FeatureIterator>
static Accumulator accumulate(SEXP values, SEXP weights, FeatureIterator firstFeat, FeatureIterator lastFeat)
{
  Rcpp::NumericVector values_vec(values);
  Rcpp::NumericVector weights_vec(weights);
  if (values_vec.size() != weights_vec.size())
    throw LogicError("values and weights must have same length.");

  Accumulator accu;
  for (;firstFeat != lastFeat; ++firstFeat)
    accu.AddFeature(*firstFeat);

  accu.Init();

  for (int i = 0; i<values_vec.size(); ++i)
    accu.Push(values_vec[i], weights_vec[i]);

  return accu;
}


RcppExport SEXP weighted_mean (SEXP values, SEXP weights)
{
  BEGIN_RBIIPS

  static StatTag features[] = {MEAN};
  Accumulator accu = accumulate(values, weights, features, features + sizeof(features)/sizeof(StatTag));

  Rcpp::List stats;
  stats["Mean"] = Rcpp::wrap(accu.Mean());

  return stats;
  END_RBIIPS
}


RcppExport SEXP weighted_var (SEXP values, SEXP weights)
{
  BEGIN_RBIIPS

  static StatTag features[] = {MEAN, VARIANCE};
  Accumulator accu = accumulate(values, weights, features, features + sizeof(features)/sizeof(StatTag));

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

  static StatTag features[] = {MEAN, VARIANCE, SKEWNESS};
  Accumulator accu = accumulate(values, weights, features, features + sizeof(features)/sizeof(StatTag));

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

  static StatTag features[] = {MEAN, VARIANCE, SKEWNESS, KURTOSIS};
  Accumulator accu = accumulate(values, weights, features, features + sizeof(features)/sizeof(StatTag));

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

  static Scalar probs[] = {0.5};
  QuantileAccumulator accu(probs, probs+sizeof(probs)/sizeof(Scalar));
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

  QuantileAccumulator accu(probs_vec.begin(), probs_vec.end());
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

  DiscreteAccumulator accu;
  accu.Init();

  for (int i = 0; i<values_vec.size(); ++i)
    accu.Push(values_vec[i], weights_vec[i]);

  Rcpp::List stats;
  const DiscreteHistogram & hist = accu.Pdf();
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

  DiscreteAccumulator accu;
  accu.Init();

  for (int i = 0; i<values_vec.size(); ++i)
    accu.Push(values_vec[i], weights_vec[i]);

  Rcpp::List stats;
  const DiscreteHistogram & hist = accu.Pdf();
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
