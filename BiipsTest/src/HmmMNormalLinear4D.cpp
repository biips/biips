//                                               -*- C++ -*-
/*! \file HmmMNormalLinear4D.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
 */

#include "HmmMNormalLinear4D.hpp"

#include "BiipsBase.hpp"
#include "common/cholesky.hpp"


namespace Biips
{

  const String HmmMNormalLinear4D::NAME_ = "HMM multivariate Normal linear 4D";

  HmmMNormalLinear4D::HmmMNormalLinear4D(int argc, char** argv, Size verbose, Bool showFinal, Bool showAll, Bool precFlag, std::ostream & os)
  : BaseType(argc, argv, verbose, showFinal, showAll, precFlag_, os)
  {
    setName(NAME_);

    // Default model parameters
    sizeParamMap_["t.max"] = 20;

    Scalar T = 1.0;

    dimArrayMap_["x"] = DimArray::Ptr(new DimArray(1,4));;
    dimArrayMap_["u"] = DimArray::Ptr(new DimArray(1,1));;
    dimArrayMap_["y"] = DimArray::Ptr(new DimArray(1,2));;

    Size dim_x = (*dimArrayMap_["x"])[0];
    Size dim_u = (*dimArrayMap_["u"])[0];
    Size dim_y = (*dimArrayMap_["y"])[0];

    dimArrayMap_["P.0"] = DimArray::Ptr(new DimArray(2,dim_x));
    dimArrayMap_["F"] = DimArray::Ptr(new DimArray(2,dim_x));
    dimArrayMap_["B"] = DimArray::Ptr(new DimArray(2));
    (*dimArrayMap_["B"])[0] = dim_x;
    (*dimArrayMap_["B"])[1] = dim_u;
    dimArrayMap_["Q"] = DimArray::Ptr(new DimArray(2,dim_x));
    dimArrayMap_["H"] = DimArray::Ptr(new DimArray(2));
    (*dimArrayMap_["H"])[0] = dim_y;
    (*dimArrayMap_["H"])[1] = dim_x;
    dimArrayMap_["R"] = DimArray::Ptr(new DimArray(2,dim_y));

    dataTypeParamMap_["mean.x.0"].SetPtr(dimArrayMap_["x"]).AllocValue(0.0);

    dataTypeParamMap_["P.0"].SetPtr(dimArrayMap_["P.0"]).AllocValue(0.0);
    MatrixRef P0_mat(dataTypeParamMap_["P.0"]);
    for (Size i=0; i<dim_x; ++i)
      P0_mat(i,i) = 1.0;

    dataTypeParamMap_["F"].SetPtr(dimArrayMap_["F"]).AllocValue(0.0);
    MatrixRef F_mat(dataTypeParamMap_["F"]);
    for (Size i=0; i<dim_x; ++i)
      F_mat(i,i) = 1.0;
    F_mat(0,2) = T;
    F_mat(1,3) = T;

    dataTypeParamMap_["B"].SetPtr(dimArrayMap_["B"]).AllocValue(0.0);

    Matrix G_mat(dim_x, 2, 0.0);
    G_mat(0,0) = T*T/2;
    G_mat(1,1) = T*T/2;
    G_mat(2,0) = T;
    G_mat(3,1) = T;
    dataTypeParamMap_["Q"].SetPtr(dimArrayMap_["Q"]).AllocValue(0.0);
    MatrixRef Q_mat(dataTypeParamMap_["Q"]);
    Q_mat = ublas::prod(G_mat, ublas::trans(G_mat));

    dataTypeParamMap_["H"].SetPtr(dimArrayMap_["H"]).AllocValue(0.0);
    MatrixRef H_mat(dataTypeParamMap_["H"]);
    H_mat(0,0) = 1.0;
    H_mat(1,1) = 1.0;

    dataTypeParamMap_["R"].SetPtr(dimArrayMap_["R"]).AllocValue(0.0);
    MatrixRef R_mat(dataTypeParamMap_["R"]);
    for (Size i=0; i<dim_y; ++i)
      R_mat(i,i) = 0.5;

    if (precFlag_)
    {
      ublas::cholesky_factorize(P0_mat);
      ublas::cholesky_invert(P0_mat);

      ublas::cholesky_factorize(Q_mat);
      ublas::cholesky_invert(Q_mat);

      ublas::cholesky_factorize(R_mat);
      ublas::cholesky_invert(R_mat);
    }

    Size t_max = sizeParamMap_.at("t.max");
    std::vector<DataType> u_vec(t_max, DataType(dimArrayMap_["u"]));
    dataValuesMap_["u"].SetPtr(u_vec.begin(), u_vec.end());

  }


  void HmmMNormalLinear4D::PrintIntro() const
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
