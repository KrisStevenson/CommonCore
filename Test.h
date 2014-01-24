/* 
The MIT License (MIT)

Copyright (c) 2010-2014 KrisStevenson

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ 
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
