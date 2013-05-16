#include "pmmh.hpp"

namespace Biips {

    void Pmmh::post_init(void) {
  
        vector<vector<size_t> > param_lower, param_upper;
        vector<string> param_varnames;

        parse_varnames(_param_names, param_varnames, param_lower, param_upper); 
     
        assert(_init_values.size() == param_names.size());
        
        // loop on parameters
        // we copy the init_values into the smc
        for(int i = 0; i < param_names.size() ;  ++i) {
              
            int VERBOSITY = 2;
            Bool mcmc = false;

            // a remplacer
            IndexRange::Indices lind(lower[i].begin(), lower[i].end());
            IndexRange::Indices uind(upper[i].begin(), upper[i].end());
            auto range = IndexRange(lind, uind);
            Multiarray marray;
            
            auto ndims = _init_values[i].NDim():
            auto dims = _init_values[i].DimPtr();
            auto r_vec =  _init_values[i].ValuesPtr(); 
            
            auto p_dim = new DimArray(ndims);
            copy(dims, dims + ndims , p_dim->begin());

            auto p_val = new ValArray(r_vec_nb_elems);
            replace_copy(r_vec ,r_vec + r_vec_nb_elems, p_val->begin(), numeric_limits<Scalar>::quiet_NaN(), BIIPS_REALNA);
            marray.SetPtr(p_dim, p_val);
            _console_ptr->ChangeData(param_names[i], range, marray, mcmc, VERBOSITY);
        }
    }
}
