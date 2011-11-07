///////////////////////////////////////////////////////////////////////////////
// tree_view::depth_first.hpp                                                //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_TREE_VIEW_DEPTH_FIRST_HPP_ER_2009
#define BOOST_TREE_VIEW_DEPTH_FIRST_HPP_ER_2009
//#include <iostream> // TODO remove
#include <deque>
#include <stdexcept>
#include <boost/switch.hpp>
#include <boost/operators.hpp>
#include <boost/tree_view/node.hpp>

namespace boost{
namespace tree_view{

    // Manages a node that moves in the tree according to depth-first
    template<unsigned n,unsigned m = BOOST_SWITCH_LIMIT>
    class depth_first
        : incrementable<
            depth_first<n,m>   
        >
    {
        typedef std::deque<bool> bools_;
        typedef node<n,m> node_type;
        public:
        
        // [ Construction ]
        depth_first();
        // Initialized with the root node:
        depth_first(unsigned n_stages); //terminal stage = n_stages - 1
        
        // [ Update ]
        void operator++(); 
        
        // [ Access ]
        // Whether the subtree rooted at the current node was visited
        bool is_subtree()const; 
        const node_type& node()const;
        unsigned n_stages()const;

        private:
        const bools_& tree()const{ return this->tree_; }
        unsigned    n_stages_;
        bools_      tree_; // keeps is_subtree for each node in the tree
        node_type   node_;
    };
    
    // Update
    template<unsigned n,unsigned m>
    void 
    depth_first<n,m>::operator++(){
        
        bool b = false;
        if(this->is_subtree()){
            static const node_type root_node;
            if(this->node() == root_node){
                static const char* msg 
        = "tree_view::depth_first : next of completed root_node not allowed";
                throw std::runtime_error(
                    msg
                );
            }
            if( parent( next( this->node() ) ) != parent( this->node()) ){ 
                this->node_ = parent(this->node_);
                b = true;
            }else{
                ++this->node_;
                unsigned j = (this->node()).stage;
                if( (j+1) == this->n_stages()){
                    b = true;
                }
            }
        }else{
            this->node_ = first_child(this->node_);
            unsigned j = ((this->node()).stage);
            if((j+1) == this->n_stages()){
                b = true;
            }
        }
        this->tree_[position(this->node())] = b;
    }

    template<unsigned n,unsigned m>
    depth_first<n,m>::depth_first():n_stages_(0){}
    
    template<unsigned n,unsigned m>
    depth_first<n,m>::depth_first(unsigned n_stages)
    :n_stages_(n_stages),
    node_(0,0),
    tree_(position(last(this->node(),this->n_stages()-1)),false)
    {}
        
    // [ Access ]
    template<unsigned n,unsigned m>
    bool depth_first<n,m>::is_subtree()const{
        return this->tree()[
            position(
                this->node()
            )
        ];
    }

    template<unsigned n,unsigned m>
    const typename depth_first<n,m>::node_type& 
    depth_first<n,m>::node()const{
        return this->node_;
    }

    template<unsigned n,unsigned m>
    unsigned depth_first<n,m>::n_stages()const{
        return this->n_stages_;
    }

}// tree_view
}// boost
    
#endif