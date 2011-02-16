//                                               -*- C++ -*-
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
      PREDICTED,
      UPDATED
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
    void Init(const DataType & xtminus1, const DataType & Ptminus1, StateType stateType = UPDATED);

    void SetEvolutionModel(const DataType & Ft, const DataType & Bt, const DataType & Qt);
    void SetObservationModel(const DataType & Ht, const DataType & Rt);

    void Init(Scalar xtminus1, Scalar Ptminus1, StateType stateType = UPDATED);

    void SetEvolutionModel(Scalar Ft, Scalar Bt, Scalar Qt);
    void SetObservationModel(Scalar Ht, Scalar Rt);

    void Update(const DataType & zt);
    void Update(const DataType & zt, const DataType & ut);

    void Update(Scalar zt);
    void Update(Scalar zt, Scalar ut);

    DataType GetPriorEstimate() const { return DataType(xtPred_); }
    DataType GetPriorCovariance() const { return DataType(PtPred_); }
    DataType GetPosteriorEstimate() const { return DataType(xt_); }
    DataType GetPosteriorCovariance() const { return DataType(Pt_); }

    KalmanFilter(const DataType & xtminus1, const DataType & Ptminus1, const DataType & Rt, StateType stateType = UPDATED);

    KalmanFilter(Scalar xtminus1, Scalar Ptminus1, Scalar Rt, StateType stateType = UPDATED);
  };

}
#endif /* BIIPS_KALMANFILTER_HPP_ */
