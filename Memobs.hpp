#ifndef MEMOBS_HPP
#define MEMOBS_HPP

#include <cstring>

#include "GenericDefines.h"
#include "RefCountedPointers.hpp"
#include "Persistent.hpp"
#include "CommonMath.hpp"

namespace ArchitectureSpace {

    class Mob : public StreamablePersistent {
    public:
        virtual size_t getSize() const = 0;
        virtual ~Mob() { }
    };

    typedef RefCountedPointer<Mob> MobR;

    Klass(Memob) : public Mob {
    public:
        void* pointer;
    private:
        size_t size;
    public:
        Memob(void* iPointer, size_t iSize) : pointer(iPointer), size(iSize) { }
        Memob() : pointer(NULL), size(0) { free(pointer); }

        void transferWithStreamer(Streamer* streamer, bool read) {
            unsigned long long s = size;
            streamer->transferData(s, read);
            size = s;
            if (size > 0) {
                if (read) pointer = malloc(size);
                streamer->transfer(pointer, size, read);
            }
        }

        size_t getSize() const { return size; }
        void* getData() const { return pointer; }
    };

    class CompositeMemobStreamer;

    Klass(CompositeMemob) : public Mob {
        friend class CompositeMemobStreamer;
    private:
        std::vector<MobR> mobs;
        size_t totalSize;
    public:
        CompositeMemob() { totalSize = 0; }
        virtual ~CompositeMemob() {}

        void appendMobR(const MobR& mobR) { mobs.push_back(mobR); totalSize += mobR->getSize(); }

        void transferWithStreamer(Streamer* streamer, bool read);
        size_t getSize() const { return totalSize; }
    };

    class CompositeMemobStreamer : public Streamer {
    public:
        class Position {
        private:
            CompositeMemobR compositeMemob;
        public:
            size_t mobIx;
            size_t mobPos;

            Position(CompositeMemobR iCompositeMemob, int iMobIx, size_t iMobPos)
                : compositeMemob(iCompositeMemob), mobIx(iMobIx), mobPos(iMobPos) { }

            Position& operator += (size_t increment) {
                mobPos += increment;
                while ((mobIx < compositeMemob->mobs.size()) && (compositeMemob->mobs[mobIx]->getSize() <= mobPos)) {
                    mobPos -= compositeMemob->mobs[mobIx]->getSize();
                    mobIx++;
                }
                return *this;
            }
        };
    private:
        CompositeMemobR compositeMemob;
        Position position;
    public:
        CompositeMemobStreamer(CompositeMemobR iCompositeMemob)
            : Streamer(), compositeMemob(iCompositeMemob),
              position(Position(compositeMemob, 0, 0)) {
        }
        ~CompositeMemobStreamer() {}

        size_t transfer(void* data, size_t size, bool read) {
            if (read) {
                size_t offset = 0;
                while (position.mobIx < compositeMemob->mobs.size()) {
                    Memob* memob = dynamic_cast<Memob*>(&*compositeMemob->mobs[position.mobIx]);
                    size_t mobSize = max((size_t) 0, min(size, memob->getSize() - position.mobPos));
                    if (mobSize == 0) break;
                    memcpy(((char*) data) + offset, ((char*) memob->pointer) + position.mobPos, mobSize);
                    position += mobSize;
                    size -= mobSize;
                    offset += mobSize;
                }
                return offset;
            } else {
                void* dataCopy = malloc(size);
                memcpy(dataCopy, data, size);
                compositeMemob->appendMobR(MobR(new Memob(dataCopy, size)));
                position += size;
                return size;
            }
        }
    };


}

#endif // MEMOBS_HPP
