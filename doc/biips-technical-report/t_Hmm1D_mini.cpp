//                                               -*- C++ -*-
/*! \file t_Hmm1D_mini.cpp
 * \brief Biips minimal example
 * 
 * \author  $LastChangedBy$
 * \date    $LastChangedDate: 2010-10-25 18:42:40 +0200 (lun., 25 oct. 2010) $
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 *
 * Biips example : Hidden Markov Model linear gaussian 1D
 * ======================================================
 * x[0] --> x[1] --> ... --> x[t] --> ...
 *           |                |
 *           v                v
 *          y[1]     ...     y[t]     ...
 *
 *          x[0] ~ Normal(mu_x0, sig_x0)
 * x[t] | x[t-1] ~ Normal(x[t-1], sig_x) for all t>0
 *   y[t] | x[t] ~ Normal(x[t], sig_y)  for all t>0
 *
 */

#include "BiipsCore.hpp"
#include "BiipsBase.hpp"

int main(int argc, char* argv[])
{

  using namespace Biips;

 /*==================================*
  *                                  *
  * Instantiate the model with Biips *
  *                                  *
  *==================================*/

  // Define the data :
  //------------------
  // Final time :
  Size t_max = 20;
  // Initial mean of X[0]
  Scalar mu_x0_val = 0;
  // Variance of X[0] :
  Scalar sig_x0_val = 1;
  // Variance of X[t] | X[t-1] :
  Scalar sig_x_val = 1;
  // Variance of Y[t] | Y[t] :
  Scalar sig_y_val = 0.5;

  // Load the Base module :
  //-----------------------
  loadBaseModule();


  // Declare the Graph object :
  //---------------------------
  Graph graph;

  // Add the constant nodes to the graph :
  //--------------------------------------

  NodeId mu_x0 = graph.AddConstantNode(DataType(mu_x0_val));
  NodeId sig_x0 = graph.AddConstantNode(DataType(sig_x0_val));
  NodeId sig_x = graph.AddConstantNode(DataType(sig_x_val));
  NodeId sig_y = graph.AddConstantNode(DataType(sig_y_val));

  // Create stochastic NodeId collections to store X and Y's components identifiers :
  //---------------------------------------------------------------------------------
  Types<NodeId>::Array x(t_max+1);
  Types<NodeId>::Array y(t_max);

  // Create a DimArray object used to store the dimension of the nodes :
  //--------------------------------------------------------------------
  DimArray::Ptr scalar_dim(new DimArray(2,1));

  // Create a NodeId array to handle the parents of each stochastic node :
  //----------------------------------------------------------------------
  Types<NodeId>::Array params(2);

  // Add X_0 stochastic node to the graph :
  //---------------------------------------
  params[0] = mu_x0;
  params[1] = sig_x0;
  x[0] = graph.AddStochasticNode(scalar_dim, "dnorm", params, false);

  // Add the other stochastic nodes to the graph :
  //----------------------------------------------
  for (Size t=1; t<t_max+1; ++t)
  {
    // Add X[t]
    params[0] = x[t-1];
    params[1] = sig_x;
    x[t] = graph.AddStochasticNode(scalar_dim, "dnorm", params, false);

    // Add Y[t]
    params[0] = x[t];
    params[1] = sig_y;
    y[t-1] = graph.AddStochasticNode(scalar_dim, "dnorm", params, true);
  }

  // Check if the graph has cycles :
  //--------------------------------
  Bool has_cycle = graph.HasCycle();

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

  // Declare the SMCSampler object :
  //--------------------------------
  SMCSampler sampler(nb_particles, &graph, &my_rng);

  // Push NodeSampler factories into the list :
  //-------------------------------------------
  sampler.PushNodeSamplerFactory(ConjugateNormalFactory::Instance());

  // Initialize the SMCSampler object :
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
