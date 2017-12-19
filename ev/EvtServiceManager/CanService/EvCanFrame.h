#ifndef _EV_CAN_FRAME_H_
#define _EV_CAN_FRAME_H_

#include <utils/RefBase.h>
#include <vector>
#include <cstring>
#include "vifManagerService/vifcansig.h"
// #include "Log.h"
typedef struct
{
    uint32_t mCanId;
    uint8_t mCanData[CAN_VALUES_MAX_BUF_SIZE];
} CanInfo;

class EvCanFrame : public android::RefBase
{
  public:
    EvCanFrame(){};
    EvCanFrame(EvCanFrame* obj)
    {
        this->mFrameID = obj->mFrameID;
        this->mCanList = obj->mCanList;
    };
    EvCanFrame(sp<vifCANFrameSigData> &_fData)
    {
        mFrameID = _fData->mFrameId;
        for (int i = 0; i < _fData->mNumOfSigInFrame; i++)
        {
            CanInfo tmp;
            tmp.mCanId = _fData->mFrameSigData[i].sigid;
            uint32_t tSize = _fData->mFrameSigData[i].validvaluesize;
            tSize = (tSize >= CAN_VALUES_MAX_BUF_SIZE) ? CAN_VALUES_MAX_BUF_SIZE : tSize;
            memcpy(tmp.mCanData, _fData->mFrameSigData[i].values, tSize);
            // LOGE("<<DEBUG>> id:%d data:%d", tmp.mCanId, tmp.mCanData[0]);
            mCanList.push_back(tmp);
        }
        // for(auto tCanInfo : mCanList)
        // {
        //     LOGE("<<??>>Signal:[%d] Data:[%d]",tCanInfo.mCanId, tCanInfo.mCanData[0]);
        // }
    };
    ~EvCanFrame(){};

    int32_t search(uint32_t tCanId){
        for(auto tCanInfo : mCanList)
        {
            if(tCanInfo.mCanId == tCanId)
                return tCanInfo.mCanData[0];
        }
        LOGE("<%s> error for searching", __func__);
        return -1;
    };

    uint32_t mFrameID;
    std::vector<CanInfo> mCanList;
};

#endif // _EV_CAN_FRAME_H_
