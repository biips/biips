## COPY: Adapted from rjags package file: jags.R

##' helper function to call RBiips c++ routines
RBiips <- function(funcname, ...) {
    stopifnot(is.character(name), length(name) == 1, nchar(name) > 0)
    .Call(funcname, ..., PACKAGE = "RBiips")
}


##' load the corresponding module into the Biips environment
##' @param name the module name
##' @param quiet verbose flag
##' @return null
biips_load_module <- function(name, quiet = FALSE) {
    stopifnot(is.character(name), length(name) == 1, nchar(name) > 0)
    stopifnot(is.logical(quiet), length(quiet) == 1)
    
    ok <- RBiips("load_module", name)
    if (!ok) {
        stop("module ", name, " not found\n")
    } else if (!quiet) {
        message("module", name, "loaded\n")
    }
    invisible()
}



##' Add the corresponding R function to the bugs model
##' @param name of the new  function
##' @param number of arguments of the new function
##' @param R function returning a vector containg arguments sizes
##' @param R function computing the result of function
##' @param R function checking the arguments 
##' @param R function telling is new function is discrete wrt its arguments
##' @export
biips_add_function <- function(name, nb.args, fundim, funeval, funcheckparam, funisdiscrete) {
    stopifnot(is.character(name), length(name) == 1, nchar(name) > 0)
    RBiips("add_function", name, nb.args, fundim, funeval, funcheckparam, funisdiscrete)
    invisible()
}



# TODO
is_legal_vname <- function(name) {
    TRUE
}


##' Preprocess the data (list or env) to list for stan
##' @param data  A list or an environment of objects.
##' @details stop if no-name lists; duplicate names.
##' stop if the objects given name is not found.
##' remove NULL, non-numeric elements
##' @author Adapted from rstan.
data_preprocess <- function(data) {
    
    if (is.environment(data)) {
        data <- as.list(data)
    } else if (is.list(data)) {
        v <- names(data)
        if (is.null(v)) 
            stop("data must be a named list")
        
        if (any(duplicated(v))) {
            stop("duplicated names in data list: ", paste(v[duplicated(v)], collapse = " "))
        }
    } else {
        stop("data must be a list or an environment")
    }
    
    names <- names(data)
    for (n in names) {
        if (!is_legal_vname(n)) 
            stop("data with name ", n, " is not allowed in Biips")
    }
    
    data <- lapply(data, FUN = function(x) {
        ## change data.frame to array
        if (is.data.frame(x)) {
            x <- data.matrix(x)
        }
        
        # remove those not numeric data
        if (!is.numeric(x)) {
            x <- NULL
        }
        
        return(x)
    })
    
    data[!sapply(data, is.null)]
}


##' Make a list using names
##' @param names character strings of names of objects
##' @param env the environment to look for objects with names.
##' @note we use inherits = TRUE when calling mget
##' @author Adapted from rstan.
mklist <- function(names, env = parent.frame()) {
    d <- mget(names, env, ifnotfound = NA, inherits = TRUE, mode = "numeric")
    n <- which(is.na(d))
    if (length(n) > 0) {
        stop("objects ", paste("'", names[n], "'", collapse = ", ", sep = ""), " not found")
    }
    d
}


get_seed <- function() {
    sample.int(.Machine$integer.max, size = 1)
}



