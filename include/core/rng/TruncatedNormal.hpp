/*! 
 * COPY: Adapted from JAGS TruncatedNormal.h
 */

#ifndef BIIPS_TRUNCATEDNORMAL_HPP_
#define BIIPS_TRUNCATEDNORMAL_HPP_

#include "common/Types.hpp"

namespace Biips
{

  class Rng;

  /**
   * Draws a random sample from a left-truncated normal distribution.
   *
   * @param left Left limit of the truncated distribution
   * @param rng Pointer to a Random Number Generator
   * @param mu Mean of untruncated distribution
   * @param sigma Standard deviation of untruncated distribution
   */
  Scalar lnormal(Scalar left, Rng & rng, Scalar mu = 0.0, Scalar sigma = 1.0);

  /**
   * Draws a random sample from a right-truncated normal distribution.
   *
   * @param right Right limit of the distribution
   * @param rng Pointer to a Random Number Generator
   * @param mu Mean of untruncated distribution
   * @param sigma Standard deviation of untruncated distribution
   */
  Scalar rnormal(Scalar right, Rng & rng, Scalar mu = 0.0, Scalar sigma = 1.0);

  /**
   * Draws a random sample from an interval-truncated normal distribution.
   *
   * @param left Left limit of the distribution
   * @param right Right limit of the distribution
   * @param rng Pointer to a Random Number Generator
   * @param mu Mean of untruncated distribution
   * @param sigma Standard deviation of untruncated distribution
   */
  Scalar inormal(Scalar left, Scalar right, Rng & rng,
                 Scalar mu = 0.0, Scalar sigma = 1.0);
}

#endif /* BIIPS_TRUNCATEDNORMAL_HPP_ */
