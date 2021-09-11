#include "hilog/log.h"
#include "ipc_skeleton.h"
#include "st_se_service_manager.h"

using namespace OHOS;
using namespace OHOS::hardware::se::v1_0;

int main(int argc, char* argv[])
{
    HILOG_INFO(LOG_APP, "Hello, St SE Service.");
    auto stSeServiceManager = DelayedSingleton<StSeServiceManager>::GetInstance();
    HILOG_INFO(LOG_APP, "Start St SE Service.");
    stSeServiceManager->OnStart();
    IPCSkeleton::JoinWorkThread();
    HILOG_INFO(LOG_APP, "Stop  St SE Service.");
    stSeServiceManager->OnStop();
    HILOG_INFO(LOG_APP, "Byebye, St SE Service.");
}