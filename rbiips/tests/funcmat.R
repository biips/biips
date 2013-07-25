require(RBiips)
## define data
data <- list(
        t.max = 10,
        mean.x.init = c(0,0,1,0),
        prec.x.init = diag(1000*ones(4,1)),
        x.pos = [-10  0],
        mean.v = zeros(2, 1),
        prec.v = diag(1*ones(2,1)),
        prec.y = diag([10 100]),
        delta.t = 1)

## build evaluation functions
.Call("add_function", funcmat, 2, 'myfuncdim', 'myfunceval')
biips <- biips.model('funcmat.bug', data=data, sample.data=FALSE);
x.true = biips$data()$x.true;

## run SMC

build.sampler(p, proposal="auto")
