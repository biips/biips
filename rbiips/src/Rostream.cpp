//                                               -*- C++ -*-
/*
 * RBiips package for GNU R is an interface to BiiPS C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * RBiips is derived software based on:
 * BiiPS, Copyright (C) Inria, 2012
 * rjags, Copyright (C) Martyn Plummer, 2002-2010
 * Rcpp, Copyright (C) Dirk Eddelbuettel and Romain Francois, 2009-2011
 *
 * This file is part of RBiips.
 *
 * RBiips is free software: you can redistribute it and/or modify
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
/*! \file Rostream.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "Rostream.h"

#include <R_ext/Print.h>
#include <cstdio>


class Rstreambuf : public std::streambuf
{
public:
  typedef Rstreambuf SelfType;
  typedef std::streambuf BaseType;

  Rstreambuf() {};
protected:

  virtual int_type overflow(int_type c = EOF);

  virtual std::streamsize xsputn(const char* s, std::streamsize n);
};


class REstreambuf : public std::streambuf
{
public:
  typedef REstreambuf SelfType;
  typedef std::streambuf BaseType;

  REstreambuf() {};
protected:

  virtual int_type overflow(int_type c = EOF);

  virtual std::streamsize xsputn(const char* s, std::streamsize n);
};


Rstreambuf::int_type Rstreambuf::overflow(int_type c)
{
  if(c != EOF)
    Rprintf("%.1s",&c);

  return 1;
}


std::streamsize Rstreambuf::xsputn(const char* s, std::streamsize n)
{
  Rprintf("%.*s",n,s);
  return n;
}


REstreambuf::int_type REstreambuf::overflow(int_type c)
{
  if(c != EOF)
    REprintf("%.1s",&c);

  return 1;
}


std::streamsize REstreambuf::xsputn(const char* s, std::streamsize n)
{
  REprintf("%.*s",n,s);
  return n;
}


Rstreambuf rbiips_streambuf;
REstreambuf rbiips_estreambuf;

std::ostream rbiips_cout(&rbiips_streambuf);
std::ostream rbiips_cerr(&rbiips_estreambuf);
