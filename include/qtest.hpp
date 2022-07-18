#pragma once

#include <vector>
#include <iostream>
#include <exception>

#define QTEST_NAME(module_, testName) QTest_##module_##_##testName
#define _QTestCase(module_, testName)                                                       \
    class QTEST_NAME(module_, testName) : public qtest::QTestBase {                         \
        public:                                                                             \
            static QTEST_NAME(module_, testName)* getInstance (){                           \
                static QTEST_NAME(module_, testName) instance(#module_, #testName);         \
                return &instance;                                                           \
            }                                                                               \
            QTEST_NAME(module_, testName) (QTEST_NAME(module_, testName) const&) = delete;  \
            void operator=(QTEST_NAME(module_, testName) const&) = delete;                  \
            ~QTEST_NAME(module_, testName)() = default;                                     \
            virtual void runTest() override;                                                \
        private:                                                                            \
            QTEST_NAME(module_, testName)(const std::string& _module,                       \
                                          const std::string& _testName) :                   \
                                          qtest::QTestBase(_module, _testName) {}           \
    };                                                                                      \
void QTEST_NAME(module_, testName)::runTest()
#define QTestCase(module_, testName) _QTestCase(module_, testName)

#define QTestRegister(module_, testName) qtest::QTestRunner::getInstance().registerTest(QTEST_NAME(module_, testName)::getInstance())
#define QTestRunAll() qtest::QTestRunner::getInstance().runTests()

#define QTEST_EXPECT(exp) qtest::Expector::expect(exp, "QTEST_EXPECT(" #exp ")", __FILE__, __LINE__)
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
                msg = "FAILED: " + _file + ":" + std::to_string(_lineNumber) +
                       ":\n    " + _text + "\n";
            }
            const char* what() const noexcept override {
                return msg.c_str();
            }
        private:
            std::string msg;
    };

    struct Expector {
        static void expect(const bool& actual,
                    const std::string text,
                    const std::string file,
                    std::size_t lineNumber) {
            if (actual) return;  // ignore if true
            throw QTestException(text, file, lineNumber);
        }
    };

    class QTestBase {
        // base class for test cases
        public:
            explicit QTestBase(const std::string& _module,
                               const std::string& _testName) :
                               name(_module + "." + _testName) {}
            virtual ~QTestBase() = default;
            virtual void runTest() = 0;
            inline const std::string& getName() const { return name; }
        private:
            std::string name;
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
                std::vector<QTestBase*> failedTests;
                for (auto& test : qtests) {
                    std::cout << "RUNNING " + test->getName() + "\n";
                    try {
                        test->runTest();
                        std::cout << "PASSED\n";
                        nPass++;
                    } catch (QTestException& e) {
                        // test failed
                        std::cerr << e.what();
                        failedTests.push_back(test);
                    } catch (std::exception& e) {
                        // uncaught exception, test marked as failed
                        std::cerr << "FAILED: unexpected exception while running test:\n    "
                                  << e.what() << "\n";
                        failedTests.push_back(test);
                    }
                    nTotal++;
                }

                if (nPass == nTotal) {
                    std::cout << "\nSuccess: All tests passed ";
                } else {
                    std::cout << "\nFailure: Some tests failed (listed below) ";
                }
                std::cout << "[" + std::to_string(nPass) + "/" + std::to_string(nTotal) + "]\n";
                for (QTestBase*& test : failedTests) {
                    std::cout << "FAILED: " << test->getName() + "\n";
                }
            }

        private:
            QTestRunner() = default;
            std::vector<QTestBase*> qtests;
    };  // class QTestRunner
};  // namespace qtest