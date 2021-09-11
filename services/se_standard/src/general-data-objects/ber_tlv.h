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
#ifndef BER_TLV_H
#define BER_TLV_H

#include <memory>
#include <string>

namespace OHOS::se::security {
/*
 * Basic Encoding Rules Tag Length Value
 */
class BerTlv {
public:
    BerTlv(int tag, int length, std::string value, std::string data);
    BerTlv(int tag, int length, std::string value);
    ~BerTlv();
    int GetTag();
    int GetLength();
    std::string GetValue();
    std::string GetData();
    static std::shared_ptr<BerTlv> StrToBerTlv(std::string& data);
    void Print();

private:
    int tag_;
    int length_;
    std::string value_;
    std::string data_;
};
}  // namespace OHOS::se::security
#endif /* BER_TLV_H */
