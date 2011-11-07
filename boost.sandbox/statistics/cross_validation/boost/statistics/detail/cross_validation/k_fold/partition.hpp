//////////////////////////////////////////////////////////////////////////////
// cross_validation::k_fold::partition.hpp                                  //
//                                                                          //
//  (C) Copyright 2009 Erwann Rogard                                        //
//  Use, modification and distribution are subject to the                   //
//  Boost Software License, Version 1.0. (See accompanying file             //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)        //
//////////////////////////////////////////////////////////////////////////////
#ifndef  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_DATA_K_FOLD_PARTITION_HPP_ER_2009
#define  BOOST_STATISTICS_DETAIL_CROSS_VALIDATION_DATA_K_FOLD_PARTITION_HPP_ER_2009
#include <stdexcept>
#include <iterator>
#include <vector>
#include <ostream>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/range.hpp>
#include <boost/format.hpp>
#include <boost/utility.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/iterator/iterator_traits.hpp>

#include <boost/statistics/detail/cross_validation/extractor/meta_range.hpp>
//#include <boost/statistics/detail/tuple/meta/include.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace cross_validation{
namespace k_fold{


    // k = 4
    // train     test
    // [0][1][2] (3)    j = 3
    // -------------
    // [1][2][3] (0)    j = 0
    // [2][3][0] (1)    j = 1
    // [3][0][1] (2)    j = 2
    // [0][1][2] (3)    j = 3

    // Partitions a dataset into 2 disjoint subsets, of size n(k-1) and n.
    // 
    // U is the type of one observation unit, u, which must contain enough 
    // information to train and test an estimator. For example, t = (x,y) 
    // is needed to train a regression estimator, x is the input to the 
    // predictor, and y is a value against which the output is compared
    // Each of t, x, and y are extracted are extracted from u using instances
    // of Ft, Fi, and Fo, respectively.


    template<
        typename U,
        typename Ft = extractor::identity,
        typename Fi = extractor::identity,
        typename Fo = extractor::identity
    >
    class partition{

        // Warning:
        // Since the dataset is kept internally, U must not be a reference or 
        // contain a reference
    
        typedef std::string str_;
        public:
        typedef boost::circular_buffer<U>                       subset1_type;
        typedef std::vector<U>                                  subset2_type;
        typedef long int                                        int_;
        
        struct meta_training_range 
            : extractor::meta::range<Ft,const subset1_type>{};
        struct meta_input_range    
            : extractor::meta::range<Fi,const subset2_type>{};
        struct meta_output_range   
            : extractor::meta::range<Fo,const subset2_type>{};
                                                    
        partition();
        template<typename It>
        partition(int_ k, It b,It e); 
        template<typename It>
        partition(
            const Ft& ft,
            const Fi& fi,
            const Fo& fo,
            int_ k, It b,It e
        ); 
        partition(const partition&); 
        partition& operator=(const partition&);

        template<typename It> 
        void initialize(int_ k, It b,It e); // j = 0
        void initialize(); //restores state to j = 0
        void increment(); // ++j

        // Access
        const subset1_type& subset1()const;
        const subset2_type& subset2()const;
        
        typename meta_training_range::type   training_range()const;
        typename meta_input_range::type      input_range()const;
        typename meta_output_range::type     output_range()const;


        static str_ description_header;

        const int_& n_test()const{ return this->n(); }      
        const int_& index()const{ return this->j(); }
        const int_& n_folds()const{ return this->k(); }     

        int_ n_training()const{ return this->n() * (this->k()-1); }

        protected:
        const int_& n()const;      // size of test data
        const int_& j()const;      // index of current iteration
        const int_& k()const;      // number of iterations

        int_ k_;
        int_ j_;
        int_ n_;
        subset1_type subset1_;
        subset2_type subset2_;
        Ft ft_;
        Fi fi_;
        Fo fo_;
    };
    template<typename U,typename Ft,typename Fi,typename Fo>
    typename partition<U,Ft,Fi,Fo>::str_
    partition<U,Ft,Fi,Fo>::description_header ="(n_test,n_index,n_folds)";

    template<typename U,typename Ft,typename Fi,typename Fo>
    std::ostream& operator<<(
        std::ostream& out,const partition<U,Ft,Fi,Fo>& that){
        format f("partition(%1%,%2%,%3%)");
        f % that.n_test() % that.index() % that.n_folds();
        return (out << f.str());
    }

    // Implementation //

    template<typename U,typename Ft,typename Fi,typename Fo>
    partition<U,Ft,Fi,Fo>::partition()
    :k_(0),j_(0),n_(0){}
    
    template<typename U,typename Ft,typename Fi,typename Fo>
        template<typename It>
    partition<U,Ft,Fi,Fo>::partition(int_ k, It b,It e)
    {
        this->initialize(k,b,e);
    } 
    
    template<typename U,typename Ft,typename Fi,typename Fo>    
    template<typename It>
    partition<U,Ft,Fi,Fo>::partition(
        const Ft& ft,
        const Fi& fi,
        const Fo& fo,
        int_ k, It b,It e
    )
    :ft_(ft),fi_(fi),fo_(fo)
    {
        this->initialize(k,b,e);
    } 

