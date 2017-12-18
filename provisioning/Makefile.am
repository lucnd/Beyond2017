# Makefile.am - libprovisioning.a

common_sources:= \
    ProvisioningApp.cpp \
	ServiceDataBase.cpp \
	ConfigRequestMsg.cpp \
	ConfigDataAckMsg.cpp \
	StatusUpdateAckMsg.cpp \
	FTUpdateMsg.cpp \
	NGTPDecodeMsg.cpp

# Target name DSO
lib_LTLIBRARIES := libprovisioning.la

libprovisioning_la_CPPFLAGS := \
        -include $(WORKSPACE)/../../external/android/system/core/include/arch/linux-arm/OEConfig.h \
        -I$(WORKSPACE)/../../service_layer \
        -I$(WORKSPACE)/../../service_layer/services \
        -I$(WORKSPACE)/../../service_layer/core \
        -I$(WORKSPACE)/../../service_layer/include \
        -I$(WORKSPACE)/../../service_layer/utils \
        -I$(WORKSPACE)/../../service_layer/utils/external \
        -I$(WORKSPACE)/../../external/android/system/core/include \
        -I$(sysdir)/usr/include/libxml2 \
        -O2 \
        -g \
        -Wall \
        -Wno-unused-parameter \
        -DHAVE_SYS_UIO_H \
        -DOSSPRINT \
        -DOSSDEBUG=2
if ENABLE_SECURITY
libprovisioning_la_CPPFLAGS += \
        -fstack-protector-all \
        -fPIC \
        -D_FORTIFY_SOURCE=2
endif

libprovisioning_la_LDFLAGS = -lpthread -shared -module -export-dynamic -export-symbols-regex 'createApplication'
libprovisioning_la_LDFLAGS += -lapplication
libprovisioning_la_LDFLAGS += -ltelephonymanager
libprovisioning_la_LDFLAGS += -rdynamic
if ENABLE_SECURITY
libprovisioning_la_LDFLAGS += -Wl,-z,relro,-z,now
endif

requiredlibs := -lpthread

libprovisioning_la_SOURCES := \
        $(common_sources)
