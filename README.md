BiiPS
=============================================================================
BiiPS software is a set of C++ libraries for
Bayesian inference with interacting Particle Systems.

- Version:       0.8.1
- Last modified: 2013-11-19
- Maintainer:    biips-project <biips-project@lists.gforge.inria.fr>
- License:       GPL-3
- Website:       http://alea.bordeaux.inria.fr/biips
  
The project has started on Dec. 2009 and is supported by the [team ALEA](http://alea.bordeaux.inria.fr/) at [Inria Bordeaux](http://www.inria.fr/centre/bordeaux).

Overview
-----------------------------------------------------------------------------
 BiiPS is free software, licensed under the GPL.
 It borrows from the BUGS/JAGS software, widely used in Bayesian statistics, the statistical modeling with graphical models and the language associated with their descriptions.
 Both tools can be used by non-statisticians, via their automated "black box" inference engines.
 Unlike BUGS oriented MCMC (Markov chain Monte Carlo), BiiPS software is based on sequential Monte Carlo algorithms (SMC), also referred to as "interacting particles" or "particle filtering" and is thus more adapted to dynamic problems (tracking, signal filtering, etc).

Features
-----------------------------------------------------------------------------
Some of its features include:

- BUGS language compiler to create a graphical model.
- Black-box particle algorithms (filtering and smoothing).
- Automatically chooses the proposal samplers.
- Module with expandable set of functions, distributions and samplers.
- Runs on all major platforms (Windows, Mac, and Linux).
- Interface with the [R statistical computing software](http://www.r-project.org).

For more information on BiiPS please visit the [project website](http://alea.bordeaux.inria.fr/biips).

Authors
-----------------------------------------------------------------------------
BiiPS code is Copyright (C) Inria, 2012 

Authors:
- Adrien TODESCHINI, software designer and developer
- Francois CARON, probability expertise and specifications
- Marc FUENTES, software designer and developer

BiiPS code is adapted from:
- JAGS, Copyright (C) Martyn Plummer, 2002-2010
- SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
  
Additional information concerning adapted open source software
is included in the file NOTICES.

Getting the Code
-----------------------------------------------------------------------------
BiiPS is licensed under the GPL-3, the terms of which are included in the file COPYING.

You can find our source code repository on GForge Inria at:
  https://gforge.inria.fr/projects/biips/

Documentation
-----------------------------------------------------------------------------
For information on how to use BiiPS check out our [online documentation](http://alea.bordeaux.inria.fr/biips).

See also the following files included with the distribution:

- COPYING - BiiPS license
- AUTHORS - BiiPS authors
- NOTICES - Additional open source software included in BiiPS
- INSTALL - How to build and install BiiPS from source
  
If you have problems or want to share feedback with us please visit [our website](http://alea.bordeaux.inria.fr/biips).
For other inquiries you can also email us at [biips-project <biips-project@lists.gforge.inria.fr>](mailto:biips-project <biips-project@lists.gforge.inria.fr>).

Thanks
-----------------------------------------------------------------------------
Thanks for supporting the project:
- ALEA project-team at Inria Bordeaux (http://alea.bordeaux.inria.fr)
and especially:
- Pierre Del Moral (Head of ALEA team): guidelines, probability expertise, testing methodology
- Pierrick Legrand: continuous project monitoring, software deployment expertise

Logo design:
- Timothee Del Moral