    template<typename U,typename Ft,typename Fi,typename Fo>
    partition<U,Ft,Fi,Fo>::partition(const partition& that)
    :k_(that.k_),j_(that.j_),n_(that.n_),
    subset1_(that.subset1_),
    subset2_(that.subset2_),
    ft_(that.ft_),
    fi_(that.fi_),
    fo_(that.fo_){}
    
    template<typename U,typename Ft,typename Fi,typename Fo>
    partition<U,Ft,Fi,Fo>& partition<U,Ft,Fi,Fo>::operator=(const partition& that)
    {
        this->k_ = that.k_;
        this->j_ = that.j_;
        this->n_ = that.n_;
        this->subset1_ = that.subset1_;
        this->subset2_  = that.subset2_;
        this->ft_ = that.ft_;
        this->fi_ = that.fi_;
        this->fo_ = that.fo_;
    }

    template<typename U,typename Ft,typename Fi,typename Fo>
        template<typename It>
    void partition<U,Ft,Fi,Fo>::initialize(int_ k,It b,It e)
    {
        BOOST_ASSERT(k>1);
        typedef typename iterator_difference<It>::type  diff_;
        this->k_ = k;
        this->j_ = -1;
        diff_ d = std::distance( b, e);
        if(d % this->k() != 0){
            static const str_ msg 
                = str_("k_fold_estimate : distance(b,e)") 
                    + "%1% not a multiple of k = %2%";
            throw std::runtime_error( ( format(msg) % d % k ).str() );    
        }
        this->n_ = d / this->k(); 

        It i = boost::next( b, this->n() * (k-1) ); 
        this->subset2_.clear();
        this->subset2_.reserve(this->n());
        std::copy(
            i,
            e,
            std::back_inserter(this->subset2_)
        );
        this->subset1_.assign(
            this->n() * (k-1),
            b, 
            i
        );
        this->increment();
    }

    template<typename U,typename Ft,typename Fi,typename Fo>
    void partition<U,Ft,Fi,Fo>::initialize()
    {
        while(this->j()<this->k()){
            this->increment();
        }
        this->j_ = 0;
    }

    template<typename U,typename Ft,typename Fi,typename Fo>
    void partition<U,Ft,Fi,Fo>::increment(){
        static subset2_type tmp;
        if( !( this->j()<this->k() ) )
        {
            static const char* msg = "partition: !j<k=%1%";
            throw std::runtime_error(
                ( format( msg ) % this->k() ).str()
            );
        }
        tmp.clear();
        std::copy(
            boost::begin(this->subset1()),
            boost::next( 
                boost::begin(this->subset1()), 
                this->n() 
            ),
            std::back_inserter(tmp)
        );
        this->subset1_.insert(
            boost::end( this->subset1_ ),
            boost::begin( this->subset2() ),
            boost::end( this->subset2() )
        );
        this->subset2_ = tmp;
        ++this->j_;
    }

    // Access
    template<typename U,typename Ft,typename Fi,typename Fo>
    const typename partition<U,Ft,Fi,Fo>::subset1_type& 
    partition<U,Ft,Fi,Fo>::subset1()const
    {
        return this->subset1_;
    }

    template<typename U,typename Ft,typename Fi,typename Fo>
    const typename partition<U,Ft,Fi,Fo>::subset2_type& 
    partition<U,Ft,Fi,Fo>::subset2()const
    {
        return this->subset2_;
    }

    template<typename U,typename Ft,typename Fi,typename Fo>
    typename partition<U,Ft,Fi,Fo>::meta_training_range::type
    partition<U,Ft,Fi,Fo>::training_range()const
    {
        return meta_training_range::make(
            this->ft_,
            boost::begin(this->subset1()),
            boost::end(this->subset1())
        );
    }

    template<typename U,typename Ft,typename Fi,typename Fo>
    typename partition<U,Ft,Fi,Fo>::meta_input_range::type
    partition<U,Ft,Fi,Fo>::input_range()const
    {
        return meta_input_range::make(
            this->fi_,
            boost::begin(this->subset2()),
            boost::end(this->subset2())
        );
    }

    template<typename U,typename Ft,typename Fi,typename Fo>
    typename partition<U,Ft,Fi,Fo>::meta_output_range::type
    partition<U,Ft,Fi,Fo>::output_range()const
    {
        return meta_output_range::make(
            this->fo_,
            boost::begin(this->subset2()),
            boost::end(this->subset2())
        );
    }

    template<typename U,typename Ft,typename Fi,typename Fo>
    const typename partition<U,Ft,Fi,Fo>::int_& 
    partition<U,Ft,Fi,Fo>::n()const
    {
        return this->n_;
    }

    template<typename U,typename Ft,typename Fi,typename Fo>
    const typename partition<U,Ft,Fi,Fo>::int_& 
    partition<U,Ft,Fi,Fo>::j()const
    {
        return this->j_;
    }
    
    template<typename U,typename Ft,typename Fi,typename Fo>
    const typename partition<U,Ft,Fi,Fo>::int_& 
    partition<U,Ft,Fi,Fo>::k()const
    {
        return this->k_;
    }
    
}// k_fold
}// cross_validation
}// detail
}// statistics
}// boost

#endif