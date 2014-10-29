
#' Create a PIMH object.
#'
#' The function \code{biips_pimh_init} initializes the Particle Independent Metropolis-Hastings algorithm.
#'
#' @export
#' @param model \code{biips} model object as returned by \code{\link{biips_model}}.
#' @param variable_names  character vector. The names of the
#'                      unobserved variables to monitor. Names can contain subset
#'                      indices which must define a valid subset of the variables
#'                      of the model.
#'                      Example: \code{c("var1", "var2[1]", "var3[1:10]", "var4[1, 5:10, 3]")}.
#'
#' @return An object of class \code{pimh} which can be used to generate samples from the
#' posterior distribution of the monitored variables in \code{variable_names}.
#'
#' An object of class \code{pimh} is a list of functions that share a common
#' environment. These functions are meant for internal purpose only.
#' They are used to query information on the current state of the algorithm.
#' \item{model()}{Get the \code{\link{biips}} model object.}
#' \item{variable_names()}{Get a character vector with the names of the monitored variables.}
#' \item{sample(sample)}{Get and set the current sample.}
#' \item{log_marg_like(log_marg_like)}{Get and set the current value of the log marginal likelihood.}
#'
#' @seealso \code{\link{biips_model}}, \code{\link{biips_pimh_update}},
#'   \code{\link{biips_pimh_samples}}
#'
#' @examples
#' modelfile <- system.file("extdata", "hmm.bug", package = "Rbiips")
#' stopifnot(nchar(modelfile)>0)
#' cat(readLines(modelfile), sep="\n")
#'
#' data <- list(tmax = 10, logtau = log(10))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' n_part <- 50
#' obj_pimh <- biips_pimh_init(model, "x")  # Initialize
#' out_pimh_burn <- biips_pimh_update(obj_pimh, 100, n_part)  # Burn-in
#' out_pimh <- biips_pimh_samples(obj_pimh, 100, n_part)  # Samples
biips_pimh_init <- function(model, variable_names, ...) {
    stopifnot(is.biips(model))
    stopifnot(is.character(variable_names), length(variable_names) >
        0)
    ## TODO check variable_names

    Rbiips("message", "Initializing PIMH")

    state <- list(sample = list(), log_marg_like = -Inf)

    obj_pimh <- list(model = function() {
        model
    }, variable_names = function() {
        variable_names
    }, sample = function(sample) {
        if (!missing(sample)) state$sample <<- sample
        invisible(state$sample)
    }, log_marg_like = function(log_marg_like) {
        if (!missing(log_marg_like)) state$log_marg_like <<- log_marg_like
        invisible(state$log_marg_like)
    })
    class(obj_pimh) <- "pimh"

    return(obj_pimh)
}

#' Manipulate PIMH objects.
#'
#' @name pimh-object
#' @aliases pimh biips_pimh_update biips_pimh_samples
#' @param object a \code{pimh} object as returned by \code{\link{biips_pimh_init}}.
#' @seealso \code{\link{biips_pimh_init}}
NULL


#' @rdname pimh-object
#' @export
#' @return The function \code{is.pimh} returns \code{TRUE} if the object is of class \code{pimh}.
is.pimh <- function(object) {
    return(class(object) == "pimh")
}


#' @keywords internal
pimh_algo <- function(object, n_iter, n_part, return_samples,
    thin = 1, output = "l", ...) {
    ## check arguments
    stopifnot(is.pimh(object))
    stopifnot(is.numeric(n_iter), length(n_iter) == 1, n_iter >=
        1, is.finite(n_iter))
    n_iter <- as.integer(n_iter)
    stopifnot(is.numeric(n_part), length(n_part) == 1, n_part >=
        1, is.finite(n_part))
    n_part <- as.integer(n_part)
    stopifnot(is.logical(return_samples), length(return_samples) ==
        1)
    stopifnot(is.numeric(thin), length(thin) == 1, thin >= 1,
        thin <= n_iter)
    thin <- as.integer(thin)
    stopifnot(is.character(output))
    output <- unlist(strsplit(output, NULL))
    output <- match.arg(output, c("l"), several.ok = TRUE)
    output <- unique(output)

    ## stop biips verbosity
    verb <- Rbiips("verbosity", 0)
    on.exit(Rbiips("verbosity", verb))

    ## Initialization --------------------------------

    ## monitor variables
    variable_names <- object$variable_names()
    pn <- parse_varnames(variable_names)
    # monitored <- is_monitored(object$model(), variable_names,
    # 's', FALSE) if (!monitored)
    monitor(object$model(), variable_names, type = "s")

    ## build smc sampler if (!Rbiips('is_sampler_built',
    ## object$model()$ptr())) Rbiips('build_smc_sampler',
    ## object$model()$ptr(), FALSE)

    ## Get sample and log likelihood from PIMH object
    sample <- object$sample()
    log_marg_like <- object$log_marg_like()

    ## Output structure with MCMC samples
    n_samples <- floor(n_iter/thin)
    ind_sample <- 0

    out <- list()
    if ("l" %in% output)
        out$log_marg_like <- mcmcarray(dim = c(1, n_samples), name = "log_marg_like")

    ## display message and progress bar
    mess <- if (return_samples)
        "Generating PIMH samples with" else "Updating PIMH with"
    Rbiips("message", paste(mess, n_part, "particles"))
    bar <- Rbiips("progress_bar", n_iter, "*", "iterations")
    ### TODO: display expected time of run

    ## Independant Metropolis-Hastings iterations ------------------------------
    for (i in 1:n_iter) {
        ## SMC
        smc_forward_algo(object$model(), n_part = n_part, ...)

        ## Acceptance rate
        log_marg_like_prop <- Rbiips("get_log_norm_const", object$model()$ptr())
        log_ar <- log_marg_like_prop - log_marg_like

        ## Accept/Reject step
        if (runif(1) < exp(log_ar)) {
            log_marg_like <- log_marg_like_prop

            ## sample one particle
            sampled_value <- Rbiips("sample_gen_tree_smooth_particle",
                object$model()$ptr(), get_seed())
            for (var in variable_names) {
                var_in <- to_biips_vname(var)
                sample[[var]] <- sampled_value[[var_in]]
            }
        }

        ## Store output
        if ((i%%thin) == 0) {
            ind_sample <- ind_sample + 1

            if ("l" %in% output)
                out$log_marg_like[ind_sample] <- log_marg_like

            if (return_samples) {
                if (ind_sample == 1) {
                  ## pre-allocation here to be sure that sample is not empty
                  for (ivar in 1:length(variable_names)) {
                    var <- variable_names[[ivar]]
                    out[[var]] <- mcmcarray(dim = c(dim(sample[[var]]),
                                                    n_samples),
                                            name = pn$names[[ivar]],
                                            lower = pn$lower[[ivar]],
                                            upper = pn$upper[[ivar]])
                  }
                }

                for (var in variable_names) {
                  len <- length(sample[[var]])
                  from <- (ind_sample - 1) * len + 1
                  to <- (ind_sample - 1) * len + len
                  out[[var]][from:to] <- sample[[var]]
                }
            }
        }

        ## progress bar
        Rbiips("advance_progress_bar", bar, 1)
    }

    ## Release monitor memory
    clear_monitors(object$model(), type = "s", release_only = TRUE)

    ## Update PIMH object with current sample and log marginal
    ## likelihood
    object$sample(sample)
    object$log_marg_like(log_marg_like)

    class(out) <- "mcmcarray.list"

    ### TODO: Remove singleton dimensions for vectors? (cf
    ### matbiips)
    if (return_samples)
        return(out) else return(invisible(out))
}



