#include "Memobs.hpp"

namespace ArchitectureSpace {

    /// ##### CompositeMemob ####################################################

    void CompositeMemob::transferWithStreamer(Streamer* streamer, bool read) {
        CompositeMemobStreamer cmStreamer(this);
        size_t buffSize = 8192;
        char buffer[buffSize];
        size_t transferred = 0;
        do {
            transferred = (read ? streamer : &cmStreamer)->transfer(buffer, buffSize, true);
            (!read ? streamer : &cmStreamer)->transfer(buffer, transferred, false);
        } while (transferred == buffSize);
    }

}
