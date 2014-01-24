#ifndef TEST_H_
#define TEST_H_

#include <vector>
#include <string>

#include <CommonCore.git/GenericDefines.h>
#include <CommonCore.git/stringToString.h>
#include <CommonCore.git/RefCountedPointers.hpp>

#include "Timer.h"
#include <exception>
#include <math.h>
#include <time.h>
#include "assert.h"

namespace ArchitectureSpace {

    Klass(Test) {
    public:
        virtual void execute() = 0;
        virtual ~Test() {}
    };

    Klass(TestNode) : public Test {
    public:
        std::vector<Test*> subTests;

        void execute() {
            for (int s = 0; s < subTests.size(); ++s) {
                subTests[s]->execute();
            }
        }
    };

    class TestFailureException : public std::exception {
    private:
        streng msg;
        Test* failedTest;
    public:
        TestFailureException(streng iMsg, Test* iFailedTest) : msg(iMsg), failedTest(iFailedTest) {}

        virtual const char* what() const throw() {
            return formatString("Test failed: %s\n", msg.c_str()).c_str();
        }

        ~TestFailureException() throw() {}
    };

    template <class T>
    void assertEqual(T a, T b, Test* t) {
        assert(a == b);
        if (a != b) throw new TestFailureException("", t);
    }


    bool performTests();

}

#endif /* TEST_H_ */
