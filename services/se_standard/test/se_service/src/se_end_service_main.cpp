#include "hilog/log.h"
#include "ipc_skeleton.h"
#include "se_service_manager.h"

using namespace OHOS;
using namespace OHOS::se;

int main(int argc, char* argv[])
{
    HILOG_INFO(LOG_APP, "Hello, SE Service.");
    auto seServiceManager = DelayedSingleton<SeServiceManager>::GetInstance();
    HILOG_INFO(LOG_APP, "Start SE Service.");
    seServiceManager->OnStart();
    IPCSkeleton::JoinWorkThread();
    HILOG_INFO(LOG_APP, "Stop  SE Service.");
    seServiceManager->OnStop();
    HILOG_INFO(LOG_APP, "Byebye, SE Service.");
}