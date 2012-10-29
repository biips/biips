//                                               -*- C++ -*-
/*
 * BiiPS software is a set of C++ libraries for
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

/*! \file ProgressBar.cpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from boost::progress_display class in boost/progress.hpp
 */

#include "iostream/ProgressBar.hpp"

namespace Biips
{
  ProgressBar::ProgressBar(unsigned long expected_count,
                           std::ostream & os,
                           const std::string & s1, //leading strings
                           char symbol,
                           const std::string & iter_name,
                           bool display_time) :
    os_(os), s1_(s1), iter_name_(iter_name), symbol_(symbol),
        display_time_(display_time)
  {
    restart(expected_count, symbol, iter_name);
  }

  void ProgressBar::restart(unsigned long expected_count,
                            char symbol,
                            const std::string & iter_name,
                            bool display_bar)
  //  Effects: display appropriate scale
  //  Postconditions: count()==0, expected_count()==expected_count
  {
    count_ = next_tic_count_ = tic_ = 0;
    expected_count_ = expected_count;
    symbol_ = symbol;
    iter_name_ = iter_name;

    if (display_bar)
      os_ << s1_ << "|--------------------------------------------------| 100%"
          << std::endl;
    os_ << s1_ << "|" << std::flush;
    if (!expected_count_)
      expected_count_ = 1; // prevent divide by zero

    timer_.restart();
  }

  unsigned long ProgressBar::operator+=(unsigned long increment)
  //  Effects: Display appropriate progress tic if needed.
  //  Postconditions: count()== original count() + increment
  //  Returns: count().
  {
    if ((count_ += increment) >= next_tic_count_)
      display_tic();
    return count_;
  }

  unsigned long ProgressBar::operator++()
  {
    return operator+=(1);
  }

  unsigned long ProgressBar::count() const
  {
    return count_;
  }

  unsigned long ProgressBar::expected_count() const
  {
    return expected_count_;
  }

  void ProgressBar::display_tic()
  {
    // use of floating point ensures that both large and small counts
    // work correctly.  static_cast<>() is also used several places
    // to suppress spurious compiler warnings.
    unsigned int tics_needed = static_cast<unsigned int> (49.0 * count_
        / expected_count_);
    do
      os_ << symbol_ << std::flush;
    while (++tic_ < tics_needed);
    next_tic_count_ = static_cast<unsigned long> ((tic_ / 49.0)
        * expected_count_);
    if (count_ == expected_count_)
    {
      if (tic_ < 50)
        os_ << symbol_;
      os_ << "| " << count_ << " " << iter_name_;
      if (display_time_)
      {
        // use istream instead of ios_base to workaround GNU problem (Greg Chicares)
        std::istream::fmtflags old_flags = os_.setf(std::istream::fixed,
                                                    std::istream::floatfield);
        std::streamsize old_prec = os_.precision(2);
        os_ << " in ";
        os_ << timer_.elapsed() << " s"; // "s" is System International d'Unites std
        os_.flags(old_flags);
        os_.precision(old_prec);
      }
      os_ << std::endl;
    }
  }
}
