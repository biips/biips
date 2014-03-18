# macro to add libraries
macro (add_biips_library name sources biips_libs)
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            add_library (${name}-${arch} ${${sources}})
            list(FIND BUILD_ARCH_LIST ${arch} offset)
            list(GET BUILD_BITNB_LIST ${offset} bitnb)
            set_target_properties(${name}-${arch}
                PROPERTIES COMPILE_FLAGS -m${bitnb}
                OUTPUT_NAME ${name}
                ARCHIVE_OUTPUT_DIRECTORY ${Biips_INSTALL_LIBDIR_${arch}}
                )
            set (extra_libs "")
        endforeach(arch IN LISTS BUILD_ARCH_LIST)
    else ()
        add_library (${name} ${${sources}})
    endif()
endmacro (add_biips_library)

# macro to install libraries
macro (install_biips_library name)
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            list(FIND BUILD_ARCH_LIST ${arch} offset)
            list(GET BUILD_BITNB_LIST ${offset} bitnb)
            install (TARGETS ${name}-${arch}
                DESTINATION ${Biips_INSTALL_LIBDIR_${arch}}
            	COMPONENT "Runtime_${bitnb}bit"
            	)
        endforeach (arch IN LISTS BUILD_ARCH_LIST)
    else ()
        install (TARGETS ${name}
            DESTINATION ${Biips_INSTALL_LIBDIR}
            )
    endif ()
endmacro (install_biips_library)

# macro to install script
macro (install_biips_script dir name)
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            list(FIND BUILD_ARCH_LIST ${arch} offset)
            list(GET BUILD_BITNB_LIST ${offset} bitnb)
            install (PROGRAMS ${dir}/${name}
                DESTINATION ${Biips_INSTALL_BINDIR_${arch}}
                RENAME ${name}.bat
            	COMPONENT "Runtime_${bitnb}bit"
            	)
        endforeach (arch IN LISTS BUILD_ARCH_LIST)
    else ()
        install (PROGRAMS ${dir}/${name}
            DESTINATION ${Biips_INSTALL_BINDIR}
            )
    endif ()
endmacro (install_biips_script)

# macro to add executable
macro (add_biips_executable name sources extra_libs extra_libs64 biips_libs)
    if (WIN32)
        foreach (arch IN LISTS BUILD_ARCH_LIST)
            add_executable (${name}-${arch} ${${sources}})
            list(FIND BUILD_ARCH_LIST ${arch} offset)
            list(GET BUILD_BITNB_LIST ${offset} bitnb)
            set_target_properties(${name}-${arch}
                PROPERTIES COMPILE_FLAGS -m${bitnb}
                LINK_FLAGS -m${bitnb}
                OUTPUT_NAME ${name}
                RUNTIME_OUTPUT_DIRECTORY ${Biips_INSTALL_BINDIR_${arch}}
                )
                
            if (bitnb EQUAL 64)
                foreach (lib IN LISTS ${biips_libs})
                    list(APPEND ${extra_libs64} ${lib}-${arch})
                endforeach (lib IN LISTS ${biips_libs})
                target_link_libraries (${name}-${arch} ${${extra_libs64}})
            else ()
                foreach (lib IN LISTS ${biips_libs})
                    list(APPEND ${extra_libs} ${lib}-${arch})
                endforeach (lib IN LISTS ${biips_libs})
                target_link_libraries (${name}-${arch} ${${extra_libs}})
            endif ()
        endforeach(arch IN LISTS BUILD_ARCH_LIST)
    else ()
        add_executable (${name} ${${sources}})
        target_link_libraries (${name} ${${extra_libs}} ${${biips_libs}})
    endif()
endmacro (add_biips_executable)


# macro to copy files to binary dir
# changes in source file are taken into account
macro(copy_files_to_bindir files dir)
    foreach(f ${${files}})
        get_filename_component(n ${f} NAME)
        configure_file ( ${f}
        	${dir}/${n}
        	COPYONLY
        )
    endforeach()
endmacro(copy_files_to_bindir)


