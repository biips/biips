///////////////////////////////////////////////////////////////////////////////
// levels.hpp                                                                //
//                                                                           //
//  Copyright 2010 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_LEVELS_HPP_ER_2010
#define BOOST_STATISTICS_DETAIL_NON_PARAMETRIC_CONTINGENCY_TABLE_FACTOR_LEVELS_HPP_ER_2010
#include <vector>
#include <boost/config.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/range.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/unordered_set.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/parameter/name.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/parameters/accumulator.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/statistics_fwd.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/factor/levels.hpp>
#include <boost/statistics/detail/non_parametric/contingency_table/factor/keyword.hpp>

// Source : http://www.ats.ucla.edu/stat/R/modules/factor_variables.htm

namespace boost { 
namespace statistics{
namespace detail{
namespace contingency_table{
namespace factor{

BOOST_PARAMETER_NAME(map_of_levels);

    template<typename K,typename T>
    struct levels_traits{
        typedef typename boost::fusion::result_of::at_key<T,K>::type ref_;
        typedef typename boost::remove_reference<ref_>::type qual_;
        typedef typename boost::remove_const<qual_>::type val_;
        typedef boost::unordered_set<val_> set_;
    };

namespace impl{

    template<typename Key,typename T> 
    class levels : public boost::accumulators::accumulator_base
    {
        typedef boost::accumulators::dont_care dont_care_;
        typedef factor::levels_traits<Key,T> traits_;
        typedef typename traits_::set_ set_;
        typedef factor::kwd<Key> kwd_;

        public:
        typedef Key key_type;
        typedef typename traits_::val_ data_type;

        typedef T sample_type;
        typedef set_& result_type;

        levels(dont_care_){}
        
        template<typename Args>        
        explicit levels(const Args& args)
            :storage(
                make_set(
                    args[ _map_of_levels | set_() ]
                )
            )
            {
               // TODO allow either the above OR kwd_::instance. For now
               // this does not compile
            }

        void operator()(dont_care_)const{}

        result_type result(dont_care_)const{
           return this->storage;
        }

        private:
        static set_ make_set(const set_& s){ return s; }

        template<typename M>
        static set_ make_set(const M& map,typename M::fusion_tag* = 0){ 
            return make_set_impl( boost::fusion::at_key<Key>(map) );
        }

        template<typename L>
        static set_ make_set_impl(const L& levels){
            return set_( 
                boost::begin( levels ), 
                boost::end( levels ) 
            );
        }
        
        mutable set_ storage;
    };

}// impl

namespace tag
{
    template<typename Key>
    struct levels
      : boost::accumulators::depends_on<>
    {
    
        //typedef factor::tpl_keyword<Key> keyword_;
    
        struct impl{
            template<typename T,typename W>
            struct apply{
                typedef factor::impl::levels<Key,T> type;    	
            };
        };

    };
    
}// tag

namespace result_of{

    template<typename Key,typename AccSet>
    struct factor
    : boost::accumulators::detail::extractor_result<
        AccSet, factor::tag::levels<Key> >{};

}// result_of

namespace extract
{

  	template<typename Key,typename AccSet>
    typename factor::result_of::template 
        factor<Key,AccSet>::type
  	levels(AccSet const& acc)
    {
    	typedef factor::tag::levels<Key> the_tag;
        return boost::accumulators::extract_result<the_tag>(acc);
  	}

}// extract

using extract::levels;

}// factor
}// contingency_table
}// detail
}// statistics
}// boost

#endif

