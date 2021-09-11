#include "key_guard_manager.h"

namespace osal {
KeyguardManager::KeyguardManager(/* args */) : mKeyguardLackedStatus_(false) {}

KeyguardManager::~KeyguardManager() {}

bool KeyguardManager::IsKeyguardLocked()
{
    return mKeyguardLackedStatus_;
}

void KeyguardManager::SetKeyguardLocked(bool status)
{
    mKeyguardLackedStatus_ = status;
}
}  // namespace osal