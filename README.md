BiiPS
=============================================================================
BiiPS is a general software for Bayesian inference with interacting Particle Systems.

- Version:       0.8.1
- Last modified: 2014-03-14
- Maintainer:    biips-project <biips-project@lists.gforge.inria.fr>
- License:       GPL-3
- Website:       http://alea.bordeaux.inria.fr/biips

The project has started on Dec. 2009 and is supported by the [team ALEA](http://alea.bordeaux.inria.fr/) at [Inria Bordeaux](http://www.inria.fr/centre/bordeaux).
Its main objective is to popularize the use of particle methods to researchers, practicians and students by developing a user-friendly software similar to BUGS/JAGS.

Overview
-----------------------------------------------------------------------------
 BiiPS is a general software for Bayesian inference with interacting particle systems, a.k.a. sequential Monte Carlo (SMC) methods.
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
- Black-box particle algorithms for filtering and smoothing
- Fixed parameter estimation using particle MCMC
- Core developped in C++
- R interface: RBiips
- Matlab interface: MatBiips
- Free and open source software (GPL)
- Multi-platform: Linux, Windows, Mac OS X
- Automatically chooses the proposal samplers.
- Module with expandable set of functions, distributions and samplers.

Contact
-----------------------------------------------------------------------------
To contact us with non-public matters, please write to <biips-project@lists.gforge.inria.fr>.
Otherwise you can use the discusssion forum.

Getting Help
----------------------------------------------------------------------------- 
The discusssion forum can be used for any question or feedback about BiiPS.
This is the best place for getting help. All BiiPS users are invited to participate.

Feel free to use it at https://gforge.inria.fr/forum/?group_id=2515

We recommend [creating an account](https://gforge.inria.fr/account/register.php) at GForge Inria.
In the registration form, you can enter "ALEA" in the *INRIA research team* field.

Development
----------------------------------------------------------------------------- 
The core software libraries are written in C++.
An R package allows running BiiPS from the R statistical software and provides posterior analysis and plotting functions.
A Matlab toolbox provides similar capabilities.

The BiiPS source code is hosted on a git repository at Sourceforge. If you want to keep track of the development version, you can find downloading instructions here

  https://gforge.inria.fr/projects/biips/

License
----------------------------------------------------------------------------- 
BiiPS is licensed under the GPL-3 License. You may freely modify and redistribute it under certain conditions (see the file COPYING for details).

Authors
-----------------------------------------------------------------------------
BiiPS code is Copyright (C) Inria, 2012 

Authors:
- [Adrien TODESCHINI](http://fr.linkedin.com/in/adrientodeschini/en), software designer and developer
- [Francois CARON](http://www.math.u-bordeaux1.fr/~fcaron/), probability expertise and specifications
- Marc FUENTES, software designer and developer

BiiPS code is adapted from:
- JAGS, Copyright (C) Martyn Plummer, 2002-2010
- SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
  
Additional information concerning adapted open source software
is included in the file NOTICES.

Thanks
-----------------------------------------------------------------------------
Thanks for supporting the project:
- ALEA project-team at Inria Bordeaux (http://alea.bordeaux.inria.fr)
and especially:
- Pierre Del Moral (Head of ALEA team): guidelines, probability expertise, testing methodology
- Pierrick Legrand: continuous project monitoring, software deployment expertise

Logo design:
- Timothee Del Moral
