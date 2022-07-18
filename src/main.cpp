#include "../include/qtest.hpp"
#include <iostream>
#include <exception>

QTestCase(Add, Positive) {
    // QTEST_ASSERT_FALSE(false == true);
    QTEST_EXPECT_THROWS(throw std::exception(), qtest::QTestException);
}

int main(int argc, char const *argv[]) {
    QTestRegister(Add, Positive);
    QTestRunAll();
    return 0;
}
