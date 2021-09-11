#include <iostream>
#include <memory>

#include "loghelper.h"
#include "ipc_skeleton.h"
#include "nfc_service_manager.h"

using namespace OHOS;
using namespace OHOS::nfc;

int main(int argc, char* argv[])
{
    InfoLog("Hello, Nfc Service.");
    auto nfcServiceManager = DelayedSingleton<NfcServiceManager>::GetInstance();    
    InfoLog("Start Nfc Service.");
    nfcServiceManager->OnStart();
    IPCSkeleton::JoinWorkThread();
    InfoLog("Stop Nfc Service.");
    nfcServiceManager->OnStop();
    InfoLog("Byebye, Nfc Service.");
    return -1;
}