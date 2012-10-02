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

/*! \file   DumpReader.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: Adapted from Stan dump.hpp
 */

#ifndef BIIPS_DUMPREADER_HPP_
#define BIIPS_DUMPREADER_HPP_

#include "common/Types.hpp"
#include <iostream>

namespace Biips
{
  /**
   * Reads data from S-plus dump format.
   *
   * A <code>DumpReader</code> parses data from the S-plus dump
   * format, a human-readable ASCII representation of arbitrarily
   * dimensioned arrays of integers and arrays of floating point
   * values.
   *
   * <p>Stan's dump reader is limited to reading
   * integers, scalars and arrays of arbitrary dimensionality of
   * integers and scalars.  It is able to read from a file
   * consisting of a sequence of dumped variables.
   *
   * <p>There can be <code>NA</code>
   * (i.e., undefined or missing) values.
   *
   * <p>The dump reader class follows a standard scanner pattern.
   * The method <code>next()</code> is called to scan the next
   * input.  The type, dimensions, and values of the input is then
   * available through method calls.  Here, the type is either
   * double or integer, and the values are the name of the variable
   * and its array of values.  If there is a single value, the
   * dimension array is empty.  For a list, the dimension
   * array contains a single entry for the number of values.
   * For an array, the dimensions are the dimensions of the array.
   *
   * <p>Reads are performed in an "S-compatible" mode whereby
   * a string such as "1" or "-127" denotes and integer, whereas
   * a string such as "1." or "0.9e-5" represents a floating
   * point value.
   *
   * <p>For dumping, arrays are indexed in last-index major fashion,
   * which corresponds to column-major order for matrices
   * represented as two-dimensional arrays.  As a result, the first
   * indices change fastest.  For instance, if there is an
   * three-dimensional array <code>x</code> with dimensions
   * <code>[2,2,2]</code>, then there are 8 values, provided in the
   * order
   *
   * <p><code>[0,0,0]</code>,
   * <code>[1,0,0]</code>,
   * <code>[0,1,0]</code>,
   * <code>[1,1,0]</code>,
   * <code>[0,0,1]</code>,
   * <code>[1,0,1]</code>,
   * <code>[0,1,1]</code>,
   * <code>[1,1,1]</code>.
   *
   * definitions ::= definition+
   *
   * definition ::= name ("<-" | '=') value optional_semicolon
   *
   * name ::= char*
   *        | ''' char* '''
   *        | '"' char* '"'
   *
   * value ::= value<int> | value<double>
   *
   * value<T> ::= num<T>
   *            | seq<T>
   *            | 'structure' '(' seq<T> ',' ".Dim" '=' seq<int> ')'
   *
   * seq<int> ::= num<int>
   *            | int ':' int
   *            | cseq<int>
   *
   * seq<double> ::= num<double>
   *                | cseq<double>
   *
   * cseq<T> ::= 'c' '(' num<T> % ',' ')'
   *
   * num<T> ::= T | "NA"
   *
   */
  class DumpReader {
  private:
    bool has_na_;
    std::string name_;
    std::vector<int> stack_i_;
    std::vector<double> stack_r_;
    std::vector<size_t> dims_;
    std::istream& in_;

    bool scan_single_char(char c_expected);

    bool scan_optional_long();

    bool scan_char(char c_expected);

    bool scan_name_unquoted();

    bool scan_name();

    bool scan_chars(std::string s);

    bool scan_na();

    bool scan_number();

    void print_next_char();

    bool scan_seq_value();

    bool scan_struct_value();

    bool scan_value();

    /**
     * Helper function prints diagnostic information to std::cout.
     */
    void print();


  public:
    /**
     * Construct a reader for standard input.
     */
    DumpReader() : has_na_(false), in_(std::cin) { }

    /**
     * Construct a reader for the specified input stream.
     *
     * @param in Input stream reference from which to read.
     */
    DumpReader(std::istream& in) : has_na_(false), in_(in) { }

    /**
     * Destroy this reader.
     */
    ~DumpReader() { }


    /**
     * Return the name of the most recently read variable.
     *
     * @return Name of most recently read variable.
     */
    const std::string & name() const {
      return name_;
    }

    /**
     * Return the dimensions of the most recently
     * read variable.
     *
     * @return Last dimensions.
     */
    const std::vector<size_t> & dims() const {
      return dims_;
    }

    /**
     * Checks if there are missing values (NA) in the most recently read item.
     *
     * Return <code>true</code> if the value(s) in the most recently
     * read item have NA, i.e. missing values, and <code>false</code> if
     * not.
     */
    bool has_na() const {
      return has_na_;
    }

    /**
     * Checks if the last item read is integer.
     *
     * Return <code>true</code> if the value(s) in the most recently
     * read item are integer values and <code>false</code> if
     * they are floating point.
     */
    bool is_int() const {
      return stack_i_.size() > 0;
    }

    /**
     * Returns the integer values from the last item if the
     * last item read was an integer and the empty vector otherwise.
     *
     * @return Integer values of last item.
     */
    const std::vector<int> & int_values() const {
      return stack_i_;
    }

    /**
     * Returns the floating point values from the last item if the
     * last item read contained floating point values and the empty
     * vector otherwise.
     *
     * @return Floating point values of last item.
     */
    const std::vector<double> & double_values() const {
      return stack_r_;
    }

    /**
     * Read the next value from the input stream, returning
     * <code>true</code> if successful and <code>false</code> if no
     * further input may be read.
     *
     * @return Return <code>true</code> if a fresh variable was read.
     */
    bool next();
  };
}


#endif /* BIIPS_DUMPREADER_HPP_ */
