require(rmarkdown)

root_dir = "~/workspace/biips-git/examples"

output_dir = "~/Dropbox/Biips/biips-share/website/examples"

clear_cache = FALSE

example_scripts = list()
example_scripts[["tutorial"]] = c("tutorial1", "tutorial2", "tutorial3")
example_scripts[["object_tracking"]] = c("hmm_4d_nonlin")
example_scripts[["stoch_kinetic"]] = c("stoch_kinetic", "stoch_kinetic_gill")
example_scripts[["stoch_volatility"]] = c("stoch_volatility",
                                          "switch_stoch_volatility",
                                          "switch_stoch_volatility_param")

# loop over all example directories
for (ex_dir in names(example_scripts)) {
  # set example output directory
  out_dir = file.path(output_dir, ex_dir, "rbiips")
  dir.create(out_dir, showWarnings=FALSE)
  # loop over all scripts in example directory
  for (ex_script in example_scripts[[ex_dir]]) {
    # change directory
    setwd(file.path(root_dir, ex_dir))
    # clear cache
    if (clear_cache) {
      unlink(paste0(ex_script, "_cache"), recursive=TRUE)
    }
    # render html
    input = paste0(ex_script, ".R")
    rmarkdown::render(input,
                      output_format="html_document")
    # copy to output directory
    file.copy(from=file.path(paste0(ex_script, ".html")),
              to=out_dir,
              overwrite=TRUE)
  }
}