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
#ifndef COMMONFUNCTIONS_H_
#define COMMONFUNCTIONS_H_

#ifndef MINFUNCS
#define MINFUNCS
#define min_(a, b) (a < b ? a : b)
#define m_i_n(a, b) min_((a), (b))
#define max_(a, b) (a > b ? a : b)
#define m_a_x(a, b) max_((a), (b))
#define bound(l, a, u) m_a_x(l, m_i_n(a, u))
#endif

#include "stringToString.h"
#include "GenericDefines.h"
//template <class G> G min(G a, G b);

namespace ArchitectureSpace {

    template <class D> void movve(long count, const D*    from, D*    to) {
    //    memcpy(to, from, count * sizeof(D));
        for (long q = 0; q < count; q++) to[q] = from[q];
    }
    //template <class D> void move(int count, void* from, D*    to) { move(count, (D*) from, (D*) to); }
    template <class D> void movve(long count, const void*    from, void* to) { movve(count, (D*) from, (D*) to); }
    //template <class D> void move(int count, void* from, void* to) { move(count, (D*) from, (D*) to); }
    void movve(long size, void* from, void* to);


    #ifdef MAC
    void getOSXVersion(int& major, int& minor, int& bugfix);
    streng getOSXVersion();
    #endif

}
#endif
