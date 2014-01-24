#include "Persistent.hpp"

#include "CommonMath.hpp"

#include <exception>
#include <cstring>
#include <cstdio>

namespace ArchitectureSpace {

    class IOException : public std::exception {
    private:
        streng msg;
    public:
        IOException(streng iMsg = "") : msg(iMsg) { }
        ~IOException() throw () {}

        const char* what() const throw() { return msg.c_str(); }
    };

    void Streamer::transferString(streng& data, bool read) {
        int length = data.length();
        transferData(length, read);
        char tempS[length + 1];
        if (read) {
            tempS[length] = 0;
            transfer(tempS, length, read);
            data = tempS;
        } else {
            memcpy(tempS, data.c_str(), length + 1);
            transfer(tempS, length, read);
        }
    }
    
    size_t Streamer::computeSizeForString(const streng& data) {
        return sizeof(int) + data.length();
    }


    size_t FileStreamer::transfer(void* data, size_t size, bool read) {
        return read ? fread(data, size, 1, file) : fwrite(data, size, 1, file);
    }

    /// ###### MemoryStreamer #####################################

    size_t MemoryStreamer::transfer(void* data, size_t s, bool read) {
        size_t tSize = min(s, size - (position - startPosition));
        memcpy(read ? data : position, read ? position : data, tSize);
        position += tSize;
        return tSize;
    }

    /// ###### OStreamStreamer ############################################################

    size_t OStreamStreamer::transfer(void* data, size_t size, bool read) {
        if (read) {
            throw IOException("Cannot read from ostream of OStreamStreamer.");
        } else {
            //stream->clear();
            stream->write((char*) data, size);
            return (stream->fail() || stream->bad()) ? 0 : size;
        }
    }

}
