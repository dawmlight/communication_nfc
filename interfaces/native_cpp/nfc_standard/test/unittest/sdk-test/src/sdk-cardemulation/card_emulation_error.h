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

#ifndef CARD_EMULATION_ERROR_H
#define CARD_EMULATION_ERROR_H

namespace OHOS::nfc::cardemulation {
constexpr int ERR_OK = 0;
constexpr int ERR_COMMON_START = ERR_OK - 1;
constexpr int ERR_UNKNOWN = ERR_OK - 2;
constexpr int ERR_NULL_POINTER = ERR_OK - 3;
constexpr int ERR_UNIMPLEMENT = ERR_OK - 4;
constexpr int ERR_UNSURPPORTED = ERR_OK - 5;

constexpr int ERR_CARD_EMULATION_START = 0x80000300;
constexpr int ERR_CARD_EMULATION_SERVICE_NOT_INIT = ERR_CARD_EMULATION_START - 1;  // 卡模拟服务未初始化
constexpr int ERR_AID_ILLIGLE_LENGTH = ERR_CARD_EMULATION_START - 2;               // AID 长度错误
constexpr int ERR_AID_INVALID = ERR_CARD_EMULATION_START - 3;                      // AID 无效(包含无效字符)
constexpr int ERR_NOT_FOUND_USER_APP_INFO = ERR_CARD_EMULATION_START - 4;  // 未找到用户卡模拟服务信息
constexpr int ERR_AIDSET_IS_EMPTY = ERR_CARD_EMULATION_START - 5;  // 可能的原因: 含有无效的aid而构造失败.
constexpr int ERR_NOT_FOUND_USER_INFO = ERR_CARD_EMULATION_START - 6;                 // 未找到用户信息
constexpr int ERR_NOT_FOUND_CESERVICE_INFO = ERR_CARD_EMULATION_START - 7;            // 未找到卡模拟APP信息
constexpr int ERR_IS_ON_HOST = ERR_CARD_EMULATION_START - 8;                          // HCE App 不能修改EE位置
constexpr int ERR_NOT_FOUND_INSTALLED_CESERVICE_INFO = ERR_CARD_EMULATION_START - 9;  // 未找到已安装的卡模拟APP信息
constexpr int ERR_CESERVICE_GETTER_IS_NULL = ERR_CARD_EMULATION_START - 10;           // 卡模拟服务
constexpr int ERR_ADD_ROUTING_AID_FAILED = ERR_CARD_EMULATION_START - 11;             // 添加 aid 路由失败
constexpr int ERR_REMOVE_ROUTING_AID_FAILED = ERR_CARD_EMULATION_START - 12;          // 移除 aid 路由失败
constexpr int ERR_COMMIT_ROUTING_FAILED = ERR_CARD_EMULATION_START - 13;              // 提交路由失败
constexpr int ERR_NOT_INIT_NCI = ERR_CARD_EMULATION_START - 14;                       // nci对象未初始化
constexpr int ERR_NOT_SUPPORT_TECH = ERR_CARD_EMULATION_START - 15;                   // 不支持的技术标识
constexpr int ERR_EMPTY_APDU = ERR_CARD_EMULATION_START - 16;                         // 不支持的技术标识
constexpr int ERR_ROUTING_TABLE_NOT_ENOUGH_CAPACITY = ERR_CARD_EMULATION_START - 17;  // 路由表没有足够的容量
constexpr int ERR_DROP_HCE_EVENT_DATA = ERR_CARD_EMULATION_START - 18;                // 丢弃HCE 事件和数据
constexpr int ERR_INVALID_REQUEST_HEADER = ERR_CARD_EMULATION_START - 19;             // 远程请求头无效

inline bool IS_OK(int err_code)
{
    return err_code == ERR_OK;
}
inline bool IS_NOK(int err_code)
{
    return !IS_OK(err_code);
}
}  // namespace OHOS::nfc::cardemulation
#endif  // CARD_EMULATION_ERROR_H
