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
#include "printParseTree.hpp"

using namespace Biips;

const String RESULTS_FOLDER = "results/";

void t_ParseTree_core(const String & modelFile, std::ostream & out);

int t_ParseTree(int argc, char* argv[])
{
  using namespace std;
  try
  {
    if ( argc<2 )
    {
      cerr << "Missing model file in command line arguments." << endl;
      return 1;
    }

    String model_file = argv[1];

    t_ParseTree_core(model_file, cout);

    String results_file;
    Size pos = model_file.rfind('/');
    String file_name = model_file.substr(pos+1);
    results_file = RESULTS_FOLDER + file_name + ".parsed";

    cout << "output file : " << results_file << endl;
    ofstream ofs(results_file.c_str());

    t_ParseTree_core(model_file, ofs);

  }
  BIIPS_CATCH_ERRORS

  return 0;
}


void t_ParseTree_core(const String & modelFile, std::ostream & out)
{
  using namespace std;

  out << "Input:" << endl;
  out << "------" << endl;
  out << "model file: " << modelFile << endl;

  // Parse
  //======
  // open model file
  std::FILE *file = fopen(modelFile.c_str(), "r");

  // declare variables
  std::vector<ParseTree*> *pvariables;
  ParseTree * pdata;
  ParseTree * prelations;
  std::string message;

  out << endl;
  out << "Parser:" << endl;
  out << "------" << endl;
  // call parse_bugs : parses the model defined in file
  parse_bugs(file, pvariables, pdata, prelations, message);
  out << "message: " << message << endl;

  // print output
  //=============
  out << endl;
  out << "Parsed variables:" << endl;
  out << "-----------------" << endl;
  if (pvariables != NULL)
  {
    for (vector<ParseTree*>::iterator it_pvar = pvariables->begin(); it_pvar != pvariables->end(); ++it_pvar)
    {
      printParseTree(out, *it_pvar);
      out << endl;
    }
  }

  out << endl;
  out << "Parsed data:" << endl;
  out << "-----------------" << endl;
  if (pdata != NULL)
  {
    printParseTree(out, pdata);
  }

  out << "Parsed relations:" << endl;
  out << "-----------------" << endl;
  if (prelations != NULL)
  {
    printParseTree(out, prelations);
  }

  // close model file
  fclose(file);

  out << "========================================================" << endl;
}
