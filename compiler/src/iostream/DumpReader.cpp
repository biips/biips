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

/*! \file   DumpReader.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from Stan dump.hpp
 */

#include "iostream/DumpReader.hpp"
#include <sstream>
#include <locale>

namespace Biips
{
  bool DumpReader::scan_single_char(char c_expected)
  {
    int c = in_.peek();
    if (c != c_expected)
      return false;
    char c_skip;
    in_.get(c_skip);
    return true;
  }

  bool DumpReader::scan_optional_long()
  {
    if (scan_single_char('l'))
      return true;
    else if (scan_single_char('L'))
      return true;
    else
      return false;
  }

  bool DumpReader::scan_char(char c_expected)
  {
    char c;
    in_ >> c;
    if (c != c_expected)
    {
      in_.putback(c);
      return false;
    }
    return true;
  }

  bool DumpReader::scan_name_unquoted()
  {
    char c;
    in_ >> c; //
    if (!std::isalpha(c))
      return false;
    name_.push_back(c);
    while (in_.get(c))
    { // get turns off auto space skip
      if (std::isalpha(c) || std::isdigit(c) || c == '_' || c == '.')
      {
        name_.push_back(c);
      }
      else
      {
        in_.putback(c);
        return true;
      }
    }
    return true; // but hit eos
  }

  bool DumpReader::scan_name()
  {
    if (scan_char('"'))
    {
      if (!scan_name_unquoted())
        return false;
      if (!scan_char('"'))
        return false;
    }
    else if (scan_char('\''))
    {
      if (!scan_name_unquoted())
        return false;
      if (!scan_char('\''))
        return false;
    }
    else
    {
      if (!scan_name_unquoted())
        return false;
    }
    return true;
  }

  bool DumpReader::scan_chars(std::string s)
  {
    for (size_t i = 0; i < s.size(); ++i)
    {
      char c;
      if (!(in_ >> c))
      {
        for (size_t j = 1; j < i; ++j)
          in_.putback(s[i - j]);
        return false;
      }
      if (c != s[i])
      {
        in_.putback(c);
        for (size_t j = 1; j < i; ++j)
          in_.putback(s[i - j]);
        return false;
      }
    }
    return true;
  }

  bool DumpReader::scan_na()
  {
    if (!scan_chars("NA"))
    {
      return false;
    }
    if (stack_r_.size() == 0)
    {
      stack_i_.push_back(BIIPS_INTNA);
    }
    else
    {
      stack_r_.push_back(BIIPS_REALNA);
    }
    has_na_ = true;
    return true;
  }

  bool DumpReader::scan_number()
  {
    if (scan_na())
      return true;

    std::string buf;
    bool is_double = false;
    char c;
    // skip preceding spaces
    while (in_.get(c))
    {
      if (std::isspace(c))
        continue;
      in_.putback(c);
      break;
    }
    while (in_.get(c))
    {
      if (std::isdigit(c) || c == '-')
      {
        buf.push_back(c);
      }
      else if (c == '.' || c == 'e' || c == 'E')
      {
        is_double = true;
        buf.push_back(c);
      }
      else
      {
        in_.putback(c);
        break;
      }
    }
    if (!is_double && stack_r_.size() == 0)
    {
      int n;
      if (!(std::stringstream(buf) >> n))
        return false;
      stack_i_.push_back(n);
      scan_optional_long();
    }
    else
    {
      for (size_t j = 0; j < stack_i_.size(); ++j)
      {
        if (stack_i_[j] == BIIPS_INTNA)
        {
          stack_r_.push_back(BIIPS_REALNA);
        }
        else
        {
          stack_r_.push_back(static_cast<double>(stack_i_[j]));
        }
      }
      stack_i_.clear();
      double x;
      if (!(std::stringstream(buf) >> x))
        return false;
      stack_r_.push_back(x);
    }
    return true;
  }

  void DumpReader::print_next_char()
  {
    char c;
    bool ok = in_.get(c);
    if (ok)
    {
      std::cout << "next char=" << c << std::endl;
      in_.putback(c);
    }
    else
    {
      std::cout << "next char=<EOS>" << std::endl;
    }
  }

