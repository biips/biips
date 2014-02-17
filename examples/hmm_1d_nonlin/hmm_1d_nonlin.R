# BUGS model file
model <- file.path("hmm_1d_nonlin.bug")
model.title <- "Nonlinear gaussian univariate HMM"

# data
x_true <- c(2.18058, 13.0408, 4.5904, 0.0172347, 0.0416116, 11.638, 5.31703, 3.56488, 0.494195, 9.97843, 7.31315, 11.3852, 8.14421, 5.3909, -4.92422, 2.57024, 22.0909, 12.6866, 3.02204, 7.16082, 9.8559)
y <- c(9.25676, 1.16413, 0.687314, 2.98908, 6.68515, 2.59973, -0.86668, -0.137264, 5.93527, 2.4605, 5.84379, 1.3678, 0.634563, -0.376496, 2.00287, 24.3569, 8.40757, 1.47439, 2.79131, 4.42062)

data <- list(t_max = length(y),
             mean_x_init = 0,
             prec_x_init = 1/5,
             prec_x = 1/10,
             prec_y = 1,
             y=y)

par(bty = "n")
# -------------------- Exact solution (fine grid method) --------------------#
# True filtering posterior mean
x_gridf_mean = c(-2.93367e-17, 7.33609, 2.52861, 0.950226, 1.74819, 11.6619, 7.74044, 1.20363, 0.899005, 2.41347, -0.00242989, 9.10795, 4.23131, -0.118862, 1.61354, -1.67851, 21.9493, 12.8648, 1.21059, 3.96411, 8.03155)
# True smoothing posterior mean
x_grids_mean = c(1.86667, 13.5568, 3.33718, 3.37443, 7.58229, 11.5843, 7.79238, 2.37667, 0.285016, 1.28586, 6.47747, 10.8999, 4.52618, 1.94444, 0.126584, 4.20127, 21.9551, 12.8964, 2.54215, 5.63901, 8.03155)

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
  n.chains <- 4
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
  out.jags <- jags.samples(jags, "x", n.iter=n.iter)
  
  x.mean.jags <- summary(out.jags$x, mean)$stat
}

# -------------------- BiiPS SMC --------------------#
require(RBiips)

# model
biips <- biips.model(model, data)
if (!run.jags) {
  print(biips)
  if (interactive()) {
    cat("Type <Return> to continue: ")
    scan(quiet=TRUE)
  }
}

# build biips
build.sampler(biips, proposal="auto")

# run biips
n.part <- 1000

backward <- FALSE
if (interactive()) {
  ans <- readline("Run BiiPS backward smoothing algorithm (requires O(n.part^2) operations) ? y|[n] :")
  backward <- (ans == "y")
}
if (backward) {
  type <- 'fsb'
} else {
  type <- 'fs'
}

out.biips <- smc.samples(biips, "x",
                         n.part=n.part, type=type)

# plot kernel density estimates
if (interactive()) {
  ans <- readline("plot kernel density estimates ? y|[n] :")
  plot.dens <- (ans == "y")
}
if(plot.dens) {
  par(mfcol = c(3, 1))
  plot(density(out.biips$x, adjust=2), bty="n")
  par(mfcol = c(1,1))
}

# summary
x.summ <- summary(out.biips$x, fun=c("mean","quantiles"), probs=c(.025,.975))
print(x.summ)
if (backward) {
  par(mfcol = c(3, 1))
} else {
  par(mfcol = c(2, 1))
}
plot(x.summ)
par(mfcol = c(1,1))

# diagnostic
print(diagnostic(out.biips$x))

# -------------------- plot filtering results --------------------#
lty=1:5; lwd=2; col=1:6; type='l';
bty="n"; inset=c(0.01,0.01);
xlab="time"; ylab = "mean state x";
main = paste(model.title,"filtering mean estimates", sep="\n")
t_max <- data$t_max

matplot(matrix(c(x_true, x_gridf_mean, x.summ$filtering$Mean), nrow=t_max+1),
        lty=lty, lwd=lwd, col=col,
        type=type, xlab=xlab, ylab=ylab, main=main)
legend(x="topright",
       legend=list("x_true", "Grid method filter",
                   paste("Particle filtering (n.part=",n.part,")", sep="")),
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
  pimh.update(biips, variable.names="x", n.iter=n.burn.pimh,
              n.part=n.part.pimh)
  out.pimh <- pimh.samples(biips, variable.names="x", n.iter=n.iter.pimh, 
                           n.part=n.part.pimh)
  
  require(rjags)
  x.mean.pimh <- summary(out.pimh$x, mean)$stat
}

# -------------------- plot smoothing results --------------------#
main = paste(model.title,"smoothing mean estimates", sep="\n")

mat <- matrix(c(x_true, x_grids_mean, x.summ$smoothing$Mean), nrow=t_max+1)
leg <- list("x_true", "Grid method smoother",
            paste("Particle smoothing (n.part=",n.part,")", sep=""))

if (backward)
{
  mat <- matrix(c(mat, x.summ$backward.smoothing$Mean), nrow=t_max+1)
  leg <- c(leg, list(paste("Particle backward-smoothing (n.part=",n.part,")", sep="")))
}
if (run.jags)
{
  mat <- matrix(c(mat, x.mean.jags), nrow=t_max+1)
  leg <- c(leg, list(paste("MCMC (n.chains=",n.chains,", n.burn=",n.burn,", n.iter=",n.iter,")", sep="")))
}
if (run.pimh)
{
  mat <- matrix(c(mat, x.mean.pimh), nrow=t_max+1)
  leg <- c(leg, list(paste("Particle IMH (n.part=",n.part.pimh,", n.burn=",n.burn.pimh,", n.iter=",n.iter.pimh,")", sep="")))
}
matplot(mat,
        lty=lty, lwd=lwd, col=col,
        type=type, xlab=xlab, ylab=ylab, main=main)
legend(x="topright", legend=leg,
       bty=bty, inset=inset, lty=lty, lwd=lwd, col=col)
