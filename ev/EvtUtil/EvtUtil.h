#ifndef _EVT_UTIL_H_
#define _EVT_UTIL_H_

#define ENABLED 1
#define DISABLED 0

#define CONFIG_FILE "jlr_tcu_data_dictionary_swr_9_3.xml"
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#define LOG_LEVEL 1
#define LOG_EV(...) do{if(LOG_LEVEL > 0) { LOGI(__VA_ARGS__);}} while(0)
#define LOG_LEVEL_CN 1
#define LOG_CEV(...) do{if(LOG_LEVEL > 0) { LOGI(__VA_ARGS__);}} while(0)

#define SET_SERVICE(A) interface_cast<I##A>(defaultServiceManager()->getService(String16(A::getServiceName())))

#endif //_EVT_UTIL_H_