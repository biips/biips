///////////////////////////////////////////////////////////////////////////////
// tree_view::node.hpp                                                       //
//                                                                           //
//  Copyright 2009 Erwann Rogard. Distributed under the Boost                //
//  Software License, Version 1.0. (See accompanying file                    //
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)         //
///////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_TREE_VIEW_NODE_HPP_ER_2009
#define BOOST_TREE_VIEW_NODE_HPP_ER_2009
#include <stdexcept>
#include <ostream>
#include <boost/operators.hpp>
#include <boost/tree_view/stage.hpp>
#include <boost/tree_view/dynamic_stage.hpp>

namespace boost{
namespace tree_view{

// Data structure reprensentation:
// i : position in physical storage
// k : position in stage
// j : stage
//
// Example for n = 3 :
//
// j:   0   1   2        
//
// i:   0->(            k:  0->(
//          1->(                0->(
//              4                   0
//              5                   1
//              6                   2
//          )                   )
//          2->(                1->(
//              7                   3
//              8                   4
//              9                   5
//          )                   )
//          3->(                2->(
//              10                  6
//              11                  7
//              12                  8
//          )                   )
//      )

    // Data structure representing a node in a tree
    //
    // n : number of branches
    // m : number of stages
    template<unsigned n,unsigned m = BOOST_SWITCH_LIMIT>
    struct node : 
        incrementable<
            node<n,m>,
            decrementable<
                node<n,m>,
                less_than_comparable<
                    node<n,m>, 
                    equality_comparable<
                        node<n,m> 
                    >
                >
            >
        >
    {
        typedef std::size_t size_type;
        typedef node<n,m> this_;
        typedef dynamic_stage<n> dyna_stage_;
        unsigned stage;
        unsigned position_in_stage;
        
        node();
        node(unsigned j,unsigned k);
        
        // ++ and -- are along breadth-first
        this_& operator++();
        this_& operator--();
        bool operator==(const this_&)const;
        bool operator<(const this_&)const;
        
        static std::string header;

    };

}//tree_view

    template<unsigned n,unsigned m> std::ostream& 
    operator<<(std::ostream& out, const tree_view::node<n,m>& that);

    template<unsigned n,unsigned m> 
    bool 
    is_root(const tree_view::node<n,m>& that);

    template<unsigned n,unsigned m> 
    typename tree_view::node<n,m>::size_type 
    position(const tree_view::node<n,m>& that);
    
    template<unsigned n,unsigned m> 
    tree_view::node<n,m> 
    parent(const tree_view::node<n,m>& child);

    // first_child(a) is first node in the next stage, with parent a.
    // To visit all the nodes sharing the same parent:
    // node<n,m> node = first_child(a);
    // while(parent(node++)!=a){...}
    template<unsigned n,unsigned m> 
    tree_view::node<n,m> 
    first_child(const tree_view::node<n,m>& parent);

    // -> first, back and last of each stage 
    // [first,...,back,last)
    template<unsigned n,unsigned m> 
    tree_view::node<n,m> 
    first(const tree_view::node<n,m>& root,unsigned stage);

    template<unsigned n,unsigned m> 
    tree_view::node<n,m> 
    back(const tree_view::node<n,m>& root,unsigned stage);

    template<unsigned n,unsigned m> 
    tree_view::node<n,m> 
    last(const tree_view::node<n,m>& root,unsigned stage);
    // <-

    // prior and next by breadth first.
    template<unsigned n,unsigned m>
    tree_view::node<n,m>
    prior(const tree_view::node<n,m>& node);

    template<unsigned n,unsigned m>
    tree_view::node<n,m>
    next(const tree_view::node<n,m>& node);

namespace tree_view{

    // Implementation

    // j: stage
    // k: position relative to other nodes at the same stage in the tree_view
    // i: position in the vector
    template<unsigned n,unsigned m>    
    std::string node<n,m>::header = "(j,k,i)";

    template<unsigned n,unsigned m>    
    node<n,m>::node():stage(0),position_in_stage(0){}

    template<unsigned n,unsigned m>    
    node<n,m>::node(unsigned j,unsigned k)
    :stage(j),position_in_stage(k){
        const char* msg = "node(%1%,!(%2%<%3%)),  ";
        unsigned l = dyna_stage_::number_nodes(this->stage);
        if(! (k <  l) ){ 
            format f(msg);
            f % j % k % l;
            throw std::out_of_range(
                f.str()
            );
        }
    }

