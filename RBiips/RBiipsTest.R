require(RBiips)

# data
t.max <- 20
mean.x.init <- 0
var.x.init <- 1
var.x <- 1
var.y <- 0.5
y <- array(c(1.51773, 0.61987, 2.12763, 1.63934, 0.717594, 0.0663489, -0.319577, 3.68118, 5.68613, 5.52113, 7.11676, 7.89746, 8.01781, 7.22303, 10.1171, 9.20578, 9.93172, 12.8435, 12.0815, 13.891), dim=c(t.max))

data <- list(t.max = t.max,
             mean.x.init = mean.x.init,
             var.x.init = var.x.init,
             var.x = var.x,
             var.y = var.y, y=y)

# model
biips <- biips.model("/home/adrien-inria/workspace/biips/branches/0.06/RBiips/hmm_1d_lin_data.bug", data, sample.data=FALSE)

# monitor
type <- c("filtering", "smoothing", "backward.smoothing")
monitor.biips(biips, "x", type=type)

# run biips
results <- run.biips(biips, n.part=1000, backward=FALSE, rs.thres=0.5, rs.type="stratified", proposal="auto")

# summary
print(s <- summary(results$x))

# plot kernel density estimates
plot(dens <- density(results$x, adjust=2))

# diagnostic
print(diagnostic(results$x))

plot(s)
