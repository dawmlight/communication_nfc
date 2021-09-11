#ifndef TEST_NFC_ABILITY_H
#define TEST_NFC_ABILITY_H

#include <memory>
#include <string>
#include <vector>

#include "ability.h"

namespace OHOS {
using AAFwk::Want;
using AppExecFwk::Ability;
using AppExecFwk::AbilityInfo;
using Application = AppExecFwk::OHOSApplication;

namespace nfc {
namespace sdk {
class Tag;
class NfcAgent;
class NfcMap;
}  // namespace sdk
namespace test {
class TestReadModeTagCallBack;
class TestNfcReaderApp;

class TestNfcAbility : public Ability {
public:
    explicit TestNfcAbility();
    ~TestNfcAbility();

    static std::shared_ptr<TestNfcAbility> GenAbility(const std::shared_ptr<TestNfcReaderApp>& application);
    static std::shared_ptr<TestNfcAbility> GenAbility(std::string& abilityName,
                                                      const std::shared_ptr<TestNfcReaderApp>& application);
    static std::shared_ptr<TestNfcAbility> GenAbility(const std::shared_ptr<AbilityInfo>& abilityInfo,
                                                      const std::shared_ptr<TestNfcReaderApp>& application);

    void OnStart(const Want& want) override;
    void OnStop() override;

    void SetReaderApp(std::shared_ptr<TestNfcReaderApp>);
    std::shared_ptr<TestReadModeTagCallBack> GetTagCallback();
    void SetTagCallback(std::shared_ptr<TestReadModeTagCallBack> tagCallback);
    // tag type
    int GetTagTypeFilter();
    void SetTagTypeFilter(int tagTypeFilter);
    // Extras
    std::shared_ptr<sdk::NfcMap> GetExtras();
    void SetExtras(std::shared_ptr<sdk::NfcMap> extras);
    // tag process
    std::shared_ptr<sdk::Tag> GetTag();
    void RemoveTag();

private:
    std::shared_ptr<TestNfcReaderApp> readerApp_;

    std::shared_ptr<TestReadModeTagCallBack> mTagCallBack_{};
    int mTagTypeFilter_{0x0F};
    std::shared_ptr<sdk::NfcMap> mExtras_{};
};
}  // namespace test
}  // namespace nfc
}  // namespace OHOS
#endif /* TEST_NFC_ABILITY_H */
