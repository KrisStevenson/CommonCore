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
#ifndef PERSISTENT_HPP
#define PERSISTENT_HPP

#include <ostream>

#include "GenericDefines.h"
#include "stringToString.h"

namespace ArchitectureSpace {

    class Streamer {
    public:
        virtual size_t transfer(void* data, size_t size, bool read) = 0;

        void transferString(streng& data, bool read);
        static size_t computeSizeForString(const streng& data);

        template <class D>
        void transferData(D& data, bool read) {
            transfer(&data, sizeof(D), read);
        }

        virtual ~Streamer() { }
    };

    class SizeComputingVirtualStreamer : public Streamer {
    private:
        unsigned long long position;
    public:
        SizeComputingVirtualStreamer() { position = 0; }

        size_t transfer(void* data, size_t size, bool read) {
            position += size;
            ensure(!read);
            UNUSED(data); return size;
        }

        unsigned long long getPosition() { return position; }
    };

    class FileStreamer : public Streamer {
    private:
        FILE* file;
    public:
        FileStreamer(FILE* iFile) : file(iFile) { }
        ~FileStreamer() {}

        size_t transfer(void* data, size_t size, bool read);
    };

    class OStreamStreamer : public Streamer {
    private:
        std::ostream* stream;
    public:
        OStreamStreamer(std::ostream* iStream) : stream(iStream) { }
        ~OStreamStreamer() {}

        size_t transfer(void* data, size_t size, bool read);
    };

    class MemoryStreamer : public Streamer {
    private:
        unsigned char* startPosition;
        unsigned char* position;
        size_t size;
    public:
        MemoryStreamer(void* data, size_t iSize) {
            position = startPosition = (unsigned char*) data;
            size = iSize;
        }

        size_t transfer(void* data, size_t s, bool read);
    };

    class StreamablePersistent {
    public:
        virtual void transferWithStreamer(Streamer* streamer, bool read) = 0;
        size_t computeStoredSize() {
            SizeComputingVirtualStreamer scvs;
            transferWithStreamer(&scvs, false);
            return scvs.getPosition();
        }
    };

}

#endif // PERSISTENT_HPP
