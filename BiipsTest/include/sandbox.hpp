//                                               -*- C++ -*-
/*! \file sandbox.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_SANDBOX_HPP_
#define BIIPS_SANDBOX_HPP_

//#include <boost/shared_ptr.hpp>
#include <vector>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/weighted_mean.hpp>
#include <boost/accumulators/statistics/weighted_density.hpp>
#include <boost/accumulators/statistics/weighted_extended_p_square.hpp>
#include <boost/accumulators/statistics/weighted_p_square_cumulative_distribution.hpp>
//#include "common/Common.hpp"

//namespace Biips
//{

//}

//#define BOOST_PARAMETER_MAX_ARITY 10

void sandbox()
{
  using namespace std;
  using namespace boost::accumulators;

  typedef accumulator_set<int, features<tag::weighted_mean,
      tag::weighted_density,
      tag::weighted_p_square_cumulative_distribution,
      tag::weighted_extended_p_square>,
      double> AccType1;

  typedef accumulator_set<int, features<tag::weighted_mean,
      tag::weighted_density,
      tag::weighted_p_square_cumulative_distribution>,
      double> AccType2;

  typedef accumulator_set<int, features<tag::weighted_mean,
      tag::weighted_density,
      tag::weighted_extended_p_square>,
      double> AccType3;

  double probs[] = {0.01,0.05, 0.25, 0.33, 0.5, 0.67, 0.75, 0.95, 0.99};
  vector<double> q_probs(probs, probs + sizeof(probs) / sizeof(double));

  AccType1 acc1(tag::weighted_density::cache_size = 10,
      tag::weighted_density::num_bins = 10,
      tag::weighted_extended_p_square::probabilities = q_probs,
      tag::weighted_p_square_cumulative_distribution::num_cells = 10);
  // compiles (4 tagged arguments)

//  AccType1 acc2(sample = 42, tag::weighted_density::cache_size = 10,
//      tag::weighted_density::num_bins = 10,
//      tag::weighted_extended_p_square::probabilities = q_probs,
//      tag::weighted_p_square_cumulative_distribution::num_cells = 10);
  // Error : does not compile (5 tagged arguments)

  AccType2 acc3(sample = 42, tag::weighted_density::cache_size = 10,
      tag::weighted_density::num_bins = 10,
      tag::weighted_p_square_cumulative_distribution::num_cells = 10);
  // compiles (4 tagged arguments)

  AccType3 acc4(sample = 42, tag::weighted_density::cache_size = 10,
      tag::weighted_density::num_bins = 10,
      tag::weighted_extended_p_square::probabilities = q_probs);
  // compiles (4 tagged arguments)

}

#endif /* BIIPS_SANDBOX_HPP_ */
