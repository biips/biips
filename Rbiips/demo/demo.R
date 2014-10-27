require(Rbiips)

## Add function f to BUGS language
f_dim <- function(x_dim, t_dim) {
  # Check dimensions of the input and return
  # dimension of the output of function f
  stopifnot(prod(x_dim)==1, prod(t_dim)==1)
  x_dim
}
f_eval <- function(x, t) {
  # Evaluate function f
  0.5*x+25*x/(1+x^2)+8*cos(1.2*t)
}
biips_add_function('f', 2, f_dim, f_eval)

## Add sampling distribution dMN to BUGS language
dMN_dim <- function(mu_dim, Sig_dim) {
  # Check dimensions of the input and return
  # dimension of the output of distribution dMN
  stopifnot(prod(mu_dim)==mu_dim[1], length(Sig_dim)==2, mu_dim[1]==Sig_dim)
  mu_dim
}
dMN_sample <- function(mu, Sig) {
  # Draw a sample of distribution dMN
  mu+t(chol(Sig))%*%rnorm(length(mu))
}
biips_add_distribution('dMN', 2, dMN_dim, dMN_sample)

## Compile model
modelfile <- system.file("extdata", "hmm.bug", package = "Rbiips")
cat(readLines(modelfile), sep="\n")

data <- list(tmax = 10, logtau = log(10))
model <- biips_model(modelfile, data, sample_data = TRUE)

is.biips(model)
print(model)

model$data()
model$model()

variable.names(model)
biips_variable_names(model)

biips_nodes(model)

dotfile = "hmm.dot"
biips_print_dot(model, dotfile)
cat(readLines(dotfile), sep="\n")

rm(model)

tmax <- 10
logtau <- log(10)
datanames <- c("tmax", "logtau")
model <- biips_model(modelfile, datanames, sample_data = TRUE)

## SMC
biips_build_sampler(model, proposal = "prior")
biips_nodes(model, type = "stoch", observed = FALSE)

biips_build_sampler(model, proposal = "auto")
biips_nodes(model, type = "stoch", observed = FALSE)

n_part <- 100

out_smc <- biips_smc_samples(model, c("x[1]", "x[8:10]"), n_part, type = "fs",
                             rs_thres = 0.5, rs_type = "stratified")

### Manipulate smcarray.fsb.list object
is.smcarray.fsb.list(out_smc)
names(out_smc)
out_smc
biips_diagnosis(out_smc)
biips_summary(out_smc)
par(mfrow=c(2,2))
plot(biips_density(out_smc))

### Manipulate smcarray.fsb object
is.smcarray.fsb(out_smc[["x[8:10]"]])
names(out_smc[["x[8:10]"]])
out_smc[["x[8:10]"]]
biips_diagnosis(out_smc[["x[8:10]"]])
biips_summary(out_smc[["x[8:10]"]])
par(mfrow=c(2,2))
plot(biips_density(out_smc[["x[8:10]"]]))

### Manipulate smcarray object
is.smcarray(out_smc[["x[8:10]"]]$f)
names(out_smc[["x[8:10]"]]$f)
out_smc[["x[8:10]"]]$f
out_smc[["x[8:10]"]]$s
biips_diagnosis(out_smc[["x[8:10]"]]$f)
biips_summary(out_smc[["x[8:10]"]]$f)
par(mfrow=c(2,2))
plot(biips_density(out_smc[["x[8:10]"]]$f))


out_smc <- biips_smc_samples(model, "x", n_part)

out_smc$x$f
out_smc$x$s

biips_diagnosis(out_smc)

summ_smc <- biips_summary(out_smc, order = 2, probs = c(0.025, 0.975))

par(mfrow = c(2, 1))
plot(model$data()$x_true, type = "l", col = "green", xlab = "t",
    ylab = "x[t]")
lines(summ_smc$x$f$mean, col = "blue")
lines(summ_smc$x$s$mean, col = "red")
lines(summ_smc$x$f$quant[[1]], lty = 2, col = "blue")
lines(summ_smc$x$f$quant[[2]], lty = 2, col = "blue")
lines(summ_smc$x$s$quant[[1]], lty = 2, col = "red")
lines(summ_smc$x$s$quant[[2]], lty = 2, col = "red")
legend("topright", leg = c("true", "SMC filtering estimate",
    "SMC smoothing estimate"), lty = 1, col = c("green", "blue",
    "red"), bty = "n")

