# replace comment symbols: % ==> #'
s/^%/#'/

# remove uncommented
/#'.*|^$/ !d

# remove everything after %% Biips Project
/^#' Biips Project/,$ d

# add @export 
1,/^#' [A-Z_]+ ([[:alnum:]])+/ {
  /^#' [A-Z_]+ ([[:alnum:]])+/ a #' @export
}

# remove function name on the first line
s/^#' [A-Z_]+ ([[:alnum:]])/#' \U\1/

# group together lines and remove "..."
:dots
/\.\.\.$/{
  N
  s/\.\.\.$//
  t dots
}

# remove function example
1,/[[:alnum:]_\.]+\(.*\)/ {
  /[[:alnum:]_\.]+\(.*\)/ d
}

# Matlab ==> R
s/matlab|MATLAB|Matlab/R/g

# input section
#=======================
/^#' *I(nput|NPUT)s?:?/,/^#' *O(utput|UTPUT)s?:?/ {
  # remove input
  /I(nput|NPUT)s?:?/ d
  /(O|o)ptional *inputs?:?/ d

  # arguments items
  s/^#' *- *([[:alnum:]_]*) *\:/#' @param \1 /

  # structure ==> object
  s/structure/object/g

  # struct ==> list
  s/struct/list/g

  # cell of strings ==> character vector
  s/cell of strings/character vector/g

  # true ==> TRUE
  s/'true'/TRUE/g
  s/true/TRUE/g

  # false ==> FALSE
  s/'false'/FALSE/g
  s/false/FALSE/g
}

# output section
#=======================
/^#' *O(utput|UTPUT)s?:?/,$ {
  # remove output
  /O(utput|UTPUT)s?:?/ d
  /(O|o)ptional *outputs?:?/ d

  # process arguments items
  s/^#' *- *([[:alnum:]_]*) *:/#' @return \1 /

  # process output items
  s/^#' *\* *([[:alnum:]_]*) *:/#' \\item{\1} /

}


# see also
#=======================
s/ *See also/ @seealso/
# to lower case
/@seealso/ s/ ([[:alnum:]_\.]+)/ \\code{\\link{\L\1}}/g

# example section
#=======================
s/^#' E(XAMPLE|xample)(S|s)? *:?/#' @examples/

/@examples/,/^#'--+/ {

  # = ==> <-
  s/=/<-/

  # remove final ";"
  s/;$//

  # ; ==> ,
  s/;/,/g

  # struct ==> list
  s/struct/list/g

  # 'arg', val ==> arg=val
  s/'([[:alnum:]_]+)' *, *([[:alnum:]_\.]+)/\1=\2/g

  # {...} ==> list(...)
  s/\{(.+)\}/list(\1)/

}

# remove %-------
/^#'--+/ d

