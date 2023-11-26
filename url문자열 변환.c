#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// 구조체 정의
struct MemoryStruct {
    char* memory;
    size_t size;
};

// 콜백 함수 정의
size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    // 메모리를 재할당하고 데이터를 추가
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        // 할당 실패 시 에러 처리
        fprintf(stderr, "Memory allocation error\n");
        return 0;
    }

    // 데이터를 메모리에 복사하고 사이즈 업데이트
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// 링크 추출을 위한 함수
void ExtractLinks(const char* html) {
    const char* cursor = html;

    while ((cursor = strstr(cursor, "<a")) != NULL) {
        // <a> 태그 안에서 href 속성 찾기
        const char* hrefStart = strstr(cursor, "href=\"");
        if (hrefStart == NULL) {
            break;
        }

        hrefStart += 6; // "href=\"" 다음부터 시작

        const char* hrefEnd = strchr(hrefStart, '\"');
        if (hrefEnd == NULL) {
            break;
        }

        // href 속성 값을 출력 (또는 필요한 처리 수행)
        printf("Extracted Link: %.*s\n", (int)(hrefEnd - hrefStart), hrefStart);

        // 커서를 다음 검색 위치로 이동
        cursor = hrefEnd + 1;
    }
}

// URL과 추출하고자 하는 태그를 받아와서 처리하는 함수
void parsing(const char* url, const char* tag) {
    CURL* curl;
    CURLcode res;

    // 구조체 초기화
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  // 초기화
    chunk.size = 0;

    curl = curl_easy_init();
    if (curl) {
        // 함수의 매개변수로 전달받은 URL을 사용합니다.
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // 데이터를 받아오기 위한 콜백 함수 설정
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        // 콜백 함수에 전달될 사용자 데이터 설정
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
        // example.com is redirected, so we tell libcurl to follow redirection
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        // Check for errors 
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        // 받은 HTML을 출력하거나 필요에 따라 활용합니다.
        //printf("%s\n", chunk.memory);

        // 링크 추출 함수 호출
        ExtractLinks(chunk.memory);

        // always cleanup
        curl_easy_cleanup(curl);
    }

    // 받은 HTML을 가지고 있는 문자열을 복사하여 반환합니다.
    char* result = _strdup(chunk.memory);

    // 메모리를 해제합니다.
    free(chunk.memory);
}

int main(void) {
    // parsing 함수 호출 (URL과 추출하고자 하는 태그를 변경 가능)
    parsing("https://gall.dcinside.com/mgallery/board/lists/?id=bodybuilding&exception_mode=recommend", "<a");

    return 0;
}
