# BUGS model file
model <- file.path("hmm_1d_lin.bug")
model.title <- "Linear gaussian univariate HMM"

# data
data <- list(t.max = 100,
             mean.x.init = 0,
             prec.x.init = 1,
             prec.x = 100,
             prec.y = 1)

par(bty = "n")
# -------------------- JAGS MCMC --------------------#
run.jags <- TRUE 
require(rjags)

# model
n.chains <- 1
jags <- jags.model(model, data=data, n.chains=n.chains)
print(jags)

# burn in
n.burn <- 10000
update(jags, n.burn)

# run jags
n.iter <- 1000
out.jags <- jags.samples(jags, "x", n.iter=n.iter, thin = 1)

x.mean.jags <- summary(out.jags$x, mean)$stat

# model
data <- jags$data()