##' Create a biips model object
##' 
##' \code{biips_model} is used to create an object representing a Bayesian
##' graphical model, specified with a BUGS language description of the prior
##' distribution, and a set of data.
##' 
##' @param file a file containing a description of the model in the BiiPS/JAGS
##' dialect of the BUGS language.
##' 
##' Alternatively, \code{file} can be a readable text-mode connection, or a
##' complete URL.
##' @param data a list or environment containing the data. Any numeric objects
##' in \code{data} corresponding to node arrays used in \code{file} are taken
##' to represent the values of observed nodes in the model
##' @param sample_data logical flag. If \code{FALSE} then the \code{data} block
##' of the model will be ignored.
##' @param quiet if \code{TRUE} then messages generated during compilation will
##' be suppressed.
##' @return \code{biips_model} returns an object inheriting from class
##' \code{biips} which can be used to generate dependent samples from the
##' posterior distribution of the parameters
##' 
##' An object of class \code{biips} is a list of functions that share a common
##' environment. The functions can be used to query information on the model.
##' \item{ptr()}{Returns an external pointer to an object created by the BiiPS
##' library.} 
##' \item{data()}{Returns a list containing the data that defines the
##' observed nodes in the model.} 
##' \item{model()}{Returns a character vector
##' containing the BUGS-language representation of the model.}
##' \item{print_dot(file)}{Writes a description of the graph in dot language in
##' \code{file}.} 
##' \item{nodes(type, observed)}{Returns a \code{data.frame}
##' containing information on all the nodes of the graph: rank in the
##' topological sort, node id, variable name or formula, type, observed. After
##' calling \code{build.sampler} or any algorithm running smc sampler, the
##' \code{data.frame} will also contain a column indicating the node sampler
##' used for unobserved stochastic nodes.} 
##' \item{recompile()}{Recompiles the
##' model using the original data set.}
##' @author Adrien Todeschini, Francois Caron
##' @keywords models graphs
##' @export
##' @examples
##' 
##' ## Should be DIRECTLY executable !! 
##' ##-- ==>  Define data, use random,
##' ##--\tor do  help(data=index)  for the standard data sets.
##' 
biips_model <- function(file, data = parent.frame(), sample_data = TRUE, quiet = FALSE) {
    if (missing(file)) {
        stop("Model file name missing")
    }
    stopifnot(is.logical(sample_data), length(sample_data) == 1)
    stopifnot(is.logical(quiet), length(quiet) == 1)
    
    if (is.character(file)) {
        f <- try(file(file, "rt"))
        if (inherits(f, "try-error")) {
            stop("Cannot open model file \"", file, "\"")
        }
        close(f)
        model_code <- readLines(file, warn = FALSE)
        modfile <- file
    } else if (!inherits(file, "connection")) {
        stop("'file' must be a character string or connection")
    } else {
        model_code <- readLines(file, warn = FALSE)
        modfile <- tempfile()
        writeLines(model_code, modfile)
    }
    
    if (quiet) {
        verb <- RBiips("verbosity", 0)
        on.exit(RBiips("verbosity", verb), add = TRUE)
    }
    
    # check data before compiling model, which typically takes more time
    if (is.character(data)) 
        data <- mklist(data)
    if (length(data) > 0) 
        data <- data_preprocess(data)
    
    # make console and check model
    p <- RBiips("make_console")
    RBiips("check_model", p, modfile)
    
    # discard unused data
    varnames <- RBiips("get_variable_names", p)
    
    v <- names(data)
    relevant_variables <- v %in% varnames
    data <- data[relevant_variables]
    unused_variables <- setdiff(v, varnames)
    if (length(unused_variables) > 0) 
        warning("Unused variables in data: ", paste(unused_variables, collapse = ", "))
    
    # compile model
    RBiips("compile_model", p, data, sample_data, get_seed())
    
    # data after possible sampling (from 'data' block in the BUGS language model)
    model_data <- RBiips("get_data", p)
    
    # local initial parameters of the algorithm of adaptation of the step of random
    # walk in the PMMH algorithm
    rw <- list(rescale = TRUE, learn = FALSE, niter = 1, pmean = 0, lstep = c(), 
        beta = 0.05, alpha = 1, targetprob = 0.234, povertarget = FALSE, ncrosstarget = 10, 
        dim = list(), d = c(), count = 0, buffer = c(), buff_count = c(), mean = c(), 
        cov = c())
    # targetprob = target acceptance probability. The default seems to be a fairly
    # robust optimal value. Comes from Rosenthal 2009, Examples of Adaptives MCMC, p
    # 16. In one-dimensional case, we take 0.44 which is optimal in certain settings.
    
    # ncrosstarget = the value ncrosstarget controls the reduction in the step size
    # when rescale is called. There is no reason to give it an initial value of zero.
    # In fact this is a poor choice since the step size would be immediately halved.
    # We start with a value of 10 so the first change in step size is 10%.
    
    model <- list(ptr = function() {
        p
    }, model = function() {
        model_code
    }, print_dot = function(file) {
        RBiips("print_graphviz", p, file)
        invisible(NULL)
    }, data = function() {
        model_data
    }, .data_sync = function() {
        RBiips("get_data", p)
    }, nodes = function(type, observed) {
        sorted.nodes <- data.frame(RBiips("get_sorted_nodes", p))
        
        if (RBiips("is_sampler_built", p)) {
            samplers <- data.frame(RBiips("get_node_samplers", p))
            sorted.nodes <- cbind(sorted.nodes, samplers)
        }
        if (!missing(type)) {
            if (!is.character(type) || length(type) != 1) {
                stop("Invalid type argument.")
            }
            type <- match.arg(type, c("Constant", "Logical", "Stochastic"))
            sorted.nodes <- sorted.nodes[sorted.nodes["type"] == type, ]
        }
        if (!missing(observed)) {
            if (!is.logical(observed) || length(observed) != 1) {
                stop("Invalid observed argument.")
            }
            sorted.nodes <- sorted.nodes[sorted.nodes["observed"] == observed, ]
        }
        return(sorted.nodes)
    }, recompile = function() {
        ## Clear the console
        RBiips("clear_console", p)
        p <<- RBiips("make_console")
        ## Write the model to a temporary file so we can re-read it
        mf <- tempfile()
        writeLines(model_code, mf)
        RBiips("check_model", p, mf)
        unlink(mf)
        ## Re-compile
        RBiips("compile_model", p, data, FALSE, get_seed())
        invisible(NULL)
    }, .rw_init = function(sample) {
        ## store the dimensions of the variables
        
        ## FIXME
        
        ## store the dimensions of the variables
        sampledim <<- lapply(sample, dim)
        dim_null <- sapply(sampledim, is.null)
        if (any(dim_null)) {
            len <- lapply(sample, length)
            sampledim[dim_null] <<- len[dim_null]
        }
        if (length(rw$dim) != 0) {
            stopifnot(all(mapply(identical, rw$dim, sampledim)))
        } else {
            rw$dim <<- sampledim
            
            rw$niter <<- 1
            rw$pmean <<- 0
            rw$povertarget <<- FALSE
            rw$ncrosstarget <<- 10
            
            rw$d <<- sum(sapply(rw$dim, FUN = prod))
            rw$targetprob <<- if (rw$d == 1) 0.44 else 0.234
            rw$lstep <<- log(0.1/sqrt(rw$d))
            
            # clear learnt covariance matrix
            rw$count <<- 0
            rw$buffer <<- c()
            rw$buff_count <<- c()
            rw$mean <<- c()
            rw$cov <<- c()
        }
        
        invisible(NULL)
    }, .rw_get_step = function() {
        exp(rw$lstep)
    }, .rw_set_step = function(rw_step) {
        ## assign rw step
        
        ## check values and dimensions
        for (n in names(rw_step)) {
            if (any(is.na(rw_step[[n]]))) stop("Missing (NA) rw_step value for variable:", 
                n)
            if (any(is.infinite(rw_step[[n]]))) stop("Infinite rw_step value for variable:", 
                n)
            if (any(is.nan(rw_step[[n]]))) stop("NaN rw_step value for variable:", 
                n)
            if (any(rw_step[[n]] <= 0)) stop("Negative or zero rw_step value for variable:", 
                n)
            if (length(rw_step[[n]]) == 1) {
                rw_step[[n]] <- array(rw_step[[n]], dim = rw$dim[[n]])
            } else {
                if (is.null(dim(rw_step[[n]]))) dim(rw_step[[n]]) <- length(rw_step[[n]])
                if (length(rw_step[[n]]) != length(rw$dim[[n]]) || any(dim(rw_step[[n]]) != 
                  rw$dim[[n]])) stop("Incorrect rw_step dimension for variable:", 
                  n)
            }
        }
        
        # concatenate all log values in a vector always in the order of rw$dim
        rw$lstep <<- c()
        for (n in names(rw$dim)) {
            rw$lstep <<- c(rw$lstep, log(rw_step[[n]]))
        }
        
        # clear rescale parameters
        rw$niter <<- 1
        rw$pmean <<- 0
        rw$povertarget <<- FALSE
        rw$ncrosstarget <<- 10
        
        # clear learnt covariance matrix
        rw$count <<- 0
        rw$buffer <<- c()
        rw$buff_count <<- c()
        rw$mean <<- c()
        rw$cov <<- c()
        
        invisible(NULL)
    }, .rw_adapt = function() {
        return(rw$rescale | rw$learn)
    }, .rw_check_adapt = function() {
        if (rw$pmean == 0 || rw$pmean == 1) {
            return(FALSE)
        }
        # The distance, on a logistic scale, between pmean and the target acceptance
        # probability.
        dist <- abs(log(rw$pmean/(1 - rw$pmean)) - log(rw$targetprob/(1 - rw$targetprob)))
        return(dist < 0.5)
    }, .rw_adapt_off = function() {
        rw$rescale <<- FALSE
        rw$learn <<- FALSE
        invisible(NULL)
    }, .rw_rescale_off = function() {
        rw$rescale <<- FALSE
        invisible(NULL)
    }, .rw_learn_off = function() {
        rw$learn <<- FALSE
        invisible(NULL)
    }, .rw_learn_on = function() {
        rw$rescale <<- FALSE
        rw$learn <<- TRUE
        rw$niter <<- 1
        invisible(NULL)
    }, .rw_proposal = function(sample) {
        # concatenate all variables in a vector always in the order of rw$dim
        sample_vec <- c()
        for (n in names(rw$dim)) {
            sample_vec <- c(sample_vec, sample[[n]])
        }
        stopifnot(length(sample_vec) == rw$d)
        
        if (length(rw$cov) == 0 || (rw$learn && (runif(1) < rw$beta))) {
            # modification with diagonal covariance
            prop_vec <- sample_vec + exp(rw$lstep) * rnorm(rw$d)
        } else {
            eps <- 1/rw$niter
            cov_chol <- t(chol((1 - eps) * rw$cov + eps * diag(0.1^2, nrow = rw$d)))
            prop_vec <- sample_vec + 2.38/sqrt(rw$d) * cov_chol %*% rnorm(rw$d)
        }
        rw$niter <<- rw$niter + 1
        
        # rearrange vectorized parameter to list of arrays with correct dimensions
        prop <- list()
        from <- 1
        for (n in names(rw$dim)) {
            to <- from + prod(rw$dim[[n]]) - 1
            prop[[n]] <- array(prop_vec[from:to], dim = rw$dim[[n]])
            from <- to + 1
        }
        invisible(prop)
    }, .rw_rescale = function(p, type = "d") {
        # We keep a weighted mean estimate of the mean acceptance probability with the
        # weights in favour of more recent iterations
        p <- min(p, 1)
        rw$pmean <<- rw$pmean + 1 * (p - rw$pmean)/rw$niter
        
        if (!rw$rescale) {
            return(NULL)
        }
        
        type <- match.arg(type, c("dureau", "plummer"))
        if (type == "dureau") {
            rw$lstep <<- rw$lstep + rw$alpha^(rw$niter) * (rw$pmean - rw$targetprob)
        } else if (type == "plummer") {
            # The step size is adapted to achieve the target acceptance rate using a noisy
            # gradient algorithm.
            rw$lstep <<- rw$lstep + (p - rw$targetprob)/rw$ncrosstarget
        }
        
        if ((p > rw$targetprob) != rw$povertarget) {
            rw$povertarget <<- !rw$povertarget
            rw$ncrosstarget <<- rw$ncrosstarget + 1
        }
        
        invisible(NULL)
    }, .rw_learn_cov = function(sample, accepted) {
        if (!rw$learn) {
            return(NULL)
        }
        if (accepted || is.null(rw$buffer)) {
            # concatenate all variables in a vector always in the order of rw$dim
            sample_vec <- c()
            for (n in names(rw$dim)) {
                sample_vec <- c(sample_vec, sample[[n]])
            }
            # push sample back in buffer
            rw$buffer <<- rbind(rw$buffer, sample_vec, deparse.level = 0)
            rw$buff_count <<- c(rw$buff_count, 1)
        } else {
            # increment last sample counter
            n <- nrow(rw$buffer)
            rw$buff_count[n] <<- rw$buff_count[n] + 1
        }
        
        naccept <- nrow(rw$buffer)
        d <- ncol(rw$buffer)
        m <- sum(rw$buff_count)
        
        # update mean and covariance
        if (naccept == 2 * d) {
            # empirical mean and covariance of the buffer
            mean_buff <- colSums(rw$buffer * rw$buff_count/m)
            cov_buff <- t(rw$buffer) %*% (rw$buffer * rw$buff_count/m) - outer(mean_buff, 
                mean_buff)
            
            if (length(rw$mean) == 0) {
                rw$mean <<- mean_buff
                rw$cov <<- cov_buff
            } else {
                # update empirical mean and covariance
                n <- rw$count
                N <- n + m
                rw$mean <<- n/N * rw$mean + m/N * mean_buff
                diff <- mean_buff - rw$mean
                rw$cov <<- n/N * rw$cov + m/N * cov_buff + n * m/N^2 * diff %*% t(diff)
                
                rw$count <<- N
            }
            rw$count <<- rw$count + m
            
            # clear buffer
            rw$buffer <<- c()
            rw$buff_count <<- c()
        }
        
        invisible(NULL)
    })
    class(model) <- "biips"
    
    # TRICK: We return functions in model list that use variables of the parent
    # environment (ie the currrent function environment). This specific R trick
    # allows to read and write persistent variables, surrogating a class with private
    # members and their modifiers.
    return(model)
}


