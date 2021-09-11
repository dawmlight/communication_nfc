#include "nfc_nci_mock.h"

#include <thread>

tNFA_DM_CBACK* NfcNciMock::mNfcDeviceManagementCallback_;
tNFA_CONN_CBACK* NfcNciMock::mNfcConnectionCallback_;
tNFA_NDEF_CBACK* NfcNciMock::mNdefCallback_;
tNFA_HCI_CBACK* NfcNciMock::mHciCallback_;
tNFA_EE_CBACK* NfcNciMock::mEeCallback_;

tNFA_CONN_EVT_DATA NfcNciMock::connEventData_;
tNFA_DM_CBACK_DATA NfcNciMock::dmEventData_;
tNFA_NDEF_EVT_DATA NfcNciMock::ndefEventData_;
tNFA_HCI_EVT_DATA NfcNciMock::hciEventData_;
tNFA_EE_CBACK_DATA NfcNciMock::eeEventData_;

NfcNciMock::NfcNciMock() {}

NfcNciMock::~NfcNciMock() {}

void NfcNciMock::NfcDeviceManagementCallback(uint8_t event, tNFA_DM_CBACK_DATA* p_data)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mNfcDeviceManagementCallback_(event, p_data);
}

void NfcNciMock::NfcConnectionCallback(uint8_t event, tNFA_CONN_EVT_DATA* p_data)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mNfcConnectionCallback_(event, p_data);
}

void NfcNciMock::NdefCallback(tNFA_NDEF_EVT event, tNFA_NDEF_EVT_DATA* p_data)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mNdefCallback_(event, p_data);
}

void NfcNciMock::NfcHciCallback(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* p_data)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mHciCallback_(event, p_data);
}

void NfcNciMock::NfcEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* p_data)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    mEeCallback_(event, p_data);
}

void NfcNciMock::NfaInit(tHAL_NFC_ENTRY* pHalEntryTbl) {}

