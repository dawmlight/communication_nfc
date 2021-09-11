#include "test_nfc_mifareclassic.h"

#include <string.h>

#include <string>
#include <thread>

#include "loghelper.h"
#include "mifare_classic_tag.h"
#include "nfc_sdk_common.h"
#include "test_nfc_reader_app.h"
#include "test_read_mode_tag_callback.h"
#include "test_nfc_ability.h"

using namespace  std;
namespace OHOS {
using OHOS::nfc::sdk::MifareClassicTag;
using OHOS::nfc::sdk::Tag;
namespace nfc {
namespace test {
bool TestNfcMifareClassic::StartTest()
{
    TurnOnNfc();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    StartAbility();
    ActiveAbility();

    int command;
    while (true) {
        printf(
            "The MifareClassic Test Options:\n"
            "    30 - Read MifareClassic\n"
            "    31 - Write MifareClassic\n"
            "    32 - Increment MifareClassic\n"
            "    33 - Decrement MifareClassic\n"
            "   <= 0 - Exit\n");
        std::cin >> command;
        if (command <= 0) {
            printf("Test end\n");
            break;
        }
        try {
            switch (command) {
                case 30:
                    InfoLog("Read MifareClassic.");
                    TestReadMifareClassicTag();
                    break;
                case 31:
                    InfoLog("Write MifareClassic.");
                    TestWriteMifareClassicTag();
                    break;
                case 32:
                    InfoLog("Increment MifareClassic.");
                    TestIncrementMifareClassicTag();
                    break;
                case 33:
                    InfoLog("Decrement MifareClassic.");
                    TestDecrementMifareClassicTag();
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
std::shared_ptr<MifareClassicTag> TestNfcMifareClassic::ReadWriteMifareClassicTagSetUp()
{
    if (!callback_) {
        return std::shared_ptr<MifareClassicTag>();
    }
    DebugLog("MifareClassicTag GetTag");
    std::shared_ptr<Tag> tag = callback_->GetTag();
    std::shared_ptr<MifareClassicTag> mifareClassicTag = MifareClassicTag::GetTag(tag);
    if (!mifareClassicTag) {
        DebugLog("MifareClassicTag is null");
        return std::shared_ptr<MifareClassicTag>();
    }

    DebugLog("MifareClassicTag Connect");
    int ret = mifareClassicTag->Connect();
    if (ret != sdk::NfcErrorCode::NFC_SUCCESS) {
        DebugLog("connect MifareClassicTag result = %d", ret);
        return std::shared_ptr<MifareClassicTag>();
    }
    return mifareClassicTag;
}

bool TestNfcMifareClassic::TestMifareClassicTagDiscovered()
{
    DebugLog("MifareClassic tag discovered");
    return true;
}

void TestNfcMifareClassic::TestReadMifareClassicTag()
{
    DebugLog("Read MifareClassicTag ");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        }

        // 读tag准备
        std::shared_ptr<MifareClassicTag> mifareClassicTag = ReadWriteMifareClassicTagSetUp();

        if (!mifareClassicTag) {
            ErrorLog("TestNfcMifareClassic::TestReadMifareClassicTag get mifareClassicTag err");
            return;
        }

        // auth sector
        int sectorIndex = 0x00;
        bool bIsKeyA = true;
        DebugLog("Auth sector.%d with keyA", sectorIndex);
        char keyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        string key(keyA, sizeof(keyA));
        bool authResult = mifareClassicTag->AuthenticateSector(sectorIndex, key, bIsKeyA);
        
        if (!authResult)
        {
            ErrorLog("TestNfcMifareClassic::TestReadMifareClassicTag auth sector.%d err", sectorIndex);
            return;
        }
        
        // int blockTotelNum = mifareClassicTag->GetBlockCountInSector(sectorIndex);
        int blockIndexStart = mifareClassicTag->GetBlockIdexFromSector(sectorIndex);

        // read block                
        DebugLog("read block.%d ", blockIndexStart);
        std::string readData = mifareClassicTag->ReadSingleBlock(blockIndexStart);
        DebugLog("MifareClassicTag->ReadSingleBlock:%s", readData.c_str());
        return;
    }
    return;
}

void TestNfcMifareClassic::TestWriteMifareClassicTag()
{
    DebugLog("Write MifareClassicTag");

    // 读tag准备
    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        }
        std::shared_ptr<MifareClassicTag> mifareClassicTag = ReadWriteMifareClassicTagSetUp();
        if (mifareClassicTag) {
            // auth sector
            int sectorIndex = 0x00;
            bool bIsKeyA = true;
            DebugLog("Auth sector.%d with keyA", sectorIndex);
            char keyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            string key(keyA, sizeof(keyA));
            bool authResult = mifareClassicTag->AuthenticateSector(sectorIndex, key, bIsKeyA);
            
            if (!authResult)
            {
                ErrorLog("TestNfcMifareClassic::TestWriteMifareClassicTag auth sector.%d err", sectorIndex);
                return;
            }

            // int blockTotelNum = mifareClassicTag->GetBlockCountInSector(sectorIndex);
            int blockIndexStart = mifareClassicTag->GetBlockIdexFromSector(sectorIndex);

            // read block                
            DebugLog("write block.%d ", blockIndexStart);

            char writeData[16] = {
                0x46, 0x4D, 0x53, 0x48, 0x46, 0x4D, 0x53, 0x48, 0x46, 0x4D, 0x53, 0x48, 0x46, 0x4D, 0x53, 0x48};
            string dataString(writeData, 16);
            mifareClassicTag->WriteSingleBlock(blockIndexStart, dataString);

            DebugLog("After write, blockIndex info:");
            std::string readData = mifareClassicTag->ReadSingleBlock(blockIndexStart);
            printf("MifareClassicTag->ReadSingleBlock:%s", readData.c_str());
            return;
        } else {
            DebugLog("tag get null");
            return;
        }
    }
}


void TestNfcMifareClassic::TestDecrementMifareClassicTag()
{
    DebugLog("Decrement Block MifareClassicTag ");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        } else {
            // 读tag准备
            std::shared_ptr<MifareClassicTag> mifareClassicTag = ReadWriteMifareClassicTagSetUp();

            if (mifareClassicTag) {
                // auth sector
                int sectorIndex = 6;
                int blockIndex = mifareClassicTag->GetBlockIdexFromSector(sectorIndex) + 1;
                DebugLog("before decrementBlock.%d ",  blockIndex);
                int value = ReadWalletValue(mifareClassicTag, sectorIndex, blockIndex, true);

                int decrementValue = 30;
                // decrease
                mifareClassicTag->DecrementBlock(blockIndex, decrementValue);
                mifareClassicTag->TransferToBlock(blockIndex);

                int valueAfter = ReadWalletValue(mifareClassicTag, sectorIndex, blockIndex, false);
                if (((value - valueAfter)) == (decrementValue)) {
                    DebugLog("decrease test success");
                }
                else
                {
                    DebugLog("decrease test fail");
                }
            }
            return;
        }
    }
    return;
}

void TestNfcMifareClassic::TestIncrementMifareClassicTag()
{
    DebugLog("Increment Block MifareClassicTag ");

    bool waitForTag = true;
    while (waitForTag) {
        if (!(callback_->GetTag())) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            // DebugLog("no tag discovered!!!");
            continue;
        } else {
            // 读tag准备
            std::shared_ptr<MifareClassicTag> mifareClassicTag = ReadWriteMifareClassicTagSetUp();

            if (mifareClassicTag) {
                // auth sector
                int sectorIndex = 6;
                int blockIndex = mifareClassicTag->GetBlockIdexFromSector(sectorIndex) + 1;
                DebugLog("before decrementBlock.%d ",  blockIndex);
                int value = ReadWalletValue(mifareClassicTag, sectorIndex, blockIndex, true);

                int increaceValue = 50;
                // increace
                mifareClassicTag->IncrementBlock(blockIndex, increaceValue);
                mifareClassicTag->TransferToBlock(blockIndex);

                int valueAfter = ReadWalletValue(mifareClassicTag, sectorIndex, blockIndex, false);
                if (((valueAfter - value)) == (increaceValue)) {
                    DebugLog("increace test success");
                }
                else
                {
                    DebugLog("increace test fail");
                }
            }
            return;
        }
    }
    return;
}


