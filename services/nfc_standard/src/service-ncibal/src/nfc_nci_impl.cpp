/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "nfc_nci_impl.h"

#include "NfcAdaptation.h"

namespace OHOS {
namespace nfc {
namespace ncibal {
NfcNciImpl::NfcNciImpl() {}

NfcNciImpl::~NfcNciImpl() {}

void NfcNciImpl::NfaInit(tHAL_NFC_ENTRY* pHalEntryTbl)
{
    NFA_Init(pHalEntryTbl);
}

tNFA_STATUS NfcNciImpl::NfaEnable(tNFA_DM_CBACK* pDmCback, tNFA_CONN_CBACK* pConnCback)
{
    return NFA_Enable(pDmCback, pConnCback);
}

tNFA_STATUS NfcNciImpl::NfaDisable(bool graceful)
{
    return NFA_Disable(graceful);
}

tNFA_STATUS NfcNciImpl::NfaEnablePolling(tNFA_TECHNOLOGY_MASK pollMask)
{
    return NFA_EnablePolling(pollMask);
}

tNFA_STATUS NfcNciImpl::NfaDisablePolling(void)
{
    return NFA_DisablePolling();
}

tNFA_STATUS NfcNciImpl::NfaEnableListening(void)
{
    return NFA_EnableListening();
}

tNFA_STATUS NfcNciImpl::NfaDisableListening(void)
{
    return NFA_DisableListening();
}

tNFA_STATUS NfcNciImpl::NfaStartRfDiscovery(void)
{
    return NFA_StartRfDiscovery();
}

tNFA_STATUS NfcNciImpl::NfaStopRfDiscovery(void)
{
    return NFA_StopRfDiscovery();
}

tNFA_STATUS NfcNciImpl::NfaSetRfDiscoveryDuration(uint16_t discoveryPeriodMs)
{
    return NFA_SetRfDiscoveryDuration(discoveryPeriodMs);
}

tNFA_STATUS NfcNciImpl::NfaSelect(uint8_t rfDiscId, tNFA_NFC_PROTOCOL protocol, tNFA_INTF_TYPE rfInterface)
{
    return NFA_Select(rfDiscId, protocol, rfInterface);
}

tNFA_STATUS NfcNciImpl::NfaDeactivate(bool sleepMode)
{
    return NFA_Deactivate(sleepMode);
}

tNFA_STATUS NfcNciImpl::NfaSendRawFrame(uint8_t* pRawData, uint16_t dataLen, uint16_t presenceCheckStartDelay)
{
    return NFA_SendRawFrame(pRawData, dataLen, presenceCheckStartDelay);
}

tNFA_STATUS NfcNciImpl::NfaRegisterNDefTypeHandler(
    bool handleWholeMessage, tNFA_TNF tnf, uint8_t* pTypeName, uint8_t typeNameLen, tNFA_NDEF_CBACK* pNdefCback)
{
    return NFA_RegisterNDefTypeHandler(handleWholeMessage, tnf, pTypeName, typeNameLen, pNdefCback);
}

tNFA_STATUS NfcNciImpl::NfaRwDetectNdef(void)
{
    return NFA_RwDetectNDef();
}

tNFA_STATUS NfcNciImpl::NfaRwReadNdef(void)
{
    return NFA_RwReadNDef();
}

tNFA_STATUS NfcNciImpl::NfaRwWriteNdef(uint8_t* pData, uint32_t len)
{
    return NFA_RwWriteNDef(pData, len);
}

tNFA_STATUS NfcNciImpl::NfaRwPresenceCheck(tNFA_RW_PRES_CHK_OPTION option)
{
    return NFA_RwPresenceCheck(option);
}

tNFA_STATUS NfcNciImpl::NfaRwFormatTag(void)
{
    return NFA_RwFormatTag();
}

tNFA_STATUS NfcNciImpl::NfaRwSetTagReadOnly(bool bHardLock)
{
    return NFA_RwSetTagReadOnly(bHardLock);
}

void NfcNciImpl::NdefMsgInit(uint8_t* pMsg, uint32_t maxSize, uint32_t* pCurSize)
{
    NDEF_MsgInit(pMsg, maxSize, pCurSize);
}

tNDEF_STATUS NfcNciImpl::NdefMsgAddRec(uint8_t* pMsg,
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
    return NDEF_MsgAddRec(pMsg, maxSize, pCurSize, tnf, pType, typeLen, pId, idLen, pPayload, payloadLen);
}

uint8_t NfcNciImpl::NfcGetNciVersion()
{
    return NFC_GetNCIVersion();
}

void NfcNciImpl::NfcAdaptationInitialize()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.Initialize();
}

void NfcNciImpl::NfcAdaptationFinalize()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.Finalize();
}

tHAL_NFC_ENTRY* NfcNciImpl::NfcAdaptationGetHalEntryFuncs()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    return nfcAdaptInstance.GetHalEntryFuncs();
}

void NfcNciImpl::NfcAdaptationDownloadFirmware()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.DownloadFirmware();
}

void NfcNciImpl::NfcAdaptationDump(int fd)
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.Dump(fd);
}

