//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
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

/*! \file Resampler.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from SMCTC smc::sampler::resample function
 */

#include "sampler/Resampler.hpp"

#include <boost/random/discrete_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

namespace Biips
{

  void Resampler::Resample(Types<Particle>::Array & particles,
                           Scalar & sumOfWeights,
                           Rng & rng)
  {
    nParticles_ = particles.size();
    weights_.resize(nParticles_);
    for (Size i = 0; i < nParticles_; ++i)
      weights_[i] = particles[i].Weight();

    count_.assign(nParticles_, 0);
    indices_.resize(nParticles_);

    resample(sumOfWeights, rng);

    //Map count to indices to allow in-place resampling
    for (Size i = 0, j = 0; i < nParticles_; ++i)
    {
      if (count_[i] > 0)
      {
        indices_[i] = i;
        while (count_[i] > 1)
        {
          while (count_[j] > 0)
            ++j; // find next free spot
          indices_[j++] = i; // assign index
          --count_[i]; // decrement number of remaining offsprings
        }
      }
    }

    //Perform the replication of the chosen.
    for (Size i = 0; i < nParticles_; ++i)
    {
      if (indices_[i] != i)
        particles[i].Value() = particles[indices_[i]].GetValue();
      particles[i].ResetWeight();
    }

    sumOfWeights = nParticles_;
  }

  class MultinomialResampler: public Resampler
  {
  public:
    typedef MultinomialResampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Resampler BaseType;

  protected:
    MultinomialResampler() :
      BaseType("multinomial")
    {
    }

    virtual void resample(Scalar sumOfWeights, Rng & rng);

  public:
    static BaseType::Ptr Instance()
    {
      static BaseType::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class ResidualResampler: public Resampler
  {
  public:
    typedef ResidualResampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Resampler BaseType;

  protected:
    ResidualResampler() :
      BaseType("residual")
    {
    }

    virtual void resample(Scalar sumOfWeights, Rng & rng);

  public:
    static BaseType::Ptr Instance()
    {
      static BaseType::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class StratifiedResampler: public Resampler
  {
  public:
    typedef StratifiedResampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Resampler BaseType;

  protected:
    StratifiedResampler() :
      BaseType("stratified")
    {
    }

    virtual void resample(Scalar sumOfWeights, Rng & rng);

  public:
    static BaseType::Ptr Instance()
    {
      static BaseType::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  class SystematicResampler: public Resampler
  {
  public:
    typedef SystematicResampler SelfType;
    typedef Types<SelfType>::Ptr Ptr;
    typedef Resampler BaseType;

  protected:
    SystematicResampler() :
      BaseType("systematic")
    {
    }

    virtual void resample(Scalar sumOfWeights, Rng & rng);

  public:
    static BaseType::Ptr Instance()
    {
      static BaseType::Ptr p_instance(new SelfType());
      return p_instance;
    }
  };

  void MultinomialResampler::resample(Scalar sumOfWeights, Rng & rng)
  {
    typedef boost::random::discrete_distribution<Int, Scalar> CategoricalDist;
    CategoricalDist dist(weights_.begin(), weights_.end());
    typedef boost::variate_generator<Rng::GenType&, CategoricalDist>
        CategoricalGen;
    CategoricalGen gen(rng.GetGen(), dist);
    for (Size i = 0; i < nParticles_; ++i)
      count_[gen()]++;
  }

  void ResidualResampler::resample(Scalar sumOfWeights, Rng & rng)
  {
    weights_ *= nParticles_ / sumOfWeights;

    for (Size i = 0; i < nParticles_; ++i)
    {
      count_[i] = floor(weights_[i]);
      weights_[i] -= count_[i];
    }

    Size n_multinomial = nParticles_ - std::accumulate(count_.begin(),
                                                       count_.end(),
                                                       0);

    typedef boost::random::discrete_distribution<Int, Scalar> CategoricalDist;
    CategoricalDist dist(weights_.begin(), weights_.end());
    typedef boost::variate_generator<Rng::GenType&, CategoricalDist>
        CategoricalGen;
    CategoricalGen gen(rng.GetGen(), dist);
    for (Size i = 0; i < n_multinomial; ++i)
      count_[gen()]++;
  }

  void StratifiedResampler::resample(Scalar sumOfWeights, Rng & rng)
  {
    typedef boost::uniform_real<Scalar> UniformDist;
    Scalar weight_mean = sumOfWeights / nParticles_;
    UniformDist dist(0.0, weight_mean);
    typedef boost::variate_generator<Rng::GenType&, UniformDist> UniformGen;
    UniformGen gen(rng.GetGen(), dist);

    Scalar rand_unif = gen();
    Scalar weight_cumul = 0.0;
    Size count_cumul = 0;

    for (Size i = 0; count_cumul < nParticles_; ++i)
    {
      weight_cumul += weights_[i];
      while (rand_unif < weight_cumul && count_cumul < nParticles_)
      {
        ++count_[i];
        ++count_cumul;
        weight_cumul -= weight_mean;
        rand_unif = gen();
      }
    }

  }

  void SystematicResampler::resample(Scalar sumOfWeights, Rng & rng)
  {
    typedef boost::uniform_real<Scalar> UniformDist;
    Scalar weight_mean = sumOfWeights / nParticles_;
    UniformDist dist(0.0, weight_mean);
    typedef boost::variate_generator<Rng::GenType&, UniformDist> UniformGen;
    UniformGen gen(rng.GetGen(), dist);

    Scalar rand_unif = gen(); // the same sample is used for each stratum
    Scalar weight_cumul = 0.0;
    Size count_cumul = 0;

    for (Size i = 0; count_cumul < nParticles_; ++i)
    {
      weight_cumul += weights_[i];
      while (rand_unif < weight_cumul && count_cumul < nParticles_)
      {
        ++count_[i];
        ++count_cumul;
        weight_cumul -= weight_mean;
      }
    }
  }

  ResamplerTable::ResamplerTable()
  {
    Insert(MultinomialResampler::Instance());
    Insert(ResidualResampler::Instance());
    Insert(StratifiedResampler::Instance());
    Insert(SystematicResampler::Instance());
  }

}
