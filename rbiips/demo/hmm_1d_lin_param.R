# BUGS model file
model <- file.path(find.package("RBiips"), "extdata", "hmm_1d_lin_param.bug")
model.title <- "Fixed parameter estimation of linear gaussian univariate HMM"

# data
data <- list(t.max = 100,
             mean.x.init = 0,
             prec.x.init = 1,
             prec.x = 100)

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
  jags <- jags.model(model, data=data, n.chains=n.chains)
  print(jags)
  if (interactive()) {
    cat("Type <Return> to continue: ")
    scan(quiet=TRUE)
  }
  
  # burn in
  n.burn.jags <- 200
  ptm <- proc.time()[[1]]
  update(jags, n.burn.jags)
  cat(n.burn.jags, "iterations in", proc.time()[[1]] - ptm, "s.\n")
  
  # jags samples
  n.iter.jags <- 1000
  ptm <- proc.time()[[1]]
  out.jags <- jags.samples(jags, c("x","prec.y"),
                           n.iter=n.iter.jags)
  cat(n.iter.jags, "iterations in", proc.time()[[1]] - ptm, "s.\n")
  
  par(mfcol = c(2,1))
  # plot mcmc samples trace
  plot(out.jags$prec.y,
       xlab="iteration", ylab="value",
       main=paste("Trace of", n.iter.jags, "JAGS MCMC samples"))
  legend("topright", leg="prec.y", pch=1, bty='n')
       
  # plot mcmc samples histogram
  hist(out.jags$prec.y,
       xlab="value", ylab="frequency",
       main=paste("Histogram of", n.iter.jags, "JAGS MCMC samples"))
  legend("topright", leg="prec.y", pch=0, bty='n')
  
  par(mfcol = c(1,1))
}

# -------------------- BiiPS --------------------#
require(RBiips)

n.part <- 50
n.burn <- 1000
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
  log.prec.y.all <- seq(-3, 3, length=100)
  out.sens <- smc.sensitivity(biips,
                            params=list(log.prec.y=log.prec.y.all),
                            n.part=n.part)
  
  prec.y.all <- exp(log.prec.y.all)
  plot(exp(log.prec.y.all), out.sens$log.marg.like,
       xlab="prec.y", ylab="Log. Marginal Likelihood",
       main="Sensitivity analysis")
  legend("topright", leg="log.marg.like", pch=1, bty='n')
  
  # initialize pmmh with max
  inits <- list(log.prec.y = out.sens$max.param$log.prec.y)
  init.pmmh(biips, param.names="log.prec.y", 
            inits=inits, n.part=n.part)
}

#       PMMH 
#---------------------

# burn in
update.pmmh(biips, param.names=c("log.prec.y"),
             n.iter=n.burn,
             n.part=n.part)
# sample
out.pmmh <- pmmh.samples(biips, param.names=c("log.prec.y"),
                       n.iter=n.iter,
                       n.part=n.part)


prec.y.biips <- exp(out.pmmh$log.prec.y)

par(mfcol = c(2,1))
# plot PMMH samples trace
plot(drop(prec.y.biips),
     xlab="iteration", ylab="value",
     main=paste("Trace of", n.iter, "BiiPS PMMH samples"))
legend("topright", leg="prec.y", pch=1, bty='n')

# plot PMMH samples histogram
hist(prec.y.biips,
     xlab="value", ylab="frequency",
     main=paste("Histogram of", n.iter, "BiiPS PMMH samples"))
legend("topright", leg="prec.y", pch=0, bty='n')


#---------------------- display results ----------------------#
cat("True value: prec.y =", biips$data()$prec.y.true, "\n")

# mean value
if (run.jags) {
  cat("JAGS MCMC mean value: prec.y =", mean(out.jags$prec.y), "\n")
}

cat("BiiPS PMMH mean value: prec.y =", mean(prec.y.biips), "\n")

par(mfcol = c(1,1))
