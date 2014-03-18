Bugs:
=====  
- [x] Problème affectation conjugateMNormalLinear sampler avec modèle hmm_4d_gps 
- [ ] (1) Résoudre sampling des enfants de noeuds stochastiques observés 
- [ ] (1) Résoudre comportement quand variable non définie dans un tableau 
- [ ] (1) Résoudre Backward smoothing: poids quasiment égaux à filtering dans les modèles avec DMNorm: De plus, les estimées backward sont trés proches de filtering dans le modèle stoch_kinetic
- [ ] (2) Revoir les plots d'histogrammes dans BiipsTest et BiipsTestCompiler: car DiscreteScalarHistogram a été revue
- [ ] Problème compilateur JAGS: Erreur quand un indice est logique observé
 
Features:
=========   
- [x] Monitor de sub range 
- [ ] (1) fonctions, distributions 
- [ ] (1) Revoir les messages affichés dans RBiips 
- [ ] (2) option quiet 
- [ ] (2) calcul de stat par Monitor 
- [ ] (2) Samplers conjugate 
- [ ] (2) Ajouter HPD (high probability density) dans le summary 
- [ ] (2) MixtureNodes 
- [ ] (2) LinkFunction 
- [ ] (2) filtrer noeuds dot 
- [ ] (2) RNG factories 
- [ ] (2) pause/ reprise / annulation des calculs 
- [ ] (2) Lien dynamique des modules 
- [ ] (3) "Extension: Algorithme ""Adaptive""-Lag particle filter": (avec seuil sur ESS des itérations passées). Le seuil doit changer au cours du temps.
- [ ] (3) Variance accumulator for vectors 
- [ ] (3) execution iteration par iteration: (avec resultats intermediaires)
- [ ] Méthode Console::ClearMonitors 
- [ ] Calcul de la densité a posteriori pour chaque particule: pour calcul du MAP sur la densité jointe (trajectoire).
- [ ] separer data generation et model dans deux fichiers 
 
Refactoring:
=====   
- [ ] (2) Généraliser ConjugateNormal : pour multi observations de type dnorm et dmnorm
- [ ] (2) Generaliser ConjugateSampler : pour Linear conjugate methodes
- [ ] (2) Revoir GetLinearTransform 
- [ ] (2) Sortir StatsTag de Accumulator.hpp 
- [ ] (2) Remplacer Types<Scalar>::Array par ValArray 
- [ ] (2) Traits pattern au lieu de Types 
- [ ] (2) Array -> Vectors 
- [ ] (2) Vector -> NumVector 
- [ ] (2) Matrix -> NumMatrix 
- [ ] (2) MultiArray -> NumArray 
- [ ] (2) MultiArray::Values -> Data 
- [ ] (2) Flags -> Mask 
- [ ] (2) Parents -> Parameters 
- [ ] (2) Graph::GetNodeValues -> NodeDataPtrs 
- [ ] (2) Séparer data generation de model dans Console::Compile 
- [ ] (2) Sortir VertexWriter de Graph.hpp 
- [ ] (2) Smooth tree -> smoothing 
- [ ] (2) Smooth -> backward smoothing 
- [ ] (2) LogicalNode -> DeterministicNode 
- [ ] (2) utiliser vector_as_graph 
- [ ] (2) supprimer GraphTypes 
- [ ] (3) utiliser une liste de contributeurs à la vraisemblance 
- [ ] (3) Propreté: typedefs in functions 
- [ ] (3) typedefs pour les iterators 
- [ ] (3) Supprimer ou améliorer VectorRef, MatrixRef: créer MultiArray::SwapData et checkSwap
- [ ] Singleton sur ForwardSAmpler 
- [ ] A utiliser: Boost.Iterator, arrays of pointers, Boost.Range 
- [ ] GetValue != GetData 
- [ ] revoir getNodeValue et NodeSampler 
- [ ] supprimer iterNodeId et iterSamplerSequence de ForwardSampler 
- [ ] const-correctness
- [ ] redesign the management of object/pointer/reference
- [ ] sub-namespaces
- [ ] separate API headers
- [ ] comments

