#                                               -*- R -*-
#
#  RBiips package for GNU R is an interface to BiiPS C++ libraries for
#  Bayesian inference with interacting Particle Systems.
#  Copyright (C) Inria, 2012
#  Authors: Adrien Todeschini, Francois Caron
#  
#  RBiips is derived software based on:
#  BiiPS, Copyright (C) Inria, 2012
#  rjags, Copyright (C) Martyn Plummer, 2002-2010
#  Rcpp, Copyright (C) Dirk Eddelbuettel and Romain Francois, 2009-2011
#
#  This file is part of RBiips.
#
#  RBiips is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
#  \file     particles.R
#  \brief    R functions manipulating particles objects
#
#  \author   $LastChangedBy$
#  \date     $LastChangedDate$
#  \version  $LastChangedRevision$
#  Id:       $Id$
#

which.type <- function(x, type=NULL)
{
  if (!is.null(type) && !is.character(type))
    stop("type must be a character vector or NULL.")
  
  type <- match.arg(type, c("filtering", "smoothing", "backward.smoothing"), several.ok = TRUE)
  
  ind <- c()
  for (p in seq(along=x)) {      
    if (is.null(type) || x[[p]]$type %in% type)
      ind <- c(ind, p)
  }
  
  return(ind)
}


print.particles <- function(x, fun = c("mean","mode"), probs = c(0.25, 0.5, 0.75),...)
{
  print(summary(x, fun, probs), ...)
}


print.particles.list <- function(x, fun = c("mean","mode"), probs = c(0.25, 0.5, 0.75), ...)
{
  print(summary(x, fun, probs), ...)
}


stat.particles <- function(x, fun="mean", probs = c(0.25, 0.5, 0.75))
{
  if (!is.character(fun))
    stop("invalid fun argument.")
  
  fun <- match.arg(fun, c("mean",
                          "var",
                          "skew",
                          "kurt",
                          "median",
                          "quantiles",
                          "mode"), several.ok = TRUE)
  if (any(fun == "quantiles")) {
    if (!is.numeric(probs) || any(probs<=0) || any(probs>=1))
      stop("invalid probs argument.")
    probs <- sort(probs)
  }
  
  n.part <- dim(x$values)["particle"]
  drop.dims <- names(dim(x$values)) %in% c("particle")
  dim.stat <- dim(x$values)[!drop.dims]
  len <- prod(dim.stat)
  
  stat <- list()
  for (d in 1:len) {
    indvec <- seq(d, len*(n.part-1)+d,len)
    for (f in fun) {
      if (f == "quantiles")
        d.stat <- .Call("weighted_quantiles", x$values[indvec], n.part*x$weights[indvec], probs, PACKAGE="RBiips")
      else
        d.stat <- .Call(paste("weighted_",f,sep=""), x$values[indvec], n.part*x$weights[indvec], PACKAGE="RBiips")
      stat.names <- names(d.stat)[names(d.stat) != "Table"]
      if (d==1) {
        for (n in stat.names) {
          if (is.null(stat[[n]]))
            stat[[n]] <- array(dim=dim.stat)
        }
      }
      for (n in stat.names)
        stat[[n]][d] <- d.stat[[n]]
    }
  }
  return(stat)
}


summary.particles <- function(object, fun, probs = c(0.25,0.5,0.75), ...)
{
  if (is.null(object$values) || is.null(object$weights) || is.null(object$discrete)) {
    NextMethod()
  }
  
  if (all(object$discrete)) {
    if (missing(fun))
      fun <- "mode"
    else
      fun <- match.arg(fun, c("mode",
                              "mean",
                              "var",
                              "skew",
                              "kurt",
                              "median",
                              "quantiles"), several.ok = TRUE)
  } else {
    if (missing(fun))
      fun <- "mean"
    else
      fun <- match.arg(fun, c("mean",
                              "var",
                              "skew",
                              "kurt",
                              "median",
                              "quantiles"), several.ok = TRUE)
  }

  drop.dims <- names(dim(object$values)) %in% c("particle")
  
  stat <- stat.particles(object, fun, probs)
  stat[["drop.dims"]] <- dim(object$values)[drop.dims]
  stat[["name"]] <- deparse.varname(object$name, object$lower, object$upper)
  stat[["type"]] <- object$type
  class(stat) <- "summary.particles"

  return(stat)
}


summary.particles.list <- function(object, ...)
{
  ans <- list()
  for (n in names(object))
    ans[[n]] <- summary(object[[n]], ...)
  
  class(ans) <- "summary.particles.list"

  return(ans)
}


