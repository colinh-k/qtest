#include "../include/qtest.hpp"
#include <iostream>
#include <exception>

int add(int a, int b) {
    return a + b;
}

QTEST_CASE(Add, Positive) {
    QTEST_EXPECT_THROWS(throw std::exception(), std::exception);
    QTEST_EXPECT(add(1, 2) == 3);
    QTEST_EXPECT(add(5, 5) == 10);
}

QTEST_CASE(Add, Negative) {
    QTEST_EXPECT(add(4, -4) == 0);
    QTEST_EXPECT(add(-4, 4) == 0);
}

QTEST_CASE(Add, Throws) {
    QTEST_EXPECT_THROWS(throw std::exception();, std::exception);
}

int main(int argc, char const *argv[]) {
    QTEST_RUN_ALL();
    return 0;
}