    template<unsigned n,unsigned m>
    node<n,m>& 
    node<n,m>::operator++(){
        unsigned mm = dyna_stage_::position_last(this->stage);
        if( 1 + position(*this) < mm){
            ++(this->position_in_stage);
        }else{
            ++stage;
            (this->position_in_stage) = 0;
        }
        return (*this);
    }

    template<unsigned n,unsigned m>
    node<n,m>& 
    node<n,m>::operator--(){
        const char* msg = "--node<n,m> at root";
        if(this->stage == 0){
            throw std::out_of_range(
                msg
            );
        }
        unsigned mm = dyna_stage_::position_first( this->stage );
        if( position(*this)-1 >= mm){
            --(this->position_in_stage);
        }else{
            --stage;
            (this->position_in_stage)   
                = dyna_stage_::number_nodes( this->stage) - 1;
        }
        return (*this);
    }

    template<unsigned n,unsigned m>
    bool node<n,m>::operator==(const this_& that)const{
        return position(*this) == position(that);
    }

    template<unsigned n,unsigned m>
    bool node<n,m>::operator<(const this_& that)const{
        return position(*this) < position(that);
    }

}

    template<unsigned n,unsigned m> 
    std::ostream& operator<<(
        std::ostream& out, 
        const tree_view::node<n,m>& that
    ){
        out 
            << '(' << that.stage 
            << ',' << that.position_in_stage
            << ',' << position(that)
            << ')'; 
        return out;
    }

    template<unsigned n,unsigned m> 
    bool 
    is_root(const tree_view::node<n,m>& that){
        typedef tree_view::node<n,m> node_;
        static const node_ root_node;
        return that == root_node;
    }

    template<unsigned n,unsigned m>
    tree_view::node<n,m> 
    first_child(const tree_view::node<n,m>& that){
        typedef tree_view::node<n,m> node_;
        node_ node = that;
        ++node.stage;
        (node.position_in_stage) *= n;
        return node;
    }

    template<unsigned n,unsigned m>
    tree_view::node<n,m> 
    parent(const tree_view::node<n,m>& that){
        typedef tree_view::node<n,m> node_;
        const char* msg = "node<n,m>::parent() root has no parent";
        if( that.stage == 0 ){
            throw std::out_of_range(
                msg
            );
        }
        node_ node = that;
        --node.stage;
        node.position_in_stage /= n;
        return node;
    }

    template<unsigned n,unsigned m>
    typename tree_view::node<n,m>::size_type
    position(const tree_view::node<n,m>& that){
        typedef tree_view::node<n,m> node_;
        typedef typename node_::dyna_stage_ dyna_stage_;
        return dyna_stage_::position_first(that.stage)
            +(that.position_in_stage);
    }

    template<unsigned n,unsigned m> 
    tree_view::node<n,m> 
    first(const tree_view::node<n,m>& root,unsigned stage){
        typedef tree_view::node<n,m> node_;
        typedef typename node_::dyna_stage_ dyna_stage_;
        return node_(stage,0);
    }

    template<unsigned n,unsigned m> tree_view::node<n,m> 
    back(const tree_view::node<n,m>& root,unsigned stage){
        typedef tree_view::node<n,m> node_;
        typedef typename node_::dyna_stage_ dyna_stage_;
        unsigned k = dyna_stage_::number_nodes( stage ) - 1;
        return node_(stage,k);
    }

    template<unsigned n,unsigned m> tree_view::node<n,m> 
    last(const tree_view::node<n,m>& root,unsigned stage){
        return first(root,stage+1);
    }

    template<unsigned n,unsigned m>
    tree_view::node<n,m>
    prior(const tree_view::node<n,m>& node){
        typedef tree_view::node<n,m> node_;
        node_ new_node = node;
        return --new_node;
    }

    template<unsigned n,unsigned m>
    tree_view::node<n,m>
    next(const tree_view::node<n,m>& node){
        typedef tree_view::node<n,m> node_;
        node_ new_node = node;
        return ++new_node;
    }

}// boost

#endif