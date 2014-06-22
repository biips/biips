Adrien le 3/6/2014 :
====================
J'ai enfin réussi à compiler matbiips avec Visual Studio.
J'ai d'abord tenté de compiler, d'une part, les librairies core, base et compiler avec cmake et VS et, d'autre part, matbiips avec la commande mex de matlab, faisant appel à VS.
J'ai dû résoudre pas mal de conflits en passant à VS (différences sur la librairie standard C++) pour finalement être coincé à l'édition des liens en compilant matbiips.
Finalement, j'ai opté pour compiler tout le code biips+matiips avec mex.
Par contre, le crash est toujours là... snif
J'ai pu compiler en debug et
- [ ] je vais pouvoir investiguer ce bug avec cette méthode:
  http://www.mathworks.fr/fr/help/matlab/matlab_external/debugging-on-microsoft-windows-platforms.html
  ce qui n'était pas encore possible.

François le 16/4/2014 :
=======================
J'ai commence a bosser sur la page d'exemples:
https://alea.bordeaux.inria.fr/biips/doku.php?id=examples

Adrien le 16/4/2014 :
=====================
Hello François,
Ce bug est corrigé. Il mais m'a fait partir dans plusieurs directions mais il était finalement très simple.
Il suffisait de faire un premier change-data avec les inits avant de monitorer les latentes.
Sinon :
* biips_init n'est plus nécessaire
* biips_model ne renvoie plus data
* autres corrections mineurs
* j'ai modifié les exemples et test en conséquence
Tu peux généralement utiliser les fonctions all/any au lieu de prod/sum pour tester une condition sur un vecteur de booleens.

François le 11/4/2014 :
=======================
Salut Adrien,
As-tu changer qq chose pour le monitoring des noeuds observes dans le SMC?

J'ai un modele avec (attache)
    tau ~ dgamma(a,b)
    sigma <- 1/sigma^2

Je fais tourner un PMMH avec tau comme variable de parametre, et sigma 
comme latente (pour recuperer directement ses valeurs depuis biips).
Avec la precedente version du PMMH, je n'avais pas de soucis. Maintenant 
il rale avec l'erreur suivante:

    Warning: LOGIC ERROR: Observed nodes can't have sampling iteration!

    Warning: Failure running SMC forward sampler
     > In matlab\private\pmmh_one_update at 60
       In biips_pmmh at 116
       In biips_pmmh_update at 59
       In switch_stoch_volatility_param at 188
    Warning: LOGIC ERROR: Node is not monitored, in NodeArrayMonitor::addMonitoredNode.

    Error using inter_biips
    sample_gen_tree_smooth_particle: BiiPS c++ exception: Failed to sample smooth particle.

Vois-tu d'ou vient le probleme? Est-ce un bout de code que j'ai oublie de mettre.

Adrien le 1/4/2014 :
====================
- [ ]  revoir dimensions et affichage de la sortie de biips_get_nodes.

François le 25/3/2014 :
=======================
- [x] Il faudrait une fonction biips add_function_rnd, qui rajouterait une 
    fonction pour simuler, que l'on ne pourrait appeler dand bugs que pour 
    des noeuds stochastiques non observés. (sinon renvoit une erreur). On ne 
    prendrait pas en compte le cas plus compliqué des noeuds observés, car 
    dans ce cas il faudrait fournir la densité.

    Je sais que c'est encore un ajout au C++, mais j'ai l'impression que 
    c'est un ajout relativement simple et qui ajoute beaucoup, car on peut 
    traiter des cas pour lesquels on n'a pas accès à l'évaluation de 
    p(x_t|x_t-1), on sait juste simuler selon cette loi, et donc les MCMC 
    standards ne sont pas applicables.

Adrien le 24/3/2014 :
=====================
- [x] Pour les crashs windows, essyer de désactiver ReleaseNodes dans sensitivity et pmmh --> ca n'a rien donné
- [ ] Faire tourner les exemples sous Jags, notamment `switching_stoch_volatility`
- [ ] pmmh: modifier traitement sortie `get_log_prior_density`, NaN = erreur numérique
- [ ] Tester validité des distributions et samplers

