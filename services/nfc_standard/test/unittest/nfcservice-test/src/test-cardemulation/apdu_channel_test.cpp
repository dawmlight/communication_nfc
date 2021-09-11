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

#include "apdu_channel.h"

#include <gtest/gtest.h>

#include <cstdio>

using namespace OHOS::nfc::sdk::cardemulation;

namespace OHOS::nfc::sdk::cardemulation::test {
struct MsgHandler {
    MsgHandler(const MsgHandler& other)
    {
        if (other.expectedMsg_) {
            expectedMsg_ = std::make_unique<Msg>(*other.expectedMsg_);
        }
    }
    MsgHandler& operator=(const MsgHandler& other){
        if (this != &other ){
            if (other.expectedMsg_) {
                expectedMsg_ = std::make_unique<Msg>(*other.expectedMsg_);
            }
        }

        return *this;
    }
    explicit MsgHandler(std::unique_ptr<Msg> m) : expectedMsg_(std::move(m))
    {
    }

    void operator()(std::unique_ptr<Msg> m)
    {
        ASSERT_TRUE(expectedMsg_);
        ASSERT_TRUE(m);
        EXPECT_TRUE(m->operator==(*expectedMsg_));
    }

    std::unique_ptr<Msg> expectedMsg_;
};

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(Msg, Constructor)
{
    Msg m1(1, 2, 3);
    m1.SetData({0x31, 0x32, 0x33});
    auto stub = OHOS::sptr<ApduChannelStub>(new ApduChannelStub());
    m1.SetSource(RemoteObjectPool::GetRemoteObject(stub->AsObject()));

    Msg m2(m1);
    EXPECT_EQ(m1, m2);

    Msg m3(std::move(m2));
    EXPECT_EQ(m3, m1);

    Msg m4 = m3;
    EXPECT_EQ(m4, m1);

    Msg m5 = std::move(m4);
    EXPECT_EQ(m5, m1);

    Msg m6(4, 5, 6);
    m6 = m5;
    EXPECT_EQ(m6, m1);

    Msg m7(7, 8, 9);
    m7 = std::move(m6);
    EXPECT_EQ(m7, m1);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(Msg, Equals)
{
    Msg m1(1, 2, 3);
    m1.SetData({0x31, 0x32, 0x33});
    auto stub = OHOS::sptr<ApduChannelStub>(new ApduChannelStub());
    m1.SetSource(RemoteObjectPool::GetRemoteObject(stub->AsObject()));

    EXPECT_EQ(m1, m1);
    EXPECT_TRUE(m1.operator==(m1));

    Msg m2(1, 2, 3);
    m2.SetData({0x31, 0x32, 0x33});
    m2.SetSource(RemoteObjectPool::GetRemoteObject(stub->AsObject()));

    EXPECT_EQ(m2, m1);
    EXPECT_TRUE(m2.operator==(m1));

    Msg m3(1, 2, 3);
    m3.SetData({0x41, 0x42, 0x43});
    m3.SetSource(RemoteObjectPool::GetRemoteObject(stub->AsObject()));

    EXPECT_FALSE(m3 == m1);
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(Msg, setget)
{
    Msg msg(1, 2, 3);
    EXPECT_EQ(msg.Id(), 1);
    EXPECT_EQ(msg.Arg1(), 2);
    EXPECT_EQ(msg.Arg2(), 3);
    EXPECT_TRUE(msg.GetData().empty());
    EXPECT_FALSE(msg.GetSource());

    std::vector<uint8_t> data{0x31, 0x32, 0x33};
    msg.SetData(data);
    auto d = msg.GetData();
    ASSERT_FALSE(d.empty());
    EXPECT_EQ(memcmp(&d[0], &data[0], d.size() * sizeof(d[0])), 0);
}

/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(Msg, withRemoteObject)
{
    Msg msg(1, 2, 3);

    std::vector<uint8_t> data{0x31, 0x32, 0x33};
    msg.SetData(data);
    auto stub = OHOS::sptr<ApduChannelStub>(new ApduChannelStub());
    msg.SetSource(RemoteObjectPool::GetRemoteObject(stub->AsObject()));
    auto proxy = msg.GetSource();

    MsgHandler mh(std::make_unique<Msg>(msg));
    stub->SetHandler(mh);
    proxy->Send(std::make_unique<Msg>(msg));
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(Msg, EqualsReplyRemoteObject)
{
    Msg msg(1, 2, 3);
    auto stub = OHOS::sptr<ApduChannelStub>(new ApduChannelStub());

    msg.SetSource(RemoteObjectPool::GetRemoteObject(stub->AsObject()));

    EXPECT_TRUE(msg.EqualsReplyRemoteObject(RemoteObjectPool::GetRemoteObject(stub->AsObject())));

    {
        auto stub0 = OHOS::sptr<ApduChannelStub>(new ApduChannelStub());
        auto ro = RemoteObjectPool::GetRemoteObject(stub0->AsObject());

        bool b = msg.EqualsReplyRemoteObject(ro);
        EXPECT_FALSE(b);
    }
}
/**
* @tc.number:
* @tc.name  :
* @tc.desc  :
*/
TEST(Msg, Marshalling)
{
    Msg msg(1, 2, 3);

    std::vector<uint8_t> data{0x31, 0x32, 0x33};
    msg.SetData(data);
    auto stub = OHOS::sptr<ApduChannelStub>(new ApduChannelStub());

    msg.SetSource(RemoteObjectPool::GetRemoteObject(stub->AsObject()));

    OHOS::Parcel parcel;

    msg.Marshalling(parcel);

    Msg msg1(0, 0, 0);

    msg1.ReadFromParcel(parcel);

    EXPECT_EQ(msg1.Id(), 1);
    EXPECT_EQ(msg1.Arg1(), 2);
    EXPECT_EQ(msg1.Arg2(), 3);
    auto d = msg1.GetData();

    EXPECT_EQ(memcmp(&d[0], &data[0], d.size()), 0);
    EXPECT_TRUE(msg1.GetSource());
}
}  // namespace OHOS::nfc::sdk::test