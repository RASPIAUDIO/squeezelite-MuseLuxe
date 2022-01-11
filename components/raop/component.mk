#
# Component Makefile
#
# This Makefile should, at the very least, just include $(SDK_PATH)/Makefile. By default,
# this will take the sources in the src/ directory, compile them and link them into
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the SDK documents if you need to do this.
#

CFLAGS += -fstack-usage\
	-I$(PROJECT_PATH)/components/tools	\
	-I$(PROJECT_PATH)/components/codecs/inc/alac \
	-I$(PROJECT_PATH)/main/	
COMPONENT_ADD_INCLUDEDIRS := .
COMPONENT_SRCDIRS := . 