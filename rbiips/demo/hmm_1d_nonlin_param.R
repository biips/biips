model <- file.path(find.package("RBiips"), "extdata", "hmm_1d_nonlin_param.bug")
model.title <- "Fixed parameter estimation of nonlinear gaussian univariate HMM"

# data
data <- list(t_max = 20,
             mean_x_init = 0,
             prec_x_init = 1/5,
             prec_x = 1/10)

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
  n.burn.jags <- 12000
  ptm <- proc.time()[[1]]
  update(jags, n.burn.jags)
  cat(n.burn.jags, "iterations in", proc.time()[[1]] - ptm, "s.\n")
  
  # jags samples
  n.iter.jags <- 2000
  ptm <- proc.time()[[1]]
  out.jags <- jags.samples(jags, c("x","prec_y"),
                           n.iter=n.iter.jags)
  cat(n.iter.jags, "iterations in", proc.time()[[1]] - ptm, "s.\n")
  
  par(mfcol = c(2,1))
  # plot mcmc samples trace
  plot(out.jags$prec_y,
       xlab="iteration", ylab="value",
       main=paste("Trace of", n.iter.jags, "JAGS MCMC samples"))
  legend("topright", leg="prec_y", pch=1, bty="n")
  
  # plot mcmc samples histogram
  hist(out.jags$prec_y,
       xlab="value", ylab="frequency",
       main=paste("Histogram of", n.iter.jags, "JAGS MCMC samples"))
  legend("topright", leg="prec_y", pch=0, bty="n")
  
  par(mfcol = c(1,1))
}

# -------------------- BiiPS --------------------#
require(RBiips)

n.part <- 50
n.burn <- 2000
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
  log_prec_y_all <- seq(-3, 3, length=100)
  out.sens <- smc.sensitivity(biips,
                              params=list(log_prec_y=log_prec_y_all),
                              n.part=n.part)
  
  prec_y_all <- exp(log_prec_y_all)
  plot(exp(log_prec_y_all), out.sens$log.marg.like,
       xlab="prec_y", ylab="Log. Marginal Likelihood",
       main="Sensitivity analysis")
  legend("topright", leg="log.marg.like", pch=1, bty="n")
  
  # initialize pmmh with max
  inits <- list(log_prec_y = out.sens$max.param$log_prec_y)
  init.pmmh(biips, param.names="log_prec_y", 
            inits=inits, n.part=n.part)
}

#       PMMH 
#---------------------

# burn in
update.pmmh(biips, param.names=c("log_prec_y"),
            n.iter=n.burn,
            n.part=n.part)
# sample
out.pmmh <- pmmh.samples(biips, param.names=c("log_prec_y"),
                         n.iter=n.iter,
                         n.part=n.part)


prec_y_biips <- exp(out.pmmh$log_prec_y)

par(mfcol = c(2,1))
# plot PMMH samples trace
plot(drop(prec_y_biips),
     xlab="iteration", ylab="value",
     main=paste("Trace of", n.iter, "BiiPS PMMH samples"))
legend("topright", leg="prec_y", pch=1, bty="n")

# plot PMMH samples histogram
hist(prec_y_biips,
     xlab="value", ylab="frequency",
     main=paste("Histogram of", n.iter, "BiiPS PMMH samples"))
legend("topright", leg="prec_y", pch=0, bty="n")


#---------------------- display results ----------------------#
cat("True value: prec_y =", biips$data()$prec_y_true, "\n")

# mean value
if (run.jags) {
  cat("JAGS MCMC mean value: prec_y =", mean(out.jags$prec_y), "\n")
}

cat("BiiPS PMMH mean value: prec_y =", mean(prec_y_biips), "\n")

par(mfcol = c(1,1))