  bool DumpReader::scan_seq_value()
  {
    if (!scan_char('('))
      return false;
    if (scan_char(')'))
    {
      dims_.push_back(0U);
      return true;
    }
    if (!scan_number())
      return false;; // first entry
    while (scan_char(','))
    {
      if (!scan_number())
        return false;
    }
    dims_.push_back(stack_r_.size() + stack_i_.size());
    return scan_char(')');
  }

  bool DumpReader::scan_struct_value()
  {
    if (!scan_char('('))
      return false;
    if (scan_char('c'))
    {
      scan_seq_value();
    }
    else
    {
      scan_number();

      if (scan_char(':'))
      {
        if (!is_int() || has_na_)
          return false;
        int start = stack_i_[0];
        int end;
        in_ >> end;
        if (start <= end)
        {
          for (size_t i = start + 1; i <= end; ++i)
            stack_i_.push_back(i);
        }
        else
        {
          for (size_t i = start - 1; i >= end; --i)
            stack_i_.push_back(i);
        }
      }
    }
    dims_.clear();
    if (!scan_char(','))
      return false;
    if (!scan_char('.'))
      return false;
    if (!scan_chars("Dim"))
      return false;
    if (!scan_char('='))
      return false;
    if (scan_char('c'))
    {
      if (!scan_char('('))
        return false;
      size_t dim;
      in_ >> dim;
      scan_optional_long();
      dims_.push_back(dim);
      while (scan_char(','))
      {
        in_ >> dim;
        scan_optional_long();
        dims_.push_back(dim);
      }
      if (!scan_char(')'))
        return false;
    }
    else
    {
      size_t start;
      in_ >> start;
      if (!scan_char(':'))
        return false;
      size_t end;
      in_ >> end;
      if (start < end)
      {
        for (size_t i = start; i <= end; ++i)
          dims_.push_back(i);
      }
      else
      {
        for (size_t i = start; i >= end; --i)
          dims_.push_back(i);
      }
    }
    if (!scan_char(')'))
      return false;
    return true;
  }

  bool DumpReader::scan_value()
  {
    if (scan_char('c'))
      return scan_seq_value();
    if (scan_chars("structure"))
      return scan_struct_value();
    if (!scan_number())
      return false;
    if (!scan_char(':'))
      return true;
    if (stack_i_.size() != 1)
      return false;
    if (!scan_number())
      return false;
    if (stack_i_.size() != 2)
      return false;
    int start = stack_i_[0];
    int end = stack_i_[1];
    stack_i_.clear();
    if (start <= end)
    {
      for (int i = start; i <= end; ++i)
        stack_i_.push_back(i);
    }
    else
    {
      for (int i = start; i >= end; --i)
        stack_i_.push_back(i);
    }
    dims_.push_back(stack_i_.size());
    return true;
  }

  /**
   * Helper function prints diagnostic information to std::cout.
   */
  void DumpReader::print()
  {
    std::cout << "var name=|" << name_ << "|" << std::endl;
    std::cout << "dims=(";
    for (size_t i = 0; i < dims_.size(); ++i)
    {
      if (i > 0)
        std::cout << ",";
      std::cout << dims_[i];
    }
    std::cout << ")" << std::endl;
    std::cout << "float stack:" << std::endl;
    for (size_t i = 0; i < stack_r_.size(); ++i)
    {
      std::cout << "  [" << i << "] ";
      if (stack_r_[i] == BIIPS_REALNA)
        std::cout << "NA";
      else
        std::cout << stack_r_[i];
      std::cout << std::endl;
    }
    std::cout << "int stack:" << std::endl;
    for (size_t i = 0; i < stack_i_.size(); ++i)
    {
      std::cout << "  [" << i << "] ";
      if (stack_i_[i] == BIIPS_INTNA)
        std::cout << "NA";
      else
        std::cout << stack_i_[i];
      std::cout << std::endl;
    }
  }

  bool DumpReader::next()
  {
    has_na_ = false;
    stack_r_.clear();
    stack_i_.clear();
    dims_.clear();
    name_.erase();
    if (!scan_name())  // set name
      return false;
    if (!scan_char('<')) // set <-
      return false;
    if (!scan_char('-'))
      return false;
    if (!scan_value()) // set stack_r_, stack_i_, dims_
    {
      print();
      return false;
    }

    char c;
    // skip trailing spaces
    while (in_.get(c))
    {
      if (std::isspace(c))
        continue;
      in_.putback(c);
      break;
    }

    return true;
  }
}
