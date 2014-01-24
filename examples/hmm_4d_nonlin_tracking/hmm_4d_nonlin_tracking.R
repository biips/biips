# BUGS model file
model <- file.path(find.package("RBiips"), "extdata", "hmm_4d_nonlin_tracking.bug")
model.title <- "Radar tracking"

# data
t.max <- 100
mean.x.init <- c(0,0,1,0)
Prec.x.init <- 1000*diag(4)
delta.t <- 1
seq1 <- c(1,0,delta.t,0, 0,1,0,delta.t, 0,0,1,0, 0,0,0,1)
F <- matrix(seq1,4,4,byrow = T)
seq2 <- c(delta.t^2/2,0, 0,delta.t^2/2, delta.t,0, 0,delta.t)
G <- matrix(seq2, 4,2,byrow = T)
x.pos <- c(-10,0)
mean.v <- c(0,0)
Prec.v <-diag(2) 
Prec.y <- diag(c(10,100)) 

data <- list(t.max = t.max,
             mean.x.init = mean.x.init,
             Prec.x.init = Prec.x.init,
             Prec.y = Prec.y,
             F = F, G = G,
             mean.v = mean.v,
             Prec.v = Prec.v,
             x.pos = x.pos)

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
build.sampler(biips, proposal="prior")

# run biips
n.part <- 1000
backward <- FALSE
if (backward) {
  type <- 'fsb'
} else {
  type <- 'fs'
}

out.biips <- smc.samples(biips, paste("x[1:2,1:",t.max,"]"),
                         n.part=n.part, type=type)

# plot kernel density estimates
if (interactive()) {
  ans <- readline("plot kernel density estimates ? y|[n] :")
  plot.dens <- (ans == "y")
}
if(plot.dens) {
  par(mfcol = c(3, 2))
  plot(density(out.biips$x, adjust=2, subset=paste("x[1,1:",t.max,"]")), bty="n")
  plot(density(out.biips$x, adjust=2, subset=paste("x[2,1:",t.max,"]")), bty="n")
  par(mfcol = c(1, 1))
}

# summary
x.summ <- summary(out.biips$x)
print(x.summ)

# diagnostic
print(diagnostic(out.biips$x))

# -------------------- plot filtering results --------------------#
lty=1:5; lwd=2; col=1:6;
type='l'; xlab="x position"; ylab = "y position";
main = paste(model.title,"fitering mean trajectory estimates", sep="\n")
bty="n"; inset=c(0.01,0.01);

matplot(matrix(c(x.true[1,], x.summ$filtering$Mean[1,]), nrow=t.max),
        matrix(c(x.true[2,], x.summ$filtering$Mean[2,]), nrow=t.max),
        lty=lty, lwd=lwd, col=col,
        type=type, xlab=xlab, ylab=ylab, main=main)
legend(x="topright",
       legend=list("x.true", paste("Particle filtering (n.part=",n.part,")", sep="")),
       bty=bty, inset=inset, lty=lty, lwd=lwd, col=col)

# -------------------- plot smoothing results --------------------#
main = paste(model.title,"smoothing mean trajectory estimates", sep="\n")

matx <- matrix(c(x.true[1,], x.summ$smoothing$Mean[1,]), nrow=t.max)
maty <- matrix(c(x.true[2,], x.summ$smoothing$Mean[2,]), nrow=t.max)
leg <- list("x.true",
            paste("Particle smoothing (n.part=",n.part,")", sep=""))

if (backward)
{
  matx <- matrix(c(matx, x.summ$backward.smoothing$Mean[1,]), nrow=t.max)
  maty <- matrix(c(maty, x.summ$backward.smoothing$Mean[2,]), nrow=t.max)
  leg <- c(leg, list(paste("Particle backward-smoothing (n.part=",n.part,")", sep="")))
}
if (run.jags)
{
  matx <- matrix(c(matx, x.mean.jags[1,]), nrow=t.max)
  maty <- matrix(c(maty, x.mean.jags[2,]), nrow=t.max)
  leg <- c(leg, list(paste("MCMC (n.chains=",n.chains,", n.burn=",n.burn,", n.iter=",n.iter,")", sep="")))
}

matplot(matx,
        maty,
        lty=lty, lwd=lwd, col=col,
        type=type, xlab=xlab, ylab=ylab, main=main)
legend(x="topright",
       legend=leg,
       bty=bty, inset=inset, lty=lty, lwd=lwd, col=col)
