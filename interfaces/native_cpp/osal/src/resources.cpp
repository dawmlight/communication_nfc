#include "resources.h"

using namespace osal;

Resources::Resources() {}

template <typename T> std::shared_ptr<T> Resources::ConvertData(std::shared_ptr<void> data)
{
    if (!data) {
        return nullptr;
    }
    return std::static_pointer_cast<T>(data);
}

bool Resources::GetBool(int id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    std::map<int, std::shared_ptr<void>>::iterator iter = rss_.find(id);
    if (iter == rss_.end()) {
        return false;
    }
    return *ConvertData<bool>(iter->second);
}

int Resources::GetInt(int id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    std::map<int, std::shared_ptr<void>>::iterator iter = rss_.find(id);
    if (iter == rss_.end()) {
        return 0;
    }
    return *ConvertData<int>(iter->second);
}

std::string* Resources::GetStringArray(int id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    std::map<int, std::shared_ptr<void>>::iterator iter = rss_.find(id);
    if (iter == rss_.end()) {
        return nullptr;
    }
    return *ConvertData<std::string*>(iter->second);
}

 void Resources::PutInt(int id, int key) 
 {
     std::lock_guard<std::mutex> lock(mtx_);
     std::map<int, std::shared_ptr<void>>::iterator iter = rss_.find(id);
     if (iter == rss_.end()) {
         rss_.insert(std::pair<int, std::shared_ptr<void>>(id, std::make_shared<int>(key)));
         return;
     }
     iter->second = std::make_shared<int>(key);
 }

 void Resources::PutBool(int id, bool key)
 {
     std::lock_guard<std::mutex> lock(mtx_);
     std::map<int, std::shared_ptr<void>>::iterator iter = rss_.find(id);
     if (iter == rss_.end()) {
         rss_.insert(std::pair<int, std::shared_ptr<void>>(id, std::make_shared<bool>(key)));
         return;
     }
     iter->second = std::make_shared<bool>(key);
 }