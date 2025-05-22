# Setting this outside of the macro so that the path to the code generators remain independent
# of where the macro is invoked

find_program(PYTHON3_BIN python3 REQUIRED)

if(NOT TARGET clean-generated-files)
add_custom_target(clean-generated-files
    COMMENT "Cleaning up generated calibration files"
)
endif()

macro(generate_parameters NAME GENERATOR CONFIG_FILE OUTPUT_SOURCE OUTPUT_HEADER)
    add_custom_command(
        OUTPUT ${OUTPUT_HEADER} ${OUTPUT_SOURCE}
        COMMAND ${PYTHON3_BIN} ${GENERATOR} --config-file ${CONFIG_FILE} --output-source ${OUTPUT_SOURCE} --output-header ${OUTPUT_HEADER}
        DEPENDS
        ${CONFIG_FILE}
        ${GENERATOR}
    )
    add_custom_target(${NAME} DEPENDS ${OUTPUT_HEADER} ${OUTPUT_SOURCE})
    add_custom_target(clean-generated-files-${NAME}
        COMMAND rm -f ${OUTPUT_HEADER} ${OUTPUT_SOURCE}
        COMMENT "Cleaning up files for '${NAME}'"
    )
  add_dependencies(clean-generated-files clean-generated-files-${NAME})
endmacro(generate_parameters)