Optimization:
=============   
- [x] Optimiser liberation memoire 
- [ ] (1) Optimisation de création des noeuds: Constant et LogicalFactory
- [ ] (1) Améliorer la gestion de release memory des noeuds 
- [ ] (1) DimFactory 
- [ ] (2) Optimiser checks redondants avec try catch 
- [ ] (2) Optimiser calcul de densités lorsqu'on n'a pas besoin de constante de normalisation 
- [ ] (2) Supprimer noeuds constants non utilisés.: utiliser listS au lieu de vecS dans boost graph (remove vertices plus facile). Ou créer graphe des index expressions
- [ ] (2) utiliser ptr_vector pour les Nodes 
- [ ] (2) Optimiser CategoricalDist dans le resampling: calcul de la somme des poids
- [ ] (2) weights dans un pointeur 
- [ ] (2) optimiser smoothtree monitor et smooth monitor: memes valeurs que filtermonitor
- [ ] (2) optimiser appels à checkParamValues: ex: pour noeuds constants
- [ ] (2) optimiser appels à cholesky: 
- [ ] (2) Ne pas mettre à jour les noeuds non informatifs: c'est à dire les noeuds qui ne sont pas nécessaires aux noeuds monitorés
- [ ] Supprimer les pointeurs 

Security:
=========   
- [ ] (2) Ne pas delete model à chaque catch exceptions 
- [ ] (2) Protéger clean filter monitors pour backward 
- [ ] (3) Revue des instructions: explicit, const, exception, private, protected, throw
- [ ] (3) Penser à un reset des attributs de NodeVisitor quand ils st utilisés plusieurs fois. 
- [ ] CheckBoundValues ? 
- [ ] Gestion des paths: boost::filesystem

Tests:
======   
- [x] progress bar (avec timer) 
- [ ] (1) Revoir modèle hmm_radar_angle: utilisation de atan ok ?
- [ ] (2) changer option optimal en conditional ? 
- [ ] (3) Terminer implémentation des tests 
- [ ] (3) implémenter autres tests dans Biips et Matlab 
- [ ] (3) Ajouter smoothing à BiipsTest 
- [ ] (3) sections bench.mean et bench.var 
- [ ] (3) séparer fichier data du cfg 
- [ ] (3) sauvegarde des plots 
- [ ] (3) affichage et sauvegarde des options et résultats 
- [ ] (3) Refactoriser tests: Creer lib BiipsTest 
- [ ] (3) Vérifier traitement des options 
- [ ] (3) améliorer display 
- [ ] (3) améliorer data input: check si plusieurs valeurs 
- [ ] (3) améliorer plots 
- [ ] (3) options particles et mutations sur la meme ligne 
- [ ] (3) check sample size du T-test 
- [ ] (3) Découper BiipsTestCompiler 
- [ ] Afficher trajectoire arbre généalogique 
- [ ] plots smoothtree 
- [ ] séparer génération de data 

R interface:
============   
- [x] pmcmc 
- [x] gestion des variables discretes: MAP au lieu de mean, histogramme au lieu de densité
- [x] calcul des quantiles 
- [x] Subranges dans density 
- [ ] (1) list factories 
- [ ] (1) set factories (hierachisation) 
- [ ] (1) Documentation package 
- [ ] (2) Automatiser tests 
- [ ] (2) option verbose ou quiet 
- [ ] (2) check arguments 
- [ ] (2) nom d’options abrégés 
- [ ] (2) option repeat 
- [ ] (2) message dans diagnostic 
- [ ] (2) integration de fonction quelconque 

Diffusion:
==========  
- [ ] (1) Licence, APP: remplir formulaires
- [ ] (1) portage sous Windows et Mac 
- [ ] (2) Corriger rapports 
- [ ] (2) Mailing lists: team (project), coordinators
- [ ] (2) Site

