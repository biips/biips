file(GLOB mfiles *.m)
file(COPY ${mfiles} DESTINATION .)
file(COPY private DESTINATION .)
