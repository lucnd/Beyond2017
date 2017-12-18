# Makefile.am - libspecialmode.a

common_sources:= \
        process/demomode/DemoModeProcess.cpp \
        process/demomode/DemoModeTimer.cpp \
        process/incontrollight/InControlLightProcess.cpp \
        SpecialModeApplication.cpp \
        SpecialModeHandler.cpp \
        SpecialModeReceiverManager.cpp \
        SpecialModeBaseProcess.cpp \
        SpecialModeServicesManager.cpp

# Target name DSO
lib_LTLIBRARIES := libspecialmode.la

libspecialmode_la_CPPFLAGS := \
        -include $(WORKSPACE)/../../external/android/system/core/include/arch/linux-arm/OEConfig.h \
        -I$(WORKSPACE)/../../service_layer \
        -I$(WORKSPACE)/../../service_layer/services \
        -I$(WORKSPACE)/../../service_layer/utils \
        -I$(WORKSPACE)/../../service_layer/core \
        -I$(WORKSPACE)/../../service_layer/include \
        -I$(WORKSPACE)/../../external/android/system/core/include \
        -I$(WORKSPACE)/../../service_layer/utils/external \
        -I$(sysdir)/usr/include/libxml2 \
        -O2 \
        -g \
        -Wall \
        -Wno-unused-parameter \
        -DHAVE_SYS_UIO_H \
        -D_DEVICE_NAD \
        -fpermissive \
        -DOSSPRINT \
        -DOSSDEBUG=2
if ENABLE_SECURITY
libspecialmode_la_CPPFLAGS += \
        -fstack-protector-all \
        -fPIC \
        -D_FORTIFY_SOURCE=2
endif

#libhello_la_LDFLAGS = -lpthread -shared -module -export-dynamic -export-symbols ../../service_layer/frameworks/core/application/export.syms
libspecialmode_la_LDFLAGS = -lpthread -shared -module -export-dynamic -export-symbols-regex 'createApplication'
libspecialmode_la_LDFLAGS += -lapplication -lcpptoed -losscpp
libspecialmode_la_LDFLAGS += -ltelephonymanager
libspecialmode_la_LDFLAGS += -rdynamic
if ENABLE_SECURITY
libspecialmode_la_LDFLAGS += -Wl,-z,relro,-z,now
endif

#libecallapplication_la_LIBADD = \
#        $(top_srcdir)/../../service_layer/core/application/libapplication.a \
#        $(top_srcdir)/../../service_layer/ngtp/dspt/libngtpdspt.a \
#        $(top_srcdir)/../../service_layer/ngtp/svc/libngtpsvc.a

requiredlibs := -lpthread

libspecialmode_la_SOURCES := \
        $(common_sources)
