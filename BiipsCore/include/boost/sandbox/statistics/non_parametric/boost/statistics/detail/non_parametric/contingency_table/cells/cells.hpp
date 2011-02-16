///////////////////////////////////////////////////////////////////////////////
// cells.hpp                                                                 //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_CELLS_CELLS_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_CELLS_CELLS_HPP_ER_2010
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>

#include <boost/math/policies/policy.hpp>

#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/container/map/detail/extract_keys.hpp> // needed?
#include <boost/fusion/container/map/detail/subset_traits.hpp>
#include <boost/fusion/container/map/detail/hashable_map.hpp>

#include <boost/unordered_map.hpp>

#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <boost/accumulators/framework/parameters/weight.hpp>
#include <boost/accumulators/framework/detail/unpack_depends_on.hpp>

#include <boost/statistics/detail/non_parametric/contingency_table/detail/policy_keyword.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/detail/raise_domain_error.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/factor/levels.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/factor/vec_levels.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/factor/domain_error_logger.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/factor/check_against_domain.hpp>

// Usage:
//  typedef cells<mpl::vector<X,Y,Z> > cells_
// This is a Boost.Accumulator feature specifying a contingency table asso-
// ciated with variables X, Y, and Z.

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace cells{

   template<typename Keys>
   struct cells_depends_on{
        typedef typename factor::vec_levels<Keys>::type vec_levels_;
        typedef typename boost::accumulators::detail::unpack_depends_on<
            vec_levels_
        >::type type;
   };

    template<typename T,typename Keys>
    struct cells_traits{
        typedef boost::fusion::detail::map_subset_traits<T,Keys> inner_traits;
        typedef typename inner_traits::map subsample_;
        typedef boost::fusion::detail::hashable_map<
            subsample_> hashable_subsample_;
        typedef typename boost::unordered_map<
            hashable_subsample_,std::size_t> map_;
        
    };

namespace impl{

    template<typename T,typename Keys>
    class cells : public boost::accumulators::accumulator_base{
        typedef boost::accumulators::dont_care dont_care_;
        typedef std::size_t size_;
                
        typedef contingency_table::cells::cells_traits<T,Keys> traits_;
        typedef typename traits_::hashable_subsample_ hashable_subsample_;
        typedef typename traits_::map_ map_;
        
        public:
		
        typedef T 		sample_type; 
        typedef size_ 	size_type;	 

        typedef map_& result_type; // not const bec map[] is non-const

        cells(dont_care_){}

        template<typename A1,typename A2>
        void operator()(const A1& a1,const A2& a2){
            
        }

        template<typename Args>
        void operator()(const Args& args){
            namespace ns = boost::statistics::detail;
            typedef boost::math::policies::policy<> pol_;
            this->update_if(
                hashable_subsample_( args[boost::accumulators::sample] ),
                args[ boost::accumulators::accumulator ],
                args[ ns::_policy |  pol_() ],
                args[boost::accumulators::weight]
            ); 
        }
		
        result_type result(dont_care_)const{
            return this->map;
        }

		private:
        template<typename V,typename A,typename P,typename N>
        void update_if(
            const V& s,
            const A& acc,
            const P& policy,
            const N& size
        ){
            this->error_logger.reset();
            boost::fusion::for_each( 
                s, 
                factor::make_check_against_domain( acc, this->error_logger) 
            );
            if(!this->error_logger.is_error())
            {
                ( this->map )[ s ] += size;
            }else{
                namespace ns = contingency_table;
                static const char* fun = "impl::cells::update_if %1%";
                ns::raise_domain_error<V>(
                  fun,
                  this->error_logger().c_str(), 
                  policy
                );
            }    
        }

        mutable map_ map;
        typedef contingency_table::factor::domain_error_logger error_logger_;
        error_logger_ error_logger; 
	};
    
}// impl
namespace tag
{

    template<typename Keys>
    struct cells 
        : contingency_table::cells::cells_depends_on<Keys>::type
    {
        typedef typename factor::par_spec< Keys >::type par_spec_;
    
        struct impl{
            template<typename T,typename W>
            struct apply{
        		typedef contingency_table
                    ::cells::impl::cells<T,Keys> type;    	
            };
        };
    };
    
}// tag
namespace result_of{
namespace extract{

    template<typename Keys,typename AccSet>
    struct cells
    : boost::accumulators::detail::extractor_result<
        AccSet,
        contingency_table::cells::tag::cells<Keys>
    >{};

}// extract
}// result_of
namespace extract
{

    // Usage :
    //    cells<Key>(acc,policy) 
  	template<typename Keys,typename P,typename AccSet>
    typename contingency_table::cells::result_of::extract::template 
        cells<Keys,AccSet>::type
  	cells(AccSet const& acc,const P& policy)
    {
    	typedef contingency_table::cells::tag::cells<Keys> the_tag;
        return boost::accumulators::extract_result<the_tag>(acc,policy);
  	}

  	template<typename Keys,typename AccSet>
    typename contingency_table::cells::result_of::extract::template 
        cells<Keys,AccSet>::type
  	cells(AccSet const& acc)
    {
    	typedef boost::math::policies::policy<> policy_;
        return extract::cells<Keys>(acc,policy_());
  	}

}// extract

using extract::cells;

}// cells
}// contingency_table
}// detail
}// statistics
}// boost

#endif
