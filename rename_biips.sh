#/bin/bash

export src_ext_list="txt md c cc cpp cxx h hh hpp hxx R m tex bib sty in IN bat cfg bug ac win yy ll cmake sed"
export src_name_list="COPYING README NOTICES cleanup Makevars biips_info Doxyfile build_biips.sh git_repair_author.sh"

# rename RBiips to Rbiips in source files
#for ext in $src_ext_list
#do
#    sed $1 $2 $3 -E "s/RBiips/Rbiips/g" *.$ext */*.$ext */*/*.$ext */*/*/*.$ext */*/*/*/*.$ext 2> error_sed | grep --colour -E "Rbiips"
#done

#for name in $src_name_list
#do
#    sed $1 $2 $3 -E "s/RBiips/Rbiips/g" $name */$name */*/$name */*/*/$name */*/*/*/$name 2> error_sed | grep --colour -E "Rbiips"
#done

# rename MatBiips to Matbiips in source files
#for ext in $src_ext_list
#do
#    sed $1 $2 $3 -E "s/MatBiips/Matbiips/g" *.$ext */*.$ext */*/*.$ext */*/*/*.$ext */*/*/*/*.$ext 2> error_sed | grep --colour -E "Matbiips"
#done

#for name in $src_name_list
#do
#    sed $1 $2 $3 -E "s/MatBiips/Matbiips/g" $name */$name */*/$name */*/*/$name */*/*/*/$name 2> error_sed | grep --colour -E "Matbiips"
#done

# rename BiiPS to Biips in source files
for ext in $src_ext_list
do
    sed $1 $2 $3 -E "s/BiiPS/Biips/g" *.$ext */*.$ext */*/*.$ext */*/*/*.$ext */*/*/*/*.$ext 2> error_sed | grep --colour -E "Biips"
done

for name in $src_name_list
do
    sed $1 $2 $3 -E "s/BiiPS/Biips/g" $name */$name */*/$name */*/*/$name */*/*/*/$name 2> error_sed | grep --colour -E "Biips"
done

# rename RBiips* folder an files to Rbiips*
#git mv RBiips Rbiips
#git mv Rbiips/R/RBiips-package.R Rbiips/R/Rbiips-package.R
#git mv Rbiips/src/RBiips.cpp Rbiips/src/Rbiips.cpp
#git mv Rbiips/src/RBiips_utils.cpp Rbiips/src/Rbiips_utils.cpp
#git mv Rbiips/src/RBiips_utils.h Rbiips/src/Rbiips_utils.h

