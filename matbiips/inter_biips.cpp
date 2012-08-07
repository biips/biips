//                                               -*- C++ -*-
/*! \file inter_biips.cpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include <deque>
#include "inter_utils.h"
#include <fstream>
#include "iostream/outStream.hpp"
#include "iostream/ProgressBar.hpp"

std::deque<Console_ptr> consoles;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,const mxArray *prhs[])
{
    
    if (nrhs < 1) {
     mexErrMsgIdAndTxt("inter_biips:Args", "must have at least one argument ");
    }
  
    if ( mxIsChar(prhs[0]) != 1)
      mexErrMsgIdAndTxt( "inter_biips:inputNotString",
              "Input must be a string.");
    
    if (mxGetM(prhs[0])!=1)
      mexErrMsgIdAndTxt( "inter_biips:inputNotVector",
              "Input must be a row vector.");
    
    char * input_buf = mxArrayToString(prhs[0]);
   
    String name_func = String(input_buf);

    /////////////////////////////////////////
    // MAKE_CONSOLE FUNCTION
    /////////////////////////////////////////
    if (name_func == "make_console") {
   
          consoles.push_back(new Console(mbiips_cout, mbiips_cerr));
          plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
          *mxGetPr(plhs[0]) = consoles.size()-1;
    }
   
    /////////////////////////////////////////
    // CLEAR_CONSOLE FUNCTION
    /////////////////////////////////////////
    else if (name_func == "clear_console") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       
       delete consoles[id];
       consoles[id] = NULL;
       
    }       
    /////////////////////////////////////////
    // CHECK_MODEL FUNCTION
    /////////////////////////////////////////
    else if (name_func == "check_model") {

       CheckRhs(nrhs, 2, name_func);
       
       char * filename = mxArrayToString(prhs[2]);
       
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id]; 
 
       mbiips_cout << PROMPT_STRING << "Parsing model in: " << filename << endl;
       if (! p_console->CheckModel(String(filename), VERBOSITY))
                 mexErrMsgTxt("Model syntax is incorrect.");
       mxFree(filename);
    }       
    /////////////////////////////////////////
    // COMPILE_MODEL FUNCTION
    /////////////////////////////////////////
    else if (name_func == "compile_model") {

       CheckRhs(nrhs, 4, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
       
       if (!mxIsStruct(prhs[2])) 
          mexErrMsgTxt("second argument must be a struct");

       std::map<String, MultiArray> data_map = writeDataTable<MultiArray::StorageOrderType>(prhs[2]);

       Bool sample_data = static_cast<Bool>(*mxGetPr(prhs[3]));
       Size data_rng_seed = static_cast<Size>(*mxGetPr(prhs[4]));

       // Compile model
       if (! p_console->Compile(data_map, sample_data, data_rng_seed, VERBOSITY))
         throw RuntimeError("Failed to compile model.");
       if (sample_data && VERBOSITY>1)
         mbiips_cout << INDENT_STRING << "data.rng.seed = " << data_rng_seed << endl;
        
    }      
    /////////////////////////////////////////
    // GET_DATA FUNCTION
    /////////////////////////////////////////
    else if (name_func == "get_data") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
       
       std::map<String, MultiArray> data_table;

       if (! p_console->DumpData(data_table))
         throw RuntimeError("Failed to read data.");

       readDataTable<MultiArray::StorageOrderType>(data_table, &plhs[0]);
        
    }
    /////////////////////////////////////////
    // LOAD MODULE FUNCTION
    /////////////////////////////////////////
    else if (name_func == "load_module") {

       CheckRhs(nrhs, 1, name_func);
       
       char * mod_name = mxArrayToString(prhs[1]);
       plhs[0] = mxCreateLogicalMatrix(1, 1);
       if (!std::strcmp(mod_name, "basemod")) {
         load_base_module();
         *mxGetLogicals(plhs[0]) = 1;
       }
       else {
         *mxGetLogicals(plhs[0]) = 0;
       }
    }      
    
    /////////////////////////////////////////
    // VERBOSITY FUNCTION
    /////////////////////////////////////////
    else if (name_func == "verbosity") {

       CheckRhs(nrhs, 1, name_func);
       Size old_verb = VERBOSITY;
       VERBOSITY = static_cast<Size>(*mxGetPr(prhs[1]));
       plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
       *mxGetPr(plhs[0]) = old_verb;
    }      
    
    /////////////////////////////////////////
    // GET_VARIABLE_NAMES FUNCTION
    /////////////////////////////////////////
    else if (name_func == "get_variable_names") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
       
       const Types<String>::Array & names = p_console->VariableNames();
       mwSize ndim = names.size();
       mwSize dims[] = { ndim };
       plhs[0] = mxCreateCellArray(1, dims);
       
       for (int i = 0; i < ndim ; ++i) {
            String var_name = names[i]; 
	    mwSize  nd[] = { var_name.size() }; 
	    mxArray * value = mxCreateCharArray(1, nd);
	    std::copy(var_name.c_str(), var_name.c_str() + var_name.size(), mxGetChars(value));
	    mxSetCell(plhs[0], i, value);
       }
    }      
    /////////////////////////////////////////
    // SET_DEFAULT_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "set_default_monitors") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
        
       if (VERBOSITY>1)
          mbiips_cout << PROMPT_STRING << "Setting default filter monitors for backward smoothing step" << endl;

       if (!p_console->SetDefaultFilterMonitors())
         throw RuntimeError("Failed to set default filter monitors");

    }
    /////////////////////////////////////////
    // SET_FILTER_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "set_filter_monitors") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
        
       if (VERBOSITY>1)
          mbiips_cout << PROMPT_STRING << "Filter monitoring variables:";
       
       const mxArray * varNames = prhs[2];
      
       const mxArray * monitored_lower  = prhs[3];
       const mxArray * monitored_upper  = prhs[4];
      
       mwSize nbVarNames = mxGetNumberOfElements(varNames);

       if ((nbVarNames != mxGetNumberOfElements(prhs[3])) || (nbVarNames != mxGetNumberOfElements(prhs[4])))
            mexErrMsgTxt("set_filter_monitors  : arguments 2, 3 and 4 must have the same number of Elements");

   
       if (!mxIsCell(varNames))
          mbiips_cerr << "set_filter_monitors  : second argument must be a cell" ;

        for(int i = 0; i <  nbVarNames; ++i )
	 {
           const mxArray * m   = mxGetCell(varNames, i);
           const mxArray * low = mxGetCell(monitored_lower, i);
           const mxArray * up  = mxGetCell(monitored_upper, i);
           String name = mxArrayToString(m);
	   
	   IndexRange range = makeRange(low, up);
           Bool ok = p_console->SetSmoothMonitor(name, range);
           if (ok && VERBOSITY>1)
              {
                  mbiips_cout << " " << name;
                  if (!range.IsNull())
                      mbiips_cout << range;
              }
	 } 
        if (VERBOSITY>1)
            mbiips_cout << endl;
    }
    else {
       mexErrMsgTxt("bad name of function\n");

    }
    /* set C-style string output_buf to MATLAB mexFunction output*/
    mxFree(input_buf);
    return;

}

