# BUGS model file
model <- file.path(find.package("RBiips"), "extdata", "hmm_1d_lin_param.bug")
model.title <- "Fixed parameter estimation of linear gaussian univariate HMM"

# data
data <- list(t.max = 50,
             mean.x.init = 0,
             prec.x.init = 1,
             prec.x = 50,
             prec.y.true = 1)

# -------------------- JAGS --------------------#
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
  
  # burn in
  n.burn <- 10000
  update(jags, n.burn)
  
  # run jags
  n.iter <- 2000
  out.jags <- jags.samples(jags, c("x","prec.y"), n.iter=n.iter)
  
  prec.y.jags <- out.jags$prec.y
  hist(prec.y.jags)
  prec.y.mean.jags <- summary(out.jags$prec.y, mean)$stat
  print(prec.y.mean.jags)
}

# -------------------- BiiPS PMMH --------------------#
require(RBiips)

n.part <- 50
n.burn <- 50
n.iter <- 500

# random walk starting point
log.prec.y <- runif(1,-1,5)
# random walk standard deviation
rw.sd <- .05

# model
if (run.jags)
  data <- jags$data()

biips <- biips.model(model, data=data, sample.data=!run.jags)
if (!run.jags)
  print(biips)

# PMMH
#-------
# burn in
update.pmmh(biips, variable.names=c("log.prec.y"),
             n.iter=n.burn,
             rw.sd=list(log.prec.y=rw.sd),
             n.part=n.part)
# sample
out.pmmh <- pmmh.samples(biips, variable.names=c("log.prec.y"),
                       n.iter=n.iter,
                       rw.sd=list(log.prec.y=rw.sd),
                       n.part=n.part)

plot(exp(out.pmmh$log.prec.y))
hist(exp(out.pmmh$log.prec.y))

prec.y.mean.biips <- mean(exp(out.pmmh$log.prec.y))
print(prec.y.mean.biips)
