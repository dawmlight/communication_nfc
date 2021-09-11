#ifndef _RESOURCES_H_
#define _RESOURCES_H_

#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace osal {
class Context;
enum R {
    toast_debounce_time_ms = 0,
    unknown_tag_polling_delay,
    enable_nfc_blocking_alert,
    nfc_blocking_count,
    enable_notify_dispatch_failed,
    enable_nfc_provisioning,
    config_ara_aid_list_ese,
    config_ara_aid_list_ese_length
};

class Resources {
public:
    bool GetBool(int id);
    int GetInt(int id);
    std::string* GetStringArray(int id);
    // put
    void PutInt(int id, int key);
    void PutBool(int id, bool key);

    Resources();
    Resources(const Resources& manager) = delete;
    const Resources& operator=(const Resources& manager) = delete;

    template <typename T> std::shared_ptr<T> ConvertData(std::shared_ptr<void> data);

private:
    std::mutex mtx_;
    std::map<int, std::shared_ptr<void>> rss_{};

    friend class Context;
};
}  // namespace osal
#endif  // !_RESOURCES_H_
