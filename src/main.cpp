#include "../include/qtest.hpp"
#include <iostream>
#include <exception>

int add(int a, int b) {
    return a + b;
}

QTestCase(Add, Positive) {
    QTEST_EXPECT_THROWS(std::exception(), std::exception);
    QTEST_EXPECT(add(1, 2) == 3);
    QTEST_EXPECT(add(5, 5) == 10);
}

QTestCase(Add, Negative) {
    QTEST_EXPECT(add(4, -4) == 0);
    QTEST_EXPECT(add(-4, 4) != 0);
}

int main(int argc, char const *argv[]) {
    QTestRegister(Add, Positive);
    QTestRegister(Add, Negative);
    QTestRunAll();
    return 0;
}
