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
