///////////////////////////////////////////////////////////////////////////////
// ars::proposal_sampler.hpp                                                 //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_STATISTICS_DETAIL_ARS_PROPOSAL_SAMPLER_HPP_ER_2009
#define BOOST_STATISTICS_DETAIL_ARS_PROPOSAL_SAMPLER_HPP_ER_2009
#include <cmath>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
#include <iterator>
#include <numeric>
#include <limits>

#include <boost/static_assert.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/bind.hpp>
#include <boost/random.hpp>
#include <boost/range.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/function.hpp>
#include <boost/format.hpp>

#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/math/special_functions/log1p.hpp>
#include <boost/math/special_functions/sign.hpp>

#include <boost/ars/constant.hpp>
#include <boost/ars/parameter.hpp>
#include <boost/ars/error.hpp>
#include <boost/ars/point.hpp>
#include <boost/ars/function/signature.hpp>
#include <boost/ars/detail/data.hpp>
#include <boost/ars/detail/area.hpp>

namespace boost{
namespace statistics{
namespace detail{
namespace ars{

// This class maintains an approximation to the cumulative density of interest 
// and generate draws from it
//
// Requirements:
// Cont holds the data and is required to be bi-directional and is to be used
// in conjunction with lower_bound and insert, so deque is asymptotically the 
// best choice. In practice, though, vector is probably the best choice.
template<
    typename T,
    template<typename,typename> class Cont = std::vector,
    template<typename> class Alloc = std::allocator
>
class proposal_sampler{

    BOOST_STATIC_ASSERT(std::numeric_limits<T>::has_infinity);

    protected:
    typedef std::string                                         str_t;
    typedef constant<T>                                         const_;
    typedef parameter<T>                                        param_;
    typedef point<T>                                            point_t;
    typedef tangent_intersection<T>                             tang_t;
    typedef data<T>                                             data_t;
    typedef Alloc<data_t>                                       alloc_t;
    typedef Cont<data_t,alloc_t>                                datas_t;
    typedef typename boost::range_difference<datas_t>::type     diff_t;
    typedef typename boost::range_iterator<datas_t>::type       iter_t;
    typedef boost::uniform_real<T>                              runif_t;

    public:
    typedef typename ars::function::signature<T>::type          signature;
    typedef boost::function<signature>                          delegate_type;

    typedef typename boost::range_size<datas_t>::type           size_type;
    typedef T                                                   result_type;
    typedef typename runif_t::input_type                        input_type;

    proposal_sampler();
    proposal_sampler(const proposal_sampler& that);
    proposal_sampler& operator=(const proposal_sampler& that);

    // x_min must be constant<T>::inf_ if unbounded. Likewise x_max
    // fun's syntax is fun(x,y,dy) writes to x and dy
    void set_function(T x_min,T x_max,const delegate_type& fun);
    template<typename R> //Range
    void initialize(const R& initial_data);
    void initialize(T x_0,T x_1);
    // Increasing this number improves precision but may cause
    // an infinite area (TODO investigate). In doubt, don't use it
    void set_max_log(T v);

    // Returns true if the draw is accepted, false otherwise
    template<typename U>
    bool sample(U &urng,T& draw) const;

    delegate_type function()const{ return fun_; }
    // Maximum number of evalutions
    size_type max_data_count()const{ return max_data_count_; }

    const T& x_min() const { return x_min_; }
    const T& x_max() const { return x_max_; }
    const datas_t& datas() const { return datas_; }

    T max_log()const{ return max_log_; }
    T max_tangent()const{ return max_tangent_; }
    T cum_sum()const{ return cum_sum_; }
    size_type size_data()const{ return size_datas_; }
    T inverse_cum_sums(const T& u) const;

    T offset()const; //> exp(t-offset)<inf

    //TODO (RandomDistribution)
    //os << x;
    //is >> u;
    // min()
    // max()

    std::ostream& description(std::ostream& stream)const;

    protected:
    template <typename U> iter_t inv_cum_sums_impl(T u,T& q);
    void update_cum_sums(iter_t iter) const;
    void insert(const point_t& p,iter_t iter)const;
    // Returns first iter such that iter->cum_sum_>u
    iter_t inv_cum_sums_impl(const T& u,T& draw)const;

