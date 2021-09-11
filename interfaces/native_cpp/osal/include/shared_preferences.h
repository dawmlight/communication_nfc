#ifndef _SHARED_PREFERENCES_H_
#define _SHARED_PREFERENCES_H_

#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace osal {
class Context;

class SharedPreferences {
public:
    bool GetBool(std::string key, bool defValue);
    void PutBool(std::string key, bool value);

    SharedPreferences();
    SharedPreferences(const SharedPreferences& manager) = delete;
    const SharedPreferences& operator=(const SharedPreferences& manager) = delete;

private:
    std::map<std::string, bool> sharedPrefMap_{};
    std::mutex mtx_{};

    friend class Context;
};
}  // namespace osal
#endif  // _SHARED_PREFERENCES_H_
