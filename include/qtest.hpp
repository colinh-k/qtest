#pragma once

#include <vector>
#include <iostream>

#define _QTestCase(className)                          \
    class className : public qtest::QTestBase {        \
        public:                                        \
            static className* getInstance (){          \
                static className instance;             \
                return &instance;                      \
            }                                          \
            className (className const&) = delete;     \
            void operator=(className const&) = delete; \
            ~className() = default;                    \
            virtual void runTest() override;           \
        private:                                       \
            className() {}                             \
    };                                                 \
void className::runTest()

// use two levels of indirection in order to
// resolve __LINE__ correctly
#define JOIN(x, y) _QTestCase(x##y)
#define CONCAT(x,y) JOIN(x,y)
#define QTEST_NAME(module, testName) CONCAT(module##testName, __LINE__)
// #define QTestCase(module_, testName) QTEST_NAME(module_, testName)
#define QTestCase(module_, testName) _QTestCase(QTest_##module_##_##testName)

#define _QTestRegister(className) qtest::QTestRunner::getInstance().registerTest(className::getInstance())
#define QTestRegister(module_, testName) _QTestRegister(QTest_##module_##_##testName)

namespace qtest {
    class QTestBase {
        // base class for test cases
        public:
            explicit QTestBase() : _hasFailed(false) {}
            virtual ~QTestBase() = default;
            virtual void runTest() = 0;
            bool hasFailed() { return _hasFailed; }
        protected:
            bool _hasFailed;
    };

    class QTestRunner {
        public:
            // singleton pattern
            static QTestRunner& getInstance() {
                static QTestRunner instance;
                return instance;
            }
            QTestRunner(QTestRunner const&) = delete;
            void operator=(QTestRunner const&) = delete;
            ~QTestRunner() = default;

            void registerTest(QTestBase* test) {
                qtests.push_back(test);
            }

            void runTests() {
                std::uint32_t nFail = 0, nTotal = 0;
                for (auto& test : qtests) {
                    test->runTest();
                    nTotal++;

                    if (test->hasFailed()) {
                        nFail++;
                    } else {

                    }
                }

                if (nFail == 0) {
                    std::cout << "Success: All tests passed ";
                } else {
                    std::cout << "Failure: Some tests failed ";
                }
                std::cout << "[" + std::to_string(nTotal - nFail) + "/" + std::to_string(nTotal) + "]\n";
            }

        private:
            QTestRunner() = default;
            std::vector<QTestBase*> qtests;
    };  // class QTestRunner
};  // namespace qtest