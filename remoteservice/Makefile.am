# Makefile.am - libremoteservice.a

common_sources:= \
    RemoteServiceApp.cpp \
    RemoteService_RCC.cpp \
    RemoteService_RES.cpp \
    RemoteService_RISM.cpp \
    RemoteService_RPC.cpp \
    RemoteService_RVI.cpp \
    RemoteService_log.cpp \
    RemoteServiceApp_NGTP.cpp \
    RemoteServiceApp_Utils.cpp \
    RemoteServiceApp_Timer.cpp \
    RemoteServiceApp_Variable.cpp \
    RemoteServiceApp_canSignals.cpp \
    RemoteServiceApp_receiveMessage.cpp

# Target name DSO
lib_LTLIBRARIES := libremoteservice.la

libremoteservice_la_CPPFLAGS := \
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
libremoteservice_la_CPPFLAGS += \
        -fstack-protector-all \
        -fPIC \
        -D_FORTIFY_SOURCE=2
endif

#libhello_la_LDFLAGS = -lpthread -shared -module -export-dynamic -export-symbols ../../service_layer/frameworks/core/application/export.syms
libremoteservice_la_LDFLAGS = -lpthread -shared -module -export-dynamic -export-symbols-regex 'createApplication'
libremoteservice_la_LDFLAGS += -lapplication
libremoteservice_la_LDFLAGS += -rdynamic
if ENABLE_SECURITY
libremoteservice_la_LDFLAGS += -Wl,-z,relro,-z,now
endif

requiredlibs := -lpthread

libremoteservice_la_SOURCES := \
        $(common_sources)
