idf_component_register(SRC_DIRS .
						INCLUDE_DIRS .  ${IDF_PATH}/components/driver
						REQUIRES json tools platform_config display
						PRIV_REQUIRES soc esp32
)


