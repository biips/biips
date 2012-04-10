//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
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

/*! \file KalmanFilter.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_KALMANFILTER_HPP_
#define BIIPS_KALMANFILTER_HPP_

#include "common/Common.hpp"

namespace Biips
{

  class KalmanFilter
  {
  public:
    enum StateType
    {
      PREDICTED, UPDATED
    };

  protected:
    Size t_;
    Size dimx_;
    Size dimz_;
    Vector xtminus1_; // previous state estimate
    Matrix Ptminus1_; // previous estimate error covariance matrix
    Matrix Ft_; // state transition matrix applied to the previous state
    Vector ut_; // control vector
    Matrix Bt_; // control-input model matrix applied to the control vector
    Matrix Qt_; // covariance matrix of the process noise
    Vector zt_; // observation vector
    Matrix Ht_; // observation model matrix
    Matrix Rt_; // covariance matrix of the observation noise

    Vector xtPred_; // predicted (prior) state estimate
    Matrix PtPred_; // predicted (prior) estimate error covariance matrix

    Vector ztPred_; // predicted measure
    Matrix StPred_; // predicted innovation (or measurement residual) covariance matrix
    Matrix Kt_; // optimal Kalman Gain

    Vector xt_; // posterior state estimate
    Matrix Pt_; // posterior estimate error covariance matrix

    StateType stateType_;

    void updateCore();

  public:
    void Init(const MultiArray & xtminus1,
              const MultiArray & Ptminus1,
              StateType stateType = UPDATED);

    void SetEvolutionModel(const MultiArray & Ft,
                           const MultiArray & Bt,
                           const MultiArray & Qt);
    void SetObservationModel(const MultiArray & Ht, const MultiArray & Rt);

    void Init(Scalar xtminus1, Scalar Ptminus1, StateType stateType = UPDATED);

    void SetEvolutionModel(Scalar Ft, Scalar Bt, Scalar Qt);
    void SetObservationModel(Scalar Ht, Scalar Rt);

    void Update(const MultiArray & zt);
    void Update(const MultiArray & zt, const MultiArray & ut);

    void Update(Scalar zt);
    void Update(Scalar zt, Scalar ut);

    MultiArray GetPriorEstimate() const
    {
      DimArray::Ptr p_dim(new DimArray(1, xtPred_.size()));
      ValArray::Ptr p_val(new ValArray(xtPred_.data()));
      return MultiArray(p_dim, p_val);
    }
    MultiArray GetPriorCovariance() const
    {
      DimArray::Ptr p_dim(new DimArray(2, PtPred_.size1()));
      ValArray::Ptr p_val(new ValArray(PtPred_.data()));
      return MultiArray(p_dim, p_val);
    }
    MultiArray GetPosteriorEstimate() const
    {
      DimArray::Ptr p_dim(new DimArray(1, xt_.size()));
      ValArray::Ptr p_val(new ValArray(xt_.data()));
      return MultiArray(p_dim, p_val);
    }
    MultiArray GetPosteriorCovariance() const
    {
      DimArray::Ptr p_dim(new DimArray(2, Pt_.size1()));
      ValArray::Ptr p_val(new ValArray(Pt_.data()));
      return MultiArray(p_dim, p_val);
    }

    KalmanFilter(const MultiArray & xtminus1,
                 const MultiArray & Ptminus1,
                 const MultiArray & Rt,
                 StateType stateType = UPDATED);

    KalmanFilter(Scalar xtminus1,
                 Scalar Ptminus1,
                 Scalar Rt,
                 StateType stateType = UPDATED);
  };

}
#endif /* BIIPS_KALMANFILTER_HPP_ */
