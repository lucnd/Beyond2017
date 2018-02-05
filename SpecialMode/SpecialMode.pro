TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += \
    "E:\apps_proc\system\core\include" \
    "E:\apps_proc\lge" \
    "E:\apps_proc\lge\services" \
    "E:\apps_proc\lge\services\include" \
    "E:\apps_proc\lge\services\include\services" \
    "E:\apps_proc\lge\servicelayer\core\service_layer" \
    "E:\apps_proc\lge\servicelayer\core\service_layer\include" \
    "E:\apps_proc\lge\servicelayer\core\service_layer\include\corebase" \
    "E:\apps_proc\lge\servicelayer\core\service_layer\include\utils" \
    "E:\apps_proc\system\core\include" \
    "E:\apps_proc\lge\servicelayer\variant\service_layer" \
    "E:\apps_proc\lge\servicelayer\variant\service_layer\include" \
    "E:\apps_proc\frameworks\native\include"

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
    ../specialmode.json \
    ../autogen.sh \
    ../property/specialmode.prop \
    ../configure.ac \
    ../Makefile.am

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
    ../SpecialModeType.h \
    ../SpecialModeServicesManager.h
