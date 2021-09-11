#include "test_nfc_isodep.h"

#include <string.h>

#include <string>
#include <thread>

#include "loghelper.h"
#include "isodep_tag.h"
#include "nfc_sdk_common.h"
#include "test_nfc_reader_app.h"
#include "test_read_mode_tag_callback.h"
#include "test_nfc_ability.h"
#include "itag_session.h"

using namespace std;
namespace OHOS {
using OHOS::nfc::reader::ResResult;
using OHOS::nfc::sdk::IsoDepTag;
using OHOS::nfc::sdk::Tag;

namespace nfc {
namespace test {
bool TestNfcIsoDep::StartTest()
{
    DebugLog("start IsoDep test");
    TurnOnNfc();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    StartAbility();
    ActiveAbility();

    int command;
    while (true) {
        printf(
            "The IsoDep Test Options:\n"
            "    20 - Send command to isodep tag\n"
            "   <= 0 - Exit\n");
        std::cin >> command;
        if (command <= 0) {
            printf("Test end\n");
            break;
        }
        try {
            switch (command) {
                case 20:
                    InfoLog("Isodep send command.");
                    TestIsoDepTagSendCommand();
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
// IsoDepTag ReadMode读写tag， tag发现之后，业务读写之前，正常业务调用的
std::shared_ptr<IsoDepTag> TestNfcIsoDep::ReadWriteIsoDepTagSetUp()
{
    if (!callback_) {
        return std::shared_ptr<IsoDepTag>();
    }
    DebugLog("IsoDepTag GetTag");
    std::shared_ptr<Tag> tag = callback_->GetTag();
    std::shared_ptr<IsoDepTag> isoDepTag = IsoDepTag::GetTag(tag);
    if (!isoDepTag) {
        DebugLog("IsoDepTag is null");
        return std::shared_ptr<IsoDepTag>();
    }

    DebugLog("IsoDepTag Connect");
    int ret = isoDepTag->Connect();
    if (ret != sdk::NfcErrorCode::NFC_SUCCESS) {
        DebugLog("connect IsoDepTag result = %d", ret);
        return std::shared_ptr<IsoDepTag>();
    }
    return isoDepTag;
}

bool TestNfcIsoDep::TestIsoDepTagDiscovered()
{
    DebugLog("TestIsoDep tag discovered");
    return true;
}

void TestNfcIsoDep::TestIsoDepTagSendCommand()
{
    DebugLog("send command to IsoDep tag");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        } else {
            // 读tag准备
            std::shared_ptr<IsoDepTag> isoDepTag = ReadWriteIsoDepTagSetUp();

            if (isoDepTag) {                
                char readCommand[] = { 0x00, 0xA4, 0x04, 0x00, 0x00 };
                string command(readCommand, 5);

                int response = ResResult::ResponseResult::RESULT_FAILURE;
                string readData = isoDepTag->SendCommand(command, false, response);
                DebugLog("isoDep->TestIsoDepTagSendCommand:%s", readData.c_str());
            }
            return;
        }
    }
    return;
}

}  // namespace test
}  // namespace nfc
}  // namespace OHOS