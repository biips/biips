Biips
=============================================================================
**Biips** is a probabilistic programming software for **Bayesian inference with interacting particle systems**.

- Version:       0.9.1
- Last modified: 2014-10-29
- Maintainer:    biips-project <biips-project@lists.gforge.inria.fr>
- License:       GPL-3
- Website:       <http://alea.bordeaux.inria.fr/biips>

The project has started on Dec. 2009 and is supported by the [ALEA team](http://alea.bordeaux.inria.fr/) at [Inria Bordeaux](http://www.inria.fr/centre/bordeaux).
Its main objective is to popularize the use of particle methods to researchers, practicians and students by developing a user-friendly software similar to [BUGS](http://www.mrc-bsu.cam.ac.uk/software/bugs/)/[JAGS](http://mcmc-jags.sourceforge.net/).

Overview
-----------------------------------------------------------------------------
 Biips is a probabilistic programming software for Bayesian inference with interacting particle systems, a.k.a. sequential Monte Carlo (SMC) methods.
 It aims at popularizing the use of these methods to non-statistician researchers and students, thanks to its automated "black box" inference engine.
 It borrows from the BUGS/JAGS software, widely used in Bayesian statistics, the statistical modeling with graphical models and the language associated with their descriptions.
 Unlike MCMC methods used by BUGS/JAGS, SMC methods are more adapted to dynamic problems (tracking, signal filtering, etc).

More context
-----------------------------------------------------------------------------
Bayesian inference consists in approximating an unknown parameter dependent conditional probability law given a set of observations. A large number of problems, e.g. non-supervised classification, filtering, etc., can be addressed having as basis the aforementioned formulation. The underlying probability law, while not calculable in analytical manner for the general case, can be approximated by using Monte Carlo Markov Chain (MCMC) methods. These methods are popular for bayesian inference thanks to BUGS software and the WinBUGS graphical interface.

Emerged as a result of recent research studies, interacting particle based algorithms – a.k.a. Sequential Monte Carlo (SMC) methods in which the most common implementation is the particle filter – proved to have superior performances when compared to classical MCMC approaches. What is more, interacting particle algorithms are well adapted for dynamic estimation problems as encountered, for example, in filtering, tracking or classification problems. They do not require burn in convergence time and include calculation of the normalizing constant.

Features
-----------------------------------------------------------------------------
- BUGS language compatible
- Black-box inference engine with:
    - SMC/particle algorithms for filtering and smoothing
    - Static parameter estimation using particle MCMC
    - Automatic choice of the proposal samplers
- Core developped in C++
- R interface: Rbiips
- Matlab/Octave interface: Matbiips
- Easy language extensions with user-defined R and Matlab functions
- Multi-platform: Linux, Windows, Mac OSX
- Free and open source (GPL)

Roadmap
-----------------------------------------------------------------------------
- Optimization
- Parallelization
- Particle Gibbs algorithm

Contact
-----------------------------------------------------------------------------
To contact us with non-public matters, please write to <biips-project@lists.gforge.inria.fr>.
Otherwise you can use the discusssion forum.

Getting Help
----------------------------------------------------------------------------- 
The discusssion [forum](https://gforge.inria.fr/forum/?group_id=2515) can be used for any question or feedback about Biips.
This is the best place for getting help. All Biips users are invited to participate.

We recommend [creating an account](https://gforge.inria.fr/account/register.php) at GForge Inria.
In the registration form, you can enter "ALEA" in the *INRIA research team* field.

Development
----------------------------------------------------------------------------- 
The core software libraries are written in C++.
An R package allows running Biips from the R statistical software and provides posterior analysis and plotting functions.
A Matlab toolbox provides similar capabilities.

The Biips source code is hosted on a git repository at GForge Inria. If you want to keep track of the development version, you can find downloading instructions on the [GForge Biips project](https://gforge.inria.fr/scm/?group_id=2515).

License
----------------------------------------------------------------------------- 
Biips is licensed under the GPL-3 License. You may freely modify and redistribute it under certain conditions (see the file COPYING for details).

The source code is freely accessible at [GForge Inria](https://gforge.inria.fr/projects/biips/).

Authors
-----------------------------------------------------------------------------
Biips code is Copyright (C) Inria, 2010-2014.

Authors:

- [Adrien TODESCHINI](https://sites.google.com/site/adrientodeschini/), software designer and developer
- [Francois CARON](http://www.stats.ox.ac.uk/~caron/), probability expertise and specifications
- Marc FUENTES, software designer and developer

Biips code is adapted from:

- [JAGS](http://mcmc-jags.sourceforge.net/), Copyright (C) Martyn Plummer, 2002-2010
- [SMCTC](http://www2.warwick.ac.uk/fac/sci/statistics/staff/academic-research/johansen/smctc/), Copyright (C) Adam M. Johansen, 2008-2009
  
Additional information concerning adapted open source software
is included in the file NOTICES.

Thanks
-----------------------------------------------------------------------------
Thanks for supporting the project:

- [ALEA project-team](http://alea.bordeaux.inria.fr) at [Inria Bordeaux](http://www.inria.fr/centre/bordeaux)
and especially:
- [Pierre Del Moral](http://www.math.u-bordeaux1.fr/~pdelmora/) (Head of ALEA team): guidelines, probability expertise, testing methodology
- [Pierrick Legrand](http://www.sm.u-bordeaux2.fr/~legrand/): continuous project monitoring, software deployment expertise

Logo design:

- [Timothée Del Moral](http://rubis3.blogspot.fr/)
