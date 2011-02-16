// Erwann Rogard, wrote in July 2009:
//
// This iterator is by nbecker and was found in the boost's vault. 
// Changes that I made are shown by ER_2007_07
//
// arch-tag: d007cbba-7c3d-48e0-9567-af7299fe5708
#ifndef cycle_iterator2_ext_H
#define cycle_iterator2_ext_H

#include <boost/iterator/iterator_adaptor.hpp>
#include <iterator>

// See http://www.nabble.com/cycle-iterators-td25215321.html

namespace boost {

  //! This is a cycle iterator that does NOT keep track of wraparound.
  template<typename BaseIterator, typename offset_t>
  class cycle_iterator2_ext : public boost::iterator_adaptor<cycle_iterator2_ext<BaseIterator, offset_t>,
							 BaseIterator
						       >
  {
  public:
    typedef typename boost::iterator_adaptor<cycle_iterator2_ext<BaseIterator, offset_t>,
					     BaseIterator
					    > super_t;

    typedef typename super_t::difference_type difference_type;
    typedef typename super_t::reference reference;

    explicit cycle_iterator2_ext()
    :size(0),position(0) // ER_2009_07
      {}

    explicit cycle_iterator2_ext (BaseIterator const& _b, BaseIterator const& _e, offset_t offset=0) :
      // base(_b),  // ER_2009_07
      super_t(_b),
      size (std::distance (_b, _e)) {
      SetPos (offset);
    }

    template <typename OtherBase, typename OtherOffset>
    cycle_iterator2_ext (cycle_iterator2_ext<OtherBase,OtherOffset> const& other,
		     typename enable_if_convertible<OtherBase, BaseIterator>::type* = 0) :
      // base (other.base), //ER_2009_07
      super_t(other),
      size (other.size),
      position (other.position)
    {}

  private:
    friend class boost::iterator_core_access;
  

    void increment () {
      ++position;
      if (position >= size) {
	position -= size;
      }
    }

    void decrement () {
      --position;
      if (position < 0) {
	position += size;
      }
    }

    void SetPos (offset_t newpos) {
      position = newpos % size;
      if (position < 0)
	position += size;
    }

    void advance (difference_type n) {
      offset_t newpos = position + n;
      SetPos (newpos);
    }

    template<typename OtherBase, typename OtherOffset>
    difference_type
    distance_to (cycle_iterator2_ext<OtherBase, OtherOffset> const& y) const {
      if (size == 0)
	return 0;

      else {
	offset_t pos1 = realposition();
	offset_t pos2 = y.realposition();
	//	return -(pos1 - pos2);
	offset_t diff = pos1 - pos2;
	if (diff < 0)
	  diff += size;
	return -diff;
      }
    }

    template<typename OtherBase, typename OtherOffset>
    bool equal (cycle_iterator2_ext<OtherBase, OtherOffset> const& y) const {
      return distance_to (y) == 0;
    }

    reference dereference() const { 
        //return *(base + position); //ER_2009_07
        return  *(this->base_reference() + position);
    }

    offset_t PositiveMod (offset_t x) const {
      offset_t y = x % size;
      if (y < 0)
	y += size;
      return y;
    }

  public:


    reference operator[] (difference_type n) const { 
//        return *(base + PositiveMod (position + n)); //ER_2009_07
        return *(this->base_reference() + PositiveMod (position + n)); 
    }

    offset_t offset() const { return position; }

    offset_t realposition () const {
      return position;
    }


    //  private:

    //BaseIterator base; //ER_2009_07
    offset_t size;
    offset_t position;
  };

  template<typename offset_t, typename BaseIterator>
  cycle_iterator2_ext<BaseIterator, offset_t> make_cycle_iterator2_ext(BaseIterator b, BaseIterator e, offset_t offset=0) {
    return cycle_iterator2_ext<BaseIterator, offset_t> (b, e, offset);
  }

  template<typename BaseIterator>
  cycle_iterator2_ext<BaseIterator, int> make_cycle_iterator2_ext(BaseIterator b, BaseIterator e, int offset=0) {
    return cycle_iterator2_ext<BaseIterator, int> (b, e, offset);
  }

} //namespace boost

#endif
