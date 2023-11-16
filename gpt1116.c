#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

/* Resizable buffer */
typedef struct {
    char* buf;
    size_t size;
} Memory;

size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    Memory* mem = (Memory*)userp;

    char* ptr = realloc(mem->buf, mem->size + realsize + 1);
    if (!ptr) {
        fprintf(stderr, "Memory allocation error\n");
        return 0;
    }

    mem->buf = ptr;
    memcpy(&(mem->buf[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->buf[mem->size] = 0;

    return realsize;
}

void scrape_page(const char* base_url, const char* path) {
    char url[256]; // Adjust the size according to your needs
    snprintf(url, sizeof(url), "%s/%s", base_url, path);

    CURL* curl;
    CURLcode res;
    Memory chunk;

    chunk.buf = malloc(1); // Allocate initial buffer
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else {
            // HTML parsing using libxml2
            htmlDocPtr doc = htmlReadMemory(chunk.buf, chunk.size, url, NULL, HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);

            if (doc) {
                // Use XPath to select and print content within all <p> tags
                xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
                if (xpathCtx) {
                    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(BAD_CAST("//p/text()"), xpathCtx);
                    if (xpathObj) {
                        xmlNodeSetPtr nodes = xpathObj->nodesetval;
                        if (nodes) {
                            for (int i = 0; i < nodes->nodeNr; ++i) {
                                xmlNodePtr node = nodes->nodeTab[i];
                                xmlChar* content = xmlNodeGetContent(node);
                                if (content) {
                                    printf("Content within <p> tags on %s:\n%s\n", url, content);
                                    xmlFree(content);
                                }
                            }
                        }
                        xmlXPathFreeObject(xpathObj);
                    }
                    xmlXPathFreeContext(xpathCtx);
                }

                // Continue with your other processing if needed...

                xmlFreeDoc(doc);
            }
        }

        curl_easy_cleanup(curl);
    }

    free(chunk.buf);
    curl_global_cleanup();
}

void crawl_all_subdirectories(const char* base_url, const char* path) {
    scrape_page(base_url, path);

    // Continue crawling recursively for subdirectories
    // You need to implement logic to extract links or paths from the current page's content
    // and call crawl_all_subdirectories for each of them.
    // For simplicity, let's assume that subdirectories are found in the form of "/subdir1", "/subdir2", etc.
    // Adjust this logic based on the actual structure of the pages you are crawling.
    for (int i = 1; i <= 3; ++i) {
        char subdir_path[256]; // Adjust the size according to your needs
        snprintf(subdir_path, sizeof(subdir_path), "/subdir%d", i);
        crawl_all_subdirectories(base_url, subdir_path);
    }
}

int main(void) {
    const char* base_url = "https://fun.ssu.ac.kr/ko/notige2/info/view";
    crawl_all_subdirectories(base_url, "/2773?p=1");

    return 0;
}