print.summary.particles <- function(x, ...)
{
  cat(x$name, x$type, "particles:\n")
  print(x[!(names(x) %in% c("drop.dims", "name", "type"))], ...)
  if (length(x$drop.dims) > 0) {
    cat("Marginalizing over:", 
      paste(paste(names(x$drop.dims), "(", x$drop.dims,")" , sep=""),
        collapse=","),"\n")
  }
  invisible()
}


print.summary.particles.list <- function(x, ...)
{
  for (n in names(x)) {
    print(x[[n]], ...)
    cat("\n")
  }
  invisible()
}


plot.summary.particles <- function(x, type="l", lty=1:5, lwd=2, col=1:6, xlab="offset",
                                   ylab="value", main, sub, leg.args=list(), ...)
{
  ltyy <- lty
  lwdd <- lwd
  coll <- col
  
  legend.summary.particles <- function(x="topright", y=NULL,
                                       lty=ltyy, lwd=lwdd, col=coll, bty="n", inset=c(0.01,0.01), ...)
  {
    return(legend(x=x, y=y, lty=lty, lwd=lwd, col=col, bty=bty, inset=inset, ...))
  }
  
  stat.names <- names(x)[!(names(x) %in% c("2nd mt.",
                                           "Var.",
                                           "SD",
                                           "3rd mt.",
                                           "Skew.",
                                           "4th mt.",
                                           "Kurt.",
                                           "drop.dims",
                                           "name",
                                           "type"))]
  
  values <- c()
  for (n in stat.names) {
    values <- c(values, x[[n]])
  }
  
  mat <- matrix(values, ncol=length(stat.names))
  n.part <- x$drop.dims[["particle"]]
  if (missing(main))
    main <- paste(x$name, x$type, "particle estimates")
  if (missing(sub))
    sub <- paste("Marginalizing over:", paste(paste(names(x$drop.dims), "(", x$drop.dims,")" , sep=""),
        collapse=","))
  matplot(mat, type=type, xlab=xlab, ylab=ylab, main=main, sub=sub, lty=lty, lwd=lwd, col=col, ...)
  
  leg.args[["legend"]] <- stat.names
  do.call("legend.summary.particles", leg.args)
  invisible()
}


plot.summary.particles.list <- function(x, ...)
{
  for (n in names(x))
    plot(x[[n]], ...)
  invisible()
}


plot.particles <- function(x, fun = c("mean","mode"), probs = c(0.25,0.5,0.75), ...)
{
  plot(summary(x, fun, probs), ...)
  invisible()
}


plot.particles.list <- function(x, fun = c("mean","mode"), probs = c(0.25,0.5,0.75), ...)
{
  for (n in names(x))
    plot(x[[n]], fun, probs, ...)
  invisible()
}


diagnostic <- function(object, ...)
  UseMethod("diagnostic")


diagnostic.particles <- function(object, ess.thres=30, ...)
{
  ess.min <- min(object$ess)
  ans <- list("ESS min."=ess.min, "valid"=ess.min>ess.thres)
  class(ans) <- "diagnostic.particles"
  return(ans)
}


print.diagnostic.particles <- function(x, ...)
{
  if (x$valid)
    cat("diagnostic: GOOD\n")
  else {
    cat("diagnostic: POOR\n")
    cat("    The minimum effective sample size is too low: ", x$ESS, "\n", sep="")
    cat("    Estimates may be poor for some variables.\n")
    cat("    You should increase n.part.")
  }
}


diagnostic.particles.list <- function(object, ...)
{
  ans <- list()
  for (n in names(object))
    ans[[n]] <- diagnostic(object[[n]], ...)
  return(ans)
}


get.index <- function(offset, lower, upper)
{
  if (length(lower) != length(upper))
    stop("lower and upper dimension mismatch.")
  dim <- upper-lower+1
  if (any(dim <= 0))
    stop("incorrect lower and upper values.")
  ind <- lower
  offset <- offset-1
  for (i in 1:length(dim))
  {
    ind[i] <- ind[i] + (offset) %% dim[i];
    offset <- offset %/% dim[i];
  }
  return(ind)
}


deparse.varname <- function(name, lower=NULL, upper=lower)
{
  v <- parse(text=name, n=1)
  if (!is.expression(v) || length(v) != 1 || !is.name(v[[1]]))
    stop("invalid variable name.")
  varname <- deparse(v[[1]])
  
  if (length(lower) == 0)
    return(varname)
  
  varname <- paste(varname, "[", sep="")
  if (length(upper) != length(lower))
    stop("lower and upper lengths mismatch.")
  if (any(upper<lower))
    stop("incorrect lower and upper values.")
  sep <- ""
  for (i in seq(along=lower)) {
    varname <- paste(varname, lower[[i]] , sep=sep)
    if (upper[[i]] > lower[[i]])
      varname <- paste(varname, upper[[i]] , sep=":")
    sep <- ","
  }
  varname <- paste(varname, "]", sep="")
  return(varname)
}


