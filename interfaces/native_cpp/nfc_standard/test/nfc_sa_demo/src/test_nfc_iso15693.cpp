#include "test_nfc_iso15693.h"
#include <string.h>

#include <string>
#include <thread>

#include "loghelper.h"
#include "iso15693_tag.h"
#include "nfc_sdk_common.h"
#include "test_nfc_reader_app.h"
#include "test_read_mode_tag_callback.h"
#include "test_nfc_ability.h"

using namespace  std;
namespace OHOS {
using OHOS::nfc::sdk::Iso15693Tag;
using OHOS::nfc::sdk::Tag;
namespace nfc {
namespace test {
bool TestNfcIso15693Tag::StartTest()
{
    TurnOnNfc();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    StartAbility();
    ActiveAbility();

    int command;
    while (true) {
        printf(
            "The MifareClassic Test Options:\n"
            "    40 - Read Iso15693Tag single block\n"
            "    41 - Write Iso15693Tag single block\n"
            "    42 - Read Iso15693Tag multi block\n"
            "    43 - Write Iso15693Tag multi block\n"
            "    44 - Lock Iso15693Tag single block\n"
            "   <= 0 - Exit\n");
        std::cin >> command;
        if (command <= 0) {
            printf("Test end\n");
            break;
        }
        try {
            switch (command) {
                case 40:
                    InfoLog("Read Iso15693Tag single block.");
                    TestReadIso15693TagSingleBlock();
                    break;
                case 41:
                    InfoLog("Write Iso15693Tag single block.");
                    TestWriteIso15693TagSingleBlock();
                    break;
                case 42:
                    InfoLog("Read Iso15693Tag multi block.");
                    TestReadIso15693TagMultiBlock();
                    break;
                case 43:
                    InfoLog("Write Iso15693Tag multi block.");
                    TestWriteIso15693TagMultiBlock();
                    break;
                case 44:
                    InfoLog("Lock Iso15693Tag single block.");
                    TestLockIso15693TagSingleBlock();
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
// Iso15693Tag ReadMode读写tag， tag发现之后，业务读写之前，正常业务调用的
std::shared_ptr<Iso15693Tag> TestNfcIso15693Tag::ReadWriteIso15693TagSetUp()
{
    if (!callback_) {
        return std::shared_ptr<Iso15693Tag>();
    }
    DebugLog("Iso15693Tag GetTag");
    std::shared_ptr<Tag> tag = callback_->GetTag();
    std::shared_ptr<Iso15693Tag> iso15693Tag = Iso15693Tag::GetTag(tag);
    if (!iso15693Tag) {
        DebugLog("Iso15693Tag is null");
        return std::shared_ptr<Iso15693Tag>();
    }

    DebugLog("Iso15693Tag Connect");
    int ret = iso15693Tag->Connect();
    if (ret != sdk::NfcErrorCode::NFC_SUCCESS) {
        DebugLog("connect Iso15693Tag result = %d", ret);
        return std::shared_ptr<Iso15693Tag>();
    }
    return iso15693Tag;
}

bool TestNfcIso15693Tag::TestIso15693TagDiscovered()
{
    DebugLog("Iso15693Tag discovered");
    return true;
}

void TestNfcIso15693Tag::TestReadIso15693TagSingleBlock()
{
    DebugLog("TestReadIso15693TagSingleBlock Iso15693Tag ");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        }

        // 读tag准备
        std::shared_ptr<Iso15693Tag> iso15693Tag = ReadWriteIso15693TagSetUp();

        if (!iso15693Tag) {
            ErrorLog("TestNfcIso15693Tag::TestReadIso15693TagSingleBlock get Iso15693Tag err");
            return;
        }

        int flag = 0x22;
        int blockIndex = 2;
        string readData = iso15693Tag->ReadSingleBlock(flag, blockIndex);
        DebugLog("TestNfcIso15693Tag::TestReadIso15693TagSingleBlock read data.%s", readData.c_str());
        return;
    }
    return;
}

void TestNfcIso15693Tag::TestWriteIso15693TagSingleBlock()
{
    DebugLog("TestWriteIso15693TagSingleBlock Iso15693Tag");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        }

        // 读tag准备
        std::shared_ptr<Iso15693Tag> iso15693Tag = ReadWriteIso15693TagSetUp();

        if (!iso15693Tag) {
            ErrorLog("TestNfcIso15693Tag::TestWriteIso15693TagSingleBlock get Iso15693Tag err");
            return;
        }

        int flag = 0x22;
        int blockIndex = 2;
        
        char writeData[4] = { 0x46, 0x4D, 0x53, 0x48 };
        string data(writeData, 4);
        iso15693Tag->WriteSingleBlock(flag, blockIndex, data);
        
        string readData = iso15693Tag->ReadSingleBlock(flag, blockIndex);
        DebugLog("TestNfcIso15693Tag::TestWriteIso15693TagSingleBlock read data.%s", readData.c_str());
        if (readData == data)
        {
            DebugLog("TestNfcIso15693Tag::TestWriteIso15693TagSingleBlock write success");
            return;
        }
        DebugLog("TestNfcIso15693Tag::TestWriteIso15693TagSingleBlock write fail");
        return;
    }
    return;
}

void TestNfcIso15693Tag::TestReadIso15693TagMultiBlock()
{
    DebugLog("TestReadIso15693TagMultiBlock Iso15693Tag ");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        }

