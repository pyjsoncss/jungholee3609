#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

// 구조체를 정의하여 libcurl에서 데이터를 저장하는 데 사용
struct CURLResponse {
    char* data;
    size_t size;
};

// libcurl 콜백 함수
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realSize = size * nmemb;
    struct CURLResponse* mem = (struct CURLResponse*)userp;

    char* ptr = realloc(mem->data, mem->size + realSize + 1);
    if (ptr == NULL) {
        fprintf(stderr, "realloc() failed\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realSize);
    mem->size += realSize;
    mem->data[mem->size] = 0;

    return realSize;
}

// libxml2에서 XPath를 이용해 HTML 문서를 파싱하는 함수
void parseHTML(const char* html) {
    htmlDocPtr doc;
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    // HTML 문서를 메모리에서 읽어와서 파싱
    doc = htmlReadMemory(html, strlen(html), NULL, "utf-8", HTML_PARSE_NOERROR | HTML_PARSE_RECOVER);
    if (doc == NULL) {
        fprintf(stderr, "HTML parsing failed\n");
        return;
    }

    // XPath 문맥 초기화
    context = xmlXPathNewContext(doc);
    if (context == NULL) {
        fprintf(stderr, "XPath context initialization failed\n");
        xmlFreeDoc(doc);
        return;
    }

    // XPath를 이용하여 특정 부분 선택 (여기서는 title 태그를 선택하는 예시)
    result = xmlXPathEvalExpression((xmlChar*)"//div[@data-role='wysiwyg-content']", context);
    if (result == NULL) {
        fprintf(stderr, "XPath evaluation failed\n");
        xmlXPathFreeContext(context);
        xmlFreeDoc(doc);
        return;
    }

    // 선택한 요소의 내용 출력
    for (int i = 0; i < result->nodesetval->nodeNr; ++i) {
        xmlNodePtr node = result->nodesetval->nodeTab[i];
        xmlChar* content = xmlNodeGetContent(node);

        // XML에서 읽어온 한글을 출력할 때, %ls를 사용하여 출력
        wprintf(L"Title: %ls\n", content);
        xmlFree(content);
    }

    // 메모리 해제
    xmlXPathFreeObject(result);
    xmlXPathFreeContext(context);
    xmlFreeDoc(doc);
}

int main(void) {
    CURL* curl;
    CURLcode res;

    // libcurl 초기화
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // libcurl 핸들 생성
    curl = curl_easy_init();
    if (curl) {
        // 가져올 URL 설정
        curl_easy_setopt(curl, CURLOPT_URL, "https://fun.ssu.ac.kr/ko/notige2/info/view/2773?p=1");

        // 콜백 함수 등록
        struct CURLResponse response;
        response.data = malloc(1);
        response.size = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);

        // HTTPS에 대한 보안 관련 설정
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // HTTP GET 요청 수행
        res = curl_easy_perform(curl);

        // 요청 실패 시 오류 출력
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else {
            // 성공한 경우 HTML 내용 출력
            wprintf(L"HTML Content:\n%hs\n", response.data);

            // HTML 파싱 및 특정 부분 선택
            parseHTML(response.data);
        }

        // 사용이 끝난 자원 정리
        free(response.data);
        curl_easy_cleanup(curl);
    }

    // libcurl 종료
    curl_global_cleanup();

    return 0;
}
