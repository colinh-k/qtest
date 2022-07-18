#pragma once

#include <vector>
#include <iostream>
#include <exception>

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

// #define QTestCase(module_, testName) QTEST_NAME(module_, testName)
#define QTestCase(module_, testName) _QTestCase(QTest_##module_##_##testName)

#define _QTestRegister(className) qtest::QTestRunner::getInstance().registerTest(className::getInstance())
#define QTestRegister(module_, testName) _QTestRegister(QTest_##module_##_##testName)
#define QTestRunAll() qtest::QTestRunner::getInstance().runTests()

// use two levels of indirection in order to
// resolve __LINE__ correctly
#define JOIN(x, y) x##y
#define CONCAT(x,y) JOIN(x,y)
// #define JOIN_SPECIAL(e, m) CONCAT(, __LINE__)
// assertion macros
// #define ASSERT_TRUE(exp)        qtest::qtestAssertEquals(exp, #exp, __FILE__, __LINE__)
#define QTEST_EXPECT_TRUE(exp) qtest::expect(exp, true, #exp, __FILE__, __LINE__)
#define QTEST_EXPECT_FALSE(exp)  qtest::expect(exp, false, #exp, __FILE__, __LINE__)
#define QTEST_EXPECT_THROWS(exp, exceptionType) \
try {\
    exp;\
    throw qtest::QTestException(#exp, __FILE__, __LINE__);\
} catch (exceptionType& e) {}

namespace qtest {
    class QTestException : public std::exception {
        public:
            QTestException(const std::string& _text,
                           const std::string& _file,
                           const std::size_t& _lineNumber) {
                msg = _file + ":" + std::to_string(_lineNumber) +
                       ": FAILED:\n    " + _text + "\n";
            }
            const char* what() const noexcept override {
                return msg.c_str();
            }
        private:
            std::string msg;
            // std::string text;
            // std::string file;
            // std::size_t lineNumber;
    };

    void expect(const bool& actual, const bool& expected,
                const std::string text, const std::string file,
                std::size_t lineNumber) {
        if (actual == expected) return;  // ignore
        throw QTestException(text, file, lineNumber);
    }

    class QTestBase {
        // base class for test cases
        public:
            explicit QTestBase() {}
            virtual ~QTestBase() = default;
            virtual void runTest() = 0;
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
                std::uint32_t nPass = 0, nTotal = 0;
                for (auto& test : qtests) {
                    try {
                        test->runTest();
                        nPass++;
                    } catch (QTestException& e) {
                        // test failed
                        std::cerr << e.what();
                    } catch (std::exception& e) {
                        // uncaught exception, test marked as failed
                        std::cerr << "Unexpected exception while running test:\n    "
                                  << e.what() << "\n";
                    }
                    nTotal++;
                }

                if (nPass == nTotal) {
                    std::cout << "Success: All tests passed ";
                } else {
                    std::cout << "Failure: Some tests failed ";
                }
                std::cout << "[" + std::to_string(nPass) + "/" + std::to_string(nTotal) + "]\n";
            }

        private:
            QTestRunner() = default;
            std::vector<QTestBase*> qtests;
    };  // class QTestRunner
};  // namespace qtest