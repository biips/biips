# BUGS model file
model <- file.path(find.package("RBiips"), "extdata", "stoch_volatility_param.bug")
model.title <- "Fixed parameter estimation of Stochastic volatility model"

# data
data(SP500, package="RBiips")
y <- SP500[,"Daily.Return"]

ind <- 800:900

y <- y[ind]

# data
data <- list(t.max = length(y),
             mean.x.init = 0,
             prec.x.init = 1,
             y = y)

par(bty = "n")
# -------------------- JAGS  MCMC--------------------#
run.jags <- FALSE
if (interactive()) {
  ans <- readline("Compare BiiPS PMMH with JAGS MCMC algorithm (requires rjags package) ? y|[n] :")
  run.jags <- (ans == "y")
}
if(run.jags)
{
  require(rjags)
  
  # model
  n.chains <- 1
  jags <- jags.model(model, data=data, n.chains=n.chains, n.adapt=10000)
  print(jags)
  if (interactive()) {
    cat("Type <Return> to continue: ")
    scan(quiet=TRUE)
  }
  
  # burn in
  n.burn.jags <- 10000
  ptm <- proc.time()[[1]]
  update(jags, n.burn.jags)
  cat(n.burn.jags, "iterations in", proc.time()[[1]] - ptm, "s.\n")
  
  # jags samples
  n.iter.jags <- 2000
  ptm <- proc.time()[[1]]
  out.jags <- jags.samples(jags, c("x","prec.x"),
                           n.iter=n.iter.jags)
  cat(n.iter.jags, "iterations in", proc.time()[[1]] - ptm, "s.\n")
  
  par(mfcol = c(2,1))
  # plot mcmc samples trace
  plot(out.jags$prec.x,
       xlab="iteration", ylab="value",
       main=paste("Trace of", n.iter.jags, "JAGS MCMC samples"))
  legend("topright", leg="prec.x", pch=1, bty="n")
  
  # plot mcmc samples histogram
  hist(out.jags$prec.x,
       xlab="value", ylab="frequency",
       main=paste("Histogram of", n.iter.jags, "JAGS MCMC samples"))
  legend("topright", leg="prec.x", pch=0, bty="n")
  
  par(mfcol = c(1,1))
}

# -------------------- BiiPS --------------------#
require(RBiips)

n.part <- 100
n.burn <- 100
n.iter <- 1000

# model
if (run.jags)
  data <- jags$data()

biips <- biips.model(model, data=data, sample.data=!run.jags)
if (!run.jags) {
  print(biips)
  if (interactive()) {
    cat("Type <Return> to continue: ")
    scan(quiet=TRUE)
  }
}

# Sensitivity analysis 
#---------------------
run.sens <- FALSE
if (interactive()) {
  ans <- readline("Analyze sensitivity to parameters ? y|[n] :")
  run.sens <- (ans == "y")
}
if(run.sens)
{
  log.prec.x.all <- seq(-3, 3, length=100)
  out.sens <- smc.sensitivity(biips,
                              params=list(log.prec.x=log.prec.x.all),
                              n.part=n.part)
  
  prec.x.all <- exp(log.prec.x.all)
  plot(exp(log.prec.x.all), out.sens$log.marg.like,
       xlab="prec.x", ylab="Log. Marginal Likelihood",
       main="Sensitivity analysis")
  legend("topright", leg="log.marg.like", pch=1, bty="n")
  
  # initialize pmmh with max
  inits <- list(log.prec.x = out.sens$max.param$log.prec.x)
  init.pmmh(biips, variable.names=c("log.prec.x"), 
            inits=inits, n.part=n.part)
}

#       PMMH 
#---------------------

# burn in
update.pmmh(biips, variable.names=c("log.prec.x"),
            n.iter=n.burn,
            n.part=n.part)
# sample
out.pmmh <- pmmh.samples(biips, variable.names=c("log.prec.x"),
                         n.iter=n.iter,
                         n.part=n.part)


prec.x.biips <- exp(out.pmmh$log.prec.x)

par(mfcol = c(2,1))
# plot PMMH samples trace
plot(drop(prec.x.biips),
     xlab="iteration", ylab="value",
     main=paste("Trace of", n.iter, "BiiPS PMMH samples"))
legend("topright", leg="prec.x", pch=1, bty="n")

# plot PMMH samples histogram
hist(prec.x.biips,
     xlab="value", ylab="frequency",
     main=paste("Histogram of", n.iter, "BiiPS PMMH samples"))
legend("topright", leg="prec.x", pch=0, bty="n")

par(mfcol = c(1,1))

#---------------------- display results ----------------------#
cat("True value: prec.x =", biips$data()$prec.x.true, "\n")

# mean value
if (run.jags) {
  cat("JAGS MCMC mean value: prec.x =", mean(out.jags$prec.x), "\n")
}
cat("BiiPS PMMH mean value: prec.x =", mean(prec.x.biips), "\n")