tNFA_STATUS NfcNciMock::NfaEnable(tNFA_DM_CBACK* pDmCback, tNFA_CONN_CBACK* pConnCback)
{
    mNfcDeviceManagementCallback_ = pDmCback;
    mNfcConnectionCallback_ = pConnCback;
    if (mEnableScene_ == 1) {
        mEnableScene_ = 0;
        return NFA_STATUS_FAILED;
    } else if (mEnableScene_ == 2) {
        mEnableScene_ = 0;
        dmEventData_.status = NFA_STATUS_FAILED;
        std::thread(&NfcDeviceManagementCallback, NFA_DM_ENABLE_EVT, &dmEventData_).detach();
        return NFA_STATUS_OK;
    }
    dmEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcDeviceManagementCallback, NFA_DM_ENABLE_EVT, &dmEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetEnableScene(int enableScene)
{
    mEnableScene_ = enableScene;
}

tNFA_STATUS NfcNciMock::NfaDisable(bool graceful)
{
    if (mDisableScene_ == 1) {
        mDisableScene_ = 0;
        return NFA_STATUS_FAILED;
    }
    std::thread(&NfcDeviceManagementCallback, NFA_DM_DISABLE_EVT, &dmEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetDisableScene(int disableScene)
{
    mDisableScene_ = disableScene;
}

tNFA_STATUS NfcNciMock::NfaEnablePolling(tNFA_TECHNOLOGY_MASK pollMask)
{
    std::thread(&NfcConnectionCallback, NFA_POLL_ENABLED_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfaDisablePolling(void)
{
    std::thread(&NfcConnectionCallback, NFA_POLL_DISABLED_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfaEnableListening(void)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfaDisableListening(void)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfaStartRfDiscovery(void)
{
    std::thread(&NfcConnectionCallback, NFA_RF_DISCOVERY_STARTED_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfaStopRfDiscovery(void)
{
    std::thread(&NfcConnectionCallback, NFA_RF_DISCOVERY_STOPPED_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfaSetRfDiscoveryDuration(uint16_t discoveryPeriodMs)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfaSelect(uint8_t rfDiscId, tNFA_NFC_PROTOCOL protocol, tNFA_INTF_TYPE rfInterface)
{
    if (mSelectScene_ == 1) {
        mSelectScene_ = 0;
        return NFA_STATUS_FAILED;
    } else if (mSelectScene_ == 2) {
        mSelectScene_ = 0;
        return NFA_STATUS_OK;
    }
    connEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcConnectionCallback, NFA_SELECT_RESULT_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetSelectScene(int selectScene)
{
    mSelectScene_ = selectScene;
}

tNFA_STATUS NfcNciMock::NfaDeactivate(bool sleepMode)
{
    if (mDeactivateScene_ == 1) {
        mDeactivateScene_ = 0;
        return NFA_STATUS_FAILED;
    }
    if (sleepMode) {
        connEventData_.deactivated.type = NFA_DEACTIVATE_TYPE_SLEEP;
    } else {
        connEventData_.deactivated.type = NFA_DEACTIVATE_TYPE_IDLE;
    }
    std::thread(&NfcConnectionCallback, NFA_DEACTIVATED_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetDeactivateScene(int deactivateScene)
{
    mDeactivateScene_ = deactivateScene;
}

tNFA_STATUS NfcNciMock::NfaSendRawFrame(uint8_t* pRawData, uint16_t dataLen, uint16_t presenceCheckStartDelay)
{
    if (mSendRawFrameScene_ == 1) {
        mSendRawFrameScene_ = 0;
        return NFA_STATUS_FAILED;
    } else if (mSendRawFrameScene_ == 2) {
        mSendRawFrameScene_ = 0;
        return NFA_STATUS_OK;
    }
    connEventData_.status = NFA_STATUS_OK;
    char response[] = {0x01, 0x02, 0x03, 0x04};
    connEventData_.data.p_data = (unsigned char*)response;
    connEventData_.data.len = 4;
    std::thread(&NfcConnectionCallback, NFA_DATA_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetSendRawFrameScene(int sendRawFrameScene)
{
    mSendRawFrameScene_ = sendRawFrameScene;
}

tNFA_STATUS NfcNciMock::NfaRegisterNDefTypeHandler(
    bool handleWholeMessage, tNFA_TNF tnf, uint8_t* pTypeName, uint8_t typeNameLen, tNFA_NDEF_CBACK* pNdefCback)
{
    mNdefCallback_ = pNdefCback;
    std::thread(&NdefCallback, NFA_NDEF_REGISTER_EVT, &ndefEventData_).detach();
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfaRwDetectNdef(void)
{
    if (mRwDetectNdefScene_ == 1) {
        mRwDetectNdefScene_ = 0;
        connEventData_.ndef_detect.status = NFA_STATUS_OK;
        connEventData_.ndef_detect.max_size = 128;
        connEventData_.ndef_detect.cur_size = 4;
        connEventData_.ndef_detect.flags = 0x99;
        std::thread(&NfcConnectionCallback, NFA_NDEF_DETECT_EVT, &connEventData_).detach();
        return NFA_STATUS_OK;
    } else if (mRwDetectNdefScene_ == 2) {
        mRwDetectNdefScene_ = 0;
        connEventData_.ndef_detect.status = NFA_STATUS_FAILED;
        connEventData_.ndef_detect.max_size = 128;
        connEventData_.ndef_detect.cur_size = 4;
        connEventData_.ndef_detect.flags = 0x66;
        std::thread(&NfcConnectionCallback, NFA_NDEF_DETECT_EVT, &connEventData_).detach();
        return NFA_STATUS_OK;
    } else if (mRwDetectNdefScene_ == 3) {
        mRwDetectNdefScene_ = 0;
        connEventData_.ndef_detect.status = NFA_STATUS_TIMEOUT;
        connEventData_.ndef_detect.max_size = 128;
        connEventData_.ndef_detect.cur_size = 4;
        connEventData_.ndef_detect.flags = 0x66;
        std::thread(&NfcConnectionCallback, NFA_NDEF_DETECT_EVT, &connEventData_).detach();
        return NFA_STATUS_OK;
    } else if (mRwDetectNdefScene_ == 4) {
        mRwDetectNdefScene_ = 0;
        return NFA_STATUS_FAILED;
    } else if (mRwDetectNdefScene_ == 5) {
        mRwDetectNdefScene_ = 0;
        return NFA_STATUS_OK;
    }
    connEventData_.ndef_detect.status = NFA_STATUS_OK;
    connEventData_.ndef_detect.max_size = 128;
    connEventData_.ndef_detect.cur_size = 4;
    connEventData_.ndef_detect.flags = 0x66;
    std::thread(&NfcConnectionCallback, NFA_NDEF_DETECT_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetRwDetectNdefScene(int rwDetectNdefScene)
{
    mRwDetectNdefScene_ = rwDetectNdefScene;
}

tNFA_STATUS NfcNciMock::NfaRwReadNdef(void)
{
    char response[] = {0x01, 0x02, 0x03, 0x04};
    ndefEventData_.ndef_data.p_data = (unsigned char*)response;
    ndefEventData_.ndef_data.len = 4;
    std::thread(&NdefCallback, NFA_NDEF_DATA_EVT, &ndefEventData_).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    if (mRwReadNdefScene_ == 1) {
        mRwReadNdefScene_ = 0;
        connEventData_.status = NFA_STATUS_FAILED;
        std::thread(&NfcConnectionCallback, NFA_READ_CPLT_EVT, &connEventData_).detach();
        return NFA_STATUS_OK;
    }
    connEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcConnectionCallback, NFA_READ_CPLT_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetRwReadNdefScene(int rwReadNdefScene)
{
    mRwReadNdefScene_ = rwReadNdefScene;
}

tNFA_STATUS NfcNciMock::NfaRwWriteNdef(uint8_t* pData, uint32_t len)
{
    if (mRwWriteNdefScene_ == 1) {
        mRwWriteNdefScene_ = 0;
        return NFA_STATUS_FAILED;
    } else if (mRwWriteNdefScene_ == 2) {
        mRwWriteNdefScene_ = 0;
        connEventData_.status = NFA_STATUS_FAILED;
        std::thread(&NfcConnectionCallback, NFA_WRITE_CPLT_EVT, &connEventData_).detach();
        return NFA_STATUS_OK;
    }
    connEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcConnectionCallback, NFA_WRITE_CPLT_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetRwWriteNdefScene(int rwWriteNdefScene)
{
    mRwWriteNdefScene_ = rwWriteNdefScene;
}

tNFA_STATUS NfcNciMock::NfaRwPresenceCheck(tNFA_RW_PRES_CHK_OPTION option)
{
    if (mRwPresenceCheckScene_ == 1) {
        mRwPresenceCheckScene_ = 0;
        connEventData_.status = NFA_STATUS_FAILED;
        std::thread(&NfcConnectionCallback, NFA_PRESENCE_CHECK_EVT, &connEventData_).detach();
        return NFA_STATUS_OK;
    } else if (mRwPresenceCheckScene_ == 2) {
        mRwPresenceCheckScene_ = 0;
        return NFA_STATUS_OK;
    }
    connEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcConnectionCallback, NFA_PRESENCE_CHECK_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetRwPresenceCheckScene(int rwPresenceCheckScene)
{
    mRwPresenceCheckScene_ = rwPresenceCheckScene;
}

tNFA_STATUS NfcNciMock::NfaRwFormatTag(void)
{
    if (mRwFormatTagScene_ == 1) {
        mRwFormatTagScene_ = 0;
        return NFA_STATUS_FAILED;
    } else if (mRwFormatTagScene_ == 2) {
        mRwFormatTagScene_ = 0;
        connEventData_.status = NFA_STATUS_FAILED;
        std::thread(&NfcConnectionCallback, NFA_FORMAT_CPLT_EVT, &connEventData_).detach();
        return NFA_STATUS_OK;
    }
    connEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcConnectionCallback, NFA_FORMAT_CPLT_EVT, &connEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetRwFormatTagScene(int rwFormatTagScene)
{
    mRwFormatTagScene_ = rwFormatTagScene;
}

tNFA_STATUS NfcNciMock::NfaRwSetTagReadOnly(bool bHardLock)
{
    if (mRwSetTagReadOnlyScene_ == 1) {
        mRwSetTagReadOnlyScene_ = 0;
        return NCI_STATUS_REJECTED;
    } else if (mRwSetTagReadOnlyScene_ == 2) {
        mRwSetTagReadOnlyScene_ = 0;
        return NFA_STATUS_FAILED;
    }
    return NFA_STATUS_OK;
}

void NfcNciMock::SetRwSetTagReadOnlyScene(int rwSetTagReadOnlyScene)
{
    mRwSetTagReadOnlyScene_ = rwSetTagReadOnlyScene;
}

void NfcNciMock::NdefMsgInit(uint8_t* pMsg, uint32_t maxSize, uint32_t* pCurSize) {}

tNDEF_STATUS NfcNciMock::NdefMsgAddRec(uint8_t* pMsg,
                                       uint32_t maxSize,
                                       uint32_t* pCurSize,
                                       uint8_t tnf,
                                       uint8_t* pType,
                                       uint8_t typeLen,
                                       uint8_t* pId,
                                       uint8_t idLen,
                                       uint8_t* pPayload,
                                       uint32_t payloadLen)
{
    return NDEF_OK;
}

uint8_t NfcNciMock::NfcGetNciVersion()
{
    if (mNciVersion_ == 1) {
        mNciVersion_ = 0;
        return NCI_VERSION_1_0;
    }
    return NCI_VERSION_2_0;
}

void NfcNciMock::SetNciVersion(int nciVersion)
{
    mNciVersion_ = nciVersion;
}

void NfcNciMock::NfcAdaptationInitialize() {}

void NfcNciMock::NfcAdaptationFinalize() {}

tHAL_NFC_ENTRY* NfcNciMock::NfcAdaptationGetHalEntryFuncs()
{
    return nullptr;
}

void NfcNciMock::NfcAdaptationDownloadFirmware() {}

void NfcNciMock::NfcAdaptationDump(int fd) {}

void NfcNciMock::NfcAdaptationFactoryReset() {}

void NfcNciMock::NfcAdaptationDeviceShutdown() {}

tNFA_STATUS NfcNciMock::NfcHciRegister(char* pAppName, tNFA_HCI_CBACK* pCback, bool bSendConnEvts)
{
    mHciCallback_ = pCback;
    hciEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcHciCallback, NFA_HCI_REGISTER_EVT, &hciEventData_).detach();
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfcEeGetInfo(uint8_t* pNumNfcee, tNFA_EE_INFO* pInfo)
{
    uint8_t num = 2;
    pNumNfcee = &num;
    tNFA_EE_INFO eeInfo[num];
    eeInfo[0].ee_handle = 0x82;
    eeInfo[0].ee_interface[0] = NCI_NFCEE_INTERFACE_HCI_ACCESS;
    eeInfo[0].ee_status = NFA_EE_STATUS_ACTIVE;
    eeInfo[1].ee_handle = 0x84;
    eeInfo[1].ee_interface[0] = NCI_NFCEE_INTERFACE_APDU;
    eeInfo[1].ee_status = NFA_EE_STATUS_ACTIVE;
    pInfo = eeInfo;
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfcEeRegister(tNFA_EE_CBACK* pCback)
{
    mEeCallback_ = pCback;
    eeEventData_.ee_register = NFA_STATUS_OK;
    std::thread(&NfcEeCallback, NFA_EE_REGISTER_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfcEeDeregister(tNFA_EE_CBACK* pCback)
{
    eeEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcEeCallback, NFA_EE_DEREGISTER_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfcEeSetDefaultTechRouting(tNFA_HANDLE eeHandle,
                                                   tNFA_TECHNOLOGY_MASK technologiesSwitchOn,
                                                   tNFA_TECHNOLOGY_MASK technologiesSwitchOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesBatteryOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenLock,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenOffLock)
{
    if (mSetTechRoutingScene_ == 1) {
        mSetTechRoutingScene_ = 0;
        return NFA_STATUS_FAILED;
    }
    eeEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcEeCallback, NFA_EE_SET_TECH_CFG_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetSetDefaultTechRoutingScene(int setDefaultTechRoutingScene)
{
    mSetTechRoutingScene_ = setDefaultTechRoutingScene;
}

tNFA_STATUS NfcNciMock::NfcEeClearDefaultTechRouting(tNFA_HANDLE eeHandle, tNFA_TECHNOLOGY_MASK clearTechnology)
{
    if (mClearTechRoutingScene_ == 1) {
        mClearTechRoutingScene_ = 0;
        return NFA_STATUS_FAILED;
    }
    eeEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcEeCallback, NFA_EE_CLEAR_TECH_CFG_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetClearDefaultTechRoutingScene(int clearDefaultTechRoutingScene)
{
    mClearTechRoutingScene_ = clearDefaultTechRoutingScene;
}

tNFA_STATUS NfcNciMock::NfcEeSetDefaultProtoRouting(tNFA_HANDLE eeHandle,
                                                    tNFA_PROTOCOL_MASK protocolsSwitchOn,
                                                    tNFA_PROTOCOL_MASK protocolsSwitchOff,
                                                    tNFA_PROTOCOL_MASK protocolsBatteryOff,
                                                    tNFA_PROTOCOL_MASK technologiesScreenLock,
                                                    tNFA_PROTOCOL_MASK technologiesScreenOff,
                                                    tNFA_PROTOCOL_MASK technologiesScreenOffLock)
{
    if (mSetProtoRoutingScene_ == 1) {
        mSetProtoRoutingScene_ = 0;
        return NFA_STATUS_FAILED;
    }
    eeEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcEeCallback, NFA_EE_SET_PROTO_CFG_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetSetDefaultProtoRoutingScene(int setDefaultProtoRoutingScene)
{
    mSetProtoRoutingScene_ = setDefaultProtoRoutingScene;
}

tNFA_STATUS NfcNciMock::NfcEeClearDefaultProtoRouting(tNFA_HANDLE eeHandle, tNFA_PROTOCOL_MASK clearProtocol)
{
    if (mClearProtoRoutingScene_ == 1) {
        mClearProtoRoutingScene_ = 0;
        return NFA_STATUS_FAILED;
    }
    eeEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcEeCallback, NFA_EE_CLEAR_PROTO_CFG_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetClearDefaultProtoRoutingScene(int clearDefaultProtoRoutingScene)
{
    mClearProtoRoutingScene_ = clearDefaultProtoRoutingScene;
}

tNFA_STATUS NfcNciMock::NfcEeAddAidRouting(
    tNFA_HANDLE eeHandle, uint8_t aidLen, uint8_t* pAid, tNFA_EE_PWR_STATE powerState, uint8_t aidInfo)
{
    eeEventData_.status = NFA_STATUS_OK;
    if (mAddAidScene_ == 1) {
        mAddAidScene_ = 0;
        eeEventData_.status = NFA_STATUS_FAILED;
    }
    std::thread(&NfcEeCallback, NFA_EE_ADD_AID_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetAddAidScene(int addAidScene)
{
    mAddAidScene_ = addAidScene;
}

tNFA_STATUS NfcNciMock::NfcEeRemoveAidRouting(uint8_t aidLen, uint8_t* pAid)
{
    eeEventData_.status = NFA_STATUS_OK;
    if (mRemoveAidScene_ == 1) {
        mRemoveAidScene_ = 0;
        eeEventData_.status = NFA_STATUS_FAILED;
    }
    std::thread(&NfcEeCallback, NFA_EE_REMOVE_AID_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetRemoveAidScene(int removeAidScene)
{
    mRemoveAidScene_ = removeAidScene;
}

tNFA_STATUS NfcNciMock::NfcEeUpdateNow(void)
{
    eeEventData_.status = NFA_STATUS_OK;
    std::thread(&NfcEeCallback, NFA_EE_UPDATED_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfcEeModeSet(tNFA_HANDLE eeHandle, tNFA_EE_MD mode)
{
    eeEventData_.mode_set.status = NFA_STATUS_OK;
    std::thread(&NfcEeCallback, NFA_EE_MODE_SET_EVT, &eeEventData_).detach();
    return NFA_STATUS_OK;
}

uint16_t NfcNciMock::NfcGetAidTableSize()
{
    return 100;
}

tNFA_STATUS NfcNciMock::NfcCeSetIsoDepListenTech(tNFA_TECHNOLOGY_MASK tech_mask)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfcCeRegisterAidOnDH(uint8_t aid[NFC_MAX_AID_LEN], uint8_t aidLen, tNFA_CONN_CBACK* pConnCback)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS NfcNciMock::NfcSetPowerSubStateForScreenState(uint8_t screenState)
{
    if (mSetPowerStatusScene_ == 1) {
        mSetPowerStatusScene_ = 0;
        return NFA_STATUS_FAILED;
    }
    dmEventData_.power_sub_state.status = NFA_STATUS_OK;
    std::thread(&NfcDeviceManagementCallback, NFA_DM_SET_POWER_SUB_STATE_EVT, &dmEventData_).detach();
    return NFA_STATUS_OK;
}

void NfcNciMock::SetSetPowerStatusScene(int setPowerStatusScene)
{
    mSetPowerStatusScene_ = setPowerStatusScene;
}

tNFA_STATUS NfcNciMock::NfcSetConfig(tNFA_PMID paramId, uint8_t length, uint8_t* pData)
{
    std::thread(&NfcDeviceManagementCallback, NFA_DM_SET_CONFIG_EVT, &dmEventData_).detach();
    return NFA_STATUS_OK;
}