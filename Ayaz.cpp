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

#include "Ayaz.h"
#include "math.h"

#ifdef _QT_
#include <QDebug>
#endif

#include <CommonCore.git/stringToString.h>

namespace ArchitectureSpace {

size_t HttpGetWriteData(void *buffer, size_t size, size_t nmemb, void *userp) { return ((HttpGet*) userp)->curlWriteData(buffer, size, nmemb); }
size_t HttpPostWriteData(void *buffer, size_t size, size_t nmemb, void *userp) { return ((HttpPost*) userp)->curlWriteData(buffer, size, nmemb); }
//size_t HttpPostReadData(void *buffer, size_t size, size_t nmemb, void *userp) { return ((HttpPost*) userp)->curlReadData(buffer, size, nmemb); }

//---------------------------------------------------------------------------------------------------------------

streng StringDictionary::getUrlQueryString() {
    streng result;
    for (int k = 0; k < (int) items.size(); ++k) {
      result += (k == 0) ? "" : "&";
      result += items[k].getName() + streng("=") + items[k].getValue();
    }
    return result;
}

    /// ##### HttpGet ###########################################################################

    bool HttpGet::perform() {
        data = new DynamicMemoryBlob();
        curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_WRITEFUNCTION, HttpGetWriteData);
        curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_WRITEDATA, this);
        checkError(result = curl_easy_perform(curlHandleR->getHandle()), "curl_easy_perform");
        return true;
    }

    /// ###### HttpPost #############################################################################

    HttpPost::HttpPost(streng iUrl, HttpRequest* iTemplateHttpRequest) : url(iUrl), HttpRequest(iUrl, iTemplateHttpRequest) {
        data = NULL;
        owner = true;
    }

    HttpPost::HttpPost() : url(""), HttpRequest("", NULL) {
        data = NULL;
        owner = true;
    }

    size_t HttpPost::curlWriteData(void* buffer, size_t size, size_t nmemb) {
        data->addChunk((char*) buffer, size*nmemb, true);
        return size*nmemb;
    }

    /*size_t HttpPost::curlReadData(void* buffer, size_t size, size_t nmemb) {
        //        data.addChunk((char*) buffer, size*nmemb, true);
        DBGX("(Uploading %i bytes)", (int) (size*nmemb));
        return uploadDataStreamer->transfer(buffer, size * nmemb, true);
    //    streng r(size*nmemb, ' ');
      //  for (int q = 0; q < (int) (size*nmemb); ++q) r[q] = ((char *) buffer)[q];
     //   DBGX("[Post response = %s]", data->getDataAsString().c_str());
    }*/

    int httpRequestProgressFunction(HttpRequest* httpRequest, double dltotal, double dlnow, double ultotal, double ulnow) {
        httpRequest->onProgress(dltotal, dlnow, ultotal, ulnow);
      //  DBGX("Progress %f %f %f %f: ", dltotal, dlnow, ultotal, ulnow);
        return 0;
    }

    void HttpRequest::activateProgress() {
        curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_PROGRESSDATA, this);
        curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_PROGRESSFUNCTION, &httpRequestProgressFunction);
    }

    bool HttpPost::perform() {
        CURLcode res;
        data = new DynamicMemoryBlob();

        struct curl_httppost *formpost=NULL;
        struct curl_httppost *lastptr=NULL;

        //        curl_global_init(CURL_GLOBAL_ALL);

        /* Fill in the file upload field */
        for (unsigned int i = 0; i < parameters.count(); ++i) {
            //            curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, streng.c_str(), CURLFORM_FILE, "postit2.c", CURLFORM_END);
            DBGX("Parameter %s: %s", parameters[i].getName().c_str(), parameters[i].getValue().c_str());
            curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, parameters[i].getName().c_str(), CURLFORM_COPYCONTENTS, parameters[i].getValue().c_str(), CURLFORM_END);
        }

        prepareCurlHandle();
        activateProgress();
        if (&*curlHandleR) {
            struct curl_slist *headerlist = NULL;
            try {
            //    headerlist = curl_slist_append(headerlist, buf);
                curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_URL, url.c_str());
    //            curl_easy_setopt(curlH, CURLOPT_VERBOSE, 1);
           //     curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_VERBOSE, 1);
                curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_WRITEFUNCTION, HttpPostWriteData);
                curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_WRITEDATA, this);
//                curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_POSTFIELDS, parameters.getQueryString().c_str());
  //              curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_POST, 1);
                if (&*uploadDataFolderR) {
                    headers.addItem("Content-Type", "multipart/form-data");
                    for (MobFolder::iterator it = uploadDataFolderR->begin(); it != uploadDataFolderR->end(); ++it) {
                        streng key = it->first;
                        MemobR memobR = it->second.convertTo<Memob>().retain();
                        DBGX("Uploading mobfolder item with key %s, size %i", key.c_str(), memobR->getSize());
                        curl_off_t buffSize = memobR->getSize();
                        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, key.c_str(), CURLFORM_BUFFER, key.c_str(),
                                     CURLFORM_BUFFERLENGTH, buffSize, CURLFORM_BUFFERPTR, memobR->getData(), CURLFORM_END);
                    }
                }
                curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_HTTPPOST, formpost);

                for (unsigned int i = 0; i < headers.count(); ++i) {
                    streng h = formatString("%s: %s", headers[i].getName().c_str(), headers[i].getValue().c_str());
                    //qDebug("Header = %s", h.c_str());
                    headerlist = curl_slist_append(headerlist, h.c_str());
                }
                curl_easy_setopt(curlHandleR->getHandle(), CURLOPT_HTTPHEADER, headerlist); /* only disable 100-continue header if explicitly requested */
    //            DEBOUT("[Performing]");
                checkError(res = curl_easy_perform(curlHandleR->getHandle()), "Curl HttpPost");
      //          DEBOUT("\n[Perform done]");
        //        DBG2("[Perform done (%i)]", res);
            } catch (std::exception& e) {
                errMsg = formatString("Exception in curl operation ('%s').", e.what());
                DBGX("Exception in curl operation ('%s').", e.what());
            }

            curl_formfree(formpost);
            curl_slist_free_all(headerlist);
            return (res == CURLE_OK);
        } else { return false; }
    }

    void HttpPost::setUploadData(const MobFolderR& nUploadDataFolderR) {
        uploadDataFolderR = nUploadDataFolderR;
    }

}
