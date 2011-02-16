//                                               -*- C++ -*-
/*! \file t_ParseTree.cpp
* \brief
*
* $LastChangedBy$
* $LastChangedDate$
* $LastChangedRevision$
* $Id$
*/

#include <cstdio>
#include <cstring>
#include <fstream>

#include "common/Common.hpp"
#include "compiler/parser_extra.h"
#include "compiler/printParseTree.hpp"

using namespace Biips;
using namespace std;

const String data_folder = "data/";
const String results_folder = "results/";

void t_ParseTree(int argc, char* argv[])
{
  try
  {
    // input
    //======
    String model_file;
    String results_file;
    if ( argc>2 )
    {
      model_file = data_folder + argv[2];
      results_file = results_folder + argv[2] + ".parsed";
    }

    Bool promptFlag = true;
    if ( argc>3 )
      promptFlag =  !(strcmp(argv[3], "--noprompt") == 0);

    cout << "output file : " << results_file << endl;
    ofstream ofs(results_file.c_str());

    cout << "Input:" << endl;
    cout << "------" << endl;
    cout << "model file: " << model_file << endl;
    ofs << "Input:" << endl;
    ofs << "------" << endl;
    ofs << "model file: " << model_file << endl;

    // Parse
    //======
    // open model file
    std::FILE *file = fopen(model_file.c_str(), "r");

    // declare variables
    std::vector<ParseTree*> *pvariables;
    ParseTree * pdata;
    ParseTree * prelations;
    std::string message;

    cout << endl;
    cout << "Parser:" << endl;
    cout << "------" << endl;
    ofs << endl;
    ofs << "Parser:" << endl;
    ofs << "------" << endl;
    // call parse_bugs : parses the model defined in file
    parse_bugs(file, pvariables, pdata, prelations, message);
    cout << "message: " << message << endl;
    ofs << "message: " << message << endl;

    // print output
    //=============
    cout << endl;
    cout << "Parsed variables:" << endl;
    cout << "-----------------" << endl;
    ofs << endl;
    ofs << "Parsed variables:" << endl;
    ofs << "-----------------" << endl;
    if (pvariables != NULL)
    {
      for (vector<ParseTree*>::iterator it_pvar = pvariables->begin(); it_pvar != pvariables->end(); ++it_pvar)
      {
        printParseTree(cout, *it_pvar);
        printParseTree(ofs, *it_pvar);
        cout << endl;
        ofs << endl;
      }
    }

    cout << endl;
    cout << "Parsed data:" << endl;
    cout << "-----------------" << endl;
    ofs << endl;
    ofs << "Parsed data:" << endl;
    ofs << "-----------------" << endl;
    if (pdata != NULL)
    {
      printParseTree(cout, pdata);
      printParseTree(ofs, pdata);
    }

    cout << "Parsed relations:" << endl;
    cout << "-----------------" << endl;
    ofs << "Parsed relations:" << endl;
    ofs << "-----------------" << endl;
    if (prelations != NULL)
    {
      printParseTree(cout, prelations);
      printParseTree(ofs, prelations);
    }

    // close model file
    fclose(file);

    cout << "========================================================" << endl;
    ofs << "========================================================" << endl;
  }
  BIIPS_CATCH

}
