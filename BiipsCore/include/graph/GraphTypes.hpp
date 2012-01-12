//                                               -*- C++ -*-
/*! \file GraphTypes.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_GRAPHTYPES_HPP_
#define BIIPS_GRAPHTYPES_HPP_

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/reverse_graph.hpp>

#include "Node.hpp"

namespace boost
{
  //! Node pointer graph property
  /*!
   * The Node::Ptr objects are stored in a vertex property map of the graph.
   * This property map is stored by the boost graph object.
   */
  enum vertex_node_ptr_t
  {
    vertex_node_ptr = 101
  }; // a unique number
  BOOST_INSTALL_PROPERTY(vertex, node_ptr);

  enum vertex_observed_t
  {
    vertex_observed = 103
  }; // a unique number
  BOOST_INSTALL_PROPERTY(vertex, observed);

  enum vertex_value_t
  {
    vertex_value = 104
  }; // a unique number
  BOOST_INSTALL_PROPERTY(vertex, value);

  enum vertex_discrete_t
  {
    vertex_discrete = 105
  }; // a unique number
  BOOST_INSTALL_PROPERTY(vertex, discrete);
}

namespace Biips
{

  //! Nodes relation type
  /*!
   * This type is returned by nodesRelation(A, B) function
   * saying the relationship between any node A and a stochastic node B
   */
  enum NodesRelationType
  {
    //! Known relationship
    /*!
     * Means A's value is known when computing B in the SMC algorithm.
     */
    KNOWN,
    //! Depending relationship
    /*!
     * Means computing B only depends on A's value, i.e. all other parent values are known.
     */
    DEPENDING,
    //! Unknown relationship
    /*!
     * Means A's value is unknown when computing B in the SMC algorithm.
     */
    UNKNOWN
  };

  //! Node values array storage
  /*!
   * This type the value type of a particle.
   * It stores ValArray shared pointers associated to the nodes of the Graph object.
   * Ex: considering a NodeValues object called node_values,
   * node_values[5] stores the ValArray pointer of node 5.
   * NodeValues objects have the size of the Graph object.
   */
  typedef Types<Types<NumArray::StorageType>::Ptr>::Array NodeValues;

  //! Graph related types
  /*!
   * This structure includes types, specific to the Graph object.
   */
  struct GraphTypes
  {
    //! The boost adjacency list full graph type
    /*!
     * FullGraph is the underlying Boost.Graph structure used to represent
     * the graph vertices and edges.
     * It is full in that it defines both direct edges and stochastic edges of the DAG.
     * Direct edges link one node to its direct parents or direct children.
     * Stochastic edges link a node to its stochastic parents or chiLdren.
     * The latter are built initially because they are used many times in the algorithm.
     *
     * The model used is an adjacency list where the graph edges are represented by a vector
     * of pointed vertices for each vertex, stored in a vector. The edges are bidirectional,
     * i.e. one node can access its targets as well as its sources.
     *
     * The node pointers properties are internally stored.
     * Other vertices or edges properties have to be stored in external property maps
     * \see NodeValuesMap
     */
    typedef boost::adjacency_list<boost::vecS, boost::vecS,
        boost::bidirectionalS, boost::property<boost::vertex_node_ptr_t,
            Node::Ptr, boost::property<boost::vertex_observed_t, Bool,
                boost::property<boost::vertex_discrete_t, Bool,
                    boost::property<boost::vertex_value_t, Types<
                        MultiArray::StorageType>::Ptr> > > > > ParentsGraph;

    //! Node values property map
    /*!
     * Property maps associate properties, here the node values, to the vertices, or the edges
     * of the graph. This object exposes a common interface to property maps, whatever
     * is the underlying structure of the graph.
     * Those property maps can be a simple access to an existing array, here A NodeValues object.
     */
    typedef NodeValues NodeValuesMap;
    //! Constant node values boost property map
    /*!
     * \see NodeValuesMap
     */
    typedef const NodeValues ConstNodeValuesMap;
    //! Flags property map
    /*!
     * Stores flags (booleans) (direct, stochastic parent/child) properties of the vertices.
     * For instance: true if the node has been computed.
     * \see NodeValuesMap
     */
    typedef Flags FlagsMap;

    typedef boost::property_map<ParentsGraph, boost::vertex_value_t>::type
        ValuesPropertyMap;
    typedef boost::property_map<ParentsGraph, boost::vertex_observed_t>::type
        ObservedPropertyMap;
    typedef boost::property_map<ParentsGraph, boost::vertex_discrete_t>::type
        DiscretePropertyMap;

    typedef boost::property_map<ParentsGraph, boost::vertex_value_t>::const_type
        ConstValuesPropertyMap;
    typedef boost::property_map<ParentsGraph, boost::vertex_observed_t>::const_type
        ConstObservedPropertyMap;
    typedef boost::property_map<ParentsGraph, boost::vertex_discrete_t>::const_type
        ConstDiscretePropertyMap;

    typedef boost::graph_traits<ParentsGraph>::adjacency_iterator
        ParentIterator;
    typedef boost::reverse_graph<ParentsGraph> ChildrenGraph;
    typedef boost::graph_traits<ChildrenGraph>::adjacency_iterator
        ChildIterator;

    typedef std::set<NodeId>::const_iterator StochasticParentIterator;

    typedef std::set<NodeId>::const_iterator StochasticChildIterator;

    typedef std::set<NodeId>::const_iterator LikelihoodChildIterator;
  };

}

#endif /* BIIPS_GRAPHTYPES_HPP_ */
