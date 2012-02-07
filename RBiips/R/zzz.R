.onLoad <- function(lib, pkg) {
  ## Load the RBiips wrapper ...
  library.dynam("RBiips", pkg, lib, local=FALSE)
  packageStartupMessage("linking to BiiPS ",
                          .Call("get_version", PACKAGE="RBiips"))
  
  #.Call("init_biips_console", PACKAGE="RBiips")
  
  ## ... and the modules
  load.biips.module("basemod")
  
  set.biips.verbosity(1)
}

.onUnload <- function(libpath)
{
  library.dynam.unload("RBiips", libpath)
}
