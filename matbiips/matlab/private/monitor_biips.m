function monitor_biips(p, variable.names, type)
  
    
  pn <- parse.varnames(variable.names)
  
  type <- match.arg(type, c("filtering", "smoothing", "backward.smoothing"), several.ok = TRUE)
  if ("filtering" %in% type) {
    .Call("set_filter_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  if ("smoothing" %in% type) {
    .Call("set_smooth_tree_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
  if ("backward.smoothing" %in% type) {
    .Call("set_smooth_monitors", obj$ptr(), pn$names, pn$lower, pn$upper, PACKAGE="RBiips")
  }
