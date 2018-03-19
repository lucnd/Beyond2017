TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH += \
    "B:\MY20_MODEM_AP\apps_proc\system\core\include" \
    "B:\MY20_MODEM_AP\apps_proc\lge" \
    "B:\MY20_MODEM_AP\apps_proc\lge\services" \
    "B:\MY20_MODEM_AP\apps_proc\lge\services\include" \
    "B:\MY20_MODEM_AP\apps_proc\lge\services\include\services" \
    "B:\MY20_MODEM_AP\apps_proc\lge\servicelayer\core\service_layer" \
    "B:\MY20_MODEM_AP\apps_proc\lge\servicelayer\core\service_layer\include" \
    "B:\MY20_MODEM_AP\apps_proc\lge\servicelayer\core\service_layer\include\corebase" \
    "B:\MY20_MODEM_AP\apps_proc\lge\servicelayer\core\service_layer\include\utils" \
    "B:\MY20_MODEM_AP\apps_proc\system\core\include" \
    "B:\MY20_MODEM_AP\apps_proc\lge\servicelayer\variant\service_layer" \
    "B:\MY20_MODEM_AP\apps_proc\lge\servicelayer\variant\service_layer\include" \
    "B:\MY20_MODEM_AP\apps_proc\frameworks\native\include"

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
    ../SpecialModeHandler.h \
    ../SpecialModeReceiverManager.h \
    ../SpecialModeType.h \
    ../SpecialModeServicesManager.h
