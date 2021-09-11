#ifndef I_SECURE_ELEMENT_H
#define I_SECURE_ELEMENT_H

#include <memory>
#include <string>

#include "iremote_broker.h"

namespace OHOS {
namespace hardware {
namespace se {
namespace v1_0 {
enum SecureElementStatus : int32_t {
    SUCCESS = 0,
    FAILED,
    CHANNEL_NOT_AVAILABLE,
    NO_SUCH_SE_ERROR,
    UNSUPPORTED_OPERATION,
    IO_ERROR
};

class ISecureElementCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.hardware.se.v1.0.ISecureElementCallback");

    virtual ~ISecureElementCallback() {}
    // SE State Change
    virtual void OnStateChange(bool state, std::string reason) = 0;
};

class ISecureElement : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.hardware.se.v1.0.ISecureElement");

    virtual ~ISecureElement() {}
    struct BasicRespData : public Parcelable {
        std::string resp{};
        SecureElementStatus status{FAILED};
        inline bool Marshalling(OHOS::Parcel& parcel) const override
        {
            parcel.WriteInt32(status);
            parcel.WriteString(resp);
            return true;
        }
        static BasicRespData* Unmarshalling(OHOS::Parcel& parcel)
        {
            BasicRespData* res = new BasicRespData();
            res->status = static_cast<SecureElementStatus>(parcel.ReadInt32());
            res->resp = parcel.ReadString();
            return res;
        }
    };
    struct LogicalRespData : public BasicRespData {
        uint8_t channelNumber{0};
        inline bool Marshalling(OHOS::Parcel& parcel) const override
        {
            BasicRespData::Marshalling(parcel);
            parcel.WriteUint8(channelNumber);
            return true;
        }
        static LogicalRespData* Unmarshalling(OHOS::Parcel& parcel)
        {
            LogicalRespData* res = new LogicalRespData();
            res->status = static_cast<SecureElementStatus>(parcel.ReadInt32());
            res->resp = parcel.ReadString();
            res->channelNumber = parcel.ReadInt32();
            return res;
        }
    };
    virtual void Initialize(const sptr<IRemoteObject>& callback) = 0;
    virtual std::string GetAtr() = 0;
    virtual bool IsCardPresent() = 0;
    virtual std::string Transmit(const std::string& cmd) = 0;
    virtual std::unique_ptr<BasicRespData> OpenBasicChannel(const std::string& aid, char p2) = 0;
    virtual std::unique_ptr<LogicalRespData> OpenLogicalChannel(const std::string& aid, char p2) = 0;
    virtual SecureElementStatus CloseChannel(uint8_t channelNumber) = 0;
    virtual SecureElementStatus Reset() = 0;

    static sptr<ISecureElement> GetService(const std::string& name,
                                           const sptr<IRemoteObject::DeathRecipient>& recipient);
};
}  // namespace v1_0
}  // namespace se
}  // namespace hardware
}  // namespace OHOS

namespace {
using SEStatus = OHOS::hardware::se::v1_0::SecureElementStatus;
using SEBasicRespData = OHOS::hardware::se::v1_0::ISecureElement::BasicRespData;
using SELogicalRespData = OHOS::hardware::se::v1_0::ISecureElement::LogicalRespData;
using SECallback = OHOS::hardware::se::v1_0::ISecureElementCallback;
}  // namespace

#endif  // !I_SECURE_ELEMENT_H
