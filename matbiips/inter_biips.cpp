#include "mex.h"
#include <string>
#include <Console.hpp>
#include "Mostream.h"

using namespace std;
using namespace Biips;
#define MAX_CONSOLES 10
typedef Console * Console_ptr;
Console_ptr consoles[MAX_CONSOLES];
int nb_consoles = 0;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs,const mxArray *prhs[])
{
    
    if (nrhs < 1) {
     mexErrMsgIdAndTxt("inter_biips:Args", "must have at least one argument ");
    }
  
    if ( mxIsChar(prhs[0]) != 1)
      mexErrMsgIdAndTxt( "inter_biips:inputNotString",
              "Input must be a string.");
    
    if (mxGetM(prhs[0])!=1)
      mexErrMsgIdAndTxt( "MATLAB:revord:inputNotVector",
              "Input must be a row vector.");
    
    /* get the length of the input string */
    //buflen = (mxGetM(prhs[0]) * mxGetN(prhs[0])) + 1;

    /* copy the string data from prhs[0] into a C string input_ buf.    */
    char * input_buf = mxArrayToString(prhs[0]);
   
    string name_func = string(input_buf);

    if (name_func == "make_console") {
   
       if (nb_consoles < MAX_CONSOLES - 1) {
	  consoles[nb_consoles] = new Console(mbiips_cout, mbiips_cerr);
	  nb_consoles++;
          plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
	  *mxGetPr(plhs[0]) = nb_consoles++;
       }
       else {
         mexErrMsgTxt("Too many existing consoles opened!");
      }
    }
    
    else {

       mexPrintf("mauvais nom de fonction\n");

    }
    /* set C-style string output_buf to MATLAB mexFunction output*/
    mxFree(input_buf);
    return;

}

