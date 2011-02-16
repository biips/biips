///////////////////////////////////////////////////////////////////////////////
// flatten_iterator.hpp                              				 	 	 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_ITERATOR_FLATTEN_ITERATOR_HPP_ER_2010
#define BOOST_ITERATOR_FLATTEN_ITERATOR_HPP_ER_2010
#include <stdexcept>
//#include <boost/mpl/if.hpp>
#include <boost/next_prior.hpp>
#include <boost/range.hpp>
#include <boost/type_traits.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <boost/iterator/iterator_facade.hpp>

// As per M.P.'s suggestion

namespace boost{

	template<typename It>
	struct flatten_iterator_nested
    {
    	typedef typename boost::iterator_reference<It>::type ref_range_;
        typedef typename boost::remove_reference<ref_range_>::type range_;

        // Why const? That's need to be made rigorous. For now it just works
        // in cases it doesn't without
        typedef typename boost::range_iterator<
        	typename boost::add_const<range_>::type
        >::type it_;
        typedef typename boost::iterator_reference<it_>::type ref_;
        typedef typename boost::remove_cv<
        	typename boost::remove_reference<ref_>::type
        >::type val_;
        typedef typename boost::iterator_difference<it_>::type diff_;
        // TODO category_traversal
    };

	template<typename It>
	struct flatten_iterator_category_traversal{
		// TODO
    	// weaker of outer and inner iterator
        // also not allowed decremment and advance(n<0)
    };
    
	template<
    	class It,
        typename C = random_access_traversal_tag,
        typename R =  typename flatten_iterator_nested<It>::ref_,
        typename D = typename flatten_iterator_nested<It>::diff_
    >
	class flatten_iterator : public boost::iterator_facade<
        flatten_iterator<It,C,R,D>
      , typename flatten_iterator_nested<It>::val_
      , C
      , R
      , D
    >{
		typedef flatten_iterator_nested<It> nested_;
		typedef typename nested_::it_ nit_;

		typedef typename boost::iterator_facade<
        	flatten_iterator<It,C,R,D>
      		, typename flatten_iterator_nested<It>::val_
      		, C
      		, R
      		, D
    	> super_;

        public:        

		typedef typename super_::difference_type diff_;
		typedef typename super_::reference ref_;
                        
        flatten_iterator(){}

        explicit flatten_iterator(It b,It e):super_(),b_(b),e_(e){
        	this->update();
        }

        flatten_iterator(const flatten_iterator& that)
        :super_(that),b_(that.b_),e_(that.e_)
        {
        	if(!this->is_end()){
				this->nb_ = (that.nb_);
                this->ne_ = (that.ne_);            
            }
        }

		flatten_iterator& 
        operator=(const flatten_iterator& that){
        	if(&that!=this){
            	super_::operator=(that);
				this->b_ = that.b_;
                this->e_ = that.e_;
                if(!this->is_end()){
                    this->nb_ = (that.nb_);
                    this->ne_ = (that.ne_);            
                }
            }
            return (*this);
        }

		protected:
        friend class boost::iterator_core_access;
			
        void increment(){ 
        	BOOST_ASSERT(this->b_!=this->e_);
            ++this->nb_;
        	if(
            	(this->nb_) == (this->ne_)
            )
        	{
            	++this->b_;
            	this->update();
        	}
		}
        
        void decrement(){
			throw std::runtime_error(
				"flatten_iterator::decrement() not allowed"            
            );
        }
	
		void advance(diff_ n){
            if(n>0){
            	BOOST_ASSERT(!this->is_end());
        		diff_ k = std::distance(this->nb_,this->ne_);
				if(n<k){
            		this->nb_ = boost::next(this->nb_,n);
            	}else{
            		++this->b_;
                	this->update(); 
            		return this->advance(n-k);
            	}
            }else{
            	if(n!=0){
                    throw std::runtime_error(
                        "flatten_iterator::advance(n<0) not allowed"            
                    );
            	}
            }
        }

    	ref_ dereference()const
    	{   
        	return (*this->nb_);
    	}

		public: // temporary

    	diff_ distance_to(const flatten_iterator& other)const
    	{   
        
        	diff_ d;
            if(this->b_ < other.b_){
            	d = dist_impl(*this,other);
            }
        	if(this->b_ == other.b_){
            	d = std::distance(this->nb_,other.nb_);
            }
            if(this->b_ > other.b_){
            	d = -dist_impl(other,*this);
            }

			return d;
    	}

    	bool equal(const flatten_iterator& rhs)const
    	{   
			// Beware : assumes the traversal over the same matrix.
            // TODO check it?

        	BOOST_ASSERT(this->e_==rhs.e_);

        	if(this->b_ == rhs.b_){
            	if(this->b_ == this->e_){
                    return true;
                }else{
                	BOOST_ASSERT(this->ne_ == rhs.ne_);
                	return (this->nb_ == rhs.nb_);
                }
            }else{
				return false;
            }

    	}

        mutable It b_,e_;
        mutable nit_ nb_,ne_; // nested in *b_

		bool is_end()const{ return ((this->b_) == (this->e_)); }

    	void update()const
    	{
        	if(!this->is_end()){
            	this->nb_ = boost::begin(*this->b_);
            	this->ne_ = boost::end(*this->b_);
        	}
    	}

    	static diff_ dist_impl(
        	const flatten_iterator& x,
            const flatten_iterator& y
        ){
        	BOOST_ASSERT(x.b_ < y.b_);
            
            diff_ d = std::distance(x.nb_,x.ne_);
            It it = x.b_;
            ++it;
            while(it!=y.b_){
            	nit_ nb = boost::begin(*it);
            	nit_ ne = boost::end(*it);
                d += std::distance(nb,ne);
                ++it;
            }
        	BOOST_ASSERT(it == y.b_);
			if(it!=x.e_){
            	nit_ nb = boost::begin(*it);
            	d += std::distance(nb,x.nb_);
            }
			return d;	
        }
        
	};        

	template<typename It>	
    flatten_iterator<It>
	make_flatten_iterator(It b,It e){
    	typedef flatten_iterator<It> result_;
    	return result_(b,e);
    }

}//boost

#endif

