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
#ifndef EF_H
#define EF_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::se::security {
class AccessRuleFilesChannel;
/*
 * Elementary File
 */
class EF {
public:
    static const auto FILE_TYPE_EF = 0x04;
    static const auto FILE_STRUCTURE_TRANSPARENT = 0x00;
    static const auto FILE_STRUCTURE_LINEAR_FIXED = 0x01;
    static const auto FILE_UNKNOWN = 0xFF;
    static const auto APDU_SUCCESS = 0x9000;
    explicit EF(std::weak_ptr<AccessRuleFilesChannel> arfChannel);
    ~EF();
    int SelectFilePath(std::string path);
    int GetFileId();
    std::string ReadData(int offset, int len);
    std::string ReadRecord(int record);
    int GetFileNbRecords();
    void ParseFile(std::string data);

private:
    void ParseSIMFile(std::string data);
    void ParseUSIMFile(std::string data);
    int fileType_;
    int fileStructure_;
    int fileSize_;
    int fileRecordSize_;
    int fileNbRecords_;
    int fileId_;
    std::weak_ptr<AccessRuleFilesChannel> arfChannel_;
};
}  // namespace OHOS::se::security
#endif  // !EF_H
