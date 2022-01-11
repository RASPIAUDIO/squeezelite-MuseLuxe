idf_component_register(	SRC_DIRS .
						INCLUDE_DIRS .  inc 
						)
add_prebuilt_library(esp_processing lib/libesp_processing.a)
target_link_libraries(${COMPONENT_LIB} PRIVATE esp_processing)
#target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--undefined=")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u pow")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u cos")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u sin")
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u sqrt")