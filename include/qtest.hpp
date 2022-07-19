#pragma once

#include <vector>
#include <iostream>
#include <exception>

// color macros for output
#define RED    "\033[1m\033[31m"
#define GREEN  "\033[1m\033[32m"
#define YELLOW "\033[1m\033[33m"
#define RESET  "\033[0m"

#define QTEST_NAME(module_, testName) QTest_##module_##_##testName
#define QTestCase(module_, testName)                                                        \
    class QTEST_NAME(module_, testName) : public qtest::QTestBase {                         \
        public:                                                                             \
            static QTEST_NAME(module_, testName)* getInstance (){                           \
                static QTEST_NAME(module_, testName) instance(#module_ "." #testName);      \
                return &instance;                                                           \
            }                                                                               \
            QTEST_NAME(module_, testName) (QTEST_NAME(module_, testName) const&) = delete;  \
            void operator=(QTEST_NAME(module_, testName) const&) = delete;                  \
            ~QTEST_NAME(module_, testName)() = default;                                     \
            virtual void runTest() override;                                                \
        private:                                                                            \
            QTEST_NAME(module_, testName)(const std::string& _name) :                       \
                                          qtest::QTestBase(_name) {}                        \
    };                                                                                      \
void QTEST_NAME(module_, testName)::runTest()

#define QTestRegister(module_, testName)             \
    qtest::QTestRunner::getInstance().registerTest(  \
        QTEST_NAME(module_, testName)::getInstance() \
    )
#define QTestRunAll() qtest::QTestRunner::getInstance().runTests()

#define QTEST_EXPECT(exp)\
    qtest::Expector::expect(exp, "QTEST_EXPECT(" #exp ")", __FILE__, __LINE__)

// use a do-while statement so a ';' is required at the end
// (for consistent style)
#define QTEST_EXPECT_THROWS(exp, exceptionType)              \
do {                                                         \
    try {                                                    \
        exp;                                                 \
    } catch (exceptionType& e) {                             \
        break;                                               \
    }                                                        \
    throw qtest::QTestException(                             \
        "Expected exception " #exceptionType " not thrown.", \
        #exp, __FILE__, __LINE__                             \
    );                                                       \
} while (false)

namespace qtest {
    class QTestException : public std::exception {
        public:
            QTestException(const std::string& _msg,
                           const std::string& _text,
                           const std::string& _file,
                           const std::size_t& _lineNumber) {
                msg = RED "FAILED: " RESET + _file + ":" +
                      std::to_string(_lineNumber) +
                      ": " + _msg + "\n    " + _text + "\n";
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
            throw QTestException("Expected true but was false.",
                                 text, file, lineNumber);
        }
    };

    class QTestBase {
        // base class for test cases
        public:
            explicit QTestBase(const std::string& _name) :
                               name(_name) {}
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

            void runTests();

        private:
            QTestRunner() = default;
            std::vector<QTestBase*> qtests;
    };  // class QTestRunner

    // implementations:
    void QTestRunner::runTests() {
        std::uint32_t nPass = 0, nTotal = 0;
        std::vector<QTestBase*> failedTests;
        for (auto& test : qtests) {
            std::cout << YELLOW "RUNNING " RESET + test->getName() + "\n";
            try {
                test->runTest();
                std::cout << GREEN "PASSED\n" RESET;
                nPass++;
            } catch (QTestException& e) {
                // test failed
                std::cerr << e.what();
                failedTests.push_back(test);
            } catch (std::exception& e) {
                // uncaught exception, test marked as failed
                std::cerr << RED "FAILED: " RESET \
                          " unexpected exception while running test:\n    "
                            << e.what() << "\n";
                failedTests.push_back(test);
            }
            nTotal++;
        }

        if (nPass == nTotal) {
            std::cout << "\n" GREEN "Success:" RESET \
                      " All tests passed ";
        } else {
            std::cout << "\n" RED "Failure:" RESET \
                      " Some tests failed (listed below) ";
        }
        std::cout << "[" << std::to_string(nPass) << "/"
                  << std::to_string(nTotal) << "]\n";
        for (QTestBase*& test : failedTests) {
            std::cout << RED "FAILED: " RESET << test->getName() + "\n";
        }
    }
};  // namespace qtest
