#include <gmock/gmock.h>
#include <gtest/gtest.h>

class MyTest : public ::testing::Test {
public:
    virtual void SetUp() override
    {
        std::cout << " SetUpTestCase." << std::endl;
    }

    virtual void TearDown() override
    {
        std::cout << " TearDownTestCase." << std::endl;
    }
};

TEST_F(MyTest, testNfcOpen001)
{
    EXPECT_TRUE(1 == 1);
}