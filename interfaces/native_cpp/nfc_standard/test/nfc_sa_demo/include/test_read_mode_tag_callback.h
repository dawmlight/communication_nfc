#ifndef TEST_READER_MODE_TAG_CALLBACK_H
#define TEST_READER_MODE_TAG_CALLBACK_H

#include <memory>
#include <string>
#include <vector>

#include "loghelper.h"
#include "nfc_agent.h"
#include "tag.h"

namespace OHOS {
namespace nfc {
namespace test {
class TestReadModeTagCallBack : public sdk::NfcAgent::TagCallback {
public:
    TestReadModeTagCallBack() {}
    ~TestReadModeTagCallBack() {}

    virtual void CBTagDiscoverCommond(std::unique_ptr<sdk::Tag> tag) override
    {
        DebugLog("[TestReadModeTagCallBack::CBTagDiscoverCommond] tag tech len.%d", tag->GetTagTechnologyList().size());
        // std::unique_ptr<sdk::Tag> tagUniq = std::move(tag);
        // mTag_ = std::shared_ptr<sdk::Tag>(tagUniq.release());
        mTag_ = std::move(tag);
        InfoLog("CBTagDiscoverCommond tagtmp tech mTag_ len.%d", mTag_->GetTagTechnologyList().size());
    }

    virtual void CBTagRemovedMsg() override
    {
        DebugLog("[TestReadModeTagCallBack::CBTagRemovedMsg]");
    }

    void SetTag(std::shared_ptr<sdk::Tag> tag)
    {
        mTag_ = tag;
    }

    std::shared_ptr<sdk::Tag> GetTag()
    {
        return mTag_;
    }

    void RemoveTag()
    {
        mTag_ = std::shared_ptr<sdk::Tag>();
    }

private:
    std::shared_ptr<sdk::Tag> mTag_;
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* TEST_READER_MODE_TAG_CALLBACK_H */
