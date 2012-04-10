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

/*! \file KalmanFilter.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "KalmanFilter.hpp"

#include "common/cholesky.hpp"
//#include <boost/numeric/ublas/operation.hpp>

namespace Biips
{

  static Vector toVector(const MultiArray & marray)
  {
    return Vector(NumArray(marray.DimPtr().get(), marray.ValuesPtr().get()));
  }

  static Matrix toMatrix(const MultiArray & marray)
  {
    return Matrix(NumArray(marray.DimPtr().get(), marray.ValuesPtr().get()));
  }

  void KalmanFilter::updateCore() // TODO optimize (using effective uBlas functions)
  {
    if (stateType_ == UPDATED)
    {
      //Prediction
      xtPred_ = ublas::prod(Ft_, xtminus1_) + ublas::prod(Bt_, ut_);
      PtPred_ = ublas::prod(Ft_, Matrix(ublas::prod(Ptminus1_,
                                                    ublas::trans(Ft_)))) + Qt_;
    }

    //Update
    ztPred_ = ublas::prod(Ht_, xtPred_);

    Matrix temp(ublas::prod(PtPred_, ublas::trans(Ht_)));
    StPred_ = ublas::prod(Ht_, temp) + Rt_;

    if (!ublas::cholesky_factorize(StPred_))
      throw LogicError("KalmanFilter::updateCore: matrix StPred_ is not positive-semidefinite.");
    ublas::cholesky_invert(StPred_);

    Kt_ = ublas::prod(temp, StPred_);

    xt_ = ublas::prod(Kt_, zt_ - ztPred_) + xtPred_;

    Pt_ = ublas::prod(ublas::identity_matrix<Scalar>(dimx_)
        - Matrix(ublas::prod(Kt_, Ht_)), PtPred_);

    xtminus1_ = xt_;
    Ptminus1_ = Pt_;

    stateType_ = UPDATED;
    ++t_;
  }

  void KalmanFilter::Init(const MultiArray & xtminus1,
                          const MultiArray & Ptminus1,
                          StateType stateType)
  {
    t_ = 0;
    stateType_ = stateType;
    if (stateType_ == UPDATED)
    {
      xtminus1_ = toVector(xtminus1);
      Ptminus1_ = toMatrix(Ptminus1);
    }
    else
    {
      xtPred_ = toVector(xtminus1);
      PtPred_ = toMatrix(Ptminus1);
    }
  }

  void KalmanFilter::SetEvolutionModel(const MultiArray & Ft,
                                       const MultiArray & Bt,
                                       const MultiArray & Qt)
  {
    Ft_ = toMatrix(Ft);
    Bt_ = toMatrix(Bt);
    Qt_ = toMatrix(Qt);
  }

  void KalmanFilter::SetObservationModel(const MultiArray & Ht,
                                         const MultiArray & Rt)
  {
    Ht_ = toMatrix(Ht);
    Rt_ = toMatrix(Rt);
  }

  void KalmanFilter::Init(Scalar xtminus1, Scalar Ptminus1, StateType stateType)
  {
    t_ = 0;
    stateType_ = stateType;
    if (stateType_ == UPDATED)
    {
      xtminus1_ = Vector(1, xtminus1);
      Ptminus1_ = Matrix(1, 1, Ptminus1);
    }
    else
    {
      xtPred_ = Vector(1, xtminus1);
      PtPred_ = Matrix(1, 1, Ptminus1);
    }
  }

  void KalmanFilter::SetEvolutionModel(Scalar Ft, Scalar Bt, Scalar Qt)
  {
    Ft_ = Matrix(1, 1, Ft);
    Bt_ = Matrix(1, 1, Bt);
    Qt_ = Matrix(1, 1, Qt);
  }

  void KalmanFilter::SetObservationModel(Scalar Ht, Scalar Rt)
  {
    Ht_ = Matrix(1, 1, Ht);
    Rt_ = Matrix(1, 1, Rt);
  }

  void KalmanFilter::Update(const MultiArray & zt)
  {
    zt_ = toVector(zt);
    updateCore();
  }
  void KalmanFilter::Update(const MultiArray & zt, const MultiArray & ut)
  {
    ut_ = toVector(ut);
    Update(zt);
  }

  void KalmanFilter::Update(Scalar zt)
  {
    zt_ = Vector(1, zt);
    updateCore();
  }

  void KalmanFilter::Update(Scalar zt, Scalar ut)
  {
    ut_ = Vector(1, ut);
    Update(zt);
  }

  KalmanFilter::KalmanFilter(const MultiArray & xtminus1,
                             const MultiArray & Ptminus1,
                             const MultiArray & Rt,
                             StateType stateType) :
    t_(0), dimx_(xtminus1.Length()), dimz_(Rt.Dim()[0]), xtminus1_(dimx_),
        Ptminus1_(dimx_, dimx_), Ft_(ublas::identity_matrix<Scalar>(dimx_)),
        ut_(1, 0.0), Bt_(dimx_, 1, 0.0), Qt_(toMatrix(Ptminus1)), zt_(dimz_),
        Ht_(ublas::identity_matrix<Scalar>(dimx_)), Rt_(toMatrix(Rt)),
        xtPred_(dimx_), PtPred_(dimx_, 1), ztPred_(dimz_),
        StPred_(dimz_, dimz_), Kt_(dimx_, dimz_), xt_(dimx_),
        Pt_(dimx_, dimx_), stateType_(stateType)
  {
    if (stateType_ == UPDATED)
    {
      xtminus1_ = toVector(xtminus1);
      Ptminus1_ = toMatrix(Ptminus1);
    }
    else
    {
      xtPred_ = toVector(xtminus1);
      PtPred_ = toMatrix(Ptminus1);
    }
  }
  ; // TODO check values


  KalmanFilter::KalmanFilter(Scalar xtminus1,
                             Scalar Ptminus1,
                             Scalar Rt,
                             StateType stateType) :
    t_(0), dimx_(1), dimz_(1), xtminus1_(1), Ptminus1_(1, 1), Ft_(1, 1, 1.0),
        ut_(1, 0.0), Bt_(1, 1, 0.0), Qt_(1, 1, Ptminus1), zt_(1),
        Ht_(1, 1, 1.0), Rt_(1, 1, Rt), xtPred_(1), PtPred_(1, 1), ztPred_(1),
        StPred_(1, 1), Kt_(1, 1), xt_(1), Pt_(1, 1), stateType_(stateType)
  {
    if (stateType_ == UPDATED)
    {
      xtminus1_ = Vector(1, xtminus1);
      Ptminus1_ = Matrix(1, 1, Ptminus1);
    }
    else
    {
      xtPred_ = Vector(1, xtminus1);
      PtPred_ = Matrix(1, 1, Ptminus1);
    }
  }
  ; // TODO check values

}
