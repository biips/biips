require(RBiips)

# data
t.max <- 20
mean.x.init <- 0
var.x.init <- 5
var.x <- 10
var.y <- 1
data <- list(t.max = t.max,
             mean.x.init = mean.x.init,
             var.x.init = var.x.init,
             var.x = var.x,
             var.y = var.y)

# model
biips <- biips.model("/home/adrien-inria/workspace/biips/trunk/RBiips/hmm_1d_non_lin.bug", data)

# monitor
type <- c("filtering", "smoothing", "backward.smoothing")
monitor.biips(biips, "x", type=type)

# run biips
results <- run.biips(biips, n.part=1000, backward=TRUE, rs.thres=0.5, rs.type="stratified", proposal="auto")

# summary
print(s <- summary(results$x))

# plot kernel density estimates
plot(dens <- density(results$x, adjust=1))

# diagnostic
print(diagnostic(results$x))

plot(s)
