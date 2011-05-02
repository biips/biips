//                                               -*- C++ -*-
/*! \file storeUnregistered.hpp
 * \brief 
 * 
 * $LastChangedBy$
 * $LastChangedDate$
 * $LastChangedRevision$
 * $Id$
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

typedef map<String, MultiArray> StoredModelMap;
typedef map<String, DimArray::Ptr> StoredDimMap;
typedef map<String, vector<MultiArray> > StoredDataMap;
typedef map<String, map<Size, vector<Scalar> > > StoredErrorsMap;

void storeUnregistered(const vector<po::parsed_options> & parsed_sources,
    const vector<string> & sources_names,
    StoredModelMap & model_map,
    StoredDimMap & dim_map,
    StoredDataMap & data_map,
    Scalar & log_norm_const_bench,
    StoredDataMap & bench_filter_map,
    StoredDataMap & bench_smooth_map,
    StoredErrorsMap & errors_filter_map,
    StoredErrorsMap & errors_smooth_map);

#endif /* BIIPS_STOREUNREGISTERED_HPP_ */