    delegate_type fun_;
    T x_min_;
    mutable T t_min_;
    T x_max_;
    bool has_function_,is_initialized_;
    mutable bool do_update_;
    size_type max_data_count_;
    mutable T max_log_;
    mutable T max_tangent_;
    mutable T cum_sum_;
    mutable datas_t datas_;
    // Calling size(...) is not necessarily O(1) so saving it is prefarable
    mutable size_type size_datas_;
    mutable point_t pending_p_;
    mutable diff_t pending_d_; // distance to where p should be inserted
};

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
std::ostream&
operator<<(std::ostream& out, const proposal_sampler<T,Cont,Alloc>& s){
    return s.description(out);
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
std::ostream&
proposal_sampler<T,Cont,Alloc>::description(std::ostream& out) const
{
    typedef data<T> data_t;
    //TODO any other relevant info?
    boost::format f("x_min = %1%, t_min = %2%|");
    f%x_min()%t_min_;
    out << f.str();
    std::copy(
        boost::begin(datas()),
        boost::end(datas()),
        std::ostream_iterator<data_t>(out, "|")
    );
    out << ", mt = " << max_tangent() << ",  offset = " << offset();

    return out;
}


template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
proposal_sampler<T,Cont,Alloc>::proposal_sampler()
:
fun_(),
x_min_(const_::inf_),
t_min_(const_::inf_),
x_max_(const_::inf_),
has_function_(false),
is_initialized_(false),
do_update_(false),
max_data_count_(param_::max_data_count),
max_log_(param_::max_log),
max_tangent_(const_::zero_),
cum_sum_(const_::zero_),
datas_(),
size_datas_(0),
pending_p_(),
pending_d_(0)
{
    datas_.reserve(param_::reserve);
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
proposal_sampler<T,Cont,Alloc>::proposal_sampler(
    const proposal_sampler& that)
:
fun_( that.fun_ ),
x_min_( that.x_min_ ),
t_min_( that.t_min_ ),
x_max_( that.x_max_ ),
has_function_( that.has_function_ ),
is_initialized_( that.is_initialized_ ),
do_update_( that.do_update_ ),
max_data_count_( that.max_data_count_ ),
max_log_( that.max_log_ ),
max_tangent_( that.max_tangent_ ),
cum_sum_( that.cum_sum_ ),
datas_( that.datas_ ),
size_datas_( that.size_datas_ ),
pending_p_( that.pending_p_ ),
pending_d_(that.pending_d_)
{}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
proposal_sampler<T,Cont,Alloc>&
proposal_sampler<T,Cont,Alloc>::operator=(const proposal_sampler& that){
    if(&that!=this){
        fun_    = that.fun_;
        x_min_  = that.x_min_;
        t_min_  = that.t_min_;
        x_max_  = that.x_max_;
        has_function_ = that.has_function_;
        is_initialized_ = that.is_initialized_;
        do_update_ = that.do_update_;
        max_data_count_ = that.max_data_count_;
        max_log_ = that.max_log_;
        max_tangent_ = that.max_tangent_;
        cum_sum_ = that.cum_sum_;
        datas_ = that.datas_;
        size_datas_ = that.size_datas_;
        pending_p_ = that.pending_p_;
        pending_d_ = that.pending_d_;
    }
    return *this;
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
T proposal_sampler<T,Cont,Alloc>::offset()const{
    return max_tangent()-max_log();
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
void proposal_sampler<T,Cont,Alloc>::set_max_log(T v){
    max_log_ = v;
    update_cum_sums(boost::begin(datas_)); //TODO anything else?
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
void proposal_sampler<T,Cont,Alloc>::set_function(
    T x_min,
    T x_max,
    const delegate_type& fun
)
{
    static const char* method = "ars::proposal_sampler::set_function";
    x_min_ = x_min;
    x_max_ = x_max;
    // removing this-> would create a name conflict
    if(!boost::math::isinf( this->x_min() )){
        if(!boost::math::isinf( this->x_max() )){
            if(!( this->x_min() < this->x_max() )){
                throw exception(
                    method,
                    "x_min > x_max",
                    *this
               );
            }
        }
    }
    fun_ = fun;
    has_function_ = true;
    is_initialized_ = false;
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
void proposal_sampler<T,Cont,Alloc>::initialize(T x_0,T x_1){

    static const char* method = "ars::proposal_sampler::initialize(%1%,%2%)";
    typedef std::vector<point_t> vec_t;
    static vec_t vec;

    if(!has_function_){
        boost::format f(method); f%x_0%x_1;
        throw exception(
            f.str(),
            "no func",
            *this
        );
    }
    vec.clear();
    vec.push_back( create_point<T,delegate_type>(x_0,fun_) );
    vec.push_back( create_point<T,delegate_type>(x_1,fun_) );

    return initialize(vec);
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
template<typename R> //Range
void proposal_sampler<T,Cont,Alloc>::initialize(const R& initial_datas){
    static const char* method
        = "ars::proposal_sampler::initialize(const R& initial_datas)";
    {
        typedef typename range_value<R>::type value_t;
        BOOST_MPL_ASSERT(
            (is_base_of<point_t,value_t>)
        );
    }
    if(size(initial_datas)<2){
        throw exception(
            method,
            "size(initial_datas)<2",
            *this
        );
    }

// NB: Gilks also has:
//      IF ((.NOT.lb).AND.(hpx(iwv(1)).LT.eps)) ifault=3
//      IF ((.NOT.ub).AND.(hpx(iwv(2)).GT.-eps)) ifault=4

    if(
        math::isinf(x_min())
    ){
        T dy = boost::begin(initial_datas)->dy();
        if(!
            (
                dy > const_::zero_
            )
        ){
            throw exception(
                method,
                (boost::format("!dy = %1%>0")%dy).str(),
                *this
            );
        }
        t_min_ = const_::quiet_nan_;
    }else{
        t_min_ = tangent(*boost::begin(initial_datas),x_min());
        max_tangent_ = t_min_;
    }

    datas_.resize( size(initial_datas) );
    // Let i,j = 0,...,n-1 iterators for data and initial_data.
    // At i,j = n-1, *i = *j
    // for i,j = n-2,...,0, *i = tangent_intersection(*j,*j+1)
    std::adjacent_difference(
        make_reverse_iterator(boost::end(initial_datas)),
        make_reverse_iterator(boost::begin(initial_datas)),
        make_reverse_iterator(boost::end(datas_)),
        update_tangent_intersection<T>()
    );

    iter_t back_iter = boost::prior(boost::end(datas_));
    {

    	// Bug prior to 01/08/2010 :
		//T t = max_element(
        //    boost::begin(datas_),back_iter,
        //    bind<const T&>( &tang_t::t, _1 )
        //)->t();

    	typedef const T& cref_;
		typedef cref_(tang_t::*fp)() const; 
		typedef boost::function<cref_(const data_t&)> fun_;
        fun_ fun =  &tang_t::t;

        T t = *std::max_element(
            boost::make_transform_iterator(boost::begin(datas_),fun),
            boost::make_transform_iterator(back_iter,fun)
        );
        max_tangent_ = (max_tangent()<t)? t : max_tangent();
    }
    const point_t& back_p = static_cast<const point_t&>(*back_iter);
    tang_t& back_ti = static_cast<tang_t&>(*back_iter);

    if(boost::math::isinf(x_max())){
        T dy = back_iter->dy();
        if(
            !(dy < const_::zero_)
        ){
            boost::format f("!dy = %1% <0"); f%dy;
            throw exception(method,f.str(),*this);
        }
        back_ti = tang_t(const_::quiet_nan_,const_::quiet_nan_);
    }else{
        back_ti =  tang_t(x_max(),tangent(back_p,x_max()));
        max_tangent_
            = (max_tangent()<back_ti.t())? back_ti.t() : max_tangent();
    }

    update_cum_sums(datas_.begin());

    // Hupper hull > function, the RHS of which is integrable. If the LHS
    // is not integrable in some segment, a midpoint (or reflection point)
    // is added.
    while( boost::math::isinf( cum_sum() ) ) {
        if(size_data()>max_data_count()){
            throw exception(
                method,
                (boost::format("size_data() = > max_data_count() = %2%")
                    %size_data()%max_data_count()).str(),
                *this
            );
        }
        typedef boost::function<const T&(const data_t&)> fun2_t;

        iter_t iter = std::find_if(
            boost::begin( datas_ ),
            boost::end( datas_ ),
            pred_isinf()
        );

        T new_x;
        if(boost::next(iter)!=boost::end(datas_)){
            new_x = iter->z();
        }else{
            T x_0, x_1;
            x_1 = iter->x();
            if(boost::math::isinf(x_max())){
                x_0 = boost::prior(iter)->x();
                //reflection
                new_x = x_1 + (x_1-x_0); //TODO what if = inf?
            }else{
                T x_2 = x_max();
                new_x = ( x_1 + x_2 )/const_::two_;
                if((new_x <= x_1)|| (new_x >= x_2))
                {
                    throw exception(
                        method,
                        "(new_x <= x_1)|| (new_x >= x_2)",
                        *this
                    );
                }
            }
        }
        point_t p = create_point(new_x,fun_);
        insert(p,iter);
    }
    size_datas_ = size(datas_);
    do_update_ = false;
    is_initialized_ = true;
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
T proposal_sampler<T,Cont,Alloc>::inverse_cum_sums(const T& u) const{
    T q;
    inv_cum_sums_impl(u,q);
    return q;
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
typename proposal_sampler<T,Cont,Alloc>::iter_t
proposal_sampler<T,Cont,Alloc>::inv_cum_sums_impl(
    const T& u, // in [0,cum_sum_)
    T& q
)const
{
    static const char* method = "ars::proposal_sampler::inv_cum_sums_impl";
// What TODO about two consecutives segments with same cum_sum due
// to roundoff? Is there a case for std::upper_bound vs std::lower_bound
// upper_bound :  for every iterator j in [first, i), *j <= value
// lower_bound : for every iterator j in [first, i), *j < value.
    if(u>cum_sum()){
        throw exception(
            method,
            "u>cum_sum()",
            *this
        );
    }
    tang_t value;
    value.cum_sum_ = u;
    iter_t iter =
        std::lower_bound(
            boost::begin(datas_),
            boost::end(datas_),
            value
        );

    //TODO delete ->
    BOOST_ASSERT(iter!=boost::end(datas_));
    BOOST_ASSERT(iter->cum_sum_>u);
    if(iter!=boost::begin(datas_)){
        BOOST_ASSERT(boost::prior(iter)->cum_sum_<u);
    }
    // <-
    bool skip = false;
    T lz, lt, x, y, dy, du;
    dump(*iter,x,y,dy);
    if( iter != boost::begin(datas_) ){
        du = u - boost::prior(iter)->cum_sum_;
        lz = boost::prior(iter)->z();
        lt = boost::prior(iter)->t();
    }else{
        if(math::isinf(x_min())){
            // with y[i] <- y[i] - offset
            // solves int[-inf,q]exp(y[i]+dy[i](x-x[i])) dx = u,
            // <=> y[i]+dy[i](q-x[i]) = log(u * dy[i])
            q = ( log( dy * u )-( y - offset() ) + x * dy ) / dy;
            skip = true;
        }else{
            du = u;
            lz = x_min();
            lt = t_min_;
        }
    }
    if(!skip){
        // with t[i-1] <- t[i-1] - offset
        lt -= offset();
        if(dy < const_::lmin_){
            // solves int[z[i-1],q] exp(t[i-1]) dx = u - area[i-1]
            q = lz + du * exp(-lt);
        }else{
            // solves int[z[i-1],q] exp(t[i-1]+dy[i](x-z[i-1])) dx = du
            // <=> q = z[i-1] + log(1 + du * dy[i] / exp(t[i-1]) ) / dy[i]
            // alt: du * dy[i] = exp( log(du) + log(|dy[i]|) ) * sign(dy[i])
            T a = log(du) + log(fabs(dy)) - lt;
            if(a<const_::lmax_){
                T sign = boost::math::sign(dy);
                a = exp(a);
                q = lz + boost::math::log1p( sign * a ) / dy;
            }else{
                q = lz + a / dy;
            }
        }
    }

    //TODO create a local enum

    if(boost::math::isnan(q)){
        throw exception(method,"isnan(q)",*this);
    }
    if(iter!=boost::begin(datas_)){
        if(q<boost::prior(iter)->z()){
            throw exception(method,"q<prior z",*this);
        }
    }else{
        if(!math::isinf(x_min())){
            if(q < x_min()){
                throw exception(method,"q<x_min",*this);
            }
        }
    }
    if(
        ( iter!=boost::end(datas_))
    ){
        if(q>iter->z()){
            throw exception(method,"q>z",*this);
        }
    }else{
        if(q>iter->z()){
            if( !boost::math::isinf(x_max()) ){
                throw exception(method,"q>z",*this);
            }
        }
    }
    return iter;
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
template <typename U>
bool proposal_sampler<T,Cont,Alloc>::sample(U &urng,T& draw) const
{
    static const char* method = "ars::proposal_sampler::sample";
    if(!is_initialized_){
        throw exception(method,"no init",*this);
    }
    if(do_update_){
        insert(
            pending_p_,
            boost::next(
                boost::begin(datas_),
                pending_d_
            )
        ); 
        size_datas_ = size(datas_);
    }
    bool adaptive = (size_data()<max_data_count());

    //Sample from the upper hull a.k.a the enveloppe
    T u1, log_u2, upper;
    {
        runif_t runif(const_::zero_, cum_sum());
        u1 = runif(urng);
    }
    iter_t iter = inv_cum_sums_impl(u1,draw);
    upper = tangent(*iter,draw);
    {
        runif_t unif(const_::zero_, const_::one_);
        log_u2 = log(unif(urng));
    }
    // Squeezing test
    bool accept = false;
    if(adaptive){
        T lower;
        // lower is -inf outside (x[1],x[n])
        if(
            ( (iter!=boost::begin(datas_)) || (draw>(iter->x())))
            && ( (iter!=boost::prior(boost::end(datas_))) || (draw<(iter->x())))
        ){
            if(draw<=(iter->x())){
                lower = linearly_interpolate(*boost::prior(iter),*iter,draw);
            }else{
                lower = linearly_interpolate(*iter,*boost::next(iter),draw);
            }
            accept = ( log_u2 <= (lower-upper) );
        }else{
            // lower is -inf outside (x[1],x[n])
        }
    }
    // Rejection test
    if(!accept){
        pending_p_ = create_point(draw,fun_);
        pending_d_ = std::distance(boost::begin(datas_),iter); 
        if(draw>iter->x()){
            ++pending_d_; 
        }
        accept =  ( log_u2 <= (pending_p_.y() - upper) );
        do_update_ = adaptive;
    }
    return accept;
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
void proposal_sampler<T,Cont,Alloc>::update_cum_sums(iter_t iter) const
{
    static const char* method = "ars::proposal_sampler::update_cum_sums";
    //    x1  x2  x3        point abscissae
    //   / \ / \ / \
    //  z0  z1  z2  z3      tangent abscissae
    //
    // z0 = x_min
    // zn = x_max
    //
    // Here we compute for each i>=j,
    // the area under the tangent at x[i] over (z[i-1],z[i])
    // If z0 = -inf, that's exp(t[1]-offset)/dy[1]
    // It zn = inf, that's -exp(t[n-1]-offset)/dy[1]

    //bool is_begin = false;
    T area;
    tang_t a;
    if(iter != boost::begin(datas_)) {
        a = *boost::prior(iter);
        cum_sum_ = boost::prior(iter)->cum_sum_;
    }else{
        cum_sum_ = const_::zero_;
        if(
            boost::math::isinf(x_min())
        ){
            T dy = iter->dy();
            if(dy<const_::lmin_){
                throw; //TODO
            }else{
                area = area_left_tail(*iter,offset());
            }
        }else{
            a = tang_t(x_min(),t_min_);
            area = area_segment_safeguarded(a,*iter,offset());
        }
        if(area<const_::zero_){
            boost::format f("area[%1%] = %2% <0");
            f%std::distance(boost::begin(datas_),iter)%area;
            throw exception(method,f.str(),*this);
        }
        cum_sum_ += area;
        iter->cum_sum_ = cum_sum_;
        a = *iter;
        ++iter;
    }

    iter_t j = boost::prior(boost::end(datas_));
    // TODO adjacent_difference?
    while(iter!=j){
        T area = area_segment_safeguarded(a,*iter,offset());
        if(area<const_::zero_){
            boost::format f("area[%1%] = %2% < 0");
            f%std::distance(boost::begin(datas_),iter)%area;
            throw exception(method,f.str(),*this);
        }
        cum_sum_ += area;
        iter->cum_sum_ = cum_sum_;
        a = *iter;
        ++iter;
    }

    if(
        math::isinf(x_max())
    ){
        area = area_right_tail(a,*iter,offset());
    }else{
        area = area_segment_safeguarded(a,*iter,offset());
    }
    if(area<const_::zero_){
        boost::format f("area[n] = %1%<0");
        f%area;
        throw exception(method,f.str(),*this);
    }
    cum_sum_ += area;
    iter->cum_sum_ = cum_sum_;
    ++iter;
    BOOST_ASSERT(!(iter!=boost::end(datas_)));

    {	
    	// Replaces version below to accomodate MSVC
		//http://stackoverflow.com/questions/262000/c-best-algorithm-to-check-if-a-vector-is-sorted    
    	using namespace boost::lambda;
    	BOOST_ASSERT(
			std::adjacent_find(
        		boost::begin(datas_),
        		boost::end(datas_),
                boost::lambda::bind(
                	std::less<tang_t>(),
            		_2, 
                    _1
                )
    		) == boost::end(datas_)
    	);
    }

	// Previously:
    //BOOST_ASSERT(
    //    is_sorted(
    //        begin(datas_),
    //        boost::end(datas_),
    //        bind<bool>(
    //            std::less<tang_t>(),
    //            _1,
    //            _2
    //        )
    //    )
    //);
}

template<
    typename T,
    template<typename,typename> class Cont,
    template<typename> class Alloc
>
void proposal_sampler<T,Cont,Alloc>::insert(
    const point_t& p,
    iter_t iter
)const
{
    static const char* method = "ars::proposal_sampler::insert";

    if(iter!=boost::end(datas_)){
        if(!std::less<point_t>()(p,*iter)){
            throw exception(method,"p>next",*this);
        }
    }else{
        if(!math::isinf(x_max())){
            if(!(p.x()<x_max())){
                throw exception(method,"p>max",*this);
            }
        }
    }

    if(iter!=boost::begin(datas_)){
        if(!std::less<point_t>()(*boost::prior(iter),p)){
            throw exception(method,"p<prior",*this);
        }
    }else{
        if(!math::isinf(x_min())){
            if(!(p.x()>x_min())){
                throw exception(method,"p<x_min",*this);
            }
        }
    }

    iter = datas_.insert(iter,p);
    T m = math::tools::min_value<T>();

    struct local{
        static T impl(
            data_t& a,
            const data_t& b,
            const proposal_sampler& s
        ){
            static const char* method = "ars::proposal_sampler::insert";
            const point_t& a_p = a;
            const point_t& b_p = b;
            if(!(a_p<b_p)){
                throw exception(method,"a>b",s);
            }
            if(!is_non_increasing_dy(a_p,b_p)){
                throw exception(method,"dy non increasing",s);
            }
            if(!is_concave(a,b)){
                throw exception(method,"not concave",s);
            }
            {
                tang_t& a_ti = a;
                a_ti = tang_t(a,b);
            }
            if(a.z()<a.x()){
                throw exception(method,"z<x",s);
            }
            if(a.z()>b.x()){
                throw exception(method,"z>next x",s);
            }
            return a.t();
        }
    };

    if(iter!=boost::begin(datas_)){
        T t = local::impl(*boost::prior(iter),*iter,*this);
        m = (m<t)? t : m;
    }else{
        if(!boost::math::isinf(x_min())){
            t_min_ = tangent(*iter,x_min());
        }
    }
    if(boost::next(iter)!=boost::end(datas_)){
        T t = local::impl(*iter,*boost::next(iter),*this);
        m = (m<t)? t : m;
    }else{
        const point_t& p = (*iter);
        tang_t& ti = (*iter);
        if(boost::math::isinf(x_max())){
            ti = tang_t(const_::quiet_nan_, const_::quiet_nan_);
        }else{
            ti = tang_t(x_max(),tangent(p,x_max()));
            T t = ti.t();
            m = (m<t)? t : m;
        }
    }

    if(max_tangent()<m){
        max_tangent_ = m;
        iter = boost::begin(datas_);
    }else{
        if(iter!=boost::begin(datas_)){
            iter = boost::prior(iter);
        }
    }

	
    {	
    	// Replaces version below to accomodate MSVC
		//http://stackoverflow.com/questions/262000/c-best-algorithm-to-check-if-a-vector-is-sorted    
    	using namespace boost::lambda;
    	BOOST_ASSERT(
			std::adjacent_find(
        		boost::begin(datas_),
        		boost::end(datas_),
                boost::lambda::bind(
                	std::less<point_t>(),
            		_2, 
                    _1
                )
    		) == boost::end(datas_)
    	);
    }

	// Previously:
    // BOOST_ASSERT(
    //    is_sorted(
    //        boost::begin(datas_),
    //        boost::end(datas_),
    //        boost::bind<bool>(
    //            std::less<point_t>(),
    //            _1,
    //            _2
    //        )
    //    )
    //);


    update_cum_sums(iter);
}


}// ars
}// detail
}// statistics
}// boost

#endif