density.particles <- function(x, bw="nrd0", adjust=1, subset, ...)
{
  ans <- list()
  bww <- bw
  
  n.part <- dim(x$values)["particle"]
  
  if (!missing(subset)) {
    if(!is.character(subset) || !is.atomic(subset))
      stop("invalid subset argument.")
    pn <- parse.varname(subset)
    if(pn$name != x$name)
      stop("invalid subset argument: wrong variable name.")
    if(any(pn$lower < x$lower) || any(pn$upper > x$upper))
      stop("invalid subset argument: index out of range.")
    lower <- pn$lower
    upper <- pn$upper
    subset.dim <- upper-lower+1
    len <- prod(subset.dim)
  } else {
    lower <- x$lower
    upper <- x$upper
    drop.dim <- names(dim(x$values)) %in% c("particle")
    subset.dim <- dim(x$values)[!drop.dim]
    len <- prod(subset.dim)
  }
  
  for (d in 1:len) {
    ind <- get.index(d, lower, upper)
    varname <- deparse.varname(x$name, ind)
    
    if (!missing(subset)) {
      ind.mat <- array(c(rep(ind, each=n.part), 1:n.part), dim=c(n.part, length(dim(x$values))))
      values <- x$values[ind.mat]
      weights <- x$weights[ind.mat]
      ind2 <- array(ind, dim=c(1, length(ind)))
      ess <- x$ess[ind2]
      discrete <- x$discrete[ind2]
    } else {
      ind.vec <- seq(d, len*(n.part-1)+d, len)
      values <- x$values[ind.vec]
      weights <- x$weights[ind.vec]
      ess <- x$ess[d]
      discrete <- x$discrete[d]
    }
    
    if (!is.atomic(bw))
      bww <- bw[[d]]
    
    if (discrete) {
      table <- .Call("weighted_table", values, weights, PACKAGE="RBiips")
      dens <- list(x=table[["Table"]]$x, y=table[["Table"]]$y, bw=1)
    } else {
      dens <- density(values, weights=weights, bw=bww, adjust=adjust, ...)
    }
    
    ans[[varname]] <- list(density=dens, name=varname, type=x$type, n.part=n.part, ess=ess, discrete=discrete)
    class(ans[[varname]]) <- "density.particles.atomic"
  }
  
  class(ans) <- "density.particles"
  return(ans)
}


density.particles.list <- function(x, bw="nrd0", adjust=1, subset, ...)
{
  ans <- list()
  bw.s <- bw
  
  # first treat filtering and backward.smoothing
  if (!is.null(x[["filtering"]])) {
    dens <- density.particles(x[["filtering"]], bw, adjust, subset, ...)
    bw.s <- list()
    for (n in names(dens)) {
      ans[[n]][["filtering"]] <- dens[[n]]
      bw.s[[n]] <- dens[[n]]$density$bw
    }
  }
  if (!is.null(x[["backward.smoothing"]])) {
    dens <- density.particles(x[["backward.smoothing"]], bw, adjust, subset, ...)
    bw.s <- list()
    for (n in names(dens)) {
      ans[[n]][["backward.smoothing"]] <- dens[[n]]
      bw.s[[n]] <- dens[[n]]$density$bw
    }
  }
  
  # then treat smoothing (applying previous bandwidth if any)
  if (!is.null(x[["smoothing"]])) {
    if (is.list(bw.s))
      adjust <- 1
    dens <- density.particles(x[["smoothing"]], bw.s, adjust, subset, ...)
    for (n in names(dens))
      ans[[n]][["smoothing"]] <- dens[[n]]
  }
  
  for (n in names(ans))
    class(ans[[n]]) <- "density.particles.atomic.list"

  class(ans) <- "density.particles.list"
  return(ans)
}


