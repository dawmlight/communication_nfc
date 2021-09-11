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
#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <memory>
#include <mutex>

#include "isecure_element_session.h"

namespace OHOS::se::omapi {
class Reader;
class SEService;
class Channel;

/**
 * @brief Instances of this class represent a connection session to one of the SEs available on the device. These
 * objects can be used to get a communication channel with an applet in the SE. This channel can be the basic channel or
 * a logical channel.
 */
class Session : public std::enable_shared_from_this<Session> {
public:
    ~Session();
    /**
     * @brief Get the reader that provides this session.
     *
     * @return std::weak_ptr<Reader> The Reader object.
     */
    std::weak_ptr<Reader> GetReader() const;
    /**
     * @brief Get the ATR of this SE. A Null value SHALL be returned if the ATR for this SE is not available.
     * @return The ATR as a byte array or Null.
     */
    std::string GetATR() const;
    /**
     * @brief Close the connection with the SE. This will close any channels opened by this application with this SE.
     */
    void Close();
    /**
     * @brief Tells if this session is closed.
     * @return True if the session is closed, false otherwise.
     */
    bool IsClosed();
    /**
     * @brief Close any channels opened on this session.
     */
    void CloseChannels();
    /**
     * @brief Get access to the basic channel, as defined in [ISO 7816-4] (the one that has number 0). The obtained
     * object is an instance of the channel class.
     * @param aid The AID of the applet to be selected on this channel, as a byte array, or Null if no applet is to be
     * selected.
     * @param p2 The P2 parameter of the SELECT APDU executed on this channel.
     * @return An instance of channel if available or Null.
     */
    std::shared_ptr<Channel> OpenBasicChannel(const std::string& aid, int p2);
    /**
     * @brief This method is provided to ease the development of mobile applications and for backward compatibility with
     * existing applications. This method is equivalent to openBasicChannel(aid, P2=0x00).
     * @param aid The AID of the applet to be selected on this channel, as a byte array, or Null if no applet is to be
     * selected.
     * @return An instance of channel if available or Null.
     */
    std::shared_ptr<Channel> OpenBasicChannel(const std::string& aid);
    /**
     * @brief Open a logical channel with the SE, selecting the applet represented by the given AID (when the AID is not
     * Null and the length of the AID is not 0).
     * @param aid The AID of the applet to be selected on this channel, as a byte array.
     * @param p2 The P2 parameter of the SELECT APDU executed on this channel.
     * @return An instance of channel. Null if the SE is unable to provide a new logical channel or is unable to
     * retrieve Access Control rules due to the lack of an available logical channel.
     */
    std::shared_ptr<Channel> OpenLogicalChannel(const std::string& aid, int p2);
    /**
     * @brief This method is provided to ease the development of mobile applications and for backward compatibility with
     * existing applications. This method is equivalent to openLogicalChannel(aid, P2=0x00)
     * @param aid The AID of the applet to be selected on this channel, as a byte array.
     * @return An instance of channel. Null if the SE is unable to provide a new logical channel or is unable to
     * retrieve Access Control rules due to the lack of an available logical channel.
     */
    std::shared_ptr<Channel> OpenLogicalChannel(const std::string& aid);

private:
    Session(std::weak_ptr<SEService> service, sptr<ISecureElementSession> session, std::weak_ptr<Reader> reader);

private:
    static std::string mTag_;
    sptr<ISecureElementSession> mSession_{};
    std::weak_ptr<SEService> mService_{};
    std::weak_ptr<Reader> mReader_{};
    std::mutex mMutex_{};

    friend class Reader;
};
}  // namespace OHOS::se::omapi
#endif  // SESSION_H
