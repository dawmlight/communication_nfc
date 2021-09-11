
#include "shared_preferences.h"

using namespace osal;

using SharedPrefIter = std::map<std::string, bool>::iterator;

SharedPreferences::SharedPreferences() {}

bool SharedPreferences::GetBool(std::string key, bool defValue)
{
    if (key.empty()) {
        return defValue;
    }
    std::lock_guard<std::mutex> lock(mtx_);
    SharedPrefIter iter = sharedPrefMap_.find(key);
    if (iter == sharedPrefMap_.end()) {
        return defValue;
    }
    return iter->second;
}

void SharedPreferences::PutBool(std::string key, bool value)
{
    std::lock_guard<std::mutex> lock(mtx_);
    SharedPrefIter iter = sharedPrefMap_.find(key);
    if (iter == sharedPrefMap_.end()) {
        sharedPrefMap_.insert(make_pair(key, value));
    } else {
        iter->second = value;
    }
}