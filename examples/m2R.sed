# insert setup code chunck
1i\
#+ setup, include=FALSE\nlibrary(knitr)\nopts_chunk$set(cache=TRUE, comment=NA, background='white', fig.show='hold',\n               tidy=FALSE, pars=TRUE)\nknit_hooks$set(pars = function(before, options, envir) {\n  if (before) graphics::par(bty='l')\n})\n

# group together lines and remove "..."
:dots
/\.\.\./{
N
s/\.\.\.//
t dots
}

:symbols
# replace empty code chunk symbols: %% ==> #+
s/^%% *$/#+/M

# replace first title symbols: %% ==> #' #
0,/^%%/M{
    s/^%%/#' #/M
}

# replace intermediate title symbols: %% ==> #' ##
s/^%%/#' ##/M

# replace comment symbols: % ==> #'
s/^%/#'/Mg

## skip substitutions for bugs model 
/\#'     var|\#'     data|\#'     model/,/^$/{
    /^$/!{
        n
        b symbols
    }
}

# end of line comments
s/; *%/ #/

# * text * ==> ** text **
s/\*([[:alnum:] ]+)\*/**\1**/g

# remove final ";"
s/; *$//

# replace install instructions
/Installation of Matbiips/{
    :add_next_line
    /addpath/!{
        N
        b add_next_line
    }
    c\
#' ## Installation of RBiips package\n#' Install *RBiips* package archive depending on your system:\n#' - `RBiips_x.x.x.zip` for Windows\n#' - `RBiips_x.x.x.tgz` for Mac OS X\n#' - `RBiips_x.x.x.tar.gz` for Linux\n\n#+ eval=FALSE\ninstall.packages('path\/to\/RBiips_x.x.x.ext')\n\n#' where `x.x.x` is the version of the package and `ext` is the \n#' archive extension for your system.\n#'\n#' **Note:** Linux installation needs a previous installation of\n#' *BiiPS* libraries and headers as well as *Boost*.\n\n#+\n#' **Load RBiips package**\nrequire(RBiips)
}


s/Matbiips/RBiips/g

s/struct/list/g

# 'arg', arg ==> arg=arg
s/'([[:alnum:]_]*)' *, *\1/\1=\1/g

# struct.field ==> struct$field
/^#'|filename/!s/\.([[:alpha:]][[:alnum:]_]*)/\$\1/g


s/([^_])true/\1TRUE/g
s/([^_])false/\1FALSE/g

# TODO change names in RBiips
s/biips_smc_samples/smc_samples/
s/biips_diagnosis/diagnosis/

s/biips_summary/summary/
s/biips_density/density/

# model$data -> model$data()
s/model\$data/model\$data()/

# TODO italicize _BiiPS_ in .m
/^#'/s/ _([[:alpha:]]*)_ / *\1* /g

# {'x', 'y'} ==> c('x', 'y')
s/\{ *'([[:alnum:]'_, %]+)' *\}/c('\1')/g

# TODO delete output of diagnostic in .m
#s/^[[:alnum:]_]+ *= *diagnostic(.*)$/diagnostic\1/

# TODO rename summary variable to summ in .m
#s/^ *summary/summ/g

# TODO add fun argument to summary
s/'probs' *, */probs=/g

