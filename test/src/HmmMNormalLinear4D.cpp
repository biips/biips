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

/*! \file HmmMNormalLinear4D.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "HmmMNormalLinear4D.hpp"

#include "common/cholesky.hpp"

namespace Biips
{

  const String HmmMNormalLinear4D::NAME_ = "HMM multivariate Normal linear 4D";

  HmmMNormalLinear4D::HmmMNormalLinear4D(int argc,
                                         char** argv,
                                         Size verbose,
                                         Size showMode,
                                         Bool precFlag,
                                         std::ostream & os) :
    BaseType(argc, argv, verbose, showMode, precFlag_, os)
  {
    setName(NAME_);

    // Default model parameters
    sizeParamMap_["t.max"] = 20;

    Scalar T = 1.0;

    dimArrayMap_["x"] = DimArray::Ptr(new DimArray(1, 4));
    ;
    dimArrayMap_["u"] = DimArray::Ptr(new DimArray(1, 1));
    ;
    dimArrayMap_["y"] = DimArray::Ptr(new DimArray(1, 2));
    ;

    Size dim_x = (*dimArrayMap_["x"])[0];
    Size dim_u = (*dimArrayMap_["u"])[0];
    Size dim_y = (*dimArrayMap_["y"])[0];

    dimArrayMap_["P.0"] = DimArray::Ptr(new DimArray(2, dim_x));
    dimArrayMap_["F"] = DimArray::Ptr(new DimArray(2, dim_x));
    dimArrayMap_["B"] = DimArray::Ptr(new DimArray(2));
    (*dimArrayMap_["B"])[0] = dim_x;
    (*dimArrayMap_["B"])[1] = dim_u;
    dimArrayMap_["Q"] = DimArray::Ptr(new DimArray(2, dim_x));
    dimArrayMap_["H"] = DimArray::Ptr(new DimArray(2));
    (*dimArrayMap_["H"])[0] = dim_y;
    (*dimArrayMap_["H"])[1] = dim_x;
    dimArrayMap_["R"] = DimArray::Ptr(new DimArray(2, dim_y));

    dataTypeParamMap_["mean.x.0"].SetPtr(dimArrayMap_["x"],
                                         ValArray::Ptr(new ValArray(dimArrayMap_["x"]->Length(),
                                                                    0.0)));

    dataTypeParamMap_["P.0"].SetPtr(dimArrayMap_["P.0"],
                                    ValArray::Ptr(new ValArray(dimArrayMap_["P.0"]->Length(),
                                                               0.0)));
    typedef ublas::matrix<Scalar, StorageOrder, ValArray> MatrixType;
    MatrixType P0_mat(dataTypeParamMap_["P.0"].Dim()[0],
                      dataTypeParamMap_["P.0"].Dim()[1],
                      ValArray());
    P0_mat.data().swap(dataTypeParamMap_["P.0"].Values());
    for (Size i = 0; i < dim_x; ++i)
      P0_mat(i, i) = 1.0;

    if (precFlag_)
    {
      if (!ublas::cholesky_factorize(P0_mat))
        throw RuntimeError("HmmMNormalLinear4D: matrix P0 is not positive-semidefinite.");
      ublas::cholesky_invert(P0_mat);
    }
    P0_mat.data().swap(dataTypeParamMap_["P.0"].Values());

    dataTypeParamMap_["F"].SetPtr(dimArrayMap_["F"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["F"]->Length(),
                                                             0.0)));
    MatrixType F_mat(dataTypeParamMap_["F"].Dim()[0],
                     dataTypeParamMap_["F"].Dim()[1],
                     ValArray());
    F_mat.data().swap(dataTypeParamMap_["F"].Values());
    for (Size i = 0; i < dim_x; ++i)
      F_mat(i, i) = 1.0;
    F_mat(0, 2) = T;
    F_mat(1, 3) = T;
    F_mat.data().swap(dataTypeParamMap_["F"].Values());

    dataTypeParamMap_["B"].SetPtr(dimArrayMap_["B"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["B"]->Length(),
                                                             0.0)));

    Matrix G_mat(dim_x, 2, 0.0);
    G_mat(0, 0) = T * T / 2;
    G_mat(1, 1) = T * T / 2;
    G_mat(2, 0) = T;
    G_mat(3, 1) = T;
    dataTypeParamMap_["Q"].SetPtr(dimArrayMap_["Q"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["Q"]->Length(),
                                                             0.0)));
    MatrixType Q_mat(dataTypeParamMap_["Q"].Dim()[0],
                     dataTypeParamMap_["Q"].Dim()[1],
                     ValArray());
    Q_mat.data().swap(dataTypeParamMap_["Q"].Values());
    Q_mat = ublas::prod(G_mat, ublas::trans(G_mat));

    if (precFlag_)
    {
      if (!ublas::cholesky_factorize(Q_mat))
        throw RuntimeError("HmmMNormalLinear4D: matrix Q is not positive-semidefinite.");
      ublas::cholesky_invert(Q_mat);
    }
    Q_mat.data().swap(dataTypeParamMap_["Q"].Values());

    dataTypeParamMap_["H"].SetPtr(dimArrayMap_["H"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["H"]->Length(),
                                                             0.0)));
    MatrixType H_mat(dataTypeParamMap_["H"].Dim()[0],
                     dataTypeParamMap_["H"].Dim()[1],
                     ValArray());
    H_mat.data().swap(dataTypeParamMap_["H"].Values());
    H_mat(0, 0) = 1.0;
    H_mat(1, 1) = 1.0;
    H_mat.data().swap(dataTypeParamMap_["H"].Values());

    dataTypeParamMap_["R"].SetPtr(dimArrayMap_["R"],
                                  ValArray::Ptr(new ValArray(dimArrayMap_["R"]->Length(),
                                                             0.0)));
    MatrixType R_mat(dataTypeParamMap_["R"].Dim()[0],
                     dataTypeParamMap_["R"].Dim()[1],
                     ValArray());
    R_mat.data().swap(dataTypeParamMap_["R"].Values());
    for (Size i = 0; i < dim_y; ++i)
      R_mat(i, i) = 0.5;

    if (precFlag_)
    {
      if (!ublas::cholesky_factorize(R_mat))
        throw RuntimeError("HmmMNormalLinear4D: matrix R is not positive-semidefinite.");
      ublas::cholesky_invert(R_mat);
    }

    R_mat.data().swap(dataTypeParamMap_["R"].Values());

    Size t_max = sizeParamMap_.at("t.max");
    dataValuesMap_["u"].resize(t_max);
    for (Size i = 0; i < t_max; ++i)
    {
      ValArray::Ptr p_val(new ValArray(dimArrayMap_["u"]->Length()));
      dataValuesMap_["u"][i] = MultiArray(dimArrayMap_["u"], p_val);
    }
  }

  void HmmMNormalLinear4D::PrintIntro()
  {
    using namespace std;
    os_ << "BiiPS test: Hidden Markov Model linear gaussian 4D" << endl;
    os_ << "comparison between Kalman Filter and Particle Filter" << endl;
    os_ << "====================================================" << endl;
    os_ << "x[0] --> x[1] --> ... --> x[t-1] --> x[t] --> ..." << endl;
    os_ << "          |                 |         |" << endl;
    os_ << "          v                 v         v" << endl;
    os_ << "         y[1]             y[t-1]     y[t]" << endl;
    os_ << endl;
    os_ << "         x[0] ~ Normal(0_4, I_4)" << endl;
    os_ << "x[t] | x[t-1] ~ Normal(F x[t-1], G G') for all t>0" << endl;
    os_ << "  y[t] | x[t] ~ Normal(H x[t], 0.5 I_2)  for all t>0" << endl;
    os_ << "where" << endl;
    os_ << "F = [1 0 T 0; 0 1 0 T; 0 0 1 0; 0 0 0 1]" << endl;
    os_ << "G = [T^2/2 0; 0 T^2/2; T 0; 0 T]" << endl;
    os_ << "H = [1 0 0 0; 0 1 0 0]" << endl;
    os_ << "0_p correspond to the px1 null vector" << endl;
    os_ << "and I_p to the pxp identity matrix" << endl;
    os_ << endl;
  }

//  void HmmMNormalLinear4D::InputModelParam(std::istream & is)
//  {
//    using namespace std;
//
//    if (verbose_)
//      os_ << "Final time: ";
//    is >> sizeParamMap_["t_max"];
//    if (verbose_)
//      os_ << endl << "t_max = " << sizeParamMap_["t_max"] << endl << endl;
//  }

}
