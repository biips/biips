//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file MatlabFunction.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "MatlabFunction.hpp"
#include <algorithm>
#include "mex.h"

namespace Biips
{
  void MatlabFunction::eval(ValArray & values,
                            const NumArray::Array & paramValues) const
  {
    // declare input
    int nrhs = paramValues.size();
    mxArray ** prhs = (mxArray **) mxCalloc(nrhs, sizeof(mxArray *));

    for (Size i = 0; i < nrhs; ++i)
    {
      
      std::vector<mwSize> dims(paramValues[i].DimPtr()->begin(),
                               paramValues[i].DimPtr()->end());
      if (paramValues[i].NDim() > 1)  
          prhs[i] = mxCreateNumericArray(paramValues[i].NDim(), dims.data() , mxDOUBLE_CLASS, mxREAL);
      else
          prhs[i] = mxCreateDoubleMatrix(dims[0], 1, mxREAL);
   	 
      // make array values point to existing memory
      std::copy(paramValues[i].ValuesPtr()->begin(),
                paramValues[i].ValuesPtr()->end(), 
		mxGetPr(prhs[i]));  
    }

    // declare output
    mxArray * plhs[1];

    // call matlab function
    #ifndef OCTAVE
    mxArray * exception = NULL;
    exception = mexCallMATLABWithTrap(1, plhs, nrhs, prhs, fun_eval_.c_str());
    if (exception != NULL)
    {
      mexCallMATLAB(0, (mxArray **) NULL, 1, &exception, "throw");
    }
    #else
    mexCallMATLAB(1, plhs, nrhs, prhs, fun_eval_.c_str());
    #endif

    // get output
    mwSize size_lhs = mxGetNumberOfElements(plhs[0]);
    if (size_lhs != values.size())
    {
      String msg =
          String("Error evaluating MATLAB function \"") + fun_eval_
          + "\" : output number of elements does not match the node dimension.";
      mexErrMsgTxt(msg.c_str());
    }
    
    // free allocated input
    for (Size i=0; i<nrhs; ++i)
      mxDestroyArray(prhs[i]);

    // copy output
    for (Size i = 0; i < values.size(); ++i)
      values[i] = mxGetPr(plhs[0])[i];

    // free allocated output
    mxDestroyArray(plhs[0]);
    mxFree(prhs);
  }

  DimArray MatlabFunction::dim(
      const std::vector<DimArray::Ptr> & paramDims) const
  {
    // declare input
    int nrhs = paramDims.size();
    mxArray ** prhs = (mxArray **) mxCalloc(nrhs, sizeof(mxArray *)); 

    for (Size i = 0; i < nrhs; ++i)
    {
      // allocate input
      prhs[i] = mxCreateDoubleMatrix(1, paramDims[i]->NDim(), mxREAL);

      // fill values
      std::copy(paramDims[i]->begin(), paramDims[i]->end(), mxGetPr(prhs[i]));
    }

    // declare output
    mxArray * plhs[1];

    // call matlab function
    #ifndef OCTAVE
    mxArray * exception = NULL;
    exception = mexCallMATLABWithTrap(1, plhs, nrhs, prhs, fun_dim_.c_str());

    // catch exception
    if (exception != NULL)
    {
      mexCallMATLAB(0, (mxArray **) NULL, 1, &exception, "throw");
    }
    #else
    mexCallMATLAB(1, plhs, nrhs, prhs, fun_dim_.c_str());
    #endif
    
    // get output
    mwSize size_lhs = mxGetNumberOfElements(plhs[0]);
    double * lhs = mxGetPr(plhs[0]);

    // free allocated input
    for (Size i=0; i<nrhs; ++i)
      mxDestroyArray(prhs[i]);
    
    mxFree(prhs);
    
    return DimArray(lhs, lhs + size_lhs);
  }

  Bool MatlabFunction::IsDiscreteValued(const Flags & mask) const
  {
    // default implementation
    if (fun_is_discrete_.empty())
      return false;

    // allocate input
    mxArray * prhs[1];
    prhs[0] = mxCreateLogicalMatrix(1, mask.size());

    // fill values
    std::copy(mask.begin(), mask.end(), mxGetPr(prhs[0]));

    // declare output
    mxArray * plhs[1];

    // call matlab function
    #ifndef OCTAVE
    mxArray * exception = NULL;
    exception = mexCallMATLABWithTrap(1, plhs, 1, prhs,
                                      fun_is_discrete_.c_str());

    // catch exception
    if (exception != NULL)
    {
      /* Throw the MException returned by mexCallMATLABWithTrap
       * after cleaning up any dynamically allocated resources */
      mexCallMATLAB(0, (mxArray **) NULL, 1, &exception, "throw");
    }
    #else
    mexCallMATLAB(1, plhs, 1, prhs, fun_is_discrete_.c_str());
    #endif
    mxDestroyArray(prhs[0]);
    // check output size
    mwSize size_lhs = mxGetNumberOfElements(plhs[0]);
    if (size_lhs != 1)
    {
      String msg = String("Error evaluating MATLAB function \"")
                   + fun_is_discrete_ + "\" : output must return a scalar.";
      mexErrMsgTxt(msg.c_str());
    }

    // get output
    Bool ans = *mxGetLogicals(plhs[0]);

    // free allocated input and output
    mxDestroyArray(prhs[0]);
    mxDestroyArray(plhs[0]);

    return ans;
  }

  Bool MatlabFunction::CheckParamValues(
      const NumArray::Array & paramValues) const
  {
    // default implementation
    if (fun_check_param_.empty())
      return true;

    // copy input
    int nrhs = paramValues.size();
    mxArray ** prhs = (mxArray **)mxCalloc(nrhs, sizeof(mxArray *));

    for (Size i = 0; i < nrhs; ++i)
    {
      // copy vector<unsigned int> to vector<mwSize>
      std::vector<mwSize> dims(paramValues[i].DimPtr()->begin(),
                               paramValues[i].DimPtr()->end());
      if (paramValues[i].NDim() > 1)  
          prhs[i] = mxCreateNumericArray(paramValues[i].NDim(), dims.data() , mxDOUBLE_CLASS, mxREAL);
      else
          prhs[i] = mxCreateDoubleMatrix(dims[0],1, mxREAL);
     
      // make array values point to existing memory
      std::copy(paramValues[i].ValuesPtr()->begin(),
                paramValues[i].ValuesPtr()->end(), 
		mxGetPr(prhs[i]));  
    }

    // declare output
    mxArray * plhs[1];

    #ifndef OCTAVE 
    // call matlab function
    mxArray * exception = NULL;
    exception = mexCallMATLABWithTrap(1, plhs, nrhs, prhs,
                                      fun_check_param_.c_str());
   // catch exception
    if (exception != NULL)
    {
      mexCallMATLAB(0, (mxArray **) NULL, 1, &exception, "throw");
    }
    #else
    mexCallMATLAB(1, plhs, nrhs, prhs, fun_check_param_.c_str());
    #endif
    
    
    // free allocated input
    for (Size i=0; i<nrhs; ++i)
      mxDestroyArray(prhs[i]);
  
    mxFree(prhs);
    // check output size
    mwSize size_lhs = mxGetNumberOfElements(plhs[0]);
    if (size_lhs != 1)
    {
      String msg = String("Error evaluating MATLAB function \"")
                   + fun_check_param_ + "\" : output must return a scalar.";
      mexErrMsgTxt(msg.c_str());
    }

    // get output
    Bool ans = *mxGetLogicals(plhs[0]);

    // free allocated output
    mxDestroyArray(plhs[0]);

    return ans;
  }
}
