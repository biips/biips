//                                               -*- C++ -*-
/*! \file MoveSet.hpp
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 */

#ifndef BIIPS_MOVESET_HPP_
#define BIIPS_MOVESET_HPP_

#include <smctc.hh>

#include "graph/GraphTypes.hpp"

namespace Biips
{
  typedef smc::particle<NodeValues> Particle;
  

  class MoveSet : protected smc::moveset<NodeValues>
  {
  protected:
    typedef smc::moveset<NodeValues> BaseType;

    ///The function which initializes a particle.
    Particle (*pfInitialise_)(Rng*) ;
    ///The functions which perform actual moves.
    void (*pfMove_)(long, Particle &, Rng*);
  public:

    /// Create a completely unspecified moveset
    /// The argument free MoveSet constructor simply sets the function pointers to NULL.
    MoveSet() : pfInitialise_(NULL), pfMove_(NULL) {};
    ///Create a reduced moveset with a single move
    /// The two argument MoveSet constructor creates a new set of moves and sets all of the relevant function
    /// pointers to the supplied values. Only a single move should exist if this constructor is used.
    /// \param pfInit The function which should be used to initialize particles when the system is initialized
    /// \param pfNewMove A function which moves a particle at a specified time to a new location
    MoveSet(Particle (*pfInit)(Rng*), void (*pfNewMove)(long, Particle &, Rng*))
    : pfInitialise_(pfInit), pfMove_(pfNewMove) {};

    ///Initialize a particle
    Particle DoInit(Rng * pRng) { return (*pfInitialise_)(pRng); };
    ///Select an appropriate move at time lTime and apply it to pFrom
    void DoMove(long lTime, Particle & pFrom, Rng * pRng) { (*pfMove_)(lTime, pFrom, pRng); };
    void DoMCMC(long lTime, Particle & pFrom, Rng * pRng) {};
  };

}

#endif /* BIIPS_MOVESET_HPP_ */
