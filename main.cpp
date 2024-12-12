#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

// Абстрактный базовый класс TestCaseBase
class TestCaseBase {
protected:
    string input;
    string expected;

public:
    TestCaseBase(const string& input_str, const string& expected_str)
        : input(input_str), expected(expected_str) {}

    virtual ~TestCaseBase() = default;  // Виртуальный деструктор

    virtual bool runTest() const = 0;  // Чисто виртуальная функция для выполнения теста

    // Абстрактный метод клонирования
    virtual TestCaseBase* clone() const = 0;
};

// Класс TestCase - производный от TestCaseBase
class TestCase : public TestCaseBase {
public:
    TestCase(const string& input_str, const string& expected_str)
        : TestCaseBase(input_str, expected_str) {}

    bool runTest() const override {
        return input == expected;
    }

    // Глубокое клонирование
    TestCase* clone() const override {
        return new TestCase(input, expected);
    }
};

// Класс AdvancedTestCase - производный от TestCase
class AdvancedTestCase : public TestCase {
private:
    int complexityLevel;

public:
    AdvancedTestCase(const string& input_str, const string& expected_str, int level)
        : TestCase(input_str, expected_str), complexityLevel(level) {}

    bool runTest() const override {
        cout << "Running advanced test with complexity level: " << complexityLevel << endl;
        return TestCase::runTest() && complexityLevel > 2;
    }

    // Глубокое клонирование для производного класса
    AdvancedTestCase* clone() const override {
        return new AdvancedTestCase(input, expected, complexityLevel);
    }
};

// Класс TestSuite
class TestSuite {
private:
    vector<TestCaseBase*> tests;
    static int totalTestSuitesCreated; // Статическое поле

public:
    TestSuite() {
        totalTestSuitesCreated++;
    }

    // Метод для добавления тестов
    void addTest(TestCaseBase* test) {
        tests.push_back(test);
    }

    // Получение тестов
    const vector<TestCaseBase*>& getTests() const {
        return tests;
    }

    // Количество тестов
    int getTestCount() const {
        return tests.size();
    }

    static int getTotalTestSuitesCreated() {
        return totalTestSuitesCreated;
    }

    // Перегрузка оператора присваивания для производного класса
    TestSuite& operator=(const TestSuite& other) {
        if (this != &other) {
            for (TestCaseBase* test : tests) {
                delete test;
            }
            tests.clear();
            for (TestCaseBase* test : other.tests) {
                tests.push_back(test->clone());  // Глубокое копирование
            }
        }
        return *this;
    }

    // Копирование объекта с глубоким клонированием
    TestSuite(const TestSuite& other) {
        for (TestCaseBase* test : other.tests) {
            tests.push_back(test->clone());  // Глубокое копирование
        }
    }

    ~TestSuite() {
        for (TestCaseBase* test : tests) {
            delete test;
        }
    }

    // Функция для поверхностного копирования
    void shallowCopy(const TestSuite& other) {
        tests = other.tests;  // Поверхностное копирование, т.е. просто копируем указатели
    }
};

int TestSuite::totalTestSuitesCreated = 0;

// Класс Task
class Task {
private:
    string description;
    TestSuite testSuite;

public:
    Task(const string& desc, const TestSuite& suite)
        : description(desc), testSuite(suite) {}

    const string& getDescription() const {
        return description;
    }

    const TestSuite& getTestSuite() const {
        return testSuite;
    }
};

// Класс UserSolution
class UserSolution {
private:
    string solutionCode;

public:
    UserSolution(const string& code)
        : solutionCode(code) {}

    const string& getSolutionCode() const {
        return solutionCode;
    }
};

// Класс ExecutionResult
class ExecutionResult {
private:
    string actualOutput;
    bool isPassed;

public:
    ExecutionResult() : isPassed(false) {}

    void setActualOutput(const string& output) {
        actualOutput = output;
    }

    const string& getActualOutput() const {
        return actualOutput;
    }

    void setIsPassed(bool passed) {
        isPassed = passed;
    }

    bool getIsPassed() const {
        return isPassed;
    }
};

// Класс Submission
class Submission {
private:
    UserSolution solution;
    vector<ExecutionResult> results;
    int totalPassed;

public:
    Submission(const UserSolution& userSolution, int testCount)
        : solution(userSolution), totalPassed(0) {
        results.resize(testCount);
    }

    void setTotalPassed(int passed) {
        totalPassed = passed;
    }

    int getTotalPassed() const {
        return totalPassed;
    }

    vector<ExecutionResult>& getResults() {
        return results;
    }

    const UserSolution& getSolution() const {
        return solution;
    }
};

// Функция для выполнения теста
ExecutionResult runTestCase(UserSolution& solution, const TestCaseBase& test) {
    ExecutionResult result;
    result.setActualOutput(test.runTest() ? "Passed" : "Failed");
    result.setIsPassed(test.runTest());
    return result;
}

// Функция для проверки решения
Submission checkSolution(UserSolution& solution, Task& task) {
    Submission submission(solution, task.getTestSuite().getTestCount());
    int totalPassed = 0;
    for (size_t i = 0; i < task.getTestSuite().getTestCount(); ++i) {
        const TestCaseBase& test = *task.getTestSuite().getTests()[i];
        ExecutionResult result = runTestCase(solution, test);
        submission.getResults()[i] = result;
        if (result.getIsPassed()) {
            totalPassed++;
        }
    }
    submission.setTotalPassed(totalPassed);
    return submission;
}

// Главная функция
int main() {
    TestCase* test1 = new TestCase("input1", "input1");
    TestCase* test2 = new TestCase("input2", "expected2");

    TestSuite suite;
    suite.addTest(test1);
    suite.addTest(test2);

    Task task("Example Task", suite);

    UserSolution solution("user_solution_code");

    Submission submission = checkSolution(solution, task);

    cout << "Total tests passed: " << submission.getTotalPassed() << " out of " << suite.getTestCount() << endl;

    for (size_t i = 0; i < suite.getTestCount(); i++) {
        cout << "Test " << (i + 1) << ": " << (submission.getResults()[i].getIsPassed() ? "Passed" : "Failed") << endl;
    }

    cout << "Total Test Suites Created: " << TestSuite::getTotalTestSuitesCreated() << endl;

    return 0;
}