##' Try to parse string of form 'a' or 'a[n,p:q,r]' where 'a' is a
##' variable name and n,p,q,r are integers
##' @param varname string containing the name of the variable to sparse
parse_varname <- function(varname) {
    
    v <- try(parse(text = varname, n = 1), silent = TRUE)
    if (!is.expression(v) || length(v) != 1) 
        return()
    
    v <- v[[1]]
    if (is.name(v)) {
        ## Full node array requested
        return(list(name = deparse(v)))
    } else if (is.call(v) && identical(deparse(v[[1]]), "[") && length(v) > 2) {
        ## Subset requested
        ndim <- length(v) - 2
        lower <- upper <- numeric(ndim)
        if (any(nchar(sapply(v, deparse)) == 0)) {
            ## We have to catch empty indices here or they will cause trouble below
            return()
        }
        for (i in 1:ndim) {
            index <- v[[i + 2]]
            if (is.numeric(index)) {
                ## Single index
                lower[i] <- upper[i] <- index
            } else if (is.call(index) && length(index) == 3 && identical(deparse(index[[1]]), 
                ":") && is.numeric(index[[2]]) && is.numeric(index[[3]])) {
                ## Index range
                lower[i] <- index[[2]]
                upper[i] <- index[[3]]
            } else return()
        }
        if (any(upper < lower)) 
            return()
        return(list(name = deparse(v[[2]]), lower = lower, upper = upper))
    }
    return()
}


parse_varnames <- function(varnames) {
    names <- character(length(varnames))
    lower <- upper <- vector("list", length(varnames))
    for (i in seq(along = varnames)) {
        y <- parse_varname(varnames[i])
        if (is.null(y)) {
            stop("Invalid variable subset ", varnames[i])
        }
        names[i] <- y$name
        if (!is.null(y$lower)) {
            lower[[i]] <- y$lower
        }
        if (!is.null(y$upper)) {
            upper[[i]] <- y$upper
        }
    }
    return(list(names = names, lower = lower, upper = upper))
}

check_type <- function(type, several.ok = TRUE) {
    stopifnot(is.character(type))
    type <- unlist(strsplit(type, NULL))
    type <- match.arg(type, c("f", "s", "b"), several.ok = several.ok)
} 
