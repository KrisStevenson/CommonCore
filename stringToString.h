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
#ifndef __Beat_Counter__stringToString__
#define __Beat_Counter__stringToString__

#include <string>
#include <vector>

#include "GenericDefines.h"

#ifdef _QT_
#include <QString>
#endif

typedef std::string streng;

namespace ArchitectureSpace {

streng intToStr(long i);

#ifdef APPLE
NSString*   stos(streng s);
streng stos(NSString*   s);// { return [s UTF8String]; }
#endif

#ifdef _QT_
streng stos(QString s);
QString stos(streng s);
#endif

    streng stringFromBinaryData(const void* data, size_t length);

    streng formatString(const char* format, ...);
    streng formatString(const streng format, ...);

    streng urlEncode(streng s);

    std::vector<streng> explode(streng s, streng separator);

    char *base64_encode(const unsigned char *data,
                        size_t input_length,
                        size_t *output_length);

    streng base64_encode(void *data, size_t input_length);

    unsigned char *base64_decode(const unsigned char *data, size_t input_length, size_t *output_length);

    void* base64_decode(streng data, size_t& outputSize);


}

#endif
