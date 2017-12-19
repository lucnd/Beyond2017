# Makefile.am - libev.a

common_sources:= \
    EvtUtil/minijson_reader.hpp \
    EvtUtil/EvState.cpp \
    EvApplications/EvChargeApp/EvtChargeSettings.cpp \
    EvApplications/EvClimateApp/EvtEVPreconditionStatus.cpp \
    EvApplications/EvChargeApp/EvtElectricVehicleInformation.cpp \
    EvApplications/EvChargeApp/EvtNextDepartureTimer.cpp \
    EvApplications/EvHBDApp/EvtHvBatteryPack.cpp \
    EvApplications/EvLongHorizonApp/EHApp.cpp \
    EvApplications/EvLongHorizonApp/EHAppHandler.cpp \
    EvApplications/EvLongHorizonApp/EHAppTimer.cpp \
    EvApplications/EvLongHorizonApp/EHAppServer.cpp \
    EvApplications/EvClimateApp/EvClimateApp.cpp \
    EvApplications/EvClimateApp/EvClimateAppHandler.cpp \
    EvApplications/EvClimateApp/EvClimateAppTimer.cpp \
    EvApplications/EvChargeApp/EvChargeApp.cpp \
    EvApplications/EvChargeApp/EvChargeAppTimer.cpp \
    EvApplications/EvChargeApp/EvChargeAppHandler.cpp \
    EvApplications/EvRangeApp/EvRangeApp.cpp \
    EvApplications/EvRangeApp/EvRangeAppHandler.cpp \
    EvApplications/EvHBDApp/EvHBDApp.cpp \
    EvApplications/EvHBDApp/EvHBDAppHandler.cpp \
    EvApplications/EvHBDApp/EvHBDAppTimer.cpp \
    EvtServiceManager/LCFService/EvConfigData.cpp \
    EvtServiceManager/CanService/EvtVifManager.cpp \
    EvtServiceManager/CanService/EvVifHandler.cpp \
    EvtServiceManager/NgtpService/EvtNgtpMsgManager.cpp \
    EvtServiceManager/VehicleStatus/EvVehicleStatus.cpp \
    EvAppBinder/IEvAppBinderService.cpp \
    EvAppBinder/EvAppBinderService.cpp \
    EvAppHandler.cpp \
    EvApp.cpp

# Target name DSO
lib_LTLIBRARIES := libev.la

libev_la_CPPFLAGS := \
        -I$(WORKSPACE)/../../service_layer \
        -I$(WORKSPACE)/../../service_layer/services \
        -I$(WORKSPACE)/../../service_layer/services/PowerManagerService/JLR_State \
        -I$(WORKSPACE)/../../service_layer/core \
        -I$(WORKSPACE)/../../service_layer/include \
        -I$(WORKSPACE)/../../service_layer/utils \
        -I$(WORKSPACE)/../../external/android/system/core/include \
        -include $(WORKSPACE)/../../external/android/system/core/include/arch/linux-arm/OEConfig.h \
        -I$(WORKSPACE)/../../apps/ev/boost/include \
        -I$(WORKSPACE)/../../apps/ev/EvtUtil \
        -I$(WORKSPACE)/../../apps/ev/EvtServiceManager \
        -I$(WORKSPACE)/../../apps/ev/EvtServiceManager/LCFService \
        -I$(WORKSPACE)/../../apps/ev/EvtServiceManager/CanService \
        -I$(WORKSPACE)/../../apps/ev/EvtServiceManager/NgtpService \
        -I$(WORKSPACE)/../../apps/ev/EvtServiceManager/VehicleStatus \
        -I$(WORKSPACE)/../../apps/ev/EvApplications/EvChargeApp \
        -I$(WORKSPACE)/../../apps/ev/EvApplications/EvClimateApp \
        -I$(WORKSPACE)/../../apps/ev/EvApplications/EvHBDApp \
        -I$(WORKSPACE)/../../apps/ev/EvApplications/EvLongHorizonApp \
        -I$(WORKSPACE)/../../apps/ev/EvApplications/EvRangeApp \
        -I$(WORKSPACE)/../../apps/ev/EvAppBinder \
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
        -DOSSDEBUG=2 \
        -std=c++0x \
        -Wno-unused-variable
if ENABLE_SECURITY
libev_la_CPPFLAGS += \
        -fstack-protector-all \
        -fPIC \
        -D_FORTIFY_SOURCE=2
endif

libev_la_LDFLAGS = -lpthread -shared -module -export-dynamic -export-symbols-regex 'createApplication'
libev_la_LDFLAGS += -lapplication -lcpptoed -losscpp
#libev_la_LDFLAGS += -L$(WORKSPACE)/../../apps/ev/boost/lib -lboost_thread -lboost_system -lboost_atomic
libev_la_LDFLAGS += -rdynamic
if ENABLE_SECURITY
libev_la_LDFLAGS += -Wl,-z,relro,-z,now
endif

requiredlibs := -lpthread

libev_la_SOURCES := \
        $(common_sources)
