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
#include "access_control_conditions.h"

#include "access_rule_cache.h"
#include "channel_access_rule.h"
#include "der_parser.h"
#include "ef.h"
#include "general-data-objects/aid_ref_do.h"
#include "general-data-objects/hash_ref_do.h"
#include "general-data-objects/ref_do.h"
#include "loghelper.h"
#include "se_channel.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
AccessControlConditions::AccessControlConditions(std::weak_ptr<AccessRuleCache> accessRuleCache,
                                                 std::shared_ptr<AidRefDo> aidRefDo)
    : data_(""), accessRuleCache_(accessRuleCache), aidRefDo_(aidRefDo)
{
}

AccessControlConditions::~AccessControlConditions() {}

void AccessControlConditions::ReadDataFromPath(std::weak_ptr<AccessRuleFilesChannel> arfChannel, std::string path)
{
    DebugLog("ACCF read data from path");
    EF ef = EF(arfChannel);
    if (ef.SelectFilePath(path) == EF::APDU_SUCCESS) {
        data_ = ef.ReadData(0, -1);
        AnalyzeData(data_);
    } else {
        DebugLog("ACCF not found");
    }
}

void AccessControlConditions::ReadData(std::string data)
{
    DebugLog("ACCF read data");
    data_ = data;
    AnalyzeData(data_);
}

std::string AccessControlConditions::GetData()
{
    return data_;
}

void AccessControlConditions::AnalyzeData(std::string data)
{
    DebugLog("ACCF analyze data");
    ChannelAccessRule channelAccessRule = ChannelAccessRule();
    std::shared_ptr<HashRefDo> hashRefDo;
    DerParser derParser = DerParser(data);

    std::shared_ptr<AccessRuleCache> accessRuleCache = accessRuleCache_.lock();
    if (!accessRuleCache) {
        DebugLog("AccessRuleCache has expired");
        return;
    }

    if (derParser.IsEnd()) {
        DebugLog("Empty file, deny all access");
        channelAccessRule.SetAccessRule(ChannelAccessRule::ACCESSRULE::NEVER, "Empty ACCondition");
        channelAccessRule.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
        channelAccessRule.SetHasApduFilter(false);
        channelAccessRule.SetApduFilters({});
        channelAccessRule.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::NEVER);
        hashRefDo = std::make_shared<HashRefDo>();
        std::shared_ptr<RefDo> refDo = std::make_shared<RefDo>(aidRefDo_, hashRefDo);
        accessRuleCache->AddAccessRule(refDo, channelAccessRule);
        return;
    }

    while (!derParser.IsEnd()) {
        channelAccessRule = ChannelAccessRule();
        channelAccessRule.SetAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS, "");
        channelAccessRule.SetApduAccessRule(ChannelAccessRule::ACCESSRULE::ALWAYS);
        channelAccessRule.SetHasApduFilter(false);
        channelAccessRule.SetNFCEventAccessRule(ChannelAccessRule::ACCESSRULE::UNKNOWN);
        if (derParser.ParseTlv(DerParser::ASN1_SEQUENCE) > 0) {
            std::string tlvRule = derParser.GetTlvData();
            DerParser derRule = DerParser(tlvRule);
            if (DerParser::ASN1_OCTET_STRING == tlvRule[0]) {
                derRule.ParseTlv(DerParser::ASN1_OCTET_STRING);
                std::string hash = derRule.GetTlvData();
                if (hash.length() != HashRefDo::SHA1_LEN && hash.length() != HashRefDo::SHA256_LEN &&
                    hash.length() != 0) {
                    DebugLog("Invalid hash length");
                    throw AccessControlError("Invalid hash length");
                }
                hashRefDo = std::make_shared<HashRefDo>(hash);
            } else if (DerParser::ASN1_PADDING == tlvRule[0]) {
                DebugLog("Invalid hash");
                throw AccessControlError("Invalid hash");
            } else {
                hashRefDo = std::make_shared<HashRefDo>();
            }

            if (!derRule.IsEnd()) {
                if (derRule.ParseTlv() == 0xA0) {
                    DerParser derAccessRule = DerParser(derRule.GetTlvData());
                    while (!derAccessRule.IsEnd()) {
                        switch (derAccessRule.ParseTlv()) {
                            case 0xA0: {
                                DerParser derApdu = DerParser(derAccessRule.GetTlvData());
                                int tApdu = derApdu.ParseTlv();
                                if (tApdu == 0x80) {
                                    channelAccessRule.SetApduAccessRule((derApdu.GetTlvData()[0] == 0x01)
                                                                            ? ChannelAccessRule::ACCESSRULE::ALWAYS
                                                                            : ChannelAccessRule::ACCESSRULE::NEVER);
                                } else if (tApdu == 0xA1) {
                                    DerParser derApduFilter = DerParser(derApdu.GetTlvData());
                                    int tag = derApduFilter.ParseTlv();
                                    if (tag == DerParser::ASN1_OCTET_STRING) {
                                        std::vector<std::string> apduFilters;
                                        apduFilters.push_back(derApduFilter.GetTlvData());
                                        while (!derApduFilter.IsEnd()) {
                                            if (derApduFilter.ParseTlv() == DerParser::ASN1_OCTET_STRING) {
                                                apduFilters.push_back(derApduFilter.GetTlvData());
                                            }
                                        }
                                        channelAccessRule.SetHasApduFilter(true);
                                        channelAccessRule.SetApduFilters(apduFilters);
                                    } else {
                                        DebugLog("Invalid apdu filter");
                                        throw AccessControlError("Invalid apdu filter");
                                    }
                                } else {
                                    DebugLog("Invalid apdu permission");
                                    throw AccessControlError("Invalid apdu permission");
                                }
                            } break;

                            case 0xA1: {
                                DerParser derNfc = DerParser(derAccessRule.GetTlvData());
                                if (derNfc.ParseTlv() == 0x80) {
                                    channelAccessRule.SetNFCEventAccessRule((derNfc.GetTlvData()[0] == 0x01)
                                                                                ? ChannelAccessRule::ACCESSRULE::ALWAYS
                                                                                : ChannelAccessRule::ACCESSRULE::NEVER);
                                } else {
                                    DebugLog("Invalid nfc permission");
                                    throw AccessControlError("Invalid nfc permission");
                                }
                            } break;

                            default: {
                                DebugLog("Invalid tag");
                                throw AccessControlError("Invalid tag");
                            } break;
                        }
                    }
                }
            }
        } else {
            hashRefDo = std::make_shared<HashRefDo>();
            if ((channelAccessRule.GetNFCEventAccessRule() == ChannelAccessRule::ACCESSRULE::UNKNOWN) &&
                (channelAccessRule.GetApduAccessRule() != ChannelAccessRule::ACCESSRULE::UNKNOWN)) {
                DebugLog("No nfc event access");
                channelAccessRule.SetNFCEventAccessRule(channelAccessRule.GetApduAccessRule());
            }
        }
        std::shared_ptr<RefDo> refDo = std::make_shared<RefDo>(aidRefDo_, hashRefDo);
        accessRuleCache->AddAccessRule(refDo, channelAccessRule);
    }
}
}  // namespace OHOS::se::security