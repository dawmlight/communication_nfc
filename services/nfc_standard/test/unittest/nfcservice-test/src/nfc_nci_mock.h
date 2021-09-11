#ifndef NFC_NCI_MOCK_H
#define NFC_NCI_MOCK_H

#include "infc_nci.h"

class NfcNciMock : public OHOS::nfc::ncibal::INfcNci {
public:
    NfcNciMock();
    virtual ~NfcNciMock() override;
    virtual void NfaInit(tHAL_NFC_ENTRY* pHalEntryTbl) override;
    virtual tNFA_STATUS NfaEnable(tNFA_DM_CBACK* pDmCback, tNFA_CONN_CBACK* pConnCback) override;
    virtual tNFA_STATUS NfaDisable(bool graceful) override;
    virtual tNFA_STATUS NfaEnablePolling(tNFA_TECHNOLOGY_MASK pollMask) override;
    virtual tNFA_STATUS NfaDisablePolling(void) override;
    virtual tNFA_STATUS NfaEnableListening(void) override;
    virtual tNFA_STATUS NfaDisableListening(void) override;
    virtual tNFA_STATUS NfaStartRfDiscovery(void) override;
    virtual tNFA_STATUS NfaStopRfDiscovery(void) override;
    virtual tNFA_STATUS NfaSetRfDiscoveryDuration(uint16_t discoveryPeriodMs) override;
    virtual tNFA_STATUS NfaSelect(uint8_t rfDiscId, tNFA_NFC_PROTOCOL protocol, tNFA_INTF_TYPE rfInterface) override;
    virtual tNFA_STATUS NfaDeactivate(bool sleepMode) override;
    virtual tNFA_STATUS NfaSendRawFrame(uint8_t* pRawData, uint16_t dataLen, uint16_t presenceCheckStartDelay) override;
    virtual tNFA_STATUS NfaRegisterNDefTypeHandler(bool handleWholeMessage,
                                                   tNFA_TNF tnf,
                                                   uint8_t* pTypeName,
                                                   uint8_t typeNameLen,
                                                   tNFA_NDEF_CBACK* pNdefCback) override;
    virtual tNFA_STATUS NfaRwDetectNdef(void) override;
    virtual tNFA_STATUS NfaRwReadNdef(void) override;
    virtual tNFA_STATUS NfaRwWriteNdef(uint8_t* pData, uint32_t len) override;
    virtual tNFA_STATUS NfaRwPresenceCheck(tNFA_RW_PRES_CHK_OPTION option) override;
    virtual tNFA_STATUS NfaRwFormatTag(void) override;
    virtual tNFA_STATUS NfaRwSetTagReadOnly(bool bHardLock) override;
    virtual void NdefMsgInit(uint8_t* pMsg, uint32_t maxSize, uint32_t* pCurSize) override;
    virtual tNDEF_STATUS NdefMsgAddRec(uint8_t* pMsg,
                                       uint32_t maxSize,
                                       uint32_t* pCurSize,
                                       uint8_t tnf,
                                       uint8_t* pType,
                                       uint8_t typeLen,
                                       uint8_t* pId,
                                       uint8_t idLen,
                                       uint8_t* pPayload,
                                       uint32_t payloadLen) override;
    virtual uint8_t NfcGetNciVersion() override;
    virtual void NfcAdaptationInitialize() override;
    virtual void NfcAdaptationFinalize() override;
    virtual tHAL_NFC_ENTRY* NfcAdaptationGetHalEntryFuncs() override;
    virtual void NfcAdaptationDownloadFirmware() override;
    virtual void NfcAdaptationDump(int fd) override;
    virtual void NfcAdaptationFactoryReset() override;
    virtual void NfcAdaptationDeviceShutdown() override;
    virtual tNFA_STATUS NfcHciRegister(char* pAppName, tNFA_HCI_CBACK* pCback, bool bSendConnEvts) override;
    virtual tNFA_STATUS NfcEeGetInfo(uint8_t* pNumNfcee, tNFA_EE_INFO* pInfo) override;
    virtual tNFA_STATUS NfcEeRegister(tNFA_EE_CBACK* pCback) override;
    virtual tNFA_STATUS NfcEeDeregister(tNFA_EE_CBACK* pCback) override;
    virtual tNFA_STATUS NfcEeSetDefaultTechRouting(tNFA_HANDLE eeHandle,
                                                   tNFA_TECHNOLOGY_MASK technologiesSwitchOn,
                                                   tNFA_TECHNOLOGY_MASK technologiesSwitchOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesBatteryOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenLock,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenOffLock) override;
    virtual tNFA_STATUS NfcEeClearDefaultTechRouting(tNFA_HANDLE eeHandle,
                                                     tNFA_TECHNOLOGY_MASK clearTechnology) override;
    virtual tNFA_STATUS NfcEeSetDefaultProtoRouting(tNFA_HANDLE eeHandle,
                                                    tNFA_PROTOCOL_MASK protocolsSwitchOn,
                                                    tNFA_PROTOCOL_MASK protocolsSwitchOff,
                                                    tNFA_PROTOCOL_MASK protocolsBatteryOff,
                                                    tNFA_PROTOCOL_MASK technologiesScreenLock,
                                                    tNFA_PROTOCOL_MASK technologiesScreenOff,
                                                    tNFA_PROTOCOL_MASK technologiesScreenOffLock) override;
    virtual tNFA_STATUS NfcEeClearDefaultProtoRouting(tNFA_HANDLE eeHandle, tNFA_PROTOCOL_MASK clearProtocol) override;
    virtual tNFA_STATUS NfcEeAddAidRouting(
        tNFA_HANDLE eeHandle, uint8_t aidLen, uint8_t* pAid, tNFA_EE_PWR_STATE powerState, uint8_t aidInfo) override;
    virtual tNFA_STATUS NfcEeRemoveAidRouting(uint8_t aidLen, uint8_t* pAid) override;
    virtual tNFA_STATUS NfcEeUpdateNow(void) override;
    virtual tNFA_STATUS NfcEeModeSet(tNFA_HANDLE eeHandle, tNFA_EE_MD mode) override;
    virtual uint16_t NfcGetAidTableSize() override;
    virtual tNFA_STATUS NfcCeSetIsoDepListenTech(tNFA_TECHNOLOGY_MASK tech_mask) override;
    virtual tNFA_STATUS NfcCeRegisterAidOnDH(uint8_t aid[NFC_MAX_AID_LEN],
                                             uint8_t aidLen,
                                             tNFA_CONN_CBACK* pConnCback) override;
    virtual tNFA_STATUS NfcSetPowerSubStateForScreenState(uint8_t screenState) override;
    virtual tNFA_STATUS NfcSetConfig(tNFA_PMID paramId, uint8_t length, uint8_t* pData) override;

