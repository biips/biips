//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
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

/*! \file   parser_extra.h
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: copied from JAGS
 */

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
