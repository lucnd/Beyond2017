TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../DemoModeProcess.cpp \
    ../DemoModeTimer.cpp \
    ../InControlLightProcess.cpp \
    ../SpecialModeApplication.cpp \
    ../SpecialModeBaseProcess.cpp \
    ../SpecialModeHandler.cpp \
    ../SpecialModeReceiverManager.cpp \
    ../SpecialModeServicesManager.cpp

DISTFILES += \
    ../SpecialMode.json \
    ../autogen.sh \
    ../README.md \
    ../property/specialmode.prop \
    ../configure.ac

HEADERS += \
    ../DemoModeProcess.h \
    ../DemoModeTimer.h \
    ../InControlLightProcess.h \
    ../ISpecialModeApplication.h \
    ../SpecialModeApplication.h \
    ../SpecialModeBaseProcess.h \
    ../SpecialModeDataType.h \
    ../SpecialModeEvent.h \
    ../SpecialModeHandler.h \
    ../SpecialModeReceiverManager.h \
    ../SpecialModeServicesManager.h \
    ../SpecialModeType.h
