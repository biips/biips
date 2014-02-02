model <- file.path("hmm_1d_nonlin_param.bug")
model.title <- "Fixed parameter estimation of nonlinear gaussian univariate HMM"

# data
y <- c(-1.16668163986769, 0.846373702328309, 14.1642535102668, 
       8.4485180622082, 3.02482012001013, -0.0466106316848516, 1.41498509583712, 
       1.12569995531173, 8.05923927708561, 1.06281034563451, 1.74435833241698, 
       1.13983504498104, 0.414485789343987, 1.46240234148615, 3.56528710055164, 
       0.415677598021062, 5.69525575130903, 10.0974670203905, 23.794401786745, 
       7.81328164882526)
x_true <- c(-2.58145793094694, -1.98297076796586, -16.9744826461564, 
            -12.1402560725162, -6.83849055869444, -3.48537064850209, -3.88686263554285, 
            -7.64299385342407, -12.2306866338295, -4.11792898780907, -6.69406599969166, 
            2.94265685996645, 4.88784955844157, -0.160958738400181, -11.4360877026703, 
            -2.645522917093, -10.9836555618723, -14.2512675815649, -21.4627313313005, 
            -13.3965480017617)
data <- list(t_max = 20,
             mean_x_init = 0,
             prec_x_init = 1/5,
             prec_x = 1/10,
             log_prec_y_true = log(1))#,
#              x_true=x_true,
#              y=y)

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
  pmmh.init(biips, param.names="log_prec_y", 
            inits=inits, n.part=n.part)
}

#       PMMH 
#---------------------

# burn in
out.pmmh_burn <- pmmh.samples(biips, param.names=c("log_prec_y"),
                              n.iter=n.burn,
                              n.part=n.part, rw.rescale=TRUE, rw.learn=TRUE,
                              rw.rescale.type='d', n.rescale=n.iter/4)

prec_y_biips <- exp(out.pmmh_burn$log_prec_y)
# plot PMMH samples trace
plot(drop(prec_y_biips),
     xlab="iteration", ylab="value",
     main=paste("Trace of", n.iter, "BiiPS PMMH burnin samples"))
legend("topright", leg="prec_y", pch=1, bty='n')
plot(sapply(out.pmmh_burn$step, FUN=identity), ylab='rw.step')


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