void NfcNciImpl::NfcAdaptationFactoryReset()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.FactoryReset();
}

void NfcNciImpl::NfcAdaptationDeviceShutdown()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.DeviceShutdown();
}

tNFA_STATUS NfcNciImpl::NfcHciRegister(char* pAppName, tNFA_HCI_CBACK* pCback, bool bSendConnEvts)
{
    return NFA_HciRegister(pAppName, pCback, bSendConnEvts);
}

tNFA_STATUS NfcNciImpl::NfcEeGetInfo(uint8_t* pNumNfcee, tNFA_EE_INFO* pInfo)
{
    return NFA_EeGetInfo(pNumNfcee, pInfo);
}

tNFA_STATUS NfcNciImpl::NfcEeRegister(tNFA_EE_CBACK* pCback)
{
    return NFA_EeRegister(pCback);
}

tNFA_STATUS NfcNciImpl::NfcEeDeregister(tNFA_EE_CBACK* pCback)
{
    return NFA_EeDeregister(pCback);
}

tNFA_STATUS NfcNciImpl::NfcEeSetDefaultTechRouting(tNFA_HANDLE eeHandle,
                                                   tNFA_TECHNOLOGY_MASK technologiesSwitchOn,
                                                   tNFA_TECHNOLOGY_MASK technologiesSwitchOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesBatteryOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenLock,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenOffLock)
{
    return NFA_EeSetDefaultTechRouting(eeHandle,
                                       technologiesSwitchOn,
                                       technologiesSwitchOff,
                                       technologiesBatteryOff,
                                       technologiesScreenLock,
                                       technologiesScreenOff,
                                       technologiesScreenOffLock);
}

tNFA_STATUS NfcNciImpl::NfcEeClearDefaultTechRouting(tNFA_HANDLE eeHandle, tNFA_TECHNOLOGY_MASK clearTechnology)
{
    return NFA_EeClearDefaultTechRouting(eeHandle, clearTechnology);
}

tNFA_STATUS NfcNciImpl::NfcEeSetDefaultProtoRouting(tNFA_HANDLE eeHandle,
                                                    tNFA_PROTOCOL_MASK protocolsSwitchOn,
                                                    tNFA_PROTOCOL_MASK protocolsSwitchOff,
                                                    tNFA_PROTOCOL_MASK protocolsBatteryOff,
                                                    tNFA_PROTOCOL_MASK technologiesScreenLock,
                                                    tNFA_PROTOCOL_MASK technologiesScreenOff,
                                                    tNFA_PROTOCOL_MASK technologiesScreenOffLock)
{
    return NFA_EeSetDefaultProtoRouting(eeHandle,
                                        protocolsSwitchOn,
                                        protocolsSwitchOff,
                                        protocolsBatteryOff,
                                        technologiesScreenLock,
                                        technologiesScreenOff,
                                        technologiesScreenOffLock);
}

tNFA_STATUS NfcNciImpl::NfcEeClearDefaultProtoRouting(tNFA_HANDLE eeHandle, tNFA_PROTOCOL_MASK clearProtocol)
{
    return NFA_EeClearDefaultProtoRouting(eeHandle, clearProtocol);
}

tNFA_STATUS NfcNciImpl::NfcEeAddAidRouting(
    tNFA_HANDLE eeHandle, uint8_t aidLen, uint8_t* pAid, tNFA_EE_PWR_STATE powerState, uint8_t aidInfo)
{
    return NFA_EeAddAidRouting(eeHandle, aidLen, pAid, powerState, aidInfo);
}

tNFA_STATUS NfcNciImpl::NfcEeRemoveAidRouting(uint8_t aidLen, uint8_t* pAid)
{
    return NFA_EeRemoveAidRouting(aidLen, pAid);
}

tNFA_STATUS NfcNciImpl::NfcEeUpdateNow(void)
{
    return NFA_EeUpdateNow();
}

uint16_t NfcNciImpl::NfcGetAidTableSize()
{
    return NFA_GetAidTableSize();
}

tNFA_STATUS NfcNciImpl::NfcEeModeSet(tNFA_HANDLE eeHandle, tNFA_EE_MD mode)
{
    return NFA_EeModeSet(eeHandle, mode);
}

tNFA_STATUS NfcNciImpl::NfcCeSetIsoDepListenTech(tNFA_TECHNOLOGY_MASK techMask)
{
    return NFA_CeSetIsoDepListenTech(techMask);
}

tNFA_STATUS NfcNciImpl::NfcCeRegisterAidOnDH(uint8_t aid[NFC_MAX_AID_LEN], uint8_t aidLen, tNFA_CONN_CBACK* pConnCback)
{
    return NFA_CeRegisterAidOnDH(aid, aidLen, pConnCback);
}

tNFA_STATUS NfcNciImpl::NfcSetPowerSubStateForScreenState(uint8_t screenState)
{
    return NFA_SetPowerSubStateForScreenState(screenState);
}

tNFA_STATUS NfcNciImpl::NfcSetConfig(tNFA_PMID paramId, uint8_t length, uint8_t* pData)
{
    return NFA_SetConfig(paramId, length, pData);
}
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS