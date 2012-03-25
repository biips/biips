//                                               -*- C++ -*-
/*! \file t_Hmm1D_mini.cpp
 * \brief BiiPS minimal example
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 *
 * BiiPS example : Hidden Markov Model linear gaussian 1D
 * ======================================================
 * x[0] --> x[1] --> ... --> x[t] --> ...
 *           |                |
 *           v                v
 *          y[1]     ...     y[t]     ...
 *
 *          x[0] ~ Normal(mean_x0, var_x0)
 * x[t] | x[t-1] ~ Normal(x[t-1], var_x) for all t>0
 *   y[t] | x[t] ~ Normal(x[t], var_y)  for all t>0
 *
 */

#include "BiipsCore.hpp"
#include "BiipsBase.hpp"

int main(int argc, char* argv[])
{

  using namespace Biips;

 /*==================================*
  *                                  *
  * Instantiate the model with BiiPS *
  *                                  *
  *==================================*/

  // Define the data :
  //------------------
  // Final time :
  Size t_max = 20;
  // Initial mean of X[0]
  Scalar mean_x0_val = 0;
  // Variance of X[0] :
  Scalar var_x0_val = 1;
  // Variance of X[t] | X[t-1] :
  Scalar var_x_val = 1;
  // Variance of Y[t] | Y[t] :
  Scalar var_y_val = 0.5;

  // Declare FunctionTable and a DistributionTable :
  //------------------------------------------------
  FunctionTable func_tab;
  DistributionTable dist_tab;

  // Load the Base module :
  //-----------------------
  loadBaseModule(func_tab, dist_tab);

  // Declare the Graph object :
  //---------------------------
  Graph graph;

  // Add the constant nodes to the graph :
  //--------------------------------------
  NodeId mean_x0 = graph.AddConstantNode(MultiArray(mean_x0_val));
  NodeId var_x0 = graph.AddConstantNode(MultiArray(var_x0_val));
  NodeId var_x = graph.AddConstantNode(MultiArray(var_x_val));
  NodeId var_y = graph.AddConstantNode(MultiArray(var_y_val));

  // Create stochastic NodeId collections to store X and Y's components identifiers :
  //---------------------------------------------------------------------------------
  Types<NodeId>::Array x(t_max+1);
  Types<NodeId>::Array y(t_max);

  // Create a NodeId array to handle the parents of each stochastic node :
  //----------------------------------------------------------------------
  Types<NodeId>::Array params(2);

  // Add X_0 stochastic node to the graph :
  //---------------------------------------
  params[0] = mean_x0;
  params[1] = var_x0;
  x[0] = graph.AddStochasticNode(dist_tab["dnormvar"], params, false);

  // Add the other stochastic nodes to the graph :
  //----------------------------------------------
  for (Size t=1; t<t_max+1; ++t)
  {
    // Add X[t]
    params[0] = x[t-1];
    params[1] = var_x;
    x[t] = graph.AddStochasticNode(dist_tab["dnormvar"], params, false);

    // Add Y[t]
    params[0] = x[t];
    params[1] = var_y;
    y[t-1] = graph.AddStochasticNode(dist_tab["dnormvar"], params, true);
  }

  // Build the graph :
  //------------------
  graph.Build();

  // Generate data :
  //----------------
  // Declare a random number generator, initialized with an integer seed
  Rng my_rng(0);

  // Sample values according to the stochastic nodes prior distribution
  NodeValues gen_values = graph.SampleValues(&my_rng);

  // Set the observations values according to the generated values
  graph.SetObsValues(gen_values);


 /*==================================*
  *                                  *
  *     Run the SMC algorithm        *
  *                                  *
  *==================================*/

  // Define the number of particles :
  //---------------------------------
  Size nb_particles = 1000;

  // Declare the ForwardSampler object :
  //--------------------------------
  ForwardSampler sampler(nb_particles, &graph, &my_rng);
  sampler.SetResampleParams("stratified", 0.5);

  // Initialize the ForwardSampler object :
  //-----------------------------------
  sampler.Initialize();

  // Declare and configure a ScalarAccumulator object :
  //---------------------------------------------------
  Accumulator stats_acc;
  stats_acc.AddFeature(MEAN);
  stats_acc.AddFeature(VARIANCE);

  // Declare Scalar arrays to store the posterior mean and variance estimates :
  //---------------------------------------------------------------------------
  Types<Scalar>::Array x_est_PF(t_max+1);
  Types<Scalar>::Array x_var_PF(t_max+1);

  // Iterate the SMC algorithm and extract summary statistics :
  //-----------------------------------------------------------
  for (Size t=0; t<t_max+1; ++t)
  {
    // Iterate the SMC algorithm : resample (if needed) and sample one node of the sequence
    sampler.Iterate();

    // Accumulate particles corresponding to the last updated node
    sampler.Accumulate(x[t], stats_acc);

    // extract summary statistics of the filtering density
    x_est_PF[t] = stats_acc.Mean();
    x_var_PF[t] = stats_acc.Variance();
  }

  return 0;

}
