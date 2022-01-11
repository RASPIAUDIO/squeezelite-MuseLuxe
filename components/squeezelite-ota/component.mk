#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# todo: add support for https
COMPONENT_ADD_INCLUDEDIRS := .
CFLAGS += -D LOG_LOCAL_LEVEL=ESP_LOG_INFO -DCONFIG_OTA_ALLOW_HTTP=1


