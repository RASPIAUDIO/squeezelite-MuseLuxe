# build system (Jenkins) should pass the following
#export PROJECT_BUILD_NAME="${build_version_prefix}${BUILD_NUMBER}"
#export PROJECT_BUILD_TARGET="${config_target}"

#PROJECT_BUILD_NAME?=local
#PROJECT_CONFIG_TARGET?=custom
#PROJECT_NAME?=squeezelite.$(PROJECT_CONFIG_TARGET)
#PROJECT_VER?="$(PROJECT_BUILD_NAME)-$(PROJECT_CONFIG_TARGET)"



#recovery: PROJECT_NAME:=recovery.$(PROJECT_CONFIG_TARGET)
#recovery: EXTRA_CPPFLAGS+=-DRECOVERY_APPLICATION=1

PROJECT_NAME?=squeezelite
EXTRA_CPPFLAGS+=  -I$(PROJECT_PATH)/main 

#/-Wno-error=maybe-uninitialized 
include $(IDF_PATH)/make/project.mk 

# for future gcc version, this could be needed: CPPFLAGS+= -Wno-error=format-overflow -Wno-error=stringop-truncation

