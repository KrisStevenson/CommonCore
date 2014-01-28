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
#include "stringToString.h"

#include <stdarg.h>
#include <math.h>
#include <cstdio>

namespace ArchitectureSpace {

streng stringFromBinaryData(const void* data, size_t length) {
    streng result;
    result.replace(0, streng::npos, (const char*) data, length);
    return result;
}

#ifdef APPLE
NSString* stos(std::string s) {
    NSString* result = [NSString stringWithUTF8String:s.c_str()];
    return (result != nil) ? result : @"";
}
//NSString* stos(std::string s) { return [[NSString stringWithFormat:@"%s", s.c_str()] retain]; }
streng stos(NSString* s) { return [s UTF8String]; }
#endif

#ifdef _QT_
streng stos(QString s) { return s.toStdString(); }
QString stos(streng s) { return QString::fromUtf8(s.c_str()); }
#endif

    streng intToStr(long long i) { return formatString("%lli", i); }
    streng uintToStr(u_int64_t i) { return formatString("%llu", i); }

    std::vector<streng> explode(streng s, streng separator) {
        std::vector<streng> result;

        size_t p;
        while ((p = s.find(separator)) != streng::npos) {
            result.push_back(s.substr(0, p));
            s = s.substr(p + separator.length());
        }

        result.push_back(s);

        return result;
    }

    streng formatString(const char* format, ...) {
        va_list args;
        va_start (args, format);
        
        size_t buffSize = vsnprintf(NULL, 0, format, args) + 1;
        char* buffer = new char[buffSize];
        va_end (args);
        va_start (args, format);
        vsnprintf(buffer, buffSize, format, args);
        streng result = buffer;
        delete [] buffer;
        
        va_end (args);
        
        return result;
    }
    
    streng formatString(streng format, ...) {
        va_list args;
        va_start (args, format);
        
        size_t buffSize = vsnprintf(NULL, 0, format.c_str(), args) + 1;
        char* buffer = new char[buffSize];
        va_end (args);
        va_start (args, format);
        vsnprintf(buffer, buffSize, format.c_str(), args);
        streng result = buffer;
        delete [] buffer;

        va_end (args);
        
        return result;
    }
    
    bool unreservedChar(unsigned char c) {
        return (('0' <= c) && (c <= '9')) || (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'))
               || (c == '-') || (c == '.') || (c == '_') || (c == '~');
    }

    streng urlEncode(streng s) {
        char hex[] = "0123456789ABCDEF";

        streng result;
        for (unsigned int c = 0; c < s.length(); ++c) {
            if (unreservedChar(s[c])) { result += s[c]; }
            else { result += streng("%") + hex[((unsigned char) s[c]) >> 4] + hex[((unsigned char) s[c]) & 0xF]; }
        }

        return result;
    }












#include <stdint.h>
#include <stdlib.h>


static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {

    *output_length = 4 * ((input_length + 2) / 3) + 1;

    char *encoded_data = (char*) malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (unsigned int i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i - 1] = '=';

    encoded_data[*output_length - 1] = 0;

    return encoded_data;
}

streng base64_encode(void *data, size_t input_length) {
    size_t output_length;
    char* result = base64_encode((const unsigned char*) data, input_length, &output_length);
    streng resultS = result;
    DBGX("resultS = %s", resultS.c_str());
    free(result);
    return resultS;
}


void build_decoding_table() {
    decoding_table = (char*) malloc(256);

    for (int i = 0; i < 256; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}

unsigned char *base64_decode(const unsigned char *data, size_t input_length, size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = (unsigned char*) malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t sextet_a = data[i] == '=' ? 0 : decoding_table[data[i]]; i++;
        uint32_t sextet_b = data[i] == '=' ? 0 : decoding_table[data[i]]; i++;
        uint32_t sextet_c = data[i] == '=' ? 0 : decoding_table[data[i]]; i++;
        uint32_t sextet_d = data[i] == '=' ? 0 : decoding_table[data[i]]; i++;

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}

void* base64_decode(streng data, size_t& outputSize) {
    return base64_decode((unsigned char*) &data[0], data.length(), &outputSize);
}


void base64_cleanup() {
    free(decoding_table);
}

}
