.onLoad <- function(lib, pkg) {
  ## Load the RBiips wrapper ...
  library.dynam("RBiips", pkg, lib, local=FALSE)
  
  #.Call("init_biips_console", PACKAGE="RBiips")
  
  ## ... and the modules
  load.biips.module("basemod")
  
  set.verbosity(1)
}
