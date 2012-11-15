#include "MatlabFunction.hpp"
#include <algorithm>
#include <mex.h>

namespace Biips {
void
MatlabFunction::eval(ValArray & values, const NumArray::Array & paramValues) {

  int nrhs = paramValues.size();
  mxArray * prhs = new mxArray[nrhs];
  mxArray  plhs[1];

  for(i = 0; i < nrhs; ++i) {
         mwSize zeros[2] = {0, 0 };
         prhs[i] = mxCreateNumericArray(2, zeros, mxDOUBLE_CLASS, mxREAL);
         mxSetDimensions(prhs[i], paramValues[i].DimPtr()->data(), paramValues[i]->NDim());
         mxSetPr(prhs[i], paramValues[i].ValuesPtr()->data());
  }

  mexCallMATLAB(1, plhs, nrhs, prhs, name_);
  
  for(i = 0; i < values.size(); ++i) {
           values[i] = mxGetPr(plhs[1])[i];
  }

  delete [] prhs;
}

DimArray 
MatlabFunction::dim(const Types<DimArray::Ptr>::Array & paramDims) const {

  int nrhs = paramDims.size();
  mxArray * prhs = new mxArray[nrhs];

  for(i = 0; i < nrhs; ++i) {
         mwSize zeros[2] = {0, 0 };
         prhs[i] = mxCreateNumericArray(2, zeros, mxDOUBLE_CLASS, mxREAL);
         mxSetDimensions(prhs[i], paramDims[i]->DimPtr()->data(), paramDims->NDim());
         mxSetPr(prhs[i], paramDims[i]->ValuesPtr()->Data());
  }

  mxArray  plhs[1];
  mexCallMATLAB(1, plhs, nrhs, prhs, fun_dim_);
  
  mwSize size_lhs = mxGetNumberOfElements(plhs[0]);
  double  * lhs = mxGetPr(plhs[0]);
  
  delete [] prhs;
  return DimArray(lhs , lhs + size_lhs);
   
}

virtual Bool 
MatlabFunction::IsDiscreteValued(const Flags & mask) const {

  if (fun_is_discrete_ == "")
     return false;
  
  mxArray prhs[1];
  prhs[0] = mxCreateLogicalMatrix(1, mask.size());
  
  std::copy(prhs[0], prhs[0] + mask.size());
  
  mxArray  plhs[1];
  mexCallMATLAB(1, plhs, 1, prhs, fun_is_discrete_);
  
  mwSize size_lhs = mxGetNumberOfElements(plhs[0]);
  double  * lhs = mxGetPr(plhs[0]);
  
  delete [] prhs;
  return DimArray(lhs , lhs + size_lhs);

}

Bool 
MatlabFunction::CheckParamValues(const NumArray::Array & paramValues) const {
  if (fun_check_param_ == "")
     return true;

  int nrhs = paramValues.size();
  mxArray * prhs = new mxArray[nrhs];

  for(i = 0; i < nrhs; ++i) {
         mwSize zeros[2] = {0, 0 };
         prhs[i] = mxCreateNumericArray(2, zeros, mxDOUBLE_CLASS, mxREAL);
         mxSetDimensions(prhs[i], paramValues[i].DimPtr()->data(), paramValues[i]->NDim());
         mxSetPr(prhs[i], paramValues[i].ValuesPtr()->data());
  }

  mxArray  plhs[1];
  mexCallMATLAB(1, plhs, nrhs, prhs, fun_check_param_);
  
  delete [] prhs;

  return *mxGetLogicals(plhs[0]);

}
}