#' @export
biips_pimh_update <- function(object, ...) UseMethod("biips_pimh_update")

#' The method \code{biips_pimh_update} performs burn-in iterations for the PIMH algorithm.
#'
#' @export
#' @rdname pimh-object
#' @param n_iter   integer. Number of burn-in iterations.
#' @param thin     integer. Thinning interval. Returns samples every \code{thin} iterations
#'                   (default = 1)
#' @param n_part   integer. Number of particles used in SMC algorithms.
#' @param rs_thres,rs_type,... Additional arguments to be passed to the SMC algorithm.
#'   See \code{\link{biips_smc_samples}} for more details.
#'@param output string. Select additional members to be returned in the \code{\link{mcmcarray.list}} output.
#'  \code{output = "l"} returns a \code{\link{mcmcarray}} of the log marginal likelihood estimates
#'  over iterations.
#'
#' @return The methods \code{biips_pimh_update} and \code{biips_pimh_update} return
#'   an object of class \code{\link{mcmcarray.list}}.
#'
#' @return \code{biips_pimh_samples} output contains one \code{\link{mcmcarray}}
#'   member for each monitored variable returned by the \code{variable_names()}
#'   member function of the \code{pimh} object.
#'
#' @return If the \code{output} argument is \code{"l"}, the output contains a member named \code{log_marg_like} with
#'   a \code{\link{mcmcarray}} of the log marginal likelihood estimates over iterations.
#'
#' @return Members of the \code{\link{mcmcarray.list}} object are \code{\link{mcmcarray}}
#'   objects for different variables.
#'   Assuming \code{dim} is the dimension of the monitored variable, the \code{\link{mcmcarray}}
#'   object is an array of dimension \code{c(dim, n_iter)} with the following
#'   attributes (accessible with \code{\link[base]{attr}}):
#'   \item{name}{string with the name of the variable.}
#'   \item{lower}{vector with the lower bounds of the variable.}
#'   \item{upper}{vector with the upper bounds of the variable.}
#'
#' @examples
#' modelfile <- system.file("extdata", "hmm.bug", package = "Rbiips")
#' stopifnot(nchar(modelfile)>0)
#' cat(readLines(modelfile), sep="\n")
#'
#' data <- list(tmax = 10, logtau = log(10))
#' model <- biips_model(modelfile, data, sample_data = TRUE)
#'
#' n_part <- 50
#' obj_pimh <- biips_pimh_init(model, "x")  # Initialize
#' is.pimh(obj_pimh)
#' out_pimh_burn <- biips_pimh_update(obj_pimh, 100, n_part)  # Burn-in
#' out_pimh <- biips_pimh_samples(obj_pimh, 100, n_part)  # Samples
biips_pimh_update.pimh <- function(object, n_iter, n_part, thin = 1,
    output = "l", ...) {
    out <- pimh_algo(object, n_iter = n_iter, n_part = n_part,
        thin = thin, return_samples = FALSE, output = output,
        ...)
    return(invisible(out))
}

#' @export
biips_pimh_samples <- function(object, ...) UseMethod("biips_pimh_samples")

#' The method \code{biips_pimh_samples} performs iterations for the PIMH
#' algorithm and returns samples.
#'
#' @export
#' @rdname pimh-object
biips_pimh_samples.pimh <- function(object, n_iter, n_part, thin = 1,
    output = "l", ...) {
    out <- pimh_algo(object, n_iter = n_iter, n_part = n_part,
        thin = thin, return_samples = TRUE, output = output,
        ...)
    return(out)
}
