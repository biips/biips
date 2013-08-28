require(RBiips)
## define data
t.max <- 10
data <- list(
        t.max = t.max,
        mean.x.init = c(0,0,1,0),
        prec.x.init = diag(4)*1000,
        x.pos = c(-10, 0),
        mean.v = c(0, 0),
        prec.v = diag(2),
        prec.y = diag(c(10,100)))

delta.t = 1;
F <- matrix(c(  1, 0, delta.t, 0,
                0, 0, 1,       0,
                delta.t, 0, 0, 1,
                0, 0,       0, 1),
                ncol=4)
G <- matrix(c(delta.t^2/2, 0,
              0, delta.t^2/2,
              delta.t,      0,
              0, delta.t),ncol = 2)

myfuncdim = function (x,v) { c(4,1) };
myfunceval = function (x,v) { F%*%x +G%*%v } 
myfuncheckparam = function (x,v) { TRUE }
myfunisdiscrete = function (x,v) { FALSE }


## build evaluation functions
zut <- .Call("add_function", "funcmat", 2, myfuncdim , myfunceval, myfuncheckparam, myfunisdiscrete)
biips <- biips.model('funcmat.bug', data=data, sample.data=FALSE);
x.true = biips$data()$x.true;

## run SMC

build.sampler(biips, proposal="prior")

#run biips 
n.part <- 100
type <- 'fsb'
out.biips <- smc.samples(biips, paste("x[1:2,1:",t.max,"]"),
                         n.part=n.part, type=type)
# summary
x.summ <- summary(out.biips$x)
print(x.summ)

# diagnostic
print(diagnostic(out.biips$x))

