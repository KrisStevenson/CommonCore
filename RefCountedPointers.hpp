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
#ifndef REFCOUNTEDPOINTERS_HPP
#define REFCOUNTEDPOINTERS_HPP

#include "GenericDefines.h"

#include <stdlib.h>

#define klass(a) class a; typedef RefCountedPointer<a> a##R;
#define Klass(a) klass(a) class a

#define RefCountedTypeName(tyna) tyna ## R
#define newR(ty, pa) RefCountedTypeName(ty)(new ty pa)
#define declaR(ty, na, pa) RefCountedTypeName(ty) RefCountedTypeName(na) = newR(ty, pa)
#define declR(na, ty, pa) declaR(ty, na, pa)
#define summonR(ty, na) RefCountedTypeName(ty) RefCountedTypeName(na) = RefCountedTypeName(ty)::create();

namespace ArchitectureSpace {


    template <class PointerBaseType>
    class RefCountedPointer {
    private:
        int* references;
        PointerBaseType* pointer;
    public:
        RefCountedPointer() { references = NULL; pointer = NULL; }

        RefCountedPointer(int* iReferences, PointerBaseType* iPointer)
            : references(iReferences), pointer(iPointer) {
            retain();
        }

        RefCountedPointer(const RefCountedPointer& other) {
            references = NULL; pointer = NULL;
            *this = other;
        }

        RefCountedPointer& operator = (const RefCountedPointer& other) {
            if (this != &other) {
                release();
                references = other.references;
                pointer = other.pointer;
                retain();
            }

            return *this;
        }

        RefCountedPointer(PointerBaseType* iPointer) : pointer(iPointer) {
            references = (int*) malloc(sizeof(*references));
            *references = 1;
        }

        RefCountedPointer& retain() {
            if (references) { (*references)++; }

            return *this;
        }

        void release() {
            if (references) {
                if (--(*references) == 0) {
                    delete pointer; pointer = NULL;
                    free(references); references = NULL;
                }
            }
        }

        PointerBaseType& operator * () const {
            //if (!pointer) { DBGX("Accessing NULL pointer"); }

            return *pointer;
        }

        PointerBaseType* operator -> () { return pointer; }
        const PointerBaseType* operator -> () const { return pointer; }

        static RefCountedPointer create() { return RefCountedPointer(new PointerBaseType()); }

        ~RefCountedPointer() { release(); }

        template <class T>
        RefCountedPointer<T> convertTo() {
            return RefCountedPointer<T>(references, dynamic_cast<T*>(pointer));//.retain();
        }

        template <class T>
        RefCountedPointer<T> retainAs() { return convertTo<T>(); }
    };



    template <class PointerBaseType>
    class ARCPointer {
    private:
        RefCountedPointer<PointerBaseType> refCountedPointer;
    public:
        ARCPointer() { }
        ARCPointer(ARCPointer& other) { *this = other; }
        ARCPointer(PointerBaseType* iPointer) : refCountedPointer(RefCountedPointer<PointerBaseType>(iPointer)) { }

        ARCPointer& operator = (const ARCPointer& other) {
            if (this != &other) {
                refCountedPointer.release();
                refCountedPointer = other.refCountedPointer;
                refCountedPointer.retain();
            }

            return *this;
        }

        PointerBaseType* operator * () const { return *refCountedPointer; }
    };

}

#endif // REFCOUNTEDPOINTERS_HPP
