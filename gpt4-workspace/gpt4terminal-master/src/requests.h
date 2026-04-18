#ifndef REQUESTS_H
#define REQUESTS_H

#include <curl/curl.h>

static char REQUEST_ERROR[CURL_ERROR_SIZE];

struct ResponseMemory{
    size_t size;
    char *data;
};

size_t write_callback(void* incoming_data, size_t size_member, size_t q_members, void* response_memory);

char* make_request(char* url, size_t q_headers, char** header_values, char* data);
    
#endif
