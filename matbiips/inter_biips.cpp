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
       
       CheckArgIsStruct(2);

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
       if (!std::strcmp(mod_name,"basemod")) {
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

       CheckRhs(nrhs, 4, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
        
       if (VERBOSITY>1)
          mbiips_cout << PROMPT_STRING << "Filter monitoring variables:";
       
       mwSize nbVarNames = mxGetNumberOfElements(prhs[2]);

       if ((nbVarNames != mxGetNumberOfElements(prhs[3])) || (nbVarNames != mxGetNumberOfElements(prhs[4])))
            mbiips_cerr << name_func <<  ": arguments 2, 3 and 4 must have the same number of Elements" << endl;
   
       CheckArgIsCell(2);
       CheckArgIsCell(3);
       CheckArgIsCell(4);

       for(int i = 0; i <  nbVarNames; ++i )
        {
          const mxArray * m   = mxGetCell(prhs[2], i);
          const mxArray * low = mxGetCell(prhs[3], i);
          const mxArray * up  = mxGetCell(prhs[4], i);
	  
	  CheckIsString(m);
	  String name = mxArrayToString(m);
          
          IndexRange range = makeRange(low, up);
          Bool ok = p_console->SetFilterMonitor(name, range);
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
    
    /////////////////////////////////////////
    // SET_SMOOTH_TREE_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "set_smooth_tree_monitors") {

       CheckRhs(nrhs, 4, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
        
       if (VERBOSITY>1)
          mbiips_cout << PROMPT_STRING << "Smooth tree monitoring variables:";
       
       mwSize nbVarNames = mxGetNumberOfElements(prhs[2]);

       if ((nbVarNames != mxGetNumberOfElements(prhs[3])) || (nbVarNames != mxGetNumberOfElements(prhs[4])))
            mbiips_cerr << name_func <<  ": arguments 2, 3 and 4 must have the same number of Elements" << endl;
   
       CheckArgIsCell(2);
       CheckArgIsCell(3);
       CheckArgIsCell(4);

       for(int i = 0; i <  nbVarNames; ++i )
        {
          const mxArray * m   = mxGetCell(prhs[2], i);
          const mxArray * low = mxGetCell(prhs[3], i);
          const mxArray * up  = mxGetCell(prhs[4], i);
	  
	  CheckIsString(m);
	  String name = mxArrayToString(m);
          
          IndexRange range = makeRange(low, up);
          Bool ok = p_console->SetSmoothTreeMonitor(name, range);
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
    
    /////////////////////////////////////////
    // SET_SMOOTH_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "set_smooth_monitors") {

       CheckRhs(nrhs, 4, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
        
       if (VERBOSITY>1)
          mbiips_cout << PROMPT_STRING << "Smoother monitoring variables:";
       
       mwSize nbVarNames = mxGetNumberOfElements(prhs[2]);

       if ((nbVarNames != mxGetNumberOfElements(prhs[3])) || (nbVarNames != mxGetNumberOfElements(prhs[4])))
            mbiips_cerr << name_func <<  ": arguments 2, 3 and 4 must have the same number of Elements" << endl;
   
       CheckArgIsCell(2);
       CheckArgIsCell(3);
       CheckArgIsCell(4);

       for(int i = 0; i <  nbVarNames; ++i )
        {
          const mxArray * m   = mxGetCell(prhs[2], i);
          const mxArray * low = mxGetCell(prhs[3], i);
          const mxArray * up  = mxGetCell(prhs[4], i);
	  
	  CheckIsString(m);
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
    
    /////////////////////////////////////////
    // BUILD_SMC_SAMPLER FUNCTION
    /////////////////////////////////////////
    else if (name_func == "build_smc_sampler") {

       CheckRhs(nrhs, 2, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
       
       CheckArgIsNumeric(2);
       Bool prior_flag = static_cast<Bool>(*mxGetPr(prhs[2]));

       if (!p_console->BuildSampler(prior_flag, VERBOSITY))
         throw RuntimeError("Failed to build sampler.");
    }
    
    /////////////////////////////////////////
    // IS_SAMPLER_BUILT FUNCTION
    /////////////////////////////////////////
    else if (name_func == "is_sampler_built") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
       
       plhs[0] = mxCreateLogicalMatrix(1, 1);
       *mxGetLogicals(plhs[0]) =  (p_console->SamplerBuilt() ? 1 : 0);

    }
    
    /////////////////////////////////////////
    // RUN_SMC_SAMPLER FUNCTION
    /////////////////////////////////////////
    else if (name_func == "run_smc_sampler") {

       CheckRhs(nrhs, 5, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];

       CheckArgIsNumeric(2);
       Size n_part = static_cast<Size>(*mxGetPr(prhs[2]));

       CheckArgIsNumeric(3);
       Size smc_rng_seed = static_cast<Size>(*mxGetPr(prhs[3]));

       
       CheckArgIsNumeric(4);
       Scalar  ess_threshold = static_cast<Scalar>(*mxGetPr(prhs[4]));
       
       CheckArgIsString(5);
       String resample_type = mxArrayToString(prhs[5]);

       Bool ok = p_console->RunForwardSampler(n_part, smc_rng_seed, resample_type, ess_threshold, VERBOSITY, VERBOSITY);

       plhs[0] = mxCreateLogicalMatrix(1, 1);
       *mxGetLogicals(plhs[0]) =  (ok ? 1 : 0);
    }
    
    
    /////////////////////////////////////////
    // GET_LOG_NORM_CONST FUNCTION
    /////////////////////////////////////////
    else if (name_func == "get_log_norm_const") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
       
       Scalar log_norm_const;
       if(!p_console->GetLogNormConst(log_norm_const))
          throw RuntimeError("Failed to get log normalizing constant.");

       plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
       *mxGetPr(plhs[0]) = static_cast<double>(log_norm_const);

    }
    
    /////////////////////////////////////////
    // GET_FILTER_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "get_filter_monitors") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
  
       std::map<String, NodeArrayMonitor> monitors_map;

       if (!p_console->DumpFilterMonitors(monitors_map))
         throw RuntimeError("Failed to dump filter monitors.");
       
       getMonitors<ColumnMajorOrder>(monitors_map, "filtering", &plhs[0]);

    }
    
    /////////////////////////////////////////
    // GET_SMOOTH_TREE_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "get_smooth_tree_monitors") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
  
       std::map<String, NodeArrayMonitor> monitors_map;

       if (!p_console->DumpSmoothTreeMonitors(monitors_map))
         throw RuntimeError("Failed to dump smooth tree monitors.");
       
       getMonitors<ColumnMajorOrder>(monitors_map, "smoothing", &plhs[0]);

    }
    
    /////////////////////////////////////////
    // GET_SMOOTH_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "get_smooth_monitors") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
  
       std::map<String, NodeArrayMonitor> monitors_map;

       if (!p_console->DumpSmoothTreeMonitors(monitors_map))
         throw RuntimeError("Failed to dump smooth monitors.");
       
       getMonitors<ColumnMajorOrder>(monitors_map, "backward.smoothing", &plhs[0]);

    }
    
    /////////////////////////////////////////
    // CLEAR_FILTER_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "clear_filter_monitors") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
  
       if (!p_console->ClearFilterMonitors())
         throw RuntimeError("Failed to clear filter monitors.");

    }
    
    /////////////////////////////////////////
    // CLEAR_SMOOTH_TREE_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "clear_smooth_tree_monitors") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
  
       if (!p_console->ClearSmoothTreeMonitors())
         throw RuntimeError("Failed to clear smooth tree monitors.");

    }
    
    /////////////////////////////////////////
    // CLEAR_SMOOTH_MONITORS FUNCTION
    /////////////////////////////////////////
    else if (name_func == "clear_smooth_monitors") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
  
       if (!p_console->ClearSmoothTreeMonitors())
         throw RuntimeError("Failed to clear smooth monitors.");

    }
    
    /////////////////////////////////////////
    // RUN_BACKWARD_SMOOTHER FUNCTION
    /////////////////////////////////////////
    else if (name_func == "run_backward_smoother") {

       CheckRhs(nrhs, 1, name_func);
       Size id = GetConsoleId(consoles, prhs[1], name_func);
       Console * p_console = consoles[id];
  
       if (!p_console->RunBackwardSmoother(VERBOSITY,VERBOSITY))
         throw RuntimeError("Failed to run backward smoother.");

    }
    
    else {
       mexErrMsgTxt("bad name of function\n");

    }
    /* set C-style string output_buf to MATLAB mexFunction output*/
    mxFree(input_buf);
    return;

}