int TestNfcMifareClassic::ReadWalletValue(
    std::shared_ptr<nfc::sdk::MifareClassicTag> mifareClassicTag,
    int sectorIndex,
    int blockIndex,
    bool bAuth)
{
    DebugLog("ReadWalletValue in");
    bool authResult = true;
    if (bAuth) {
        char keyA[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        string key(keyA, sizeof(keyA));
        bool bIsKeyA = true;
        authResult = mifareClassicTag->AuthenticateSector(sectorIndex, key, bIsKeyA);
    }
    if (authResult) {
        string data = mifareClassicTag->ReadSingleBlock(blockIndex);
        // 将readData值转为钱包块value
        char value[4];
        int res = -1;
        if (data.size() == 18 && data.at(0) == 16) {
            string readData = data.substr(1, 16);
            if ((readData.at(12) == readData.at(14)) &&
                (readData.at(13) == readData.at(15)))  //~address
            {
                for (size_t ilen = 0; ilen < 4; ilen++) {
                    value[ilen] = readData.at(ilen);
                }
                res = value[0] + (int(value[1]) << 8) + (int(value[2]) << 16) +
                      (int(value[3]) << 24);
            }
        }

        return res;
    }
    return -1;
}
}  // namespace test
}  // namespace nfc
}  // namespace OHOS