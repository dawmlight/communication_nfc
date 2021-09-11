#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

namespace osal {
class PowerManager final {
public:
    PowerManager(/* args */);
    ~PowerManager();

    // ��Ļ��Դ����ʱ��
    void UserActivity(long when, int event);

    bool IsScreenOn();
    void SetScreenOn(bool status);

private:
    bool mScreenState_;
};

}  // namespace osal

#endif  //_POWER_MANAGER_H_
