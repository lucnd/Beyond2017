bin_PROGRAMS = specialmode-test
noinst_LIBRARIES = libspecialmode.a

libspecialmode_a_SOURCES = \
    $(WORKSPACE)/lge/apps/SpecialMode/SpecialModeApplication.cpp \
    $(WORKSPACE)/lge/apps/SpecialMode/SpecialModeServicesManager.cpp \
    $(WORKSPACE)/lge/apps/SpecialMode/SpecialModeHandler.cpp \
    $(WORKSPACE)/lge/apps/SpecialMode/SpecialModeReceiverManager.cpp \
    $(WORKSPACE)/lge/apps/SpecialMode/SpecialModeBaseProcess.cpp \
    $(WORKSPACE)/lge/apps/SpecialMode/DemoModeTimer.cpp \
    $(WORKSPACE)/lge/apps/SpecialMode/DemoModeProcess.cpp

libspecialmode_a_CPPFLAGS = \
    -DG_TEST \
    -I$(SDKTARGETSYSROOT)/usr/include/servicelayer \
    -I$(WORKSPACE)/lge/services/include/services/TelephonyManagerService \
    -I$(WORKSPACE)/lge/services/include \
    -I$(WORKSPACE)/lge/services/NGTPManagerService \
    -I$(WORKSPACE)/lge/services/NGTPManagerService/include \
    -I$(WORKSPACE)/lge/services/NGTPManagerService/asn2cpp-svc \
    -I$(stincdir)/libxml2 \
    -I$(stincdir)/services/TelephonyManagerService \
    -D_STDATOMIC_HAVE_ATOMIC \
    -D_MORE_MARSHM \
    -DHAVE_SYS_UIO_H \
    -std=c++11

libspecialmode_a_CXXFLAGS = \
    -Wall \
    -std=c++11 \
    -Wno-unused-parameter \
    -fpermissive \
    --coverage

specialmode_test_SOURCES = \
    DemoModeProcess_test.cpp \
    SpecialModeApplication_test.cpp \
    SpecialModeReceiverManager_test.cpp \ 
    SpecialModeServicesManager_test.cpp \
    DemoModeBaseProcess_test.cpp \
    DemoModeTimer_test.cpp
    
specialmode_test_CPPFLAGS = \
    $(libspecialmode_a_CPPFLAGS) \
    -I$(WORKSPACE)/lge/apps/SpecialMode

specialmode_test_CXXFLAGS =
    -Wall \
    -Wextra \
    -Wno-unused-parameter \
    -pedantic \
    -std=c++11 \
    -O2

specialmode_test_LDADD = libspecialmode.a

specialmode_test_LDFLAGS = \
    -lpugixml \
    -lapplication \
    -liapplication \
    -lrt \
    -lbinder \
    -lcutils \
    -llog \
    -lutils \
    -lcorebase \
    -lcontext \
    -lserviceutils \
    -lialarm \
    -liaudio \
    -liconfiguration \
    -lipower \
    -lisystem \
    -litelephony \
    -livif \
    -lidebugmanager \
    -licommunication \
    -lproperty \
    -lcpptoed \
    -losscpp \
    -lingtp \
    -lgtest_main \
    -lgtest \
    -lpthread \
    --coverage

requiredlibs = -lpthread