François le 21/3/2014 :
=======================
- [x] Il y a un petit bug dans inter_biips weighted quantiles. Quand tu lui 
    des donne des probas non ordonnees, il te les retourne dans le mauvais 
    ordre (c'est-a dire ordoneees).

Adrien le 20/3/2014 :
=====================
- [ ] faire une liste des fonctions, distributions de biips avec les paramètrisations (cf tables du manuel de jags)
- [ ] faire en sorte que `change_data` vérifie qu'une valeur de noeud stochastique est dans le support de sa distribution avant de mettre à jour les enfants.
    Et dans ce cas, ne pas afficher de message d'erreur mais trouver un moyen silencieux d'avertir l'utilisateur (booleen = false?)
- [ ] corriger la densité de dbeta, pour qu'elle retourne `-Inf` si pb de bornes. actuellemnt retourne `NaN` parfois
- [ ] vérifier les calculs de log density dans toutes les distributions de Biips
- [ ] tester les crashs sous octave
  --> Il y a bien le meme bug dans octave sous windows. Par exemple `stoch_kinetic` fait crasher octave avec le message:
        panic: segmentation violation
- [ ] faire version R des `tutorialsX.m`, en rajoutant explications si besoin et repasser sur les labels, boxoff etc.
- [ ] faire publish de l'exemple `switch_stoch_volatility.m` sous linux

Note: on peut générer un fichier `contents.m` avec matlab depuis le répertoire en question: Reports -> Contents report 

Questions en suspens:
- [ ] est-ce qu'on applique une transformation des paramètres pour la marche aléatoire en fonction de leur support ?

A rajouter dans la doc de `inter_biips` :
`get_log_prior_density` lance une erreur si :
* pas de modèle
* variable non existente
* bornes invalides
* noeud non observé
* noeud stochastique avec parents non observés
* valeurs invalides pour le calcul de la densité
* autres lancement d'exceptions...

La valeur de retour est :
* NA si le noeud est constant ou logical (dans matlab ce NA est transformé en NaN)
* -Inf si hors des bornes

Note: un `NaN` peut aussi être renvoyé pour des erreurs numériques. Par exemple dans les cas suivants :
* `Inf-Inf`
* `Inf/Inf`
- [ ] ne doit-on pas lancer une erreur dans ce cas ?

Adrien le 18/3/2014 :
=====================
- [ ] enlever les bornes inutiles dans les arguments de `biips_sensitivity` et `biips_pmmh_samples`  des tutoriels et exemples 
- [ ] `biips_build_sampler` doit renvoyer une structure décrivant les itérations du SMC. Pour chaque itération :
    * liste des noeuds stochastiques mis à jour
    * liste des samplers
    * liste des noeuds logiques mis à jour
    * liste des observations de conditionnement
- [ ] Concernant les crashs MATLAB: est-ce que ça crashe dès la première exécution ou seulement à partir de la deuxième ?
- [ ] enlever C++0X/11 ?
- [ ] enlever boost regex (utiliser std::string ou autre?)
- [ ] crash MATLAB sur exemple `stoch_kinetic`

>> stoch_kinetic
* Parsing model in: stoch_kinetic_cle.bug
* Compiling data graph
   Declaring variables
   Resolving undeclared variables
   Allocating nodes

------------------------------------------------------------------------
           Access violation detected at Tue Mar 18 15:39:11 2014
------------------------------------------------------------------------

Configuration:
   Crash Decoding     : Disabled
   Default Encoding   : windows-1252
   MATLAB Architecture: win64
   MATLAB Root        : C:\Program Files\MATLAB\R2013b
   MATLAB Version     : 8.2.0.701 (R2013b)
   Operating System   : Microsoft Windows 7
   Processor ID       : x86 Family 6 Model 58 Stepping 9, GenuineIntel
   Virtual Machine    : Java 1.7.0_11-b21 with Oracle Corporation Java 
HotSpot(TM) 64-Bit Server VM mixed mode
   Window System      : Version 6.1 (Build 7601: Service Pack 1)

Fault Count: 1


Abnormal termination:
Access violation

Register State (from fault):
   RAX = 33847c90ae6f02ce  RBX = 00000000c0fb9d30
   RCX = 000000ffffffffff  RDX = 00000000c150ab70
   RSP = 000000000401da30  RBP = 0000000000000001
   RSI = 0000000006e30000  RDI = 00000000c150ab80

    R8 = 3832d9c90fbc0a64   R9 = fe72ffff206a0a65
   R10 = 0000000000000000  R11 = 0000000000000000
   R12 = 3fe01ada45dea4c6  R13 = 0000000100000001
   R14 = ffffffff00007fff  R15 = 00000000ffff0000

   RIP = 00000000770f32f2  EFL = 00010286

    CS = 0033   FS = 0053   GS = 002b

Stack Trace (from fault):
[  0] 0x00000000770f32f2 C:\Windows\SYSTEM32\ntdll.dll+00340722 RtlFreeHeap+00000306
[  1] 0x000007fefe3810c8 C:\Windows\system32\msvcrt.dll+00004296 free+00000028
[  2] 0x000000001f9281bb D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+01868219 ZN5boost6detail17sp_counted_impl_pIN5Biips8ValArrayEE7disposeEv+00000027
[  3] 0x000000001f925bb9 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+01858489 ZN5boost6detail12shared_countD1Ev+00000057
[  4] 0x000000001f929170 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+01872240 ZN5boost6detail22remove_vertex_dispatchINS_14adjacency_listINS_4vecSES3_NS_14bidirectionalSENS_8propertyINS_17vertex_node_ptr_tENS_10shared_ptrIN5Biips4NodeEEENS5_INS_17vertex_observed_tEbNS5_INS_17vertex_discrete_tEbNS5_INS_14vertex_value_tENS7_INS8_8ValArrayEEENS_11no_propertyEEEEEEEEESG_SG_NS_5listSEEEyEEvRT_T0_NS_17bidirectional_tagE+00000656
[  5] 0x000000001f8669c7 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+01075655 ZN5Biips5Graph7PopNodeEv+00000487
[  6] 0x000000001f80b4ba D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00701626 ZN5Biips8Compiler15IndexExpressionEPK9ParseTreeRi+00000842
[  7] 0x000000001f80d9e5 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00711141 ZN5Biips8Compiler8getRangeEPK9ParseTreeRKNS_10IndexRangeE+00002245
[  8] 0x000000001f80f010 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00716816 ZN5Biips8Compiler14getArraySubsetEPK9ParseTree+00000800
[  9] 0x000000001f80a692 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00698002 ZN5Biips8Compiler12GetParameterEPK9ParseTree+00000386
[ 10] 0x000000001f811cba D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00728250 ZN5Biips8Compiler15setConstantMaskEPK9ParseTree+00002842
[ 11] 0x000000001f80a8f8 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00698616 ZN5Biips8Compiler12GetParameterEPK9ParseTree+00001000
[ 12] 0x000000001f811cba D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00728250 ZN5Biips8Compiler15setConstantMaskEPK9ParseTree+00002842
[ 13] 0x000000001f80a8f8 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00698616 ZN5Biips8Compiler12GetParameterEPK9ParseTree+00001000
[ 14] 0x000000001f811ff3 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00729075 ZN5Biips8Compiler15allocateLogicalEPK9ParseTree+00000387
[ 15] 0x000000001f8136f0 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00734960 ZN5Biips8Compiler8allocateEPK9ParseTree+00000400
[ 16] 0x000000001f813e5f D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00736863 ZN5Biips8Compiler12traverseTreeEPK9ParseTreeMS0_FvS3_Eb+00000671
[ 17] 0x000000001f8140f4 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00737524 ZN5Biips8Compiler12traverseTreeEPK9ParseTreeMS0_FvS3_Eb+00001332
[ 18] 0x000000001f817393  D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00750483 ZN5Biips8Compiler14WriteRelationsEPK9ParseTree+00001091
[ 19] 0x000000001f802203 D:\Projects\biips\matbiips\matlab\inter_biips.mexw64+00664067 ZN5Biips7Console7CompileERSt3mapISsNS_10MultiArrayESt4lessISsESaISt4pairIKSsS2_EEEbjj+00003107

Marc le 17/3/2014 :
===================
Compilation Matbiips Octave sur CI:

sur ubuntu
        -DBUILD_MATBIIPS=ON  -DCMAKE_BUILD_TYPE:STRING=Release -DFIND_OCTAVE=ON
        -DOCTAVE_ROOT=/usr/bin 

sur windows : 
        -DCMAKE_CXX_COMPILER="C:/Rtools/gcc-4.6.3/bin/g++.exe"
        -DCMAKE_MAKE_PROGRAM="C:/MinGW/bin/mingw32-make.exe" -DBUILD_MATBIIPS=ON
        -DFIND_OCTAVE=ON  -DCMAKE_BUILD_TYPE:STRING=Release -DBUILD_RBIIPS=OFF
        -DOCTAVE_ROOT="C:/Program Files/octave/bin/" -DBUILD_64BIT=OFF 
avec un script pré-chargé (-C)  `env_ouin_octave.cmake`
        set(ENV{BOOST_ROOT} "c:/boost_1_50_0/")
        set(ENV{BOOST_ROOT} "c:/boost_1_50_0/stage32/lib")
        add_definitions(-m32)

Adrien le 13/3/2014 :
=====================
Biips compile en 32bits sous windows 64bits.

1. Compiler boost en 32 bits et installer dans le sous-répertoire stage32/lib :

		b2 toolset=gcc address-model=32 --stagedir=stage32 --build-type=complete --with-program_options --with-regex stage

2. Compiler Biips avec les options suivantes
		-DBUILD_64BIT=OFF 
		-DBOOST_LIBRARYDIR="%BOOST_ROOT%/stage32/lib"

Par contre je n'ai que Matlab 64bits sur cette VM et n'ai pas pu compiler matbiips pour matlab en 32bits.

François le 13/3/2014 :
=======================
Fait:
* Retirer l'argument seed des fonctions
* Renommer les fonctions matlab pimh dans private pour avoir pimh_***
* Ajouter en-tetes PIMH
J'ai ajouté une fonction `test_crash5.m` qui identifie un probleme avec l'utilisation de matrice dans le pimh/pmmh
- [ ] Resoudre le probleme dans `test_crash5.m`

François le 12/3/2014 :
=======================
Probleme dans `change_data` lorsque l'on a une variable multiD, mais associée avec différents noeuds stochastiques dans biips, e.g.
        x[1] ~dnorm(0,1)
        x[2] ~dnorm(0,1)
Dans ce cas, on ne peut pas faire `change_data` avec `x[1:2]`
voir exemple dans `test_crash4`
message d'erreur:
        Warning: RUNTIME ERROR: Can not change data: variable x[1:2]
        does not match one node exactly.
    
- [ ] resoudre le probleme si pas trop compliqué, sinon mettre un message d'erreur un peu plus explicite 
    --> Actuellement, utiliser {'x[1]', 'x[2]'} au lieu de {'x[1,2]'}

François le 11/3/2014 :
=======================
Francois: 
- [x] 3 tutos et 3 exemples en suivi d'objet, volatilite et estimation de densite finis en matbiips. 
- [ ] Ajouter doc PMMH matbiips
- [ ] finir le dernier exemple
- [x] refaire une passe sur la doc `inter_biips`

Marc: 
- [x] doc `inter_biips` (matbiips) finie
- [x] Mettre les mexfile linux Matlab et Otave sur git
- [x] pb pour mexfile octave windows car octave est en 32 bits. Avec Adrien, faire octave 32bits windows

Adrien: 
- [x] Pb dans `change_data` lorsque l'on ne fourni pas les dimensions des variable (lower et upper not defined)
- [ ] Regarder bug exemple stochastic kinetic
- [ ] ajouter les lois conditionnelles
- [ ] quand exemples matbiips finis, transcrire en Rbiips
- [ ] mexfile windows 32 bits
- [ ] quand doc PMMH matbiips finie, verifier et transcrire dans RBiips

- [ ] On vise d'avoir la version et les tutos/exemples en ligne avec l'article sur arxiv pour la fin du mois.

Adrien le 11/3/2014 :
=====================
- [ ] Modifier les benchmarks testcompiler: ne pas inclure le temps initial dans le calcul RMSE.
- [ ] Utiliser la même taille de fenêtre pour les densités de smoothing et filtering dans `biips_density` de matbiips.
- [ ] Retourner le mode dans `biips_summary` pour les lois discrètes
- [ ] Calculer un histogramme dans `biips_density` pour les lois discrètes

François le 5/3/2014 :
======================
Je me bats avec le modele de stochastic kinetic. Le mexfile bugge a des 
endroits differents sans infos particulieres (des fois a la compilation, 
d'autre fois dans le `pmmh_samples`, etc...). J'ai eu une fois l'erreur 
suivante:
		Warning: RUNTIME ERROR: Node x_true[2,24] overlaps previously defined nodes

- [x] Test sous Linux ok
- [ ] Le pb doit venir de windows. Chez moi, cela plante plus d'une fois 
sur deux. En general je n'ai pas de message d'erreur, j'ai juste une segmentation fault.

MATLAB crash file:C:\Users\adrien\AppData\Local\Temp\matlab_crash_dump.3656-1:


------------------------------------------------------------------------
	  Access violation detected at Wed Mar 12 22:35:43 2014
------------------------------------------------------------------------

Configuration:
  Crash Decoding     : Disabled
  Default Encoding   : windows-1252
  MATLAB Architecture: win64
  MATLAB Root        : C:\Program Files\MATLAB\R2013b
  MATLAB Version     : 8.2.0.701 (R2013b)
  Operating System   : Microsoft Windows 7
  Processor ID       : x86 Family 6 Model 58 Stepping 9, GenuineIntel
  Virtual Machine    : Java 1.7.0_11-b21 with Oracle Corporation Java HotSpot(TM) 64-Bit Server VM mixed mode
  Window System      : Version 6.1 (Build 7601: Service Pack 1)

Fault Count: 1


Abnormal termination:
Access violation

Register State (from fault):
  RAX = 3ca8ac4d00b9fa98  RBX = 000000001b0a4020
  RCX = 000000ffffffffff  RDX = 000000001af89780
  RSP = 0000000004015810  RBP = 0000000000000001
  RSI = 0000000008190000  RDI = 000000001af89790
 
   R8 = 000000001b055f70   R9 = ed27d9036d025f71
  R10 = 000000001826a900  R11 = 0000000000000000
  R12 = 3db12d828ac1c754  R13 = 0000000100000001
  R14 = ffffffff00007fff  R15 = 00000000ffff0000
 
  RIP = 00000000779532f2  EFL = 00010286
 
   CS = 0033   FS = 0053   GS = 002b

Stack Trace (from fault):
[  0] 0x00000000779532f2                      C:\Windows\SYSTEM32\ntdll.dll+00340722 RtlFreeHeap+00000306
[  1] 0x000007feffb610c8                     C:\Windows\system32\msvcrt.dll+00004296 free+00000028
[  2] 0x0000000017a3dbab C:\Users\adrien\workspace\biips-git\matbiips\matlab\inter_biips.mexw64+01825707 ZN5boost6detail17sp_counted_impl_pIN5Biips8ValArrayEE7disposeEv+00000027
[  3] 0x00000000179902de C:\Users\adrien\workspace\biips-git\matbiips\matlab\inter_biips.mexw64+01114846 ZN5Biips14ForwardSampler12ReleaseNodesEv+00000190
[  4] 0x00000000179a26df C:\Users\adrien\workspace\biips-git\matbiips\matlab\inter_biips.mexw64+01189599 ZN5Biips5Model11InitSamplerEjPNS_3RngERKSsd+00001663
[  5] 0x000000001791c95f C:\Users\adrien\workspace\biips-git\matbiips\matlab\inter_biips.mexw64+00641375 ZN5Biips7Console17RunForwardSamplerEjjRKSsdjb+00000927
[  6] 0x00000000178873e1 C:\Users\adrien\workspace\biips-git\matbiips\matlab\inter_biips.mexw64+00029665 mexFunction+00017985


Adrien le 2/3/2014 :
====================
- [ ] Rationaliser l'usage des arguments seed dans matbiips et RBiips.
- [ ] Modifier arguments par défaut de `plot.summary.particles`.

Adrien le 21/02/2014 :
======================
- [ ] améliorer `FindMatlab.cmake` : vérifier les options de compilation utilisées par mex (voir `MATLAB_ROOT/bin/mbuildopts.sh`)
- [ ] vérifier la validité des noms de fonction dans `add_function`
- [x] permettre de remplacer les fonctions avec add_function mais protéger les fonctions du module base
- [x] permettre compilation 32bit sous windows

François le 21/02/2014 :
========================
PMMH quasiment fini en matbiips. Creation d'une fonction `biips_pmmh_object` qui cree une structure contenant les variables utilisees pour la loi de proposition et pour l'adaptation. Maintenant, on ne peut adapter que dans `biips_pmmh_update`. 
J'ai supprimer la possibilite de choisir le type de rescaling et la variable `rw_rescale`, qui est fixee par `n_rescale`.
Exemple `hmm_1d_nonlin` termine. 

A faire dans le PMMH:
- [x] Tester le PMMH et sensitivity sur un exemple ou le parametre est multiD et/ou il y a plusieurs parametres
- [ ] Creer une fonction test permettant de tester avec les differentes options et differentes conditions
- [ ] ajouter entetes fonctions PMMH
- [x] verifier la procedure de rescaling - semble donner des resultats aberrants de temps en temps(valeurs tres faibles ou tres larges)
- [ ] Ajouter une fonction verifiant que l'objet pmmh est valide (verifier les champs de la structure, dimensions, etc)

A faire pour le PIMH:
- [ ]  Ajouter des en-tetes aux fonctions

A faire un peu partout:
- [ ]  retirer l'argument optional 'seed' partout

Adrien le 20/02/2014 :
======================
Concernant `test_crash1.m`, le problème se situe au niveau de la fonction parsevar qui ne traite pas les arguments 'logical' je pense.
En tout cas, sample_data est retourné true au lieu de false par parsevar.
D'après moi, parsevar doit renvoyer une erreur en cas de mauvais paramètre au lieu d'un warning et de prendre la valeur par défaut.

A faire:
- [x] corriger parsevar
- [ ] pb ctest matbiips sous windows: les test passent automatiquement. Regarder s'il est possible d'attendre la bonne sortie de matlab

François le 20/02/2014 :
========================
- [ ] Adrien, peux-tu aussi regarder dans `inter_biips sample_data`, pour autoriser de 
rééchantilloner une variable meme lorsqu'elle existe déjà?
A l'heure actuelle, si la valeur de `log_prec_y` est deja initialisée (par 
exemple parce que l'on a fait appel à biips_sensitivity avant) on ne 
peut pas lancer le pmmh avec initialisation aléatoire de la variable.
sinon, Existe-t-il une fonction pour supprimer la valeur, afin de 
pouvoir la rééchantilloner selon le prior?

Adrien le 18/02/2014 :
======================
- [ ] pb quand on donne une valeur de paramètres dans data: il la ré-échantillonne
- [ ] pb avec pimh qd on veut monitorer `x[1:2,1]`, cf script `test_crash3.m`
- [x] warning qd on ne donne pas de bornes dans `change_data` et `get_log_prior_density`
- [x] remplacer mbiip_cerr par message erreur mex
- [ ] rajouter les test de distribution ds rbiips et matbiips

François le 16/02/2014
======================
- [x] Vérifier s'il n'y a pas un pb dans RBiips.R, ligne 340:
        rw$d <<- sum(sapply(rw$dim, FUN=sum))
Il me semble que FUN devrait prendre le produit des dimensions plutot que la somme.
- [x] biips renvoie une erreur lorsque l'on essaie d'ajouter une fonction qui existe déjà. Ce serait bien de renvoyer juste un warning, et si possible de redéfinir la fonction (la fonction matlab peut avoir changer) - pas urgent ajouter message indiquant qu'il faut fermer matlab dans `biips_add_function` pour pouvoir redéfinir la fonction


François le 13/02/2014 :
========================
Pour dbinom, j'obtiens l'erreur suivante:
        |*Error in node m_up[1,1]
Invalid parameters values in Sample method for distribution dbin
Can't get log normalizing constant. SMC sampler did not finish!

J'ai l'impression que l'erreur vient du fait que dans dbinom(n, p), j'ai 
quelquefois n=0.

- [x] Est-ce que cela peut se gérer?

Adrien le 11/02/2014 :
======================
- [ ] exemples avec publish matlab (avec ou sans le package de Peyré ? plutôt sans...)
- [x] trouver une solution similaire pour R (voir package knitr)
    * --> OK en utilisant la fonction spin() du package knitr
    * voir : http://yihui.name/knitr/demo/stitch/
    * demo : https://github.com/yihui/knitr/blob/master/inst/examples/knitr-spin.R
    * pas sûr qu'on puisse insérer du latex --> Si on peut
- [ ] améliorer message d'erreur `Subset y[1] out of range [1:100] in Compiler::setConstantMask`.
- [ ] éviter crash matlab : test_crash3.m non résolu (le lancer plusieurs fois)
- [x] vérifier : si `sample_data=false` -> ne pas compiler bloc data
- [ ] tester octave sous 
    * [x] linux
    * [x] windows
    * [ ] mac
- [ ] Pb de headers avec octave et gcc 4.8
- [x] regarder warning dans `test_internals.m`

François le 11/02/2014 :
========================
- [ ] J'ai une erreur lorsque j'essaye de rentrer les données y dans la structure data dans `hmm_1d_lin`: (jusqu'à présent je les échantillonais). voir `test_crash2.m`

L'erreur se produit dans `inter_biips compile_model` avec l'erreur suivante:
        * Compiling data graph
          Declaring variables
          Resolving undeclared variables
          Allocating nodes
        LOGIC ERROR: Subset y[1] out of range [1:100] in Compiler::setConstantMask.

- [ ] Par ailleurs, cela crashe toujours matlab. Est-il possible d'éviter de fermer Matlab s'il y a un pb avec le fichier bugs?

        EXEMPLE:
        %%% Model parameters
        t_max = 100;
        mean_x_init = 0;
        prec_x_init = 1;
        prec_x = 1;
        prec_y = 10;
        y = randn(t_max, 1);
        data = struct('y', y, 't_max', t_max, 'prec_x_init', prec_x_init,...
            'prec_x', prec_x,  'prec_y', prec_y, 'mean_x_init', mean_x_init);
        %%% Start BiiPS console
        biips_init;
        %%% Compile BUGS model and sample data
        model = 'hmm_1d_lin.bug'; % BUGS model filename
        sample_data = true; % Boolean
        [model_id, data] = biips_model(model, data, 'sample_data', sample_data); % Create biips model and sample data

François le 09/02/2014 :
========================
A propos des conditionnelles pour le filtering:

Cela n'a pas l'air évident de retrouver les conditionnelles du filtering 
avec `biips_get_nodes`. Serait-il possible que la sortie de 
`get_filter_monitors` renvoie cette information? Par exemple, pour la 
variable x, avoir un champs supplémentaire 'conditionals' qui est une 
cellule de même taille que x, avec pour chaque entrée une cellule 
donnant les noms des variables stochastiques par rapport auuxquelles 
l'on conditionne.
Par exemple pour un x de longueur 3:

        x.conditionals = { { 'y[1]' }, {'y[1]', 'y[2]'}, {'y[1]', 'y[2]', 'y[3]'} }

Cela te paraitrait faisable? Une autre facon de faire, serait de 
retraiter la sortie de `biips_get_nodes`, mais cela me semble moins rigoureux.

Si j'ai l'ordre des noeuds, et l'indice m'indiquant s'ils sont 
stochastiques et observés, la règle suivante est-elle valide dans tous 
les cas:
Pour un noeud stochastique non observé, conditionner par rapport à
1. toutes les variables stochastiques observée le précédant
2. la première variable stochastique observée suivante
3. ajouter les variables stochastiques observées suivantes, jusqu'à ce 
qu'apparaisse un noeud stochastique non observé
Par exemple (x sto non observé, y sto observé, z déterministe)
x[1], y[1], x[2], x[3], y[2], y[3], z[1], y[4], x[4]
x[2] est conditionné à y[1], y[2], y[3], y[4]
si c'est aussi simple que cela, je peux le récupérer à partir de 
`biips_get_nodes`

A faire :
- [x] vérifier que ces conditions sont remplies
  ---> Actuellement: 1 itération = 1 noeud stochastique non observé
       * x[2] est donc seulement conditionné à y[1]
       * x[3] est lui conditionné à y[1], y[2], y[3], y[4]
       - [x] Il faut changer ce comportement
- [ ] retourner les conditionnelles sous la forme présentée plus haut


François le 07/02/2014 :
========================
Modification de parsevar pour inclure vérification des types, et arguments admissibles pour les entrées optionnelles.
Début du codage des fonctions `init_pmmh` et `biips_pmmh_update`

ERREURS avec `inter_biips`: (lancer `hmm_1d_lin_param.m`)
- [x] `change_data` : quels sont les types des entrées de cette fonction?. J'ai essayé 
        inter_biips('change_data', console, pn_param(i).name, ...
            pn_param(i).lower, pn_param(i).upper, inits{i}, true)
où `pn_param` est la sortie de `parse_varname`, mais j'ai un message m'indiquant que le deuxième argument doit etre 'cell'
- [ ] `sample_data`: même question
        samp = inter_biips('sample_data', console, pn_param(i).name,...
                pn_param(i).lower, pn_param(i).upper, inits_rng_seed)
me renvoit un message comme quoi le 3e et 4e arguments doivent etre double. Mais si les dimensions de la variable ne sont pas indiqués, parse_varname renvoit une cell vide.
- [ ] Même en indiquant les indices de la variable, la fonction crashe, cette fois-ci sans message d'erreur.

Questions concernant `inter_biips` et biips:
- [ ] Lorsque l'on rentre dans la structure data les valeurs de `x_true` et `y` (pour `hmm_1d_lin`), on a une erreur à la compilation - ne peut-on pas éviter cela? 
- [x] Message de inter_biips indiquant que seed doit etre double: ce n'est pas supposé etre un entier? La classe uint32 ne serait pas plus appropriée? ---> Réponse: utilisation de double plus commode pour l'utilisateur
- [ ] Dans la fonction `init_pmmh` de Rbiips, pourquoi mettre la valeur `latent_variables` à false quand on les monitor?
- [x] dans `init.pmmh` de rbiips, je ne comprends pas ce que fait `object$.rw.init(sample)`
- [x] idem pour `object$.rw.step(rw.step.values)` et autres dans d'autres fonction pmmh


Adrien le 4/2/2014 :
====================
A faire dans Matbiips :
- [x] modifier lecture des champs de structure : utiliser getfield
- [x] harmonisation des noms de variables
- [x] traitement des sorties MCMC dans `biips_summary` et `biips_density`
    Rq: si out.varname a les champs f, s ou b -> traitement SMC sur les sous-champs values et weights
    sinon -> traitement MCMC : pas de sous-champs values, les poids sont tous égaux
- [ ] `biips_pimh_samples` : améliorer stockage des échantillons. cf. switch/case dans le code
    Rq: L'appel de squeeze modifie les dimensions
- [x] créer exemple court et l'inclure dans matbiips
- [x] supprimer `biips_load_module` : l'intégrer dans `biips_init`
- [ ] vérifier `biips_get_nodes` : peut-on connaître les conditionnelles ? renvoie-t-elle les samplers ?
- [x] renommer `make_progress_bar` en `progress_bar`
- [x] mettre isoctave dans private
- [ ] revoir et ajouter tests matbiips et ne pas les intégrer dans l'archive
- [ ] ajouter un README.md
- [x] commenter `inter_biips` en doxygen et générer pdf --> finalement c'est un doc latex séparé

Autres tâches :
- [ ] tester l'install de RBiips sous linux
- [ ] copier binaires depuis CI sur un répertoire accessible (Dropbox ?)
- [ ] harmonisation Licence, auteurs : Fichiers COPYING et README à la racine + entêtes de fichiers communes avec auteurs, Inria, date etc.


François le 3/2/2014 :
======================
J'ai fini le PIMH et l'exemple hmm 1D linéaire. J'ai modifié les 
fonctions `biips_density` et `biips_summary` de façon a ce qu'elles prennent 
aussi en entree la sortie d'un algorithme MCMC. Dans l'idéal, il 
faudrait aussi faire la même chose pour la fonction biips_diagnosis, 
mais c'est moins evident de savoir quel critere utiliser pour le MCMC 
(en plus, plusieurs criteres utilisent les sorties de plusieurs 
chaines). Je pense que c'est ok de laisser comme cela.
Il faut encore rajouter des en-tetes à certaines fonctions, et quelques 
checks sur les entrees.
J'ai essayé d'harmoniser les notations au maximum.
Je ne pense pas pouvoir retravailler dessus cette semaine, j'attaquerai 
le PMMH la semaine prochaine.

A Faire:
- [x] Commenter un peu `inter_biips`, au moins une description succincte des entrees/sorties et ce que fait chaque fonction (si le nom n'est pas assez explicite).

Marc le 3/2/2014 :
====================
> C'est tout de même gênant que Matlab se ferme à la suite d'une exception.
> Marc, est-ce que tu penses que c'est normal ?

Je sais que Matlab a un systeme a lui pour gerer les exceptions (help
catch), mais je ne sais pas si il peut rattraper une exception issue d'un
mexfile. Ca peut etre genant que matlab ferme surtout si tu n'as pas de
message d'erreur, en effet. Apres definir quel doit etre le bon
conportement dans ce cas, n'est pas evidemment. Ce qui rassure au moins
c'est que le programme plante au lieu de faire n'importe quoi.
Apparement Adrien a pu localiser d'ou venait l'erreur, c'est donc que
les infos d'erreur etaient suffisantes pour regler le probleme.

Adrien le 30/1/2014 :
======================
> J'ai un soucis avec le calcul des quantiles dans matbiips. Il doit y 
> avoir un problème avec la function cpp, car
> `inter_biips('weighted_quantiles', values, weights, probas)` ne renvoit 
> pas les bonnes valeurs.

En fait, il faut juste multiplier les poids par N, c.f. `stat.particles` dans RBiips.
Je ne comprends plus pourquoi mais ça marche... sûrement un problème numérique !?
Je suppose que l'algo renormalise tout seul. J'ai corrigé `summary.m`.

- [ ] Je vais voir s'il ne vaut pas mieux modifier dans Biips (c++)

Adrien le 27/01/2014 :
======================
A faire:
- [x] Changer les noms de fonctions dans Rbiips : 
	- `update.pimh` -> `pimh.update`
	- `update.pmmh` -> `pmmh.update`
- [x] Améliorer l'install de RBiips:

        env BIIPS_INCLUDE=path/to/install/usr/include/biips/ BIIPS_LIB=/path/to/install/lib/ARCH R CMD INSTALL RBiips_0.8.1.tar.gz

ARCH directory depends on the machine where biips deb package was installed.

Note: the environment variables should not be needed if the biips deb package was installed as root.

...or type from R console:

        install.packages('path/to/RBiips_X.X.X.tar.gz')

FIXME: error when compiling RBiips

        g++: error: /usr/lib/libBiipsCompiler.a: Aucun fichier ou dossier de ce type
        g++: error: /usr/lib/libBiipsBase.a: Aucun fichier ou dossier de ce type
        g++: error: /usr/lib/libBiipsCore.a: Aucun fichier ou dossier de ce type

- [x] Can we change the Makevars.in so that the paths are correct, and you don't need to use the environment variables ?

Adrien, le 21/01/2014 :
=======================
* François a commencé l'article pour J. of Stat. Software
- [ ] On a convergé sur un PMMH adaptatif: à modifier dans R, puis à créer dans Matlab
- [x] Je suis en train de passer la doc de rbiips au format roxygen. Donc on va supprimer les fichiers Rd.
- [x] Marc tente de résoudre un problème de taille de disque sur Windows CI pour installer Matlab
- [ ] RBiips sous Windows CI: commandes de build à ajouter (Marc)

Adrien, le 05/01/2014 :
=======================
Erreurs des demos corrigées : les fichiers .bug utilisaient '_' au lieu de '.' donc j'ai corrigé les noms de variables dans les scripts R

A faire :
----------
- [ ] La comparaison avec Kalman dans `demo(hmm_1d_lin)` utilise un package obsolète
    package 'sspir' is not available (for R version 3.0.2)

- [ ] Note : pimh et pmmh ont besoin du package rjags (sorties de type mcarray). Il faut trouver une alternative à cette dépendance : réimplementer les opérations sur mcarray dans RBiips ?

- [x] Corriger le formatage du warning:
        Unused variables in data:t.maxmean.x.initprec.x.initprec.xprec.y

- [ ] Améliorer les résultats des exemples pmmh

François, le 04/01/2014:
=======================
* Test de Rbiips, sous Windows 7 64 bits, avec R version 3.0.2.
Installation ok. 
Test des demos (je n'ai pas la version 3.0 de jags installée et passe les comparaisons avec jags):
 -[x] erreur en lançant `demo(hmm_1d_lin)` :

        > biips <- biips.model(model, data=data, sample.data=!run.jags)
        * Parsing model in: C:/Users/fcaron/Documents/R/win-library/3.0/RBiips/extdata/hmm_1d_lin.bug
        * Compiling data graph
          Declaring variables
        RUNTIME ERROR: Compilation error on line 1.
        Unable to calculate dimensions of node y
        Error in biips.model(model, data = data, sample.data = !run.jags) : 
          Failed to compile model.
        In addition: Warning message:
        In biips.model(model, data = data, sample.data = !run.jags) :
          Unused variables in data:t.maxmean.x.initprec.x.initprec.xprec.y

 - [x] erreur en lançant `demo(hmm_1d_lin)` :

        > biips <- biips.model(model, data)
        * Parsing model in: C:/Users/fcaron/Documents/R/win-library/3.0/RBiips/extdata/hmm_1d_nonlin.bug
        * Compiling model graph
          Declaring variables
        RUNTIME ERROR: Compilation error on line 1.
        Unable to calculate dimensions of node x_true
        Error in biips.model(model, data) : Failed to compile model.
        In addition: Warning message:
        In biips.model(model, data) :
          Unused variables in data:t.maxmean.x.initprec.x.initprec.xprec.y

 - [x] `demo(hmm_1d_lin_param)` ok
 - [x] erreur avec `demo(hmm_1d_nonlin_param)`  (toujours pb de dimension), etc...

* Test de matbiips sous windows 7 64 bit et Matlab R2013b
Utilisation ok des fonctions tuto `hmm_4d_lin` et `hmm_4d_nonlin`

Adrien, le 29/12/2013:
=======================
Ajouts dans new_release:
* matbiips: archive zip avec mexfiles pour Linux 64bit, Windows 64bit, Mac 64bit
* RBiips: packages binaires R pour Windows 64bit, Windows 32bit et Mac
A tester... Voir mise à jour de install.md

A faire:
- [ ] Configurer l'esclave CI Windows 7
- [ ] Réorganiser les tests matbiips
- [ ] Permettre compilation multi-architecture de RBiips sous Windows

Adrien, le 17/12/2013:
=======================
J'ai peuplé le dossier new_release avec les éléments qu'on doit mettre à jour pour la nouvelle "libération".
Je n'ai pas apporté de modifications pour l'instant. Je laisse la main.

doc :
-----
biips-spec, biips-technical-report et notePMMH sont là pour éventuellement être réutilisés dans une doc mieux rationalisée. Un début est dans biips-user-manual qui s'inspire de la doc de JAGS.
Il y a également un bout de doc dans matbbips/doc qu'il faudra compléter ou intégrer quelque part.

examples :
----------
Il y a un dossier par modèle. Les scripts R (initialement intégrés au package RBiips) sont à mettre à jour ainsi que les .bug pour qu'ils soient compatibles matbiips.

Il y a aussi un dossier partagé Rbiips (pas dans new_release) avec des exemples.

Pierrick, le 17/12/2013:
========================

priorités :
- [x] toutes plateformes version actuelle
- [ ] exemples sous R et Matlab (sauf pmmh pimh sous matlab)
- [ ] dossier exemples dans le dropbox. Sous dossier par modèle (R,*.m, bug, *.pdf).
- [ ] RD = latex version R
- [ ] doc
- [ ] nettoyer
- [ ] refaire tests pimh et pmmh dans R


