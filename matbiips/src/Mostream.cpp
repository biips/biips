//                                               -*- C++ -*-
/*! \file Mostream.cpp
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

class Mstreambuf: public std::streambuf
{
public:
  typedef Mstreambuf SelfType;
  typedef std::streambuf BaseType;

  Mstreambuf()
  {
  }
protected:

  virtual int_type overflow(int_type c = EOF);

  virtual std::streamsize xsputn(const char* s, std::streamsize n);
};

class MEstreambuf: public std::streambuf
{
public:
  typedef MEstreambuf SelfType;
  typedef std::streambuf BaseType;

  MEstreambuf()
  {
  }
protected:

  virtual int_type overflow(int_type c = EOF);

  virtual std::streamsize xsputn(const char* s, std::streamsize n);
};

Mstreambuf::int_type Mstreambuf::overflow(int_type c)
{
  if (c != EOF) {
    mexPrintf("%.1s", &c);
    mexEvalString("pause(.001);");
  }  
  return 1;
}

std::streamsize Mstreambuf::xsputn(const char* s, std::streamsize n)
{
  mexPrintf("%.*s", n, s);
  mexEvalString("pause(.001);");
  return n;
}

MEstreambuf::int_type MEstreambuf::overflow(int_type c)
{
  if (c != EOF)
  {
    std::string st(1, c);
    mexWarnMsgTxt(st.c_str());
//    mexPrintf("%.1s", &c);
    mexEvalString("pause(.001);");
  }
  return 1;
}

std::streamsize MEstreambuf::xsputn(const char* s, std::streamsize n)
{
  std::string st(s, n);
  mexWarnMsgTxt(st.c_str());
//  mexPrintf("%.*s", n, s);
  mexEvalString("pause(.001);");
  return n;
}

Mstreambuf mbiips_streambuf;
MEstreambuf mbiips_estreambuf;

std::ostream mbiips_cout(&mbiips_streambuf);
std::ostream mbiips_cerr(&mbiips_estreambuf);
