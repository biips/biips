# BUGS model file
model <- file.path(find.package("RBiips"), "extdata", "stoch_volatility.bug")
model.title <- "Animation of the particle filter applied to a model stochastic volatility"

# data
data(SP500, package="RBiips")
y <- SP500[,"Daily.Return"]
st <- SP500[,"Stock.Price"]
dates <- row.names(SP500)

ind <- 800:850

y <- y[ind]
st <- st[ind]
dates <- dates[ind]

data <- list(t.max = length(ind),
             mean.x.init = 0,
             prec.x.init = 1e-1,
             prec.x = 10,
             nu = 10,
             y = y)

par(bty = "n")
# -------------------- BiiPS SMC --------------------#
require(RBiips)

# model
biips <- biips.model(model, data)
print(biips)
if (interactive()) {
  cat("Type <Return> to continue: ")
  scan(quiet=TRUE)
}

# run biips: smc.samples
n.part <- 25
out.biips <- smc.samples(biips, "x",
                         type="filtering",
                         n.part=n.part, backward=FALSE)

# summary
x.summ <- summary(out.biips[["x"]], fun=c("mean","quantiles"), probs=c(.025,.975))

# -------------------- Particle filtering animation --------------------#
ind.dates <- seq(0, length(dates), 10)

ylim.y <- c(min(st), max(st))
ylim.x <- c(min(out.biips$x$f$values), max(out.biips$x$f$values))
ylim.x <- c(-2,2)
lwd <- 2

palette("default")
mycols <- adjustcolor(palette(), alpha.f = 0.4)
palette(mycols)

t.max <- data$t.max

# iterate over SMC steps
for (t in 1:t.max) {
  
  # plot observation curve
  par(mfcol = c(2,1))
  plot(1:t.max-1, st, lwd=lwd, col=4, bty='n', 
       xlab="", ylab="", type='l', main="Standard and Poor's 500 stock price index",
       xlim=c(0,t.max), ylim=ylim.y, xaxt='n')
  legend("topleft", legend="S&P 500", lty=1, lwd=lwd, col=4, bty='n', inset=c(0.35,0))
  grid()
  axis(1, at=ind.dates, labels=dates[ind.dates+1])
  
  # fill area under the curve
  polygon(c(1:t.max-1, t.max:1-1), c(st, t.max:1-1), col=4, border=NA)
  
  # plot point on the current observation
  points(t-1, st[t], pch=21, cex=2, bg=4, col=NA)
  
  # plot particle state estimate curve
  plot(1:t-1, x.summ$f$Mean[1:t], type='o', lwd=lwd, lty=1, col=4, pch=18,
       xlab="", ylab="", main="Particle filter estimate of the log. stochastic volatility",
       xlim=c(0,t.max), ylim=ylim.x, xaxt='n')
  legend("topleft", legend="Mean",
         lty=1, lwd=lwd, col=4, pch=18, bty='n', inset=c(0.35,0))
  grid()
  axis(1, at=ind.dates, labels=dates[ind.dates+1])
  
  # fill confidence interval area
  polygon(c(1:t-1, t:1-1), c(x.summ$f[[2]][1:t], rev(x.summ$f[[3]][1:t])), col=2, border=NA)
  legend("topleft", legend="95% Confidence interval",
         lty=0, col=NA, pch=22, pt.bg=2, pt.cex=3, bty='n', inset=c(0.5,0))
  
  # plot particles as circles
  points(rep(t-1,n.part), out.biips$x$f$values[t,], col=NA, bg=2, cex=3*n.part*out.biips$x$f$weights[t,], pch=21)
  legend("topleft", legend="Particles",
         lty=0, col=NA, pch=21, pt.bg=2, pt.cex=2, bty='n', inset=c(0.2,0))
}

par(mfcol = c(1,1))
