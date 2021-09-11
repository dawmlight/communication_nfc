
#include "power_manager.h"
#include "stdio.h"


namespace osal {
PowerManager::PowerManager(/* args */) : mScreenState_(true) {}

PowerManager::~PowerManager() {}

void PowerManager::UserActivity(long when, int e)
{
    printf("UserActivity Uptime(%ld), Uid(%d)", when, e);
}

bool PowerManager::IsScreenOn()
{
    return mScreenState_;
}

void PowerManager::SetScreenOn(bool state)
{
    mScreenState_ = state;
}
}  // namespace osal