        // 读tag准备
        std::shared_ptr<Iso15693Tag> iso15693Tag = ReadWriteIso15693TagSetUp();

        if (!iso15693Tag) {
            ErrorLog("TestNfcIso15693Tag::TestReadIso15693TagMultiBlock get Iso15693Tag err");
            return;
        }

        int flag = 0x22;
        int blockIndex = 2;
        int blockNum = 3;
        string readData = iso15693Tag->ReadMultipleBlock(flag, blockIndex, blockNum);
        DebugLog("TestNfcIso15693Tag::TestReadIso15693TagMultiBlock read data.%s", readData.c_str());
        return;
    }
    return;
}

void TestNfcIso15693Tag::TestWriteIso15693TagMultiBlock()
{
    DebugLog("TestWriteIso15693TagMultiBlock Iso15693Tag");
    return;
}

void TestNfcIso15693Tag::TestLockIso15693TagSingleBlock()
{
    DebugLog("TestLockIso15693TagSingleBlock Iso15693Tag");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        }

        // 读tag准备
        std::shared_ptr<Iso15693Tag> iso15693Tag = ReadWriteIso15693TagSetUp();

        if (!iso15693Tag) {
            ErrorLog("TestNfcIso15693Tag::TestLockIso15693TagSingleBlock get Iso15693Tag err");
            return;
        }

        int flag = 0x22;
        int blockIndex = 0x12;
        
        char writeData[4] = { 0x46, 0x4D, 0x53, 0x48 };
        string data(writeData, 4);
        iso15693Tag->WriteSingleBlock(flag, blockIndex, data);
        
        string readData = iso15693Tag->ReadSingleBlock(flag, blockIndex);
        DebugLog("TestNfcIso15693Tag::TestLockIso15693TagSingleBlock read data.%s", readData.c_str());
        if (readData == data)
        {
            DebugLog("TestNfcIso15693Tag::TestLockIso15693TagSingleBlock write success");
            
            iso15693Tag->LockSingleBlock(flag, blockIndex);
            char writeData1[4] = { 0x48, 0x53, 0x4D, 0x46 };
            string data1(writeData1, 4);
            iso15693Tag->WriteSingleBlock(flag, blockIndex, data);
            
            string readData1 = iso15693Tag->ReadSingleBlock(flag, blockIndex);
            DebugLog("TestNfcIso15693Tag::TestLockIso15693TagSingleBlock read data.%s", readData.c_str());
            if (readData1 == data1)
            {
                DebugLog("TestNfcIso15693Tag::TestLockIso15693TagSingleBlock fail");
                return;
            }
            DebugLog("TestNfcIso15693Tag::TestLockIso15693TagSingleBlock success");
            return;
        }
        DebugLog("TestNfcIso15693Tag::TestLockIso15693TagSingleBlock write fail");

        return;
    }
    return;
}

}  // namespace test
}  // namespace nfc
}  // namespace OHOS