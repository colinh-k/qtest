#include "../include/qtest.hpp"
#include <iostream>

QTestCase(Add, Positive) {
    std::cout << "hello\n";
}

int main(int argc, char const *argv[]) {
    // qtest::QTestRunner::getInstance().registerTest();
    QTestRegister(Add, Positive);
    qtest::QTestRunner::getInstance().runTests();
    std::cout << "haha\n";
    return 0;
}
