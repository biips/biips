# BUGS model file
model <- file.path(find.package("RBiips"), "extdata", "hmm_1d_lin.bug")
model.title <- "Linear gaussian univariate HMM"

# data
data <- list(t.max = 100,
             mean.x.init = 0,
             prec.x.init = 1,
             prec.x = 100,
             prec.y = 1)

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
  n.burn <- 10000
  update(jags, n.burn)
  
  # run jags
  n.iter <- 1000
  out.jags <- jags.samples(jags, "x", n.iter=n.iter, thin = 1)
  
  x.mean.jags <- summary(out.jags$x, mean)$stat
}

# -------------------- BiiPS SMC --------------------#
require(RBiips)

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

# generated data
x.true <- biips$data()$x.true
y <- biips$data()$y

# build biips
build.sampler(biips, proposal="auto")

# run biips
n.part <- 1000

backward <- FALSE
if (interactive()) {
  ans <- readline("Run BiiPS backward smoothing algorithm (requires O(n.part^2) iterations) ? y|[n] :")
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
x.summ <- summary(out.biips$x, fun=c("mean","quantiles"), probs=c(.05,.95))
print(x.summ)
plot(x.summ)

# diagnostic
print(diagnostic(out.biips$x))

# -------------------- Kalman --------------------#
run.kalman <- FALSE
if (interactive()) {
  ans <- readline("Compare BiiPS with Kalman algorithm (requires sspir package) ? y|[n] :")
  run.kalman <- (ans == "y")
}
if(run.kalman)
{
  require(sspir)
  model.sspir <- sspir::SS(Fmat = function(tt, x, phi) {return(matrix(1))},
                        Gmat = function(tt, x, phi) {return(matrix(1))},
                        Vmat = matrix(1/data$prec.y),
                        Wmat = matrix(1/data$prec.x),
                        y=as.matrix(y),
                        m0=matrix(data$mean.x.init),
                        C0=matrix(1/data$prec.x.init))
  
  # Kalman filter
  kf.sspir <- sspir::kfilter(model.sspir)
  x.kf.mean <- kf.sspir$m
  x.kf.inf <- x.kf.mean-1.96*as.numeric(kf.sspir$C)
  x.kf.sup <- x.kf.mean+1.96*as.numeric(kf.sspir$C)
  
  # Kalman smoother
  ks.sspir <- sspir::smoother(kf.sspir)
  x.ks.mean <- ks.sspir$m
  x.ks.inf <- x.ks.mean-1.96*as.numeric(ks.sspir$C)
  x.ks.sup <- x.ks.mean+1.96*as.numeric(ks.sspir$C)
}

# -------------------- plot filtering results --------------------#
lty=1:2; lwd=c(2,2); col=1:2;
lty.leg=1:2; lwd.leg=c(2,2); col.leg=1:2;
type='l'; bty="n"; inset=c(0.01,0.01);
xlab="time"; ylab = "mean state x";
main = paste(model.title, "filtering mean estimates", sep="\n")
t.max <- data$t.max

mat <- matrix(c(x.true, x.summ$filtering$Mean), nrow=t.max)
leg <- list("x.true",
            paste("Particle filtering (n.part=",n.part,")", sep=""))

if (run.kalman)
{
  mat <- matrix(c(mat, x.kf.mean, x.kf.inf, x.kf.sup), nrow=t.max)
  leg <- c(leg, list("Kalman filter",
                     "Kalman filter 95% CI"))
  lty <- c(lty, 4,3,3)
  lty.leg <- c(lty.leg, 4,3)
  lwd <- c(lwd, 2,1,1)
  lwd.leg <- c(lwd, 2,1)
  col <- c(col, 3,3,3)
  col.leg <- c(col.leg, 3,3)
}

matplot(mat,
        lty=lty, lwd=lwd, col=col,
        type=type, xlab=xlab, ylab=ylab, main=main)
legend(x="topright", legend=leg,
       bty=bty, inset=inset, lty=lty.leg, lwd=lwd.leg, col=col.leg)

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
lty=1:2; lwd=c(2,2); col=1:2;
lty.leg=1:2; lwd.leg=c(2,2); col.leg=1:2;
main = paste(model.title, "smoothing mean estimates", sep="\n")

mat <- matrix(c(x.true, x.summ$smoothing$Mean), nrow=t.max)
leg <- list("x.true",
            paste("Particle smoothing (n.part=",n.part,")", sep=""))

if (backward)
{
  mat <- matrix(c(mat, x.summ$backward.smoothing$Mean), nrow=t.max)
  leg <- c(leg, list(paste("Particle backward-smoothing (n.part=",n.part,")", sep="")))
  lty <- c(lty, 5)
  lty.leg <- c(lty.leg, 5)
  lwd <- c(lwd, 2)
  lwd.leg <- c(lwd.leg, 2)
  col <- c(col, 3)
  col.leg <- c(col.leg, 3)
}
if (run.kalman)
{
  mat <- matrix(c(mat, x.ks.mean, x.ks.inf, x.ks.sup), nrow=t.max)
  leg <- c(leg, list("Kalman smoother",
                     "Kalman smoother 95% CI"))
  lty <- c(lty, 3,3,3)
  lty.leg <- c(lty.leg, 3,3)
  lwd <- c(lwd, 2,1,1)
  lwd.leg <- c(lwd.leg, 2,1)
  col <- c(col, 4,4,4)
  col.leg <- c(col.leg, 4,4)
}
if (run.jags)
{
  mat <- matrix(c(mat, x.mean.jags), nrow=t.max)
  leg <- c(leg, list(paste("MCMC (n.chains=",n.chains,", n.burn=",n.burn,", n.iter=",n.iter,")", sep="")))
  lty <- c(lty, 4)
  lty.leg <- c(lty.leg, 4)
  lwd <- c(lwd, 2)
  lwd.leg <- c(lwd.leg, 2)
  col <- c(col, 5)
  col.leg <- c(col.leg, 5)
}
if (run.pimh)
{
  mat <- matrix(c(mat, x.mean.pimh), nrow=t.max)
  leg <- c(leg, list(paste("Particle IMH (n.part=",n.part.pimh,", n.burn=",n.burn.pimh,", n.iter=",n.iter.pimh,")", sep="")))
  lty <- c(lty, 5)
  lty.leg <- c(lty.leg, 5)
  lwd <- c(lwd, 2)
  lwd.leg <- c(lwd.leg, 2)
  col <- c(col, 6)
  col.leg <- c(col.leg, 6)
}
matplot(mat,
        lty=lty, lwd=lwd, col=col,
        type=type, xlab=xlab, ylab=ylab, main=main)
legend(x="topright", legend=leg,
       bty=bty, inset=inset, lty=lty.leg, lwd=lwd.leg, col=col.leg)
