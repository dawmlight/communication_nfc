#include "test_nfc_mifareultralight.h"

#include <string.h>

#include <string>
#include <thread>

#include "loghelper.h"
#include "mifare_ultralight_tag.h"
#include "nfc_sdk_common.h"
#include "test_nfc_reader_app.h"
#include "test_read_mode_tag_callback.h"
#include "test_nfc_ability.h"

namespace OHOS {
using OHOS::nfc::sdk::MifareUltralightTag;
using OHOS::nfc::sdk::Tag;
namespace nfc {
namespace test {
    
bool TestNfcMifareUltralight::StartTest()
{
    DebugLog("start MifareUltralight test");
    TurnOnNfc();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    StartAbility();
    ActiveAbility();

    int command;
    while (true) {
        printf(
            "The MifareUltralight Test Options:\n"
            "    10 - Read MifareUltralight 4 pages[05-08]\n"
            "    11 - Write MifareUltralight 4 pages[05]\n"
            "   <= 0 - Exit\n");
        std::cin >> command;
        if (command <= 0) {
            printf("Test end\n");
            break;
        }
        try {
            switch (command) {
                case 10:
                    InfoLog("Read MifareUltralight Tag 4 Pages.");
                    TestReadMifareUltralightTag4Pages();
                    break;
                case 11:
                    InfoLog("Write MifareUltralight Tag Page.");
                    TestWriteMifareUltralightTagSinglePage();
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

// MifareUltralight ReadMode读写tag， tag发现之后，业务读写之前，正常业务调用的
std::shared_ptr<MifareUltralightTag> TestNfcMifareUltralight::ReadWriteMifareUltralightTagSetUp()
{
    if (!callback_) {
        return std::shared_ptr<MifareUltralightTag>();
    }
    DebugLog("mifareUltralightTag GetTag");
    std::shared_ptr<Tag> tag = callback_->GetTag();
    std::shared_ptr<MifareUltralightTag> mifareUltralightTag = MifareUltralightTag::GetTag(tag);
    if (!mifareUltralightTag) {
        DebugLog("mifareUltralightTag is null");
        return std::shared_ptr<MifareUltralightTag>();
    }

    DebugLog("mifareUltralightTag Connect");
    int ret = mifareUltralightTag->Connect();
    if (ret != sdk::NfcErrorCode::NFC_SUCCESS) {
        DebugLog("connect mifareUltralightTag result = %d", ret);
        return std::shared_ptr<MifareUltralightTag>();
    }
    return mifareUltralightTag;
}

bool TestNfcMifareUltralight::TestMifareUltralightTagDiscovered()
{
    DebugLog("MifareUltralight tag discovered");
    return true;
}

void TestNfcMifareUltralight::TestReadMifareUltralightTag4Pages()
{
    DebugLog("Read MifareUltralight 4 pages");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        } else {
            // 读tag准备
            std::shared_ptr<MifareUltralightTag> mifareUltralightTag = ReadWriteMifareUltralightTagSetUp();

            if (mifareUltralightTag) {
                char pageIndex = 5;
                std::string readData = mifareUltralightTag->ReadMultiplePages(pageIndex);
                DebugLog("MifareUltralightTag->TestReadMifareUltralightTag4Pages:%s", readData.c_str());
            }
            return;
        }
    }
    return;
}

void TestNfcMifareUltralight::TestWriteMifareUltralightTagSinglePage()
{
    DebugLog("Write MifareUltralight 1 page");

    // 读tag准备
    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        }
        std::shared_ptr<MifareUltralightTag> mifareUltralightTag = ReadWriteMifareUltralightTagSetUp();
        if (mifareUltralightTag) {
            char pageIndex = 5;
            char writeData[] = {0x46, 0x4D, 0x53, 0x48};
            std::string data(writeData, nfc::sdk::MifareUltralightTag::MU_PAGE_SIZE);
            mifareUltralightTag->WriteSinglePages(pageIndex, data);

            DebugLog("After write, pageinfo:");
            std::string readData = mifareUltralightTag->ReadMultiplePages(pageIndex);
            printf("MifareUltralightTag->TestWriteMifareUltralightTagSinglePage:%s", readData.c_str());
            return;
        } else {
            DebugLog("tag get null");
            return;
        }
    }
}
}  // namespace test
}  // namespace nfc
}  // namespace OHOS