dens_smc <- biips_density(out_smc, bw='nrd0', adjust=1, n = 100)

plot(dens_smc$x[[1]]$f, col = "blue", xlab = "x[1]", ylab = "posterior density",
    main = NA)
lines(dens_smc$x[[1]]$s, col = "red")
points(model$data()$x_true[1], 0, pch = 17, col = "green")

## PIMH
n_part <- 50
obj_pimh <- biips_pimh_init(model, "x")  # Initialize
out_pimh_burn <- biips_pimh_update(obj_pimh, 200, n_part)  # Burn-in
out_pimh <- biips_pimh_samples(obj_pimh, 200, n_part)  # Samples

par(mfrow = c(2, 2))
plot(c(out_pimh_burn$log_marg_like, out_pimh$log_marg_like),
    type = "l", col = "blue", xlab = "PIMH iteration", ylab = "log p(y)")

plot(out_pimh$x[1, ], type = "l", col = "blue", xlab = "PIMH iteration",
    ylab = "x[1]")
points(0, model$data()$x_true[1], pch = 17, col = "green")

summ_pimh <- biips_summary(out_pimh, order = 2, probs = c(0.025, 0.975))

plot(model$data()$x_true, type = "l", col = "green",
    xlab = "t", ylab = "x[t]")
lines(summ_pimh$x$mean, col = "blue")
lines(summ_pimh$x$quant[[1]], lty = 2, col = "blue")
lines(summ_pimh$x$quant[[2]], lty = 2, col = "blue")
legend("topright", leg = c("true", "PIMH estimate"), lty = c(2, 1),
       col = c("green", "blue"), bty = "n")

dens_pimh <- biips_density(out_pimh)

plot(dens_pimh$x[[1]], col = "blue", xlab = "x[1]", ylab = "posterior density",
    main = NA)
points(model$data()$x_true[1], 0, pch = 17, col = "green")

## SMC sensitivity analysis
n_part <- 50
logtau_val <- -10:10
out_sens <- biips_smc_sensitivity(model, list(logtau = logtau_val),
    n_part)

par(mfrow = c(2, 1))
plot(logtau_val, out_sens$log_marg_like, type = "l", col = "blue",
    xlab = "logtau", ylab = "log p(y|logtau) ", main = "SMC sensitivity")
points(logtau, min(out_sens$log_marg_like), pch = 17, col = "green")

plot(logtau_val, out_sens$log_marg_like_pen, type = "l", col = "blue",
    xlab = "logtau", ylab = "log p(y|logtau)+log p(logtau)")
plml <- out_sens$log_marg_like_pen
ymin <- min(plml[is.finite(plml)])
points(logtau, ymin, pch = 17, col = "green")

## PMMH
data <- list(tmax = 10)
model <- biips_model(modelfile, data, sample_data = TRUE)

n_part <- 50
obj_pmmh <- biips_pmmh_init(model, "logtau", latent_names = "x",
    inits = list(logtau = -2))  # Initialize
out_pmmh_burn <- biips_pmmh_update(obj_pmmh, 200, n_part)  # Burn-in
out_pmmh <- biips_pmmh_samples(obj_pmmh, 200, n_part, thin = 1)  # Samples

par(mfrow = c(2, 2))
plot(c(out_pmmh_burn$log_marg_like_pen, out_pimh$log_marg_like_pen),
    type = "l", col = "blue", xlab = "PMMH iteration", ylab = "log p(y|logtau) + log p(logtau)")

plot(out_pmmh$logtau[1, ], type = "l", col = "blue", xlab = "PMMH iteration",
    ylab = "logtau")
points(0, logtau, pch = 17, col = "green")

summ_pmmh <- biips_summary(out_pmmh, order = 2, probs = c(0.025,
    0.975))

plot(model$data()$x_true, type = "l", col = "green",
    xlab = "t", ylab = "x[t]")
lines(summ_pmmh$x$mean, col = "blue")
lines(summ_pmmh$x$quant[[1]], lty = 2, col = "blue")
lines(summ_pmmh$x$quant[[2]], lty = 2, col = "blue")
legend("topright", leg = c("true", "PMMH estimate"), lty = c(2,
    1), col = c("green", "blue"), bty = "n")

dens_pmmh <- biips_density(out_pmmh)

plot(dens_pmmh$logtau, col = "blue", xlab = "logtau", ylab = "posterior density",
    main = NA)
points(logtau, 0, pch = 17, col = "green")
