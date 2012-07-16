#include <deque>
#include "inter_utils.h"

typedef Console * Console_ptr;
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

       if (nrhs < 2) 
         mexErrMsgTxt("clear_console :  must have one integer argument");
       
       int console_id = static_cast<int>(*mxGetPr(prhs[1]));
       
       // the console_id does not exist in the console map
       CheckConsoleId(console_id);
       
       delete consoles[console_id];
       consoles[console_id] = NULL;
       
    }       
    /////////////////////////////////////////
    // CHECK_MODEL FUNCTION
    /////////////////////////////////////////
    else if (name_func == "check_model") {

       if (nrhs < 3) {
         mexErrMsgTxt("check_model: must have two arguments");
       }
       
       int console_id = static_cast<int>(*mxGetPr(prhs[1]));
       char * filename = mxArrayToString(prhs[2]);
       
       CheckConsoleId(console_id);
       
       Console * p_console = consoles[console_id];
 
       mbiips_cout << PROMPT_STRING << "Parsing model in: " << filename << endl;
       if (! p_console->CheckModel(String(filename), true)) 
                 mexErrMsgTxt("Model syntax is incorrect.");
       mxFree(filename);
    }       
    /////////////////////////////////////////
    // COMPILE_MODEL FUNCTION
    /////////////////////////////////////////
    else if (name_func == "compile_model") {

       if (nrhs < 5) {
         mexErrMsgTxt("compile_model: must have 4 arguments");
       }
       int id = static_cast<int>(*mxGetPr(prhs[1]));
       CheckConsoleId(id);
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

       if (nrhs < 2) {
         mexErrMsgTxt("get_data: must have 1 arguments");
       }
       int id = static_cast<int>(*mxGetPr(prhs[1]));
       CheckConsoleId(id);
       Console * p_console = consoles[id];
       
       std::map<String, MultiArray> data_table;

       if (! p_console->DumpData(data_table))
         throw RuntimeError("Failed to read data.");

       //
       readDataTable<MultiArray::StorageOrderType>(data_table, &plhs[0]);
        
    }      
    else {
       mexErrMsgTxt("bad name of function\n");

    }
    /* set C-style string output_buf to MATLAB mexFunction output*/
    mxFree(input_buf);
    return;

}

