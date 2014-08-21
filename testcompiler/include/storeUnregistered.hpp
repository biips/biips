//                                               -*- C++ -*-
/*
 * Biips software is a set of C++ libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Authors: Adrien Todeschini, Francois Caron
 *
 * Biips is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of Biips.
 *
 * Biips is free software: you can redistribute it and/or modify
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

/*! \file storeUnregistered.hpp
 * \brief 
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_STOREUNREGISTERED_HPP_
#define BIIPS_STOREUNREGISTERED_HPP_

#include "common/Types.hpp"
#include "common/DimArray.hpp"
#include "common/MultiArray.hpp"
#include <map>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

using std::vector;
using std::string;
using std::map;

using Biips::DimArray;
using Biips::MultiArray;
using Biips::Size;
using Biips::Scalar;
using Biips::String;

typedef map<string, DimArray::Ptr> StoredDimMap;
typedef map<string, vector<MultiArray> > StoredDataMap;
typedef map<string, map<Size, vector<Scalar> > > StoredErrorsMap;

void storeUnregistered(const vector<po::parsed_options> & parsed_sources,
                       const vector<string> & sources_names,
                       vector<string> & monitor,
                       StoredDimMap & dim_map,
                       StoredDataMap & data_map,
                       Scalar & log_norm_const_bench,
                       StoredDataMap & bench_filter_map,
                       StoredDataMap & bench_smooth_map,
                       StoredErrorsMap & errors_filter_map,
                       StoredErrorsMap & errors_smooth_map);

map<String, MultiArray>
    transformStoredDataMap(const StoredDataMap & storedDataMap);

#endif /* BIIPS_STOREUNREGISTERED_HPP_ */
