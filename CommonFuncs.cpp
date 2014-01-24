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
#include "CommonFuncs.h"

#ifdef MAC
#import <CoreServices/CoreServices.h>
#endif

#include <cstring>

namespace ArchitectureSpace {

void movve(long size, void* from, void* to) { memcpy(to, from, size); }
//D min(D a, D b) { return (a < b) ? a : b; }



#ifdef MAC
void getOSXVersion(int& major, int& minor, int& bugfix) {
    SInt32 majorVersion, minorVersion, bugFixVersion;
    
    Gestalt(gestaltSystemVersionMajor, &majorVersion);
    Gestalt(gestaltSystemVersionMinor, &minorVersion);
    Gestalt(gestaltSystemVersionBugFix, &bugFixVersion);

    major = majorVersion;
    minor = minorVersion;
    bugfix = bugFixVersion;
}

streng getOSXVersion() {
    int major, minor, bugfix;
    getOSXVersion(major, minor, bugfix);
    return intToStr(major) + "." + intToStr(minor) + "." + intToStr(bugfix);
}

#endif

}
