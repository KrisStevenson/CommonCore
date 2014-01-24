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
#include "Comdec.hpp"

#include <cstring>
#include <zlib.h>

namespace ArchitectureSpace {

/*    voidpf zAlloc(voidpf opaque, uInt items, uInt size) {
        (void) opaque;
        return malloc(items*size);
    }

    void zFree(voidpf opaque, voidpf address) {
        (void) opaque;
        free(address);
    }*/

    MemobR Comdec::convert(ComdecMode mode, MemobR source) {
        z_stream_s zStream;

        zStream.zalloc = Z_NULL;//zAlloc;
        zStream.zfree = Z_NULL;//zFree;
        zStream.opaque = NULL;

        std::vector<Memob> outMobs;

        int stepSize = 65536;

        checkError(mode == cmCompress ? deflateInit(&zStream, level) : inflateInit(&zStream),
                   formatString("%sflateInit", mode == cmCompress ? "de" : "in"));

        zStream.next_in = (Bytef*) source->pointer;
        zStream.avail_in = source->getSize();

        char outData[stepSize];
        zStream.next_out = (Bytef*) outData;
        zStream.avail_out = stepSize;

       while (checkError(mode == cmCompress ? deflate(&zStream, zStream.avail_in == 0 ? Z_FINISH : Z_NO_FLUSH)
                                            : inflate(&zStream, zStream.avail_in == 0 ? Z_FINISH : Z_NO_FLUSH), "flate") == Z_OK) {
            if (zStream.avail_out == 0) {
                void *d = malloc(stepSize);
                memcpy(d, outData, stepSize);
                outMobs.push_back(Memob(d, stepSize));
                zStream.next_out = (Bytef*) outData;
                zStream.avail_out = stepSize;
            }
        }

        char *result = (char*) malloc(zStream.total_out);
        size_t offs = 0;
        for (unsigned int m = 0; m < outMobs.size(); ++m) {
            Memob& mob = outMobs[m];
            memcpy(result + offs, mob.pointer, mob.getSize());
            offs += mob.getSize();
        }
        memcpy(result + offs, outData, stepSize - zStream.avail_out);

        checkError(mode == cmCompress ? deflateEnd(&zStream) : inflateEnd(&zStream), "flateEnd");

        return MemobR(new Memob(result, zStream.total_out));
    }

    int Comdec::checkError(int errorCode, streng msg) {
        if (errorCode != Z_OK) {
            if (errorCode == Z_STREAM_END) { DBGX("Z_STREAM_END"); return errorCode; }
            throw ComdecException(formatString("Comdec error '%s' (%i) in %s.", "", errorCode, msg.c_str()));
        }

        return errorCode;
    }



}
