# BUGS model file
model <- file.path(find.package("RBiips"), "extdata", "stoch_volatility_param.bug")
model.title <- "Fixed parameter estimation of Stochastic volatility model"

# data
data(SP500, package="RBiips")
y <- SP500[,"Daily.Return"]

ind <- 1:100

y <- y[ind]

# data
sigma<- 1.0
beta <- 0.5
data <- list(t.max = length(y),
             sigma = sigma,
             beta = beta,
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
  n.burn.jags <- 12000
  ptm <- proc.time()[[1]]
  update(jags, n.burn.jags)
  cat(n.burn.jags, "iterations in", proc.time()[[1]] - ptm, "s.\n")
  
  # jags samples
  n.iter.jags <- 2000
  ptm <- proc.time()[[1]]
  out.jags <- jags.samples(jags, c("x","alpha"),
                           n.iter=n.iter.jags)
  cat(n.iter.jags, "iterations in", proc.time()[[1]] - ptm, "s.\n")
  
  par(mfcol = c(2,1))
  # plot mcmc samples trace
  plot(out.jags$alpha,
       xlab="iteration", ylab="value",
       main=paste("Trace of", n.iter.jags, "JAGS MCMC samples"))
  legend("topright", leg="alpha", pch=1, bty="n")
  
  # plot mcmc samples histogram
  hist(out.jags$alpha,
       xlab="value", ylab="frequency",
       main=paste("Histogram of", n.iter.jags, "JAGS MCMC samples"))
  legend("topright", leg="alpha", pch=0, bty="n")
  
  par(mfcol = c(1,1))
}

# -------------------- BiiPS --------------------#
require(RBiips)

n.part <- 50
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
  alpha.all <- seq(0, 0.99, length=100)
  out.sens <- smc.sensitivity(biips,
                              params=list(alpha=alpha.all),
                              n.part=n.part)
  
  plot(alpha.all, out.sens$log.marg.like,
       xlab="alpha", ylab="Log. Marginal Likelihood",
       main="Sensitivity analysis")
  legend("topright", leg="log.marg.like", pch=1, bty="n")
  
  # initialize pmmh with max
  inits <- list(alpha = out.sens$max.param$alpha)
  pmmh.init(biips, param.names=c("alpha"), 
            inits=inits, n.part=n.part)
}

#       PMMH 
#---------------------

# burn in
pmmh.update(biips, param.names=c("alpha"),
            n.iter=n.burn,
            n.part=n.part)
# sample
out.pmmh <- pmmh.samples(biips, param.names=c("alpha"),
                         n.iter=n.iter,
                         n.part=n.part)


par(mfcol = c(2,1))
# plot PMMH samples trace
plot(drop(out.pmmh$alpha),
     xlab="iteration", ylab="value",
     main=paste("Trace of", n.iter, "BiiPS PMMH samples"))
legend("topright", leg="alpha", pch=1, bty="n")

# plot PMMH samples histogram
hist(out.pmmh$alpha,
     xlab="value", ylab="frequency",
     main=paste("Histogram of", n.iter, "BiiPS PMMH samples"))
legend("topright", leg="alpha", pch=0, bty="n")

par(mfcol = c(1,1))

#---------------------- display results ----------------------#
#cat("True value: alpha =", biips$data()$alpha.true, "\n")

# mean value
if (run.jags) {
  cat("JAGS MCMC mean value: alpha =", mean(out.jags$alpha), "\n")
}
cat("BiiPS PMMH mean value: alpha =", mean(out.pmmh$alpha), "\n")