plot.density.particles.atomic <- function(x, type="l", lty=1:5, lwd=2, col=1:6,
                                          xlab="value", ylab, main, sub,
                                          leg.args=list(), ...)
{
  ltyy <- lty
  lwdd <- lwd
  coll <- col
  
  legend.density.particles <- function(x="topright", y=NULL,
                                       lty=ltyy, lwd=lwdd, col=coll, bty="n", inset=c(0.01,0.01), ...)
  {
    return(legend(x=x, y=y, lty=lty, lwd=lwd, col=col, bty=bty, inset=inset, ...))
  }
  
  leg <- paste(x$type, ", ess=", round(x$ess), sep="")

  if (x$discrete) {
    if (missing(ylab))
      ylab <- "probability"
    if (missing(main))
      main <- "discrete law histograms"
    main.title <- paste(x[[t]]$name, main)
    if (missing(sub))
      sub <- paste("n.part=", x$n.part)
    
    barplot(x$density$y, names.arg=x$density$x,
            col=col, xlab=xlab, ylab=ylab, main=main.title, sub=sub,
            legend=leg, ...)
  } else {
    if (missing(ylab))
      ylab <- "density"
    if (missing(main))
      main <- "kernel density estimates"
    main.title <- paste(x[[t]]$name, main)
    if (missing(sub)) {
      bw <- x$density$bw
      sub <- paste("n.part=", x$n.part, ", bw=", signif(bw, digits = 2), sep="")
    }
    
    plot(x$density$x, x$density$y,
         type=type, lty=lty, lwd=lwd, col=col,
         xlab=xlab, ylab=ylab, main=main.title, sub=sub, ...)
    leg.args[["legend"]] <- leg
  
    do.call(legend.density.particles, leg.args)
  }
      
  invisible()
}


plot.density.particles <- function(x, type="l", lty=1:5, lwd=2, col=1:6,
                                   leg.args=list(), ...)
{
  for (n in names(x)) {
    plot(x[[n]], type=type, lty=lty, lwd=lwd, col=col, leg.args=leg.args, ...)
  }
  invisible()
}


plot.density.particles.atomic.list <- function(x, type="l", lty=1:5, lwd=2, col=1:6,
                                               xlab="value", ylab, main, sub,
                                               leg.args=list(), density=NULL, angle=NULL, ...)
{
  ltyy <- lty
  lwdd <- lwd
  coll <- col
  
  legend.density.particles <- function(x="topright", y=NULL,
                                       lty=ltyy, lwd=lwdd, col=coll, bty="n", inset=c(0.01,0.01), ...)
  {
    return(legend(x=x, y=y, lty=lty, lwd=lwd, col=col, bty=bty, inset=inset, ...))
  }

  leg <- c()
  for (t in names(x)) {
    leg <- c(leg, paste(x[[t]]$type, ", ess=", round(x[[t]]$ess), sep=""))
  }
  
  if (x[[1]]$discrete) {
    #get table locations
    xx <- c()
    for (t in names(x))
      xx <- c(xx, x[[t]]$density$x)
    xx <- sort(unique(xx))
    
    yy <- c()
    for (t in names(x)) {
      # resize values with missing 0 to empty locations
      y <- rep(0, length(xx))
      y[xx %in% x[[t]]$density$x] <- x[[t]]$density$y
      yy <- c(yy, y)
    }
    yy <- matrix(yy, nrow=length(x), byrow=TRUE)
    
    if (missing(ylab))
      ylab <- "probabilty"
    if (missing(main))
      main <- "discrete law histograms"
    main.title <- paste(x[[t]]$name, main)
    if (missing(sub))
      sub <- paste("n.part=", x[[1]]$n.part, sep="")
    
    if (length(col)>length(x))
      col <- col[1:length(x)]
    if (length(density)>length(x))
      density <- density[1:length(x)]
    if (length(angle)>length(x))
      angle <- angle[1:length(x)]
    barplot(yy, names.arg=xx,
            col=col, xlab=xlab, ylab=ylab, main=main.title, sub=sub,
            legend=leg, beside=TRUE, density=density, angle=angle, ...)
  } else {
    xx <- c()
    yy <- c()
    
    for (t in names(x)) { 
      xx <- c(xx, x[[t]]$density$x)
      yy <- c(yy, x[[t]]$density$y)
    }
    xx <- matrix(xx, ncol=length(x))
    yy <- matrix(yy, ncol=length(x))
    
    if (missing(ylab))
      ylab <- "density"
    if (missing(main))
      main <- "kernel density estimates"
    main.title <- paste(x[[t]]$name, main)
    if (missing(sub)) {
      bw <- x[[1]]$density$bw
      sub <- paste("n.part=", x[[1]]$n.part, ", bw=", signif(bw, digits = 2), sep="")
    }
  
    matplot(xx, yy,
         type=type, lty=lty, lwd=lwd, col=col,
         xlab=xlab, ylab=ylab, main=main.title, sub=sub, ...)
    
    leg.args[["legend"]] <- leg
    do.call(legend.density.particles, leg.args)
  }
      
  invisible()
}


plot.density.particles.list <- function(x, type="l", lty=1:5, lwd=2, col=1:6,
                                        leg.args=list(), ...)
{
  for (n in names(x)) {
    plot(x[[n]], type=type, lty=lty, lwd=lwd, col=col, leg.args=leg.args, ...)
  }
  invisible()
}