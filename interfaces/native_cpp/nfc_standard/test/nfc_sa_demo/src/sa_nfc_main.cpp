#include <iostream>
#include <memory>
#include <thread>

#include "common_event_manager.h"
#include "loghelper.h"
#include "nfc_test_env.h"
#include "test_nfc_iso15693.h"
#include "test_nfc_isodep.h"
#include "test_nfc_mifareclassic.h"
#include "test_nfc_mifareultralight.h"
#include "test_nfc_ndef.h"
#include "test_nfc_reader.h"

using namespace OHOS::nfc::test;
using OHOS::AAFwk::Want;
using OHOS::EventFwk::CommonEventData;
using OHOS::EventFwk::CommonEventManager;

int main(int argc, char* argv[])
{
    std::shared_ptr<NfcTestEnv> nfcTestEnv = std::make_shared<NfcTestEnv>();
    nfcTestEnv->Initialize();

    int command;
    while (true) {
        printf(
            "The NFC Test Options:\n"
            "    1 - Init Nfc test\n"
            "    2 - Turn on/off Nfc test\n"
            "    3 - Set Reader Mode Test\n"
            "    4 - MifareUltralight Tag Test\n"
            "    5 - MifareClassic Tag Test\n"
            "    6 - IsoDep Tag Test\n"
            "    7 - Iso15693 Tag Test\n"
            "    8 - Ndef Tag Test\n"
            "    9 - Ignore Tag Test\n"

            "    1101 - Send Screen On Common Event\n"
            "   <= 0 - Exit\n");
        std::cin >> command;
        if (command <= 0) {
            printf("Test end\n");
            break;
        }
        try {
            switch (command) {
                case 1:
                    printf("Init Nfc test\n");
                    break;
                case 2:
                    printf("Turn on/off Nfc test\n");
                    break;
                case 3:
                    printf("Set Reader Mode Test.");
                    break;
                case 4: {
                    printf("MifareUltralight Tag Test.");
                    std::shared_ptr<TestNfcMifareUltralight> testNfcMifareUltralight =
                        std::make_shared<TestNfcMifareUltralight>();
                    testNfcMifareUltralight->Initailize();
                    testNfcMifareUltralight->StartTest();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    testNfcMifareUltralight->DeInitailize();
                } break;
                case 5: {
                    printf("MifareClassic Tag Test.");
                    std::shared_ptr<TestNfcMifareClassic> testNfcMifareClassic =
                        std::make_shared<TestNfcMifareClassic>();
                    testNfcMifareClassic->Initailize();
                    testNfcMifareClassic->StartTest();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    testNfcMifareClassic->DeInitailize();
                } break;
                case 6: {
                    printf("IsoDep Tag Test.");
                    std::shared_ptr<TestNfcIsoDep> testNfcIsoDep = std::make_shared<TestNfcIsoDep>();
                    testNfcIsoDep->Initailize();
                    testNfcIsoDep->StartTest();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    testNfcIsoDep->DeInitailize();
                } break;
                case 7: {
                    printf("Iso15693 Tag Test.");
                    std::shared_ptr<TestNfcIso15693Tag> testNfcIso15693Tag = std::make_shared<TestNfcIso15693Tag>();
                    testNfcIso15693Tag->Initailize();
                    testNfcIso15693Tag->StartTest();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    testNfcIso15693Tag->DeInitailize();
                } break;
                case 8: {
                    printf("Ndef Tag Test.");
                    std::shared_ptr<TestNfcNdefTag> testNfcNdefTag = std::make_shared<TestNfcNdefTag>();
                    testNfcNdefTag->Initailize();
                    testNfcNdefTag->StartTest();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    testNfcNdefTag->DeInitailize();
                } break;
                case 9:
                    printf("Ignore Tag Test.");
                    break;
                case 1101: {
                    InfoLog("Send Screen On Common Event.");
                    const std::string ACTION_SCREEN_ON = "ohos.intent.action.SCREEN_ON";
                    Want want;
                    want.SetAction(ACTION_SCREEN_ON);
                    CommonEventData data;
                    data.SetWant(want);
                    CommonEventManager::PublishCommonEvent(data);
                    break;
                }
                default:
                    printf("Unknown command\n");
                    break;
            }
        } catch (std::exception& e) {
            printf("%s\n", e.what());
            return 0;
        }
    }
    return 0;
}