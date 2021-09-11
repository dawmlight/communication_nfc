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
#include "tag_end_point.h"

#include <memory>
#include <thread>

#include "loghelper.h"
#include "nci_bal_tag.h"
#include "nfa_api.h"
#include "nfc_map.h"
#include "utils/synchronize_event.h"

namespace OHOS {
namespace nfc {
namespace ncibal {

OHOS::nfc::SynchronizeEvent TagEndPoint::mPresenceCheckWatchDog_;

TagEndPoint::TagEndPoint(const std::vector<int>& techList,
                         const std::vector<int>& techHandles,
                         const std::vector<int>& techLibNfcTypes,
                         const std::string& uid,
                         const std::vector<std::string>& techPollBytes,
                         const std::vector<std::string>& techActBytes)
    : mTechList_(std::move(techList)),
      mTechHandles_(std::move(techHandles)),
      mTechLibNfcTypes_(std::move(techLibNfcTypes)),
      mUid_(uid),
      mTechExtras_(std::make_shared<sdk::NfcMap>()),
      mTechPollBytes_(std::move(techPollBytes)),
      mTechActBytes_(std::move(techActBytes)),
      mConnectedHandle_(-1),
      mConnectedTechIndex_(-1),
      mIsPresent_(true),
      mIsPresenceChecking_(false),
      mIsPausePresenceChecking_(false),
      mAddNdefTech_(false)
{
}

TagEndPoint::~TagEndPoint()
{
    mTechList_.clear();
    mTechnologyList_.clear();
    mTechHandles_.clear();
    mTechLibNfcTypes_.clear();
    mTechPollBytes_.clear();
    mTechActBytes_.clear();
}

bool TagEndPoint::Connect(int technology)
{
    DebugLog("TagEndPoint::Connect");
    PausePresenceChecking();
    std::lock_guard<std::mutex> lock(mMutex_);
    bool bResult = false;
    tNFA_STATUS status;
    for (std::size_t i = 0; i < mTechnologyList_.size(); i++) {
        if (technology != mTechnologyList_[i]) {
            continue;
        }
        // try connect the tag
        if (mConnectedHandle_ != mTechHandles_[i]) {
            if (mConnectedHandle_ == -1) {
                // first connect
                status = NciBalTag::GetInstance().Connect(mTechHandles_[i], mTechLibNfcTypes_[i], mTechList_[i]);
            } else {
                bool reResult =
                    NciBalTag::GetInstance().Reconnect(mTechHandles_[i], mTechLibNfcTypes_[i], mTechList_[i], false);
                status = reResult ? NFA_STATUS_OK : NFA_STATUS_FAILED;
            }
        } else {
            if (technology == EnumNcibalTagTechnology::ENUM_NFC_NDEF_TECH) {
                // special for ndef
                i = 0;
            }
            bool reResult =
                NciBalTag::GetInstance().Reconnect(mTechHandles_[i], mTechLibNfcTypes_[i], mTechList_[i], false);
            status = reResult ? NFA_STATUS_OK : NFA_STATUS_FAILED;
        }

        if (status == NFA_STATUS_OK) {
            mConnectedHandle_ = mTechHandles_[i];
            mConnectedTechIndex_ = i;
            mIsPresent_ = true;
            bResult = true;
        }
        break;
    }
    ResumePresenceChecking();
    DebugLog("TagEndPoint::Connect exit, result = %d", bResult);
    return bResult;
}

bool TagEndPoint::Disconnect()
{
    DebugLog("TagEndPoint::Disconnect");
    std::lock_guard<std::mutex> lock(mMutex_);
    mConnectedHandle_ = -1;
    mConnectedTechIndex_ = -1;
    mIsPresent_ = false;
    bool bResult = NciBalTag::GetInstance().Disconnect();
    {
        SynchronizeGuard guard(mPresenceCheckWatchDog_);
        mPresenceCheckWatchDog_.NotifyOne();
    }
    DebugLog("TagEndPoint::Disconnect exit, result = %d", bResult);
    return bResult;
}

bool TagEndPoint::Reconnect()
{
    DebugLog("TagEndPoint::Reconnect");
    if (mConnectedTechIndex_ == -1) {
        return true;
    }
    PausePresenceChecking();
    std::lock_guard<std::mutex> lock(mMutex_);
    bool bResult = NciBalTag::GetInstance().Reconnect(mTechHandles_[mConnectedTechIndex_],
                                                      mTechLibNfcTypes_[mConnectedTechIndex_],
                                                      mTechList_[mConnectedTechIndex_],
                                                      false);
    ResumePresenceChecking();
    DebugLog("TagEndPoint::Reconnect exit, result = %d", bResult);
    return bResult;
}

int TagEndPoint::Transceive(std::string& request, std::string& response)
{
    DebugLog("TagEndPoint::Transceive");
    PausePresenceChecking();
    std::lock_guard<std::mutex> lock(mMutex_);
    int status = NciBalTag::GetInstance().Transceive(request, response);
    ResumePresenceChecking();
    DebugLog("TagEndPoint::Transceive exit, result = %d", status);
    return status;
}

bool TagEndPoint::PresenceCheck()
{
    DebugLog("TagEndPoint::PresenceCheck");
    PausePresenceChecking();
    std::lock_guard<std::mutex> lock(mMutex_);
    mIsPresent_ = NciBalTag::GetInstance().PresenceCheck();
    ResumePresenceChecking();
    return mIsPresent_;
}

bool TagEndPoint::IsPresent()
{
    DebugLog("TagEndPoint::IsPresent, result = %d", mIsPresent_);
    return mIsPresent_;
}

void TagEndPoint::PausePresenceChecking()
{
    mIsPausePresenceChecking_ = true;
}

void TagEndPoint::ResumePresenceChecking()
{
    mIsPausePresenceChecking_ = false;
}

void TagEndPoint::PresenceChecking(int presenceCheckDelay, TagEndPoint::TagDisconnectedCallBack callback)
{
    DebugLog("PresenceChecking::Start Presence Checking");
    while (mIsPresenceChecking_) {
        SynchronizeGuard guard(mPresenceCheckWatchDog_);
        bool isNotify = mPresenceCheckWatchDog_.Wait(presenceCheckDelay);
        if (isNotify || mIsPresent_ == false) {
            break;
        }
        if (mIsPausePresenceChecking_) {
            continue;
        }
        bool bResult = NciBalTag::GetInstance().PresenceCheck();
        if (!bResult) {
            DebugLog("PresenceChecking::Tag lost...");
            break;
        }
    }
    mIsPresent_ = false;
    NciBalTag::GetInstance().Disconnect();
    if (callback != nullptr && mIsPresenceChecking_ && mTechHandles_.size() > 0) {
        DebugLog("PresenceChecking::Disconnect callback %d", mTechHandles_[0]);
        callback(mTechHandles_[0]);
    }
    DebugLog("PresenceChecking::End Presence Checking");
}

void TagEndPoint::StartPresenceChecking(int presenceCheckDelay, TagDisconnectedCallBack callback)
{
    DebugLog("TagEndPoint::StartPresenceChecking");
    mIsPresent_ = true;
    mIsPresenceChecking_ = true;
    if (presenceCheckDelay <= 0) {
        presenceCheckDelay = DEFAULT_PRESENCE_CHECK_WATCH_DOG_TIMEOUT;
    }
    std::thread(&TagEndPoint::PresenceChecking, this, presenceCheckDelay, callback).detach();
}

void TagEndPoint::StopPresenceChecking()
{
    DebugLog("TagEndPoint::StopPresenceChecking");
    mIsPresenceChecking_ = false;
}

std::vector<int> TagEndPoint::GetTechList()
{
    for (std::vector<int>::iterator it = mTechList_.begin(); it != mTechList_.end(); ++it) {
        int technology = EnumNcibalTagTechnology::ENUM_NFC_INVALID_TECH;
        switch (*it) {
            case TARGET_TYPE_ISO14443_3A:
            case TARGET_TYPE_ISO14443_3B:
            case TARGET_TYPE_ISO14443_4:
                technology = EnumNcibalTagTechnology::ENUM_NFC_ISODEP_TECH;
                break;

            case TARGET_TYPE_FELICA:
                technology = EnumNcibalTagTechnology::ENUM_NFC_FELICA_TECH;
                break;

            case TARGET_TYPE_V:
                technology = EnumNcibalTagTechnology::ENUM_NFC_ISO15693_TECH;
                break;

            case TARGET_TYPE_NDEF:
                technology = EnumNcibalTagTechnology::ENUM_NFC_NDEF_TECH;
                break;

            case TARGET_TYPE_NDEF_FORMATABLE:
                technology = EnumNcibalTagTechnology::ENUM_NFC_NDEF_FORMATABLE_TECH;
                break;

            case TARGET_TYPE_MIFARE_CLASSIC:
                technology = EnumNcibalTagTechnology::ENUM_NFC_MIFARE_CLASSIC_TECH;
                break;

            case TARGET_TYPE_MIFARE_UL:
                technology = EnumNcibalTagTechnology::ENUM_NFC_MIFARE_ULTRALIGHT_TECH;
                break;
                
            case TARGET_TYPE_KOVIO_BARCODE:
            case TARGET_TYPE_UNKNOWN:
            default:
                technology = EnumNcibalTagTechnology::ENUM_NFC_INVALID_TECH;
                break;
        }
        mTechnologyList_.push_back(technology);
    }
    return mTechnologyList_;
}

void TagEndPoint::RemoveTechnology(int technology)
{
    DebugLog("TagEndPoint::RemoveTechnology");
    if (technology == -1) {
        DebugLog("Remove all");
    }
}

std::string TagEndPoint::GetUid()
{
    return mUid_;
}

std::shared_ptr<sdk::NfcMap> TagEndPoint::GenerateBundle(int index)
{
    std::shared_ptr<sdk::NfcMap> nfcMap = std::make_shared<sdk::NfcMap>();
    int targetType = mTechList_[index];
    std::string act = mTechActBytes_[index];
    std::string poll = mTechPollBytes_[index];
    DebugLog("GenerateBundle::targetType: %d", targetType);
    switch (targetType) {
        case TARGET_TYPE_ISO14443_3A: {
            if (!(act.empty())) {
                int sak = (act.at(0) & (0xff));
                nfcMap->PutLong(NCIBAL_SAK, sak);
                DebugLog("GenerateBundle::TARGET_TYPE_ISO14443_3A NCIBAL_SAK: %d", sak);
            }
            nfcMap->PutCharArray(NCIBAL_ATQA, std::make_shared<std::string>(poll));
            DebugLog("GenerateBundle::TARGET_TYPE_ISO14443_3A NCIBAL_ATQA: %s", poll.c_str());
            break;
        }

        case TARGET_TYPE_ISO14443_3B: {
            if (!(poll.empty()) && poll.length() >= NCIBAL_APP_DATA_LENGTH + NCIBAL_PROTOCOL_INFO_LENGTH) {
                std::string sAppData = poll.substr(0, NCIBAL_APP_DATA_LENGTH);
                nfcMap->PutCharArray(NCIBAL_APP_DATA, std::make_shared<std::string>(sAppData));
                DebugLog("GenerateBundle::TARGET_TYPE_ISO14443_3B NCIBAL_APP_DATA: %s", sAppData.c_str());

                std::string sProtoInfo = poll.substr(NCIBAL_APP_DATA_LENGTH, NCIBAL_PROTOCOL_INFO_LENGTH);
                nfcMap->PutCharArray(NCIBAL_PROTOCOL_INFO, std::make_shared<std::string>(sProtoInfo));
                DebugLog("GenerateBundle::TARGET_TYPE_ISO14443_3B NCIBAL_PROTOCOL_INFO: %s", sProtoInfo.c_str());
            }
            break;
        }

        case TARGET_TYPE_ISO14443_4: {
            bool hasNfcA = false;
            for (std::size_t i = 0; i < mTechList_.size(); i++) {
                if (mTechList_[i] == TARGET_TYPE_ISO14443_3A) {
                    hasNfcA = true;
                    break;
                }
            }
            if (hasNfcA) {
                nfcMap->PutCharArray(NCIBAL_HISTORICAL_BYTES, std::make_shared<std::string>(act));
                DebugLog("GenerateBundle::TARGET_TYPE_ISO14443_4 NCIBAL_HISTORICAL_BYTES: %s", act.c_str());               
            } else {
                nfcMap->PutCharArray(NCIBAL_HILAYER_RESPONSE, std::make_shared<std::string>(act));
                DebugLog("GenerateBundle::TARGET_TYPE_ISO14443_4 NCIBAL_HILAYER_RESPONSE: %s", act.c_str());               
            }
            break;
        }

        case TARGET_TYPE_MIFARE_CLASSIC: {
            if (!(act.empty())) {
                int sak = (act.at(0) & (0xff));
                nfcMap->PutLong(NCIBAL_SAK, sak);
                DebugLog("GenerateBundle::TARGET_TYPE_MIFARE_CLASSIC NCIBAL_SAK: %d", sak);
            }
            nfcMap->PutCharArray(NCIBAL_ATQA, std::make_shared<std::string>(poll));
            DebugLog("GenerateBundle::TARGET_TYPE_MIFARE_CLASSIC NCIBAL_ATQA: %s", poll.c_str());          
            break;
        }

        case TARGET_TYPE_V: {
            if (!(poll.empty()) && poll.length() >= NCIBAL_POLL_LENGTH_MIN) {
                nfcMap->PutLong(NCIBAL_RESPONSE_FLAGS, poll.at(0));
                DebugLog("GenerateBundle::TARGET_TYPE_V NCIBAL_RESPONSE_FLAGS: %d", poll.at(0));
                nfcMap->PutLong(NCIBAL_DSF_ID, poll.at(1));
                DebugLog("GenerateBundle::TARGET_TYPE_V NCIBAL_DSF_ID: %d", poll.at(1));
            }
            break;
        }

        case TARGET_TYPE_MIFARE_UL: {
            bool isUlC = IsUltralightC();
            nfcMap->PutLong(NCIBAL_MIFARE_ULTRALIGHT_C_TYPE, isUlC);
            DebugLog("GenerateBundle::TARGET_TYPE_MIFARE_UL NCIBAL_MIFARE_ULTRALIGHT_C_TYPE: %d", isUlC);
            break;
        }

        case TARGET_TYPE_FELICA:
        default:
            break;
    }
    return nfcMap;
}

std::weak_ptr<sdk::NfcMap> TagEndPoint::GetTechExtras()
{
    DebugLog("TagEndPoint::GetTechExtras, tech len.%d", mTechList_.size());
    for (std::size_t i = 0; i < mTechList_.size(); i++) {
        if (mTechList_[i] == TARGET_TYPE_NDEF || mTechList_[i] == TARGET_TYPE_NDEF_FORMATABLE) {
            continue;
        }
        std::shared_ptr<sdk::NfcMap> mExtra = GenerateBundle(i);
        if (mExtra) {
            mTechExtras_->PutNfcMap(NCIBAL_TECH_EXTRA_BUNDLE_PREIFIX + std::to_string(i), mExtra);
        }
    }
    return mTechExtras_;
}

int TagEndPoint::GetHandle()
{
    if (mTechList_.size() > 0) {
        return mTechHandles_[0];
    }
    return 0;
}

bool TagEndPoint::MakeReadOnly()
{
    DebugLog("TagEndPoint::MakeReadOnly");
    PausePresenceChecking();
    std::lock_guard<std::mutex> lock(mMutex_);
    bool bResult = NciBalTag::GetInstance().MakeReadOnly();
    ResumePresenceChecking();
    return bResult;
}

std::string TagEndPoint::ReadNdef()
{
    DebugLog("TagEndPoint::ReadNdef");
    PausePresenceChecking();
    std::string response = "";
    this->AddNdefTech();
    std::lock_guard<std::mutex> lock(mMutex_);
    NciBalTag::GetInstance().ReadNdef(response);
    ResumePresenceChecking();
    return response;
}

void TagEndPoint::AddNdefTech()
{
    if (mAddNdefTech_) {
        return;
    }
    mAddNdefTech_ = true;
    DebugLog("TagEndPoint::AddNdefTech");
    std::lock_guard<std::mutex> lock(mMutex_);
    bool foundFormat = false;
    int formatHandle = 0;
    int formatLibNfcType = 0;
    int index = mTechList_.size();
    for (int i = 0; i < index; i++) {
        NciBalTag::GetInstance().Reconnect(mTechHandles_[i], mTechLibNfcTypes_[i], mTechList_[i], false);
        std::vector<int> ndefInfo;
        if (NciBalTag::GetInstance().CheckNdef(ndefInfo)) {
            DebugLog("Add ndef tag info, index: %d", index);
            mTechList_.push_back(TARGET_TYPE_NDEF);
            mTechHandles_.push_back(mTechHandles_[i]);
            mTechLibNfcTypes_.push_back(mTechLibNfcTypes_[i]);

            std::shared_ptr<sdk::NfcMap> nfcMap = std::make_shared<sdk::NfcMap>();
            std::string sNdefMsg = "";
            NciBalTag::GetInstance().ReadNdef(sNdefMsg);
            nfcMap->PutCharArray(NCIBAL_NDEF_MSG, std::make_shared<std::string>(sNdefMsg));
            nfcMap->PutLong(NCIBAL_NDEF_FORUM_TYPE, GetNdefType(mTechLibNfcTypes_[i]));
            DebugLog("GenerateBundle::TARGET_TYPE_NDEF NCIBAL_NDEF_FORUM_TYPE: %d", GetNdefType(mTechLibNfcTypes_[i]));
            nfcMap->PutLong("NDEF_TAG_LENGTH", ndefInfo[0]);
            nfcMap->PutLong(NCIBAL_NDEF_TAG_MODE, ndefInfo[1]);
            DebugLog("GenerateBundle::TARGET_TYPE_NDEF NCIBAL_NDEF_TAG_MODE: %d", ndefInfo[1]);
            mTechExtras_->PutNfcMap(NCIBAL_TECH_EXTRA_BUNDLE_PREIFIX + std::to_string(index), nfcMap);

            foundFormat = false;
            break;
        }
        if (!foundFormat && NciBalTag::GetInstance().IsNdefFormattable()) {
            formatHandle = mTechHandles_[i];
            formatLibNfcType = mTechLibNfcTypes_[i];
            foundFormat = true;
        }
    }
    if (foundFormat) {
        DebugLog("Add ndef formatable tag info, index: %d", index);
        mTechList_.push_back(TARGET_TYPE_NDEF_FORMATABLE);
        mTechHandles_.push_back(formatHandle);
        mTechLibNfcTypes_.push_back(formatLibNfcType);
    }
}

int TagEndPoint::GetNdefType(int protocol) const
{
    int ndefType = NDEF_UNKNOWN_TYPE;
    if (NFA_PROTOCOL_T1T == protocol) {
        ndefType = NDEF_TYPE1_TAG;
    } else if (NFA_PROTOCOL_T2T == protocol) {
        ndefType = NDEF_TYPE2_TAG;
    } else if (NFA_PROTOCOL_T3T == protocol) {
        ndefType = NDEF_TYPE3_TAG;
    } else if (NFA_PROTOCOL_ISO_DEP == protocol) {
        ndefType = NDEF_TYPE4_TAG;
    } else if (NFC_PROTOCOL_MIFARE == protocol) {
        ndefType = NDEF_MIFARE_CLASSIC_TAG;
    } else {
        /* NFA_PROTOCOL_T5T, NFA_PROTOCOL_INVALID and others */
        ndefType = NDEF_UNKNOWN_TYPE;
    }
    return ndefType;
}

bool TagEndPoint::WriteNdef(std::string& data)
{
    DebugLog("TagEndPoint::WriteNdef");
    PausePresenceChecking();
    std::lock_guard<std::mutex> lock(mMutex_);
    bool bResult = NciBalTag::GetInstance().WriteNdef(data);
    ResumePresenceChecking();
    DebugLog("TagEndPoint::WriteNdef exit, result = %d", bResult);
    return bResult;
}

bool TagEndPoint::FormatNdef(const std::string& key)
{
    DebugLog("TagEndPoint::FormatNdef");
    if (key.empty()) {
        DebugLog("key is null");
        return false;
    }
    PausePresenceChecking();
    std::lock_guard<std::mutex> lock(mMutex_);
    bool bResult = NciBalTag::GetInstance().FormatNdef();
    ResumePresenceChecking();
    DebugLog("TagEndPoint::FormatNdef exit, result = %d", bResult);
    return bResult;
}

bool TagEndPoint::IsNdefFormatable()
{
    DebugLog("TagEndPoint::IsNdefFormatable");
    bool bResult = NciBalTag::GetInstance().IsNdefFormatable();
    DebugLog("TagEndPoint::IsNdefFormatable exit, result = %d", bResult);
    return bResult;
}

bool TagEndPoint::CheckNdef(std::vector<int>& ndefInfo)
{
    DebugLog("TagEndPoint::CheckNdef");
    PausePresenceChecking();
    std::lock_guard<std::mutex> lock(mMutex_);
    bool bResult = NciBalTag::GetInstance().CheckNdef(ndefInfo);
    ResumePresenceChecking();
    if (bResult) {
        DebugLog("NDEF supported by the tag");
    } else {
        DebugLog("NDEF unsupported by the tag")
    }
    return bResult;
}

int TagEndPoint::GetConnectedTechnology()
{
    DebugLog("TagEndPoint::GetConnectedTechnology");
    if (mConnectedTechIndex_ != -1) {
        return mTechList_[mConnectedTechIndex_];
    }
    return 0;
}

bool TagEndPoint::IsUltralightC()
{
    PausePresenceChecking();
    std::lock_guard<std::mutex> lock(mMutex_);
    bool iResult = false;
    std::string command = {0x30, 0x02};
    std::string response;
    NciBalTag::GetInstance().Transceive(command, response);
    if (!(response.empty()) && response.length() == NCIBAL_MIFARE_ULTRALIGHT_C_RESPONSE_LENGTH) {
        if (response[2] == NCIBAL_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[3] == NCIBAL_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[4] == NCIBAL_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[5] == NCIBAL_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[6] == NCIBAL_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[7] == NCIBAL_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[8] == NCIBAL_MIFARE_ULTRALIGHT_C_VERSION_INFO_FIRST &&
            response[9] == NCIBAL_MIFARE_ULTRALIGHT_C_VERSION_INFO_SECOND) {
            iResult = true;
        } else if (response[4] == NCIBAL_MIFARE_ULTRALIGHT_C_NDEF_CC &&
                   ((response[5] & 0xff) < NCIBAL_MIFARE_ULTRALIGHT_C_NDEF_MAJOR_VERSION) &&
                   ((response[6] & 0xff) > NCIBAL_MIFARE_ULTRALIGHT_C_NDEF_TAG_SIZE)) {
            iResult = true;
        }
    }
    ResumePresenceChecking();
    return iResult;
}
}  // namespace ncibal
}  // namespace nfc
}  // namespace OHOS