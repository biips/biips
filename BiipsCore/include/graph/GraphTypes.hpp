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
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/reverse_graph.hpp>

#include "Node.hpp"

namespace boost
{
  //! Node pointer graph property
  /*!
   * The Node::Ptr objects are stored in a vertex property map of the graph.
   * This property map is stored by the boost graph object.
   */
  enum vertex_node_ptr_t { vertex_node_ptr = 101 }; // a unique number
  BOOST_INSTALL_PROPERTY(vertex, node_ptr);
  //! Edge type graph property
  /*!
   * The edges have a 'type' property, stored in an edge property map of the graph.
   * This property map is stored by the boost graph object.
   * \see EdgeType
   */
  enum edge_type_t { edge_type = 102 }; // a unique number
  BOOST_INSTALL_PROPERTY(edge, type);

  //! Edge type graph property
  /*!
   * The edges have a 'type' property, stored in an edge property map of the graph.
   * This property map is stored by the boost graph object.
   * \see EdgeType
   */
  enum vertex_observed_t { vertex_observed = 103 }; // a unique number
  BOOST_INSTALL_PROPERTY(vertex, observed);

  enum vertex_value_t { vertex_value = 104 }; // a unique number
  BOOST_INSTALL_PROPERTY(vertex, value);
}

namespace Biips
{

  //! Edge type
  enum EdgeType
  {
    DIRECT_EDGE, //!< Direct edge in a DAG
    STOCHASTIC_PARENT_EDGE, //!< Stochastic Parent edge in a DAG
    STOCHASTIC_CHILD_EDGE //!< Stochastic Child edge in a DAG
  };


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
  typedef Types<Types<MultiArray::StorageType>::Ptr>::Array NodeValues;


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
     * The node pointers, and edge 'types' properties are internally stored.
     * Other vertices or edges properties have to be stored in external property maps
     * \see NodeValuesMap
     */
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
    boost::property<boost::vertex_node_ptr_t, Node::Ptr,
    boost::property<boost::vertex_observed_t, Bool,
    boost::property<boost::vertex_value_t, Types<MultiArray::StorageType>::Ptr> > >,
    boost::property<boost::edge_type_t, EdgeType>
    > FullGraph;

    //! Node values boost property map
    /*!
     * Property maps associate properties, here the node values, to the vertices, or the edges
     * of the graph. This object exposes a common interface to property maps, whatever
     * is the underlying structure of the graph.
     * Those property maps can be a simple access to an existing array, here A NodeValues object.
     */
    typedef boost::iterator_property_map<NodeValues::iterator, boost::identity_property_map> NodeValuesMap;
    //! Constant node values boost property map
    /*!
     * \see NodeValuesMap
     */
    typedef boost::iterator_property_map<NodeValues::const_iterator, boost::identity_property_map> ConstNodeValuesMap;
    //! Flags boost property map
    /*!
     * Stores flags (booleans) (direct, stochastic parent/child) properties of the vertices.
     * For instance: true if the node has been computed.
     * \see NodeValuesMap
     */
    typedef boost::iterator_property_map<Flags::iterator, boost::identity_property_map> FlagsMap;
    //! Edge type boost property map
    /*!
     * Stores the 'type' (direct, stochastic parent/child) property of the edges.
     * \see NodeValuesMap
     * \see edge_type_t
     * \see EdgeType
     */
    typedef boost::property_map<FullGraph, boost::edge_type_t>::type EdgeTypePropertyMap;

    typedef boost::property_map<FullGraph, boost::vertex_value_t>::type ValuesPropertyMap;
    typedef boost::property_map<FullGraph, boost::vertex_observed_t>::type ObservedPropertyMap;

    typedef boost::property_map<FullGraph, boost::vertex_value_t>::const_type ConstValuesPropertyMap;
    typedef boost::property_map<FullGraph, boost::vertex_observed_t>::const_type ConstObservedPropertyMap;

    class DirectEdgePredicate
    {
    protected:
      EdgeTypePropertyMap edgeTypeMap_;
    public:
      template<typename Edge>
      Bool operator() (const Edge & e) const { return (boost::get(edgeTypeMap_, e) == DIRECT_EDGE); }
      DirectEdgePredicate() {};
      DirectEdgePredicate(const EdgeTypePropertyMap & edgeTypeMap) : edgeTypeMap_(edgeTypeMap) {};
    };

    typedef boost::filtered_graph<FullGraph, DirectEdgePredicate> DirectParentGraph;
    typedef boost::graph_traits<DirectParentGraph>::adjacency_iterator DirectParentNodeIdIterator;
    typedef boost::reverse_graph<DirectParentGraph> DirectChildrenGraph;
    typedef boost::graph_traits<DirectChildrenGraph>::adjacency_iterator DirectChildrenNodeIdIterator;

    class StochasticParentEdgePredicate
    {
    protected:
      EdgeTypePropertyMap edgeTypeMap_;
    public:
      template<typename Edge>
      Bool operator() (const Edge & e) const { return (boost::get(edgeTypeMap_, e) == STOCHASTIC_PARENT_EDGE); }
      StochasticParentEdgePredicate() {};
      StochasticParentEdgePredicate(const EdgeTypePropertyMap & edgeTypeMap) : edgeTypeMap_(edgeTypeMap) {};
    };

    typedef boost::filtered_graph<FullGraph, StochasticParentEdgePredicate> StochasticParentGraph;
    typedef boost::graph_traits<StochasticParentGraph>::adjacency_iterator StochasticParentNodeIdIterator;

    class StochasticChildEdgePredicate
    {
    protected:
      EdgeTypePropertyMap edgeTypeMap_;
    public:
      template<typename Edge>
      Bool operator() (const Edge & e) const { return (boost::get(edgeTypeMap_, e) == STOCHASTIC_CHILD_EDGE); }
      StochasticChildEdgePredicate() {};
      StochasticChildEdgePredicate(const EdgeTypePropertyMap & edgeTypeMap) : edgeTypeMap_(edgeTypeMap) {};
    };

    typedef boost::filtered_graph<FullGraph, StochasticChildEdgePredicate> StochasticChildrenGraph;
    typedef boost::graph_traits<StochasticChildrenGraph>::adjacency_iterator StochasticChildrenNodeIdIterator;
  };

}

#endif /* BIIPS_GRAPHTYPES_HPP_ */
