#
# Component Makefile
#
# This Makefile should, at the very least, just include $(SDK_PATH)/Makefile. By default,
# this will take the sources in the src/ directory, compile them and link them into
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the SDK documents if you need to do this.
#
COMPONENT_EMBED_FILES := style.css code.js index.html bootstrap.min.css.gz jquery.min.js.gz popper.min.js.gz bootstrap.min.js.gz favicon.ico
COMPONENT_ADD_INCLUDEDIRS := . 
COMPONENT_EXTRA_INCLUDES += $(IDF_PATH)/components/esp_http_server/src $(IDF_PATH)/components/esp_http_server/src/port/esp32  $(IDF_PATH)/components/esp_http_server/src/util $(IDF_PATH)/components/esp_http_server/src/
CFLAGS += -D LOG_LOCAL_LEVEL=ESP_LOG_INFO


