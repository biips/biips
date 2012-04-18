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

/*! \file ProgressBar.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from boost::progress_display class in boost/progress.hpp
 */

#ifndef PROGRESSBAR_HPP_
#define PROGRESSBAR_HPP_

#include <boost/progress.hpp>

namespace Biips
{

  class ProgressBar
  {
  public:
    ProgressBar(unsigned long expected_count,
                std::ostream & os = std::cout,
                const std::string & s1 = "\n", //leading string
                char symbol = '*',
                const std::string & iter_name = "iterations",
                bool display_time = true);

    //! \brief Display appropriate scale
    /*! \post count()==0, expected_count()==expected_count */
    void restart(unsigned long expected_count,
                 char symbol = '*',
                 const std::string & iter_name = "iterations",
                 bool display_bar = true);
    //! \brief Display appropriate progress tic if needed.
    //! \post count()== original count() + increment
    //! \return count().
    unsigned long operator+=(unsigned long increment);
    unsigned long operator++();
    unsigned long count() const;
    unsigned long expected_count() const;

  protected:
    std::ostream & os_; // may not be present in all imps
    const std::string s1_; // string is more general, safer than
    // const char *, and efficiency or size are

    unsigned long count_, expected_count_, next_tic_count_;
    unsigned int tic_;
    std::string iter_name_;
    char symbol_;
    boost::timer timer_;
    bool display_time_;

    void display_tic();
  };

}

#endif /* PROGRESSBAR_HPP_ */
