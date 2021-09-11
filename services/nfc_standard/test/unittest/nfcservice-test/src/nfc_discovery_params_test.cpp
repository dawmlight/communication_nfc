#include "nfc_discovery_params.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace OHOS::nfc;

using ::testing::Return;

TEST(NfcDiscoveryParams, GetNfcOffParameters_Test)
{
    std::unique_ptr<NfcDiscoveryParams> params = NfcDiscoveryParams::GetNfcOffParameters();
    EXPECT_TRUE(params);
    EXPECT_EQ(params->GetTechMask(), 0);
    EXPECT_EQ(params->ShouldEnableLowPowerDiscovery(), false);
    EXPECT_EQ(params->ShouldEnableReaderMode(), false);
    EXPECT_EQ(params->ShouldEnableHostRouting(), false);
    EXPECT_EQ(params->ShouldEnableDiscovery(), false);
}

TEST(NfcDiscoveryParams, GetTechMask_Test)
{
    NfcDiscoveryParams::Builder paramsBuilder;
    paramsBuilder.SetTechMask(10);
    std::unique_ptr<NfcDiscoveryParams> params = paramsBuilder.Build();
    EXPECT_EQ(params->GetTechMask(), 10);

    NfcDiscoveryParams::Builder builder;
    builder.SetTechMask(100);
    params = builder.Build();
    EXPECT_EQ(params->GetTechMask(), 100);
}

TEST(NfcDiscoveryParams, ShouldEnableHostRouting_Test)
{
    NfcDiscoveryParams::Builder paramsBuilder;
    paramsBuilder.SetEnableHostRouting(true);
    std::unique_ptr<NfcDiscoveryParams> params = paramsBuilder.Build();
    EXPECT_EQ(params->ShouldEnableHostRouting(), true);

    NfcDiscoveryParams::Builder builder;
    builder.SetEnableHostRouting(false);
    params = builder.Build();
    EXPECT_EQ(params->ShouldEnableHostRouting(), false);
}

TEST(NfcDiscoveryParams, ShouldEnableReaderMode_Test)
{
    NfcDiscoveryParams::Builder paramsBuilder;
    paramsBuilder.SetEnableReaderMode(true);
    std::unique_ptr<NfcDiscoveryParams> params = paramsBuilder.Build();
    EXPECT_EQ(params->ShouldEnableReaderMode(), true);

    NfcDiscoveryParams::Builder builder;
    builder.SetEnableReaderMode(false);
    params = builder.Build();
    EXPECT_EQ(params->ShouldEnableReaderMode(), false);
}

TEST(NfcDiscoveryParams, ShouldEnableDiscovery_Test)
{
    NfcDiscoveryParams::Builder paramsBuilder;
    paramsBuilder.SetTechMask(0);
    paramsBuilder.SetEnableHostRouting(false);
    std::unique_ptr<NfcDiscoveryParams> params = paramsBuilder.Build();
    EXPECT_EQ(params->GetTechMask(), 0);
    EXPECT_EQ(params->ShouldEnableHostRouting(), false);
    EXPECT_EQ(params->ShouldEnableDiscovery(), false);
    // ShouldEnableDiscovery true
    NfcDiscoveryParams::Builder builder;
    builder.SetTechMask(1);
    builder.SetEnableHostRouting(false);
    params = builder.Build();
    EXPECT_EQ(params->GetTechMask(), 1);
    EXPECT_EQ(params->ShouldEnableHostRouting(), false);
    EXPECT_EQ(params->ShouldEnableDiscovery(), true);
    // ShouldEnableDiscovery true
    NfcDiscoveryParams::Builder builder1;
    builder1.SetTechMask(0);
    builder1.SetEnableHostRouting(true);
    params = builder1.Build();
    EXPECT_EQ(params->GetTechMask(), 0);
    EXPECT_EQ(params->ShouldEnableHostRouting(), true);
    EXPECT_EQ(params->ShouldEnableDiscovery(), true);
    // ShouldEnableDiscovery true
    NfcDiscoveryParams::Builder builder2;
    builder2.SetTechMask(1);
    builder2.SetEnableHostRouting(true);
    params = builder2.Build();
    EXPECT_EQ(params->GetTechMask(), 1);
    EXPECT_EQ(params->ShouldEnableHostRouting(), true);
    EXPECT_EQ(params->ShouldEnableDiscovery(), true);
}