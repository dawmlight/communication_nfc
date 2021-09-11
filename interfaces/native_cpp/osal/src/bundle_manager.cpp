#include "bundle_manager.h"

#include <algorithm>

#include "ipc_skeleton.h"
#include "loghelper.h"
#include "osal_bundle_info.h"
#include "preferences_xml_utils.h"

using OHOS::NativePreferences::Element;
using OHOS::NativePreferences::PreferencesXmlUtils;

namespace osal {
const std::string TAG_STRING{"string"};
const std::string TAG_SET{"set"};
const std::string KEY_SET_UID_NAMES{"uid-names"};
const std::string KEY_SET_BUNDLE_INFOS{"infos"};
const std::string KEY_BUNDLE_NAME{"name"};
const std::string KEY_BUNDLE_SIGNATURES{"signatures"};
const std::string KEY_BUNDLE_PERMISSIONS{"permissions"};

BundleManager::BundleManager(/* args */)
{
    Unmarshalling(SE_TEST_CONFIG);
}

BundleManager::~BundleManager() {}

std::vector<std::string> BundleManager::GetBundlesForUid(int64_t uid)
{
    if (bundleNames_.empty()) {
        return std::vector<std::string>();
    }
    return bundleNames_.at(uid);
}

void BundleManager::SetBundlesForUid(int64_t uid, const std::vector<std::string>& bundleNames)
{
    bundleNames_[uid] = bundleNames;
}

void BundleManager::RemoveBundlesForUid(int64_t uid)
{
    std::map<int64_t, std::vector<std::string>>::iterator iter = bundleNames_.find(uid);
    if (iter == bundleNames_.end()) {
        return;
    }
    bundleNames_.erase(iter);
}

int BundleManager::CheckPermission(const std::string& permission, const std::string& bundleName)
{
    std::map<std::string, std::shared_ptr<BundleInfo>>::iterator iter = bundleInfos_.find(bundleName);
    if (iter == bundleInfos_.end()) {
        // bundle info of name isn't found.
        return PERMISSION_DENIED;
    }
    std::vector<std::string>& persessions = iter->second->permissions_;
    std::vector<std::string>::const_iterator vtIter = std::find(persessions.begin(), persessions.end(), permission);
    if (vtIter == persessions.end()) {
        // persession is unexist.
        return PERMISSION_DENIED;
    }
    return PERMISSION_GRANTED;
}

void BundleManager::AddPermission(const std::string& permission, const std::string& bundleName)
{
    if (permission.empty() || bundleName.empty()) {
        return;
    }
    std::map<std::string, std::shared_ptr<BundleInfo>>::iterator iter = bundleInfos_.find(bundleName);
    if (iter == bundleInfos_.end()) {
        std::shared_ptr<BundleInfo> bundleInfo = std::make_shared<BundleInfo>();
        bundleInfo->mBundleName_ = bundleName;
        bundleInfo->permissions_.push_back(permission);
        bundleInfos_[bundleName] = bundleInfo;
        return;
    }
    std::vector<std::string>& persessions = iter->second->permissions_;
    std::vector<std::string>::const_iterator vtIter = std::find(persessions.begin(), persessions.end(), permission);
    if (vtIter != persessions.end()) {
        // already exsit
        return;
    }
    persessions.push_back(permission);
}

std::shared_ptr<BundleInfo> BundleManager::GetBundleInfo(std::string bundleName, int flags)
{
    std::map<std::string, std::shared_ptr<BundleInfo>>::iterator iter = bundleInfos_.find(bundleName);
    if (iter == bundleInfos_.end()) {
        return std::shared_ptr<BundleInfo>();
    }
    return iter->second;
}

void BundleManager::SetBundleInfo(const std::string& bundleName, std::shared_ptr<BundleInfo> bundleInfo)
{
    bundleInfos_[bundleName] = bundleInfo;
}

std::string BundleManager::GetCallingBundleName(int64_t uid)
{
    if (bundleNames_.empty()) {
        return std::string();
    }
    std::map<int64_t, std::vector<std::string>>::iterator iter = bundleNames_.find(uid);
    if (iter == bundleNames_.end()) {
        return std::string();
    }
    if (iter->second.empty()) {
        return std::string();
    }
    return iter->second.at(0);
}

std::vector<Element> SetUpStringVector(const std::vector<std::string>& datas)
{
    if (datas.empty()) {
        return std::vector<Element>();
    }

    std::vector<Element> elements;
    for (size_t i = 0; i < datas.size(); ++i) {
        Element elment;
        elment.tag_ = TAG_STRING;
        elment.key_ = "";
        elment.value_ = datas.at(i);

        elements.push_back(elment);
    }
    return elements;
}

bool BundleManager::SetUpBundleNames(Element& bundleNames)
{
    if (bundleNames_.empty()) {
        return false;
    }
    std::vector<Element> children;
    bundleNames.tag_ = TAG_SET;
    bundleNames.key_ = KEY_SET_UID_NAMES;
    std::map<int64_t, std::vector<std::string>>::iterator iter = bundleNames_.begin();
    while (iter != bundleNames_.end()) {
        Element uid;
        uid.tag_ = TAG_SET;
        uid.key_ = std::to_string(iter->first);
        uid.children_ = SetUpStringVector(iter->second);
        bundleNames.children_.push_back(uid);

        ++iter;
    }
    return true;
}

std::vector<Element> SetUpBundleInfo(const std::shared_ptr<BundleInfo>& bundleInfo)
{
    if (!bundleInfo) {
        return std::vector<Element>();
    }
    std::vector<Element> elements;
    if (!bundleInfo->mBundleName_.empty()) {
        Element element;
        element.tag_ = TAG_STRING;
        element.key_ = KEY_BUNDLE_NAME;
        element.value_ = bundleInfo->mBundleName_;
        elements.push_back(element);
    }
    if (!bundleInfo->signatures_.empty()) {
        Element element;
        element.tag_ = TAG_SET;
        element.key_ = KEY_BUNDLE_SIGNATURES;
        element.children_ = SetUpStringVector(bundleInfo->signatures_);
        elements.push_back(element);
    }
    if (!bundleInfo->permissions_.empty()) {
        Element element;
        element.tag_ = TAG_SET;
        element.key_ = KEY_BUNDLE_PERMISSIONS;
        element.children_ = SetUpStringVector(bundleInfo->permissions_);
        elements.push_back(element);
    }
    return elements;
}

bool BundleManager::SetUpBundleInfos(Element& bundleInfos)
{
    if (bundleInfos_.empty()) {
        return false;
    }
    std::vector<Element> children;
    bundleInfos.tag_ = TAG_SET;
    bundleInfos.key_ = KEY_SET_BUNDLE_INFOS;
    std::map<std::string, std::shared_ptr<BundleInfo>>::iterator iter = bundleInfos_.begin();
    while (iter != bundleInfos_.end()) {
        Element uid;
        uid.tag_ = TAG_SET;
        uid.key_ = iter->first;
        uid.children_ = SetUpBundleInfo(iter->second);
        bundleInfos.children_.push_back(uid);

        ++iter;
    }
    return true;
}

bool BundleManager::Marshalling(const std::string& configFile)
{
    InfoLog("BundleManager::Marshalling");
    std::vector<Element> settings;
    Element bundleNames;
    if (!SetUpBundleNames(bundleNames)) {
        InfoLog("BundleManager::Marshalling: BundleNames Is Empty.");
    } else {
        settings.push_back(bundleNames);
    }
    Element bundleInfos;
    if (!SetUpBundleInfos(bundleInfos)) {
        InfoLog("BundleManager::Marshalling: BundleInfos Is Empty.");
    } else {
        settings.push_back(bundleInfos);
    }
    if (settings.empty()) {
        return false;
    }

    std::string fileName = SE_TEST_CONFIG;
    if (!configFile.empty()) {
        fileName = configFile;
    }
    InfoLog("The file (%s) is written.", fileName.c_str());
    return PreferencesXmlUtils::WriteSettingXml(fileName, settings);
}

std::vector<std::string> BundleManager::LoadStringVector(const std::vector<Element>& elements)
{
    if (elements.empty()) {
        return std::vector<std::string>();
    }

    std::vector<std::string> datas;
    std::vector<Element>::const_iterator iter = elements.begin();
    while (iter != elements.end()) {
        datas.push_back(iter->value_);
        ++iter;
    }
    return datas;
}

bool BundleManager::ParseBundleNames(const std::vector<Element>& elements)
{
    if (elements.empty()) {
        InfoLog("The bundle names can't be empty.");
        return false;
    }

    int64_t uid{0};
    std::vector<Element>::const_iterator iter = elements.begin();
    while (iter != elements.end()) {
        if (iter->key_.compare("0") == 0) {
            uid = OHOS::IPCSkeleton::GetCallingUid();
        } else {
            uid = atol(iter->key_.c_str());
        }
        bundleNames_[uid] = LoadStringVector(iter->children_);
        ++iter;
    }
    return true;
}

std::shared_ptr<BundleInfo> BundleManager::LoadBundleInfo(const std::vector<Element>& elements)
{
    if (elements.empty()) {
        return std::shared_ptr<BundleInfo>();
    }

    std::shared_ptr<BundleInfo> bundleInfo = std::make_shared<BundleInfo>();
    std::vector<Element>::const_iterator iter = elements.begin();
    while (iter != elements.end()) {
        if (iter->key_.empty()) {
            InfoLog("The attribute of bundleInfo can't be empty.");
        } else {
            if (iter->key_.compare(KEY_BUNDLE_NAME) == 0) {
                bundleInfo->mBundleName_ = iter->value_;
            } else if (iter->key_.compare(KEY_BUNDLE_SIGNATURES) == 0) {
                bundleInfo->signatures_ = LoadStringVector(iter->children_);
            } else if (iter->key_.compare(KEY_BUNDLE_PERMISSIONS) == 0) {
                bundleInfo->permissions_ = LoadStringVector(iter->children_);
            } else {
                InfoLog("The attribute (%s) of bundleInfo is unsupported", iter->key_.c_str());
            }
        }
        ++iter;
    }
    return bundleInfo;
}

bool BundleManager::ParseBundleInfos(const std::vector<Element>& elements)
{
    if (elements.empty()) {
        InfoLog("The bundle infos can't be empty.");
        return false;
    }

    std::vector<Element>::const_iterator iter = elements.begin();
    while (iter != elements.end()) {
        if (iter->key_.empty()) {
            InfoLog("The bundle name of bundleInfo can't be empty.");
        } else {
            bundleInfos_[iter->key_] = LoadBundleInfo(iter->children_);
        }
        ++iter;
    }
    return true;
}

bool BundleManager::Unmarshalling(const std::string& configFile)
{
    InfoLog("BundleManager::Unmarshalling");
    std::vector<Element> settings;
    std::string fileName = SE_TEST_CONFIG;
    if (!configFile.empty()) {
        fileName = configFile;
    }
    if (!PreferencesXmlUtils::ReadSettingXml(fileName, settings) || settings.empty()) {
        InfoLog("File (%s) is unexist. Or It's failed to parse the file.", fileName.c_str());
        return false;
    }

    std::vector<Element>::const_iterator iter = settings.begin();
    while (iter != settings.end()) {
        if (iter->key_.empty()) {
            InfoLog("The attribue of BundleManager can't be empty.");
        } else {
            if (iter->key_.compare(KEY_SET_UID_NAMES) == 0) {
                ParseBundleNames(iter->children_);
            } else if (iter->key_.compare(KEY_SET_BUNDLE_INFOS) == 0) {
                ParseBundleInfos(iter->children_);
            } else {
                InfoLog("The attribue (%s) of BundleManager is unsupported", iter->key_.c_str());
            }
        }
        ++iter;
    }
    return true;
}
}  // namespace osal