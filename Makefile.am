# Makefile.am - libspecialmode.a

common_sources:= \
	SpecialModeApplication.cpp \
	SpecialModeHandler.cpp \
	SpecialModeServicesManager.cpp \
	SpecialModeReceiverManager.cpp \
	SpecialModeBaseProcess \
	DemoModeTimer.cpp \
	DemoModeProcess.cpp \
	InControlLightProcess

# Target name DSO
lib_LTLIBRARIES := libspecialmode.la

libspecialmode_la_CPPFLAGS := \
	-I$(SDKTARGETSYSROOT)/usr/include/servicelayer \
	-I$(WORKSPACE)/lge/services/include/services/TelephonyManagerService \
	-I$(WORKSPACE)/lge/services/include \
	-I$(WORKSPACE)/lge/services/NGTPManagerService \
	-I$(WORKSPACE)/lge/services/NGTPManagerService/include \
	-I$(WORKSPACE)/lge/services/NGTPManagerService/asn2cpp-svc \
	-I$(stincdir)/libxml2 \
	-I$(stincdir)/services/TelephonyManagerService \
	-D_STDATOMIC_HAVE_ATOMIC \
	-D_MORE_MARSHM
	-O2 \
	-g \
	-Wall \
	-std=c++11 \
	-Wno-unused-parameter \
	-DHAVE_SYS_UIO_H \
	-rdynamic \
	-fpermissive
	


libspecialmode_la_LDFLAGS = \
	-lpthread \
	-lapplication \
	-liapplication \
	-lisystem \
	-shared \
	-module \
	-export-dynamic \
	-export-symbols-regex 'createApplication'

requiredlibs := -lpthread

libspecialmode_la_SOURCES := \
	$(common_sources)