    static void NfcDeviceManagementCallback(uint8_t event, tNFA_DM_CBACK_DATA* p_data);
    static void NfcConnectionCallback(uint8_t event, tNFA_CONN_EVT_DATA* p_data);
    static void NdefCallback(tNFA_NDEF_EVT event, tNFA_NDEF_EVT_DATA* p_data);
    static void NfcHciCallback(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* p_data);
    static void NfcEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* p_data);

    void SetEnableScene(int enableScene);
    void SetDisableScene(int disableScene);
    void SetSelectScene(int selectScene);
    void SetDeactivateScene(int deactivateScene);
    void SetSendRawFrameScene(int sendRawFrameScene);
    void SetRwPresenceCheckScene(int rwPresenceCheckScene);
    void SetRwSetTagReadOnlyScene(int rwSetTagReadOnlyScene);
    void SetRwReadNdefScene(int rwReadNdefScene);
    void SetRwWriteNdefScene(int rwWriteNdefScene);
    void SetRwFormatTagScene(int rwFormatTagScene);
    void SetRwDetectNdefScene(int rwDetectNdefScene);
    void SetAddAidScene(int addAidScene);
    void SetRemoveAidScene(int removeAidScene);
    void SetNciVersion(int nciVersion);
    void SetSetDefaultProtoRoutingScene(int setDefaultProtoRoutingScene);
    void SetClearDefaultProtoRoutingScene(int clearDefaultProtoRoutingScene);
    void SetSetDefaultTechRoutingScene(int setDefaultTechRoutingScene);
    void SetClearDefaultTechRoutingScene(int clearDefaultTechRoutingScene);
    void SetSetPowerStatusScene(int setPowerStatusScene);

private:
    static tNFA_DM_CBACK* mNfcDeviceManagementCallback_;
    static tNFA_CONN_CBACK* mNfcConnectionCallback_;
    static tNFA_NDEF_CBACK* mNdefCallback_;
    static tNFA_HCI_CBACK* mHciCallback_;
    static tNFA_EE_CBACK* mEeCallback_;

    static tNFA_CONN_EVT_DATA connEventData_;
    static tNFA_DM_CBACK_DATA dmEventData_;
    static tNFA_NDEF_EVT_DATA ndefEventData_;
    static tNFA_HCI_EVT_DATA hciEventData_;
    static tNFA_EE_CBACK_DATA eeEventData_;

    int mEnableScene_{0};
    int mDisableScene_{0};
    int mSelectScene_{0};
    int mDeactivateScene_{0};
    int mSendRawFrameScene_{0};
    int mRwPresenceCheckScene_{0};
    int mRwFormatTagScene_{0};
    int mRwSetTagReadOnlyScene_{0};
    int mRwReadNdefScene_{0};
    int mRwWriteNdefScene_{0};
    int mRwDetectNdefScene_{0};
    int mAddAidScene_{0};
    int mRemoveAidScene_{0};
    int mNciVersion_{0};
    int mSetProtoRoutingScene_{0};
    int mClearProtoRoutingScene_{0};
    int mSetTechRoutingScene_{0};
    int mClearTechRoutingScene_{0};
    int mSetPowerStatusScene_{0};
};
#endif  // !NFC_NCI_MOCK_H