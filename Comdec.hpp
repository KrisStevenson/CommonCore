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
#ifndef COMDEC_HPP
#define COMDEC_HPP

#include <exception>
#include <map>
#include <iostream>
#include <cstdio>

#include "GenericDefines.h"
#include "RefCountedPointers.hpp"
#include "Memobs.hpp"
#include "Persistent.hpp"

namespace ArchitectureSpace {

    class ComdecException : public std::exception {
    private:
        streng msg;
    public:
        ComdecException(streng iMsg) : msg(iMsg) { }
        ~ComdecException() throw() {}

        const char* what() const throw() { return msg.c_str(); }
    };

    enum ComdecMode { cmCompress, cmDecompress };

    class Comdec {
    private:
        int level;

        int checkError(int errorCode, streng msg);
    public:
        Comdec() { level = 9; }

        MemobR convert(ComdecMode mode, MemobR source);

    };

    Klass(MobFolder) : public StreamablePersistent {
        typedef std::map<streng, MobR> MobRMap;
        typedef std::pair<streng, MobR> KeyedMobR;
    private:
        MobRMap contents, contentsLowerKey;
    public:
        virtual ~MobFolder() {}
        typedef MobRMap::iterator iterator;
        iterator begin() { return contents.begin(); }
        iterator end() { return contents.end(); }

        int getSize() { return contents.size(); }

        void insert(const streng& key, const MobR& mobR) {
            contents.insert(KeyedMobR(key, mobR));
            streng kLower = key;
            for (size_t c = 0; c < kLower.length(); ++c) kLower[c] = tolower(kLower[c]);
            contentsLowerKey.insert(KeyedMobR(kLower, mobR));
        }

        MobR find(streng& key, bool caseSensitive = true) {
            MobRMap::iterator it = contents.find(key);
            if (it != contents.end()) return it->second;
            if (!caseSensitive) {
                streng kLower = key;
                for (size_t c = 0; c < kLower.length(); ++c) kLower[c] = tolower(kLower[c]);
                it = contentsLowerKey.find(kLower);
                if (it != contentsLowerKey.end()) return it->second;
            }
            return MobR();
        }

        bool exists(streng& key) const { return contents.find(key) != contents.end(); }

        void transferWithStreamer(Streamer* streamer, bool read) {
            //unsigned char dat[64]; streamer->transfer(dat, 64, read);
            int count = contents.size();
            streamer->transferData(count, read);
            DBGX("Count = %i", count);

            if (read) {
                MobRMap::iterator lastInsertPos;
                for (int c = 0; c < count; ++c) {
                    streng key;
                    streamer->transferString(key, read);
                    DBGX("Key = %s", key.c_str());
                    MemobR mobR = MemobR(new Memob());
                    mobR->transferWithStreamer(streamer, read);
                    MobR mR = mobR.retainAs<Mob>();
                    KeyedMobR item(key, mR);
                    lastInsertPos = (c > 0) ? contents.insert(lastInsertPos, item) : contents.insert(item).first;
                    streng kLower = key;
                    for (size_t c = 0; c < kLower.length(); ++c) kLower[c] = tolower(kLower[c]);
                    contentsLowerKey.insert(KeyedMobR(kLower, mR));
                }
            } else {
                for (MobRMap::iterator it = contents.begin(); it != contents.end(); ++it) {
                    streng key = it->first;

		    streamer->transferString(key, read);
                    it->second->transferWithStreamer(streamer, read);
                }
            }
        }
    };


}

#endif // COMDEC_HPP
