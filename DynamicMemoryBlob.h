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
#ifndef DYNAMICMEMORYBLOB_H
#define DYNAMICMEMORYBLOB_H

#include <string>
#include <vector>
#include <cstring>

#include "CommonFuncs.h"

namespace ArchitectureSpace {

class DynamicMemoryBlobElement {
private:
    char *address;
    long size;
    bool owner;
public:
    DynamicMemoryBlobElement(char* iAddress, long iSize, bool copy = false, bool iOwner = false) : size(iSize), owner(iOwner) {
        if (copy) {
            address = new char[size];
            movve(size, iAddress, address);
        } else address = iAddress;
    }

    ~DynamicMemoryBlobElement() { if (owner) delete [] address; }

    void writeTo(void* dest) {
        memcpy(dest, address, size);
//        movve(size, (void*) address, (void*) dest);
    }

    long getSize() { return size; }
};

class DynamicMemoryBlob {
private:
    int totalSize;
    std::vector<DynamicMemoryBlobElement*> elements;
public:
    DynamicMemoryBlob() { totalSize = 0; }

    ~DynamicMemoryBlob() {
        for (unsigned int e = 0; e < elements.size(); ++e) delete elements[e];
    }

    void addChunk(char* address, long size, bool copy = false, bool owner = false) {
        elements.push_back(new DynamicMemoryBlobElement(address, size, copy, copy || owner));
        totalSize += size;
    }

    int getTotalSize() { return totalSize; }

    char* getContiguousBlob() {
        char* contBlob = new char[totalSize];

        char* p = contBlob;
        for (int q = 0; q < (int) elements.size(); ++q) {
            elements[q]->writeTo(p);
            p += elements[q]->getSize();
        }

        return contBlob;
    }

    std::string getDataAsString() {
        char* contBlob = new char[totalSize + 1];

        char* p = contBlob;
        for (int q = 0; q < (int) elements.size(); ++q) {
            elements[q]->writeTo(p);
            p += elements[q]->getSize();
        }

        contBlob[totalSize] = 0;
        std::string result = contBlob;
        delete [] contBlob;

        return result;
    }
};

}

#endif // DYNAMICMEMORYBLOB_H
