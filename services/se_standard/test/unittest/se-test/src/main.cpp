#include <gtest/gtest.h>

#include <cstdio>

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    auto rv = RUN_ALL_TESTS();
    std::cout << "press any key + <Enter> to exit" << std::endl;

    getchar();
    return rv;
}