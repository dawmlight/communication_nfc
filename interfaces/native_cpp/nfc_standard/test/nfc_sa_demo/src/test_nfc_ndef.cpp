#include "test_nfc_ndef.h"

#include <string.h>

#include <string>
#include <thread>

#include "loghelper.h"
#include "ndef_tag.h"
#include "nfc_sdk_common.h"
#include "test_nfc_reader_app.h"
#include "test_read_mode_tag_callback.h"
#include "test_nfc_ability.h"
#include "ndef_message.h"

using namespace std;
namespace OHOS {
using OHOS::nfc::sdk::NdefMessage;
using OHOS::nfc::sdk::NdefRecord;
using OHOS::nfc::sdk::NdefTag;
using OHOS::nfc::sdk::Tag;

namespace nfc {
namespace test {
bool TestNfcNdefTag::StartTest()
{
    DebugLog("start IsoDep test");
    TurnOnNfc();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    StartAbility();
    ActiveAbility();

    int command;
    while (true) {
        printf(
            "The Ndef Test Options:\n"
            "    50 - Test Write Ndef Tag With Uri\n"
            "   <= 0 - Exit\n");
        std::cin >> command;
        if (command <= 0) {
            printf("Test end\n");
            break;
        }
        try {
            switch (command) {
                case 50:
                    InfoLog("Write Ndef Tag With Uri.");
                    TestWriteNdefTagWithUri();
                    break;
                default:
                    printf("Unknown command\n");
                    break;
            }
        } catch (std::exception& e) {
            printf("%s\n", e.what());
            return 0;
        }
        // RemoveTag();
    }    
    return true;
}
// NdefTag ReadMode读写tag， tag发现之后，业务读写之前，正常业务调用的
std::shared_ptr<NdefTag> TestNfcNdefTag::ReadWriteNdefTagSetUp()
{
    if (!callback_) {
        return std::shared_ptr<NdefTag>();
    }
    DebugLog("NdefTag GetTag");
    std::shared_ptr<Tag> tag = callback_->GetTag();
    std::shared_ptr<NdefTag> ndefTag = NdefTag::GetTag(tag);
    if (!ndefTag) {
        DebugLog("NdefTag is null");
        return std::shared_ptr<NdefTag>();
    }

    DebugLog("NdefTag Connect");
    int ret = ndefTag->Connect();
    if (ret != sdk::NfcErrorCode::NFC_SUCCESS) {
        DebugLog("connect NdefTag result = %d", ret);
        return std::shared_ptr<NdefTag>();
    }
    return ndefTag;
}

bool TestNfcNdefTag::TestNdefTagDiscovered()
{
    DebugLog("NdefTag tag discovered");
    return true;
}

void TestNfcNdefTag::TestWriteNdefTagWithUri()
{
    DebugLog("Write Ndef Tag With Uri");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        } 
        
        std::shared_ptr<NdefTag> ndefTag = ReadWriteNdefTagSetUp();
        if (!ndefTag) {            
            DebugLog("tag get null");
            return;
        }

        int maxSendCommandLength = ndefTag->GetMaxSendCommandLength();
        std::shared_ptr<NdefRecord> record =
            NdefMessage::MakeUriRecord("http://ascii.911cha.com/");
        std::vector<std::shared_ptr<NdefRecord>> recordList;
        recordList.push_back(record);
        std::shared_ptr<NdefMessage> newNdefMessage =
            NdefMessage::GetNdefMessage(recordList);

        DebugLog("maxSendCommandLength.%d ndefmessage.%s", maxSendCommandLength, NdefMessage::MessageToString(newNdefMessage).c_str());
        if (ndefTag->IsNdefWritable()) {
            ndefTag->WriteNdef(newNdefMessage);
        } else {
            DebugLog("Test Write Ndef Tag. Tag can not be write.");
            return;
        }

        // 读标签，校验写入的是否正确
        newNdefMessage = ndefTag->ReadNdef();
        if (!newNdefMessage) {
            DebugLog("[TestNfcNdef::TestWriteNdefTagWithUriRecord] read ndef null");
            return;
        }
        recordList = newNdefMessage->GetNdefRecords();
        DebugLog("Test Write Ndef Tag, records num %d info: ", recordList.size());
        for (size_t i = 0; i < recordList.size(); i++) {
            std::shared_ptr<NdefRecord> record = recordList.at(i);
            if (!record) {
                continue;
            }
            DebugLog("NdefRecord %d : Tnf %d Id(%s) mPayload_(%s) mTagRtdType_(%s)", 
            i, record->mTnf_, record->mId_.c_str(), record->mPayload_.c_str(), record->mTagRtdType_.c_str());
        }
        return;
    }
    return;
}

}  // namespace test
}  // namespace nfc
}  // namespace OHOS