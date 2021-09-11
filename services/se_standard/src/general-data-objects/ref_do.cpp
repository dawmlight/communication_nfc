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
#include "ref_do.h"

#include "aid_ref_do.h"
#include "hash_ref_do.h"
#include "loghelper.h"
#include "pkg_ref_do.h"
#include "se_common_exception.h"

namespace OHOS::se::security {
RefDo::RefDo(std::shared_ptr<AidRefDo> aidRefDo, std::shared_ptr<HashRefDo> hashRefDo)
    : BerTlv(REF_DO_TAG,
             (aidRefDo->GetData().length() + hashRefDo->GetData().length()),
             (aidRefDo->GetData() + hashRefDo->GetData())),
      aidRefDo_(aidRefDo),
      hashRefDo_(hashRefDo),
      pkgRefDo_(std::shared_ptr<PkgRefDo>())
{
}

RefDo::RefDo(std::shared_ptr<AidRefDo> aidRefDo,
             std::shared_ptr<HashRefDo> hashRefDo,
             std::shared_ptr<PkgRefDo> pkgRefDo)
    : BerTlv(REF_DO_TAG,
             (aidRefDo->GetData().length() + hashRefDo->GetData().length() + pkgRefDo->GetData().length()),
             (aidRefDo->GetData() + hashRefDo->GetData() + pkgRefDo->GetData())),
      aidRefDo_(aidRefDo),
      hashRefDo_(hashRefDo),
      pkgRefDo_(pkgRefDo)
{
}

RefDo::~RefDo() {}

bool RefDo::operator<(const RefDo& refDo) const
{
    if (aidRefDo_->GetAid() == refDo.aidRefDo_->GetAid()) {
        return hashRefDo_->GetHash() < refDo.hashRefDo_->GetHash();
    } else {
        return aidRefDo_->GetAid() < refDo.aidRefDo_->GetAid();
    }
}

std::shared_ptr<AidRefDo> RefDo::GetAidRefDo()
{
    return aidRefDo_;
}

std::shared_ptr<HashRefDo> RefDo::GetHashRefDo()
{
    return hashRefDo_;
}

std::shared_ptr<PkgRefDo> RefDo::GetPkgRefDo()
{
    return pkgRefDo_;
}

std::shared_ptr<RefDo> RefDo::BerTlvToRefDo(std::shared_ptr<BerTlv> berTlv)
{
    int tag = berTlv->GetTag();
    int length = berTlv->GetLength();
    std::string value = berTlv->GetValue();
    if (tag != REF_DO_TAG) {
        ErrorLog("Invalid tag in RefDo");
        throw AccessControlError("Parsing data error");
    }
    if (length > (int)value.length()) {
        ErrorLog("Value length not enough in RefDo");
        throw AccessControlError("Parsing data error");
    }

    std::shared_ptr<BerTlv> aidTlv = BerTlv::StrToBerTlv(value);
    std::shared_ptr<AidRefDo> aidRefDo = AidRefDo::BerTlvToAidRefDo(aidTlv);

    std::string hashTlv = value.substr((aidRefDo->GetData()).length());
    std::shared_ptr<BerTlv> hashRefData = BerTlv::StrToBerTlv(hashTlv);
    std::shared_ptr<HashRefDo> hashRefDo = HashRefDo::BerTlvToHashRefDo(hashRefData);

    if (hashTlv.length() == (hashRefDo->GetData()).length()) {
        return std::make_shared<RefDo>(aidRefDo, hashRefDo);
    } else {
        DebugLog("Has PKG-REF-DO");
        std::string remainData = hashTlv.substr((hashRefDo->GetData()).length());
        std::shared_ptr<BerTlv> pkgRefData = BerTlv::StrToBerTlv(remainData);
        std::shared_ptr<PkgRefDo> pkgRefDo = PkgRefDo::BerTlvToPkgRefDo(pkgRefData);
        return std::make_shared<RefDo>(aidRefDo, hashRefDo, pkgRefDo);
    }
}
}  // namespace OHOS::se::security