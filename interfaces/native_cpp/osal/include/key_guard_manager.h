#ifndef _KEY_GUARD_MANAGER_H_
#define _KEY_GUARD_MANAGER_H_

namespace osal {
class KeyguardManager {
private:
    /* data */
public:
    KeyguardManager(/* args */);
    ~KeyguardManager();

    bool IsKeyguardLocked();
    void SetKeyguardLocked(bool status);

private:
    bool mKeyguardLackedStatus_;
};
}  // namespace osal

#endif  //_KEY_GUARD_MANAGER_H_
