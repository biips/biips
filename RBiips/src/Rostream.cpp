//                                               -*- C++ -*-
/*! \file Rostream.cpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
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
