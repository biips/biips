# BUGS model file
model <- file.path(find.package("RBiips"), "extdata", "stoch_volatility_param.bug")
model.title <- "Stochastic volatility"

# data
data(SP500, package="RBiips")
y <- SP500[,"Daily.Return"]

ind <- 1:100

y <- y[ind]

# data
sigma<- 1.0
alpha<- 0.6
beta <- 0.5
data <- list(t.max = length(y),
             sigma = sigma,
             beta = beta,
             alpha = alpha,
             y = y)

par(bty = "n")
# -------------------- JAGS MCMC --------------------#
run.jags <- FALSE
if (interactive()) {
  ans <- readline("Compare BiiPS SMC with JAGS MCMC algorithm (requires rjags package) ? y|[n] :")
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
  n.burn <- 1000
  update(jags, n.burn)
  
  # run jags
  n.iter <- 1000
  out.jags <- jags.samples(jags, "x", n.iter=n.iter)
  
  x.mean.jags <- summary(out.jags$x, mean)$stat
}

# -------------------- BiiPS SMC --------------------#
require(RBiips)

biips <- biips.model(model, data)
if (!run.jags) {
  print(biips)
  if (interactive()) {
    cat("Type <Return> to continue: ")
    scan(quiet=TRUE)
  }
}

# generated data
x.true <- biips$data()$x.true

# build biips
build.sampler(biips, proposal="auto")

# run biips
n.part <- 1000

backward <- FALSE
if (interactive()) {
  ans <- readline("Run BiiPS backward smoothing algorithm (requires O(n.part^2) operations) ? y|[n] :")
  backward <- (ans == "y")
}

out.biips <- smc.samples(biips, "x",
                         n.part=n.part, backward=backward)

# plot kernel density estimates
if (interactive()) {
  ans <- readline("plot kernel density estimates ? y|[n] :")
  plot.dens <- (ans == "y")
}
if(plot.dens) {
  par(mfcol = c(5, 2))
  plot(density(out.biips$x, adjust=2), bty="n")
  par(mfcol = c(1,1))
}

# summary
x.summ <- summary(out.biips$x, fun=c("mean","quantiles"), probs=c(.025,.975))
print(x.summ)
plot(x.summ)

# diagnostic
print(diagnostic(out.biips$x))

# -------------------- plot filtering results --------------------#
lty=1:5; lwd=2; col=1:6; type='l';
bty="n"; inset=c(0.01,0.01);
xlab="time"; ylab = "mean state x";
main = paste(model.title,"filtering mean estimates", sep="\n")
t.max <- data$t.max

matplot(matrix(c(x.summ$filtering$Mean), nrow=t.max),
        lty=lty, lwd=lwd, col=col,
        type=type, xlab=xlab, ylab=ylab, main=main)
legend(x="topright",
       legend=list(paste("Particle filtering (n.part=",n.part,")", sep="")),
       bty=bty, inset=inset, lty=lty, lwd=lwd, col=col)

# -------------------- BiiPS PIMH --------------------#
run.pimh <- FALSE
if (interactive()) {
  ans <- readline("Run BiiPS PIMH algorithm (require rjags package) ? y|[n] :")
  run.pimh <- (ans == "y")
}
if(run.pimh)
{
  n.part.pimh <- 100
  n.burn.pimh <- 50
  n.iter.pimh <- 500
  update.pimh(biips, variable.names="x", n.iter=n.burn.pimh,
              n.part=n.part.pimh)
  out.pimh <- pimh.samples(biips, variable.names="x", n.iter=n.iter.pimh, 
                           n.part=n.part.pimh)
  
  require(rjags)
  x.mean.pimh <- summary(out.pimh$x, mean)$stat
}

# -------------------- plot smoothing results --------------------#
main = paste(model.title,"smoothing mean estimates", sep="\n")

mat <- matrix(c(x.summ$smoothing$Mean), nrow=t.max)
leg <- list(paste("Particle smoothing (n.part=",n.part,")", sep=""))

if (backward)
{
  mat <- matrix(c(mat, x.summ$backward.smoothing$Mean), nrow=t.max)
  leg <- c(leg, list(paste("Particle backward-smoothing (n.part=",n.part,")", sep="")))
}
if (run.jags)
{
  mat <- matrix(c(mat, x.mean.jags), nrow=t.max)
  leg <- c(leg, list(paste("MCMC (n.chains=",n.chains,", n.burn=",n.burn,", n.iter=",n.iter,")", sep="")))
}
if (run.pimh)
{
  mat <- matrix(c(mat, x.mean.pimh), nrow=t.max)
  leg <- c(leg, list(paste("Particle IMH (n.part=",n.part.pimh,", n.burn=",n.burn.pimh,", n.iter=",n.iter.pimh,")", sep="")))
}
matplot(mat,
        lty=lty, lwd=lwd, col=col,
        type=type, xlab=xlab, ylab=ylab, main=main)
legend(x="topright", legend=leg,
       bty=bty, inset=inset, lty=lty, lwd=lwd, col=col)
