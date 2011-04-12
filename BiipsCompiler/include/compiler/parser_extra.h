#ifndef PARSER_EXTRA_H_
#define PARSER_EXTRA_H_

#include <compiler/ParseTree.h>
#include <cstdio>
#include <string>

/**
 * Parse a model file that describes the graphical model using the
 * BUGS language.  @param file File containing the model description
 *
 * @param pvariables Pointer to a vector of ParseTree pointers. After
 * calling parse_bugs pvariables will point to a newly allocated
 * ParseTree representing the list of declared variables.  
 *
 * @param pvariables Pointer to a ParseTree. After calling parse_bugs
 * pvariables will point to a newly allocated ParseTree representing
 * the stochastic and logical relationships between declared nodes.
 */
int parse_bugs(std::FILE *file, std::vector<ParseTree*> * &pvariables, 
	       ParseTree * &pdata, ParseTree * &prelations,
	       std::string &message);

#endif /* PARSER_EXTRA_H_ */
