\name{SP500}
\alias{SP500}
\docType{data}
\title{
%%   ~~ data name/kind ... ~~
S&P 500 stock price index
}
\description{
%%  ~~ A concise (1-5 lines) description of the dataset. ~~
Daily observations (January 2, 1980 until December 30, 1987) from the Standard and Poor's 500 stock price index.
}
\usage{data(SP500)}
\format{
  A data frame with 2023 observations on the following 2 variables.
  \describe{
    \item{\code{Stock.Price}}{a numeric vector}
    \item{\code{Daily.Return}}{a numeric vector}
  }
  Row names contain the dates of each observations.
}
\source{
\href{http://finance.yahoo.com}{Yahoo! Finance}
}
\examples{
data(SP500)
}
\keyword{datasets}
