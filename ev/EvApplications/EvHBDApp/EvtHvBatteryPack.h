#ifndef _EV_HV_BATTERY_PACK_
#define  _EV_HV_BATTERY_PACK_

#include <vector>
#include "EvtSingleton.h"
#include "NGTPManagerService/asn2cpp-svc/tsdp-svc.h"
using namespace std;

class EvtHvBatteryPack : public EvtSingleton<EvtHvBatteryPack>
{
    public:
        EvtHvBatteryPack();
        ~EvtHvBatteryPack(){};

        void sampleDataFromCAN();

        uint32_t convertNgtpFromCAN_U(int32_t canValue
                                    , int16_t scaleFactor
                                    , int16_t offset
                                    , int32_t minValue
                                    , int32_t maxValue
                                    , int32_t defaultValue);
        int32_t convertNgtpFromCAN(int32_t canValue
                                , int16_t scaleFactor
                                , int16_t offset
                                , int32_t minValue
                                , int32_t maxValue
                                , int32_t defaultValue);



        void initHvDataRecordVec();
        svc::BatterPackDataHeader& getBatteryPackDataHeader();
        vector<svc::HighVoltageDataRecord>& getHvDataRecordVec();
    private:
        svc::BatterPackDataHeader _batteryPackDataHeader;
        vector<svc::HighVoltageDataRecord> _hvDataRecordVec;

	    void setBatteryCellMeasurements(svc::HighVoltageDataRecord& _hvDataRecord);
	    void setbatteryFaultMonitoring(svc::HighVoltageDataRecord& _hvDataRecord);
	    void setBatteryHealthMonitoring(svc::HighVoltageDataRecord& _hvDataRecord);
	    void setThermalManagement(svc::HighVoltageDataRecord& _hvDataRecord);
	    void setCellBalancing(svc::HighVoltageDataRecord& _hvDataRecord);
	    void setBmsElectricalModuleMonitoring(svc::HighVoltageDataRecord& _hvDataRecord);
	    void setParkingData(svc::HighVoltageDataRecord& _hvDataRecord);
	    void setOther(svc::HighVoltageDataRecord& _hvDataRecord);
};
#endif // _EV_HV_BATTERY_PACK_