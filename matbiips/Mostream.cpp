//                                               -*- C++ -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
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

/*! \file Rostream.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#include "Mostream.h"
#include <cstdio>
#include <mex.h>


class Mstreambuf : public std::streambuf
{
public:
  typedef Mstreambuf SelfType;
  typedef std::streambuf BaseType;

  Mstreambuf() {};
protected:

  virtual int_type overflow(int_type c = EOF);

  virtual std::streamsize xsputn(const char* s, std::streamsize n);
};


class MEstreambuf : public std::streambuf
{
public:
  typedef MEstreambuf SelfType;
  typedef std::streambuf BaseType;

  MEstreambuf() {};
protected:

  virtual int_type overflow(int_type c = EOF);

  virtual std::streamsize xsputn(const char* s, std::streamsize n);
};


Mstreambuf::int_type Mstreambuf::overflow(int_type c)
{
  if(c != EOF)
    mexPrintf("%.1s",&c);
  return 1;
}


std::streamsize Mstreambuf::xsputn(const char* s, std::streamsize n)
{
  mexPrintf("%.*s",n,s);
  return n;
}


MEstreambuf::int_type MEstreambuf::overflow(int_type c)
{
  char chaine[1024];
  if(c != EOF) {
    sprintf(chaine, "%.1s", &c);
    mexErrMsgTxt(chaine);
   }
  return 1;
}


std::streamsize MEstreambuf::xsputn(const char* s, std::streamsize n)
{
  char chaine[1024];
  sprintf(chaine,"%.*s",static_cast<int>(n),s);
  mexErrMsgTxt(chaine);
  return n;
}


Mstreambuf mbiips_streambuf;
MEstreambuf mbiips_estreambuf;

std::ostream mbiips_cout(&mbiips_streambuf);
std::ostream mbiips_cerr(&mbiips_estreambuf);
