# Pthread should be enabled everywhere when Gtest is used
AM_CPPFLAGS = \
    -isystem $(srcdir)/gmockfused \
    -std=c++11 \
    -pthread


bin_PROGRAMS = specialmode-test
noinst_LIBRARIES = libgmock-gtest-all.a


app_sources = \
    ../SpecialModeApplication.cpp \
    ../SpecialModeServicesManager.cpp \
    ../SpecialModeHandler.cpp \
    ../SpecialModeConfigurationReceiver.cpp \
    ../SpecialModeReceiverManager.cpp \
    ../SpecialModeBaseProcess.cpp \
    ../DemoModeTimer.cpp \
    ../DemoModeProcess.cpp

test_sources = \
    ConfigurationReceiver_test.cpp \
    ReceiverManager_test.cpp \
    ServicesManager_test.cpp \
    DemoModeTimer_test.cpp \
    SpecialModeBaseProcess_test.cpp \
    DemoModeProcess_test.cpp

specialmode_test_SOURCES = $(app_sources) $(test_sources)

specialmode_test_CPPFLAGS = \
    $(AM_CPPFLAGS) \
    -DG_TEST \
    -D_STDATOMIC_HAVE_ATOMIC \
    -D_MORE_MARSHM \
    -DHAVE_SYS_UIO_H \
    -isystem $(WORKSPACE)/lge/services/include/services/TelephonyManagerService \
    -isystem $(WORKSPACE)/lge/services/include \
    -isystem $(WORKSPACE)/lge/services/NGTPManagerService \
    -isystem $(WORKSPACE)/lge/services/NGTPManagerService/include \
    -isystem $(WORKSPACE)/lge/services/NGTPManagerService/asn2cpp-svc \
    -isystem $(stincdir)/libxml2 \
    -isystem $(stincdir)/services/TelephonyManagerService

specialmode_test_CXXFLAGS = \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Werror \
    -Wmissing-include-dirs \
    -Wformat=2 \
    -Wold-style-cast \
    -Wuseless-cast \
    -Wcast-align \
    -Wctor-dtor-privacy \
    -Woverloaded-virtual \
    -Winit-self \
    -Wunused-parameter \
    -Wshadow \
    -Wlogical-op \
    -Wdouble-promotion \
    -Wfloat-equal \
    -Wredundant-decls \
    -Wundef \
    -Wc++14-compat \
    -fno-elide-constructors \
    --coverage

specialmode_test_LDADD = \
    libgmock-gtest-all.a \
    -lpthread \
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
    -lingtp

specialmode_test_LDFLAGS = \
    --coverage


libgmock_gtest_all_a_SOURCES = \
    gmockfused/custom_main.cpp \
    gmockfused/gmock-gtest-all.cc \
    gmockfused/gtest/gtest.h \
    gmockfused/gmock/gmock.h

libgmock_gtest_all_a_CXXFLAGS = -O2

requiredlibs = -lpthread -lgcov

