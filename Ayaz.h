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
#ifndef AYAZ_H_
#define AYAZ_H_

#include <string>
#define HAVE_STRUCT_TIMEVAL
#include <curl/curl.h>
#include <exception>

#include "GenericDefines.h"
#include "stringToString.h"

#include "DynamicMemoryBlob.h"
#include "RefCountedPointers.hpp"
#include "Memobs.hpp"
#include "Comdec.hpp"

namespace ArchitectureSpace {

    class CurlException : public std::exception {
    private:
        streng msg;
    public:
        CurlException(streng iMsg) : msg(iMsg) { }

        ~CurlException() throw() {}

        const char* what() const throw() { return msg.c_str(); }
    };

    size_t HttpGetWriteData(void *buffer, size_t size, size_t nmemb, void *userp);
    size_t HttpPostWriteData(void *buffer, size_t size, size_t nmemb, void *userp);

    class StringDictionaryItem {
    private:
        streng name, value;
    public:
        streng getName() { return name; }
        streng getValue() { return value; }

        StringDictionaryItem(streng iName, streng iValue) : name(iName), value(iValue) { }
    };

    class StringDictionary {
    private:
        std::vector<StringDictionaryItem> items;
    public:
        void addItem(StringDictionaryItem item) { items.push_back(item); }
        void addItem(streng name, streng value) { items.push_back(StringDictionaryItem(name, value)); }

        streng getUrlQueryString();

        unsigned int count() { return items.size(); }

        StringDictionaryItem operator[] (int index) const { return items[index]; }
    };

    class Url {
    private:
        streng base;
        StringDictionary parameters, values;
    public:
        Url(streng iBase) : base(iBase) { }

        static streng decode(streng& escapedString) {
            streng ret;
            char ch;
            unsigned int i, ii;
            for (i = 0; i < escapedString.length(); i++) {
                if (int(escapedString[i]) == 37) {
                    sscanf(escapedString.substr(i + 1, 2).c_str(), "%x", &ii);
                    ch = static_cast<char>(ii);
                    ret += ch;
                    i = i + 2;
                } else {
                    ret += escapedString[i];
                }
            }

            return ret;
        }

        void addParameter(streng name, streng value) { parameters.addItem(name, value); }

        streng getParameterQueryString() { return parameters.getUrlQueryString(); }

        streng asStreng() { return base + (const char*) "?" + getParameterQueryString(); }
    };

    class ResourceRetriever {
    public:
        virtual DynamicMemoryBlob* retrieveData(bool transferOwnership) = 0;
    };

    Klass(CurlHandle) {
    private:
        CURL* h;
    public:
        CurlHandle() : h(curl_easy_init()) { }
        virtual ~CurlHandle() { curl_easy_cleanup(h); }

        CURL* getHandle() { return h; }
    };

    class HttpRequest {
        friend int httpRequestProgressFunction(HttpRequest* httpRequest, double dltotal, double dlnow, double ultotal, double ulnow);
    protected:
        double downloadedTotal, downloadedNow, uploadedTotal, uploadedNow;

        void checkError(int code, streng msg) {
            if (code != 0) throw CurlException(formatString("[Curl error %i in '%s']", code, msg.c_str()));
        }

        void activateProgress();
        void onProgress(double nDownloadedTotal, double nDownloadedNow, double nUploadedTotal, double nUploadedNow) {
            downloadedTotal = nDownloadedTotal;
            downloadedNow = nDownloadedNow;
            uploadedTotal = nUploadedTotal;
            uploadedNow = nUploadedNow;
        //    DBGX("dt %f : dn %f : ut %f : un %f", downloadedTotal, downloadedNow, uploadedTotal, uploadedNow);
        }

    public:
        HttpRequest(streng iUrl, HttpRequest* templateRequest = NULL) {
            if (templateRequest) curlHandleR = templateRequest->curlHandleR;
        }

        CurlHandleR curlHandleR;

        streng errMsg;

        double getDownloadedTotal() { return downloadedTotal; }
        double getDownloadedNow() { return downloadedNow; }
        double getUploadedTotal() { return uploadedTotal; }
        double getUploadedNow() { return uploadedNow; }

        void enableBasicAuth(streng username) {
            curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_USERNAME, username.c_str());
        }

        void enablePwdAuth(streng username, streng pwd) {
            curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_USERPWD, (username + ":" + pwd).c_str());
        }

        void prepareCurlHandle() {
            if (!&*curlHandleR) curlHandleR = CurlHandleR::create();
        }
    };

    size_t httpPutUploadFunction(char *bufptr, size_t size, size_t nitems, void *userp);



    class HttpGet : public ResourceRetriever, public HttpRequest {
    private:
        streng url;
        bool owner;
        DynamicMemoryBlob* data;
    public:
        int result;

        HttpGet(streng iUrl, HttpRequest* iTemplateHttpRequest = NULL, bool autoperform = true) : HttpRequest(iUrl, iTemplateHttpRequest), url(iUrl) {
            data = NULL;
            owner = true;

            prepareCurlHandle();

            if (autoperform) perform();
        }

        ~HttpGet() {
            if (owner) delete data;
        }

        DynamicMemoryBlob* retrieveData(bool transferOwnership) {
            owner = owner && !transferOwnership;
            return data;
        }

        streng getDataAsString(bool transferOwnership = false) {
            return retrieveData(transferOwnership)->getDataAsString();
        }

        bool perform();

        size_t curlWriteData(void* buffer, size_t size, size_t nmemb) {
            data->addChunk((char*) buffer, size*nmemb, true);
            return size*nmemb;
        }
    };

    class HttpPost : public HttpRequest {
    private:
        streng url;
        StringDictionary parameters, headers;
        bool owner;
        DynamicMemoryBlob* data;
        MobFolderR uploadDataFolderR;
    public:
        HttpPost();
        HttpPost(streng iUrl, HttpRequest* iTemplateHttpRequest = NULL);
        void addParameter(streng name, streng value) { parameters.addItem(StringDictionaryItem(name, value)); }
        void addHeader(streng name, streng value) { headers.addItem(StringDictionaryItem(name, value)); }
        bool perform();

        size_t curlWriteData(void* buffer, size_t size, size_t nmemb);
//        size_t curlReadData(void* buffer, size_t size, size_t nmemb);

        streng getParameterQueryString() { return parameters.getUrlQueryString(); }

        DynamicMemoryBlob* retrieveData(bool transferOwnership) {
            owner = owner && !transferOwnership;
            return data;
        }

        StringDictionary& getParameters() { return parameters; }

        void setUploadData(const MobFolderR& nUploadDataFolderR);
    };

}

#endif // HTTPTRANSFERS_H