# [xxx] ==> c(xxx)
/\[\[.*\]\]/!s/\[([^][']*)\]/c(\1)/g

# TODO add commas before spaces in.m
# ['xxx' yy] ==> paste('xxx', yy, sep='')
/xlabel|ylabel|title/{
    s/\[([^]]*)\]/paste(\1, sep='')/g
}

# TODO make summary field names match in matbiips and RBiips
s/\$mean/\$Mean/g
s/\$med/\$Med/g

s/^([[:alpha:]_]+)_quant *= *([[:alpha:]_\$]+)quant$/\1_inf = \2'Qu. 0.025'\n\1_sup = \2'Qu. 0.925'/

# x_f_quant(1,:) ==> x_f_inf
s/([[:alnum:]_]*)_quant\( *1 *, *\: *\)/\1_inf/g
# x_f_quant(2,:) ==> x_f_sup
s/([[:alnum:]_]*)_quant\( *2 *, *\: *\)/\1_sup/g

# TODO use figure titles
# remove "figure('name', ..."
s/^ *figure\( *'name' *,.*$//M

# remove "hold on"
s/^ *hold on.*$//M

# remove "set(h,"
s/^ *set *\( *[h0] *,.*$//M

# remove "legend('boxoff')"
s/^ *legend\('boxoff'\).*$//M

# remove "box off"
s/^ *box off.*$//M

# remove subplot
s/^ *subplot.*$//M

s/fill\(/polygon\(/g
s/fliplr\(/rev\(/g

# remove "h = "
s/^ *h *= *//

# insert lightcols
0,/polygon/{
    /polygon/i\
\
lightcols = adjustcolor(palette('default'), alpha.f=.3)
}

# insert palette and legend
/^polygon/{
    i\
palette(lightcols)
    a\
legend('topleft', leg='95 % credible interval', \
       col='blue', pch=15, pt.cex=2, bty='n')\npalette('default')
}

# t_max:-1:1 ==> t_max:1
s/([[:alnum:]_]*) *\: *-1 *\:([[:alnum:]_]*)/\1:\2/g

# change polygon call
/^polygon/{
    # change inner parenthesis to brackets
    :paren_to_brack
    s/([^c])\(([^(){}]*)\)/\1\{\2\}/g
    t paren_to_brack
    
    s/^polygon\( *c\(([^()]*)\) *, *c\(([^()]*)\) *,[ \n\t]*c\(([^()]*)\) *\)/plot(c(\1), c(\2), type='n')\npolygon(c(\1), c(\2),\n        col='blue', border=NA))/
    
    # change brackets back to parenthesis
    :brack_to_paren
    s/\{([^{}]*)\}/(\1)/g
    t brack_to_paren
    n
    b symbols
}

# change plot call
/^plot/s/plot/lines/

/plot|lines|points/{
    s/'linewidth' *, *([[:digit:]])/lwd=\1/
    /'[rgb]'/{
        s/'r'/col='red'/
        s/'g'/col='green3'/
        s/'b'/col='blue'/
    }
    /'\*[rgb]'/{
        s/'\*r'|'r\*'/col='red', pch=8/
        s/'\*g'|'g\*'/col='green3', pch=8/
        s/'\*b'|'b\*'/col='blue', pch=8/
    }
}

# title( ==> title(main= 
s/title\(/title(main=/

# xlabel( ==> title(xlab= 
s/([xy])label\(/title(\1lab=/

# change legend call
s/^legend\( *c\( *('95 %[^']*') *, *([^)]*)\) *\)/legend('topright', leg=c(\2),\n       col=c('green3','red'), lwd=c(3,2), bty='n')/

# change for instructions
/^for/{
    i\
par(mfrow=c(2,2))
    
    s/for *([^ =]+) *= *([^ =]+)/for (\1 in \2) {/
}

# remove num2str
s/num2str\(([^)]*)\)/\1/

# (k) ==> [k]
s/\(k\)/[k]/g

# f(tk) ==> f[[tk]]
s/([fs])\(tk\)/\1[[tk]]/g

# (tk) ==> [tk]
s/\(tk\)/[tk]/g

# end ==> }
s/^( *)end *$/\1}/

# TODO Capitalize legends in .m

# change plot call for densities ######### FIXME
/plot\( *kde_estimates\$[[:alpha:]_]*\$f/{
    /plot\( *kde_estimates\$[[:alpha:]_]*\$s/!{
        N
        b symbols
    }
    s/plot\( *kde_estimates\$([[:alpha:]_]*)\$f([^$), ]*)\$x *, *kde_estimates\$\1\$f\2\$f *\)[\n ]*plot\( *kde_estimates\$\1\$s\2\$x *, *kde_estimates\$\1\$s\2\$f.*$/plot(kde_estimates\$\1\2, col=c('blue','red'), lty=1, sub='',\n         legend.text=FALSE)/
}

# paste('x_{' tk '}', sep='') ==> bquote(x[.(tk)])
/title/s/paste\( *'([[:alpha:]_]*)_\{' *,? *tk *,? *'\}' *, *sep= *'' *\)/bquote(\1[.(tk)])/

# plot(data$ ==> points(data$
/plot/s/plot\(data\$/points(data\$/

# change legend call
/^legend\( *c\( *([^)]*) *\) *\)/{
    s/^legend\( *c\( *([^)]*) *\) *\)/legend('topright', leg=c(\1),\n       col=c('green3','blue','red'), pch=c(NA,NA,8), lty=c(1,1,NA), bg='white')/
    a\
par(mfrow=c(1,1))
}
