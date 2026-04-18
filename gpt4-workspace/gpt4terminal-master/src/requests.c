#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "requests.h"

size_t write_callback(void* incoming_data, size_t size_member, size_t q_members, void* response_memory){
    size_t real_size = size_member * q_members;
    struct ResponseMemory *memory = (struct ResponseMemory *) response_memory;
    char *reallocated_data = realloc(memory->data, memory->size + real_size  + 1);
    if(reallocated_data == NULL)
	return 0;
    memory->data = reallocated_data;
    memcpy(&(memory->data[memory->size]), incoming_data, real_size);
    memory->size += real_size;
    memory->data[memory->size] = '\0';
    return real_size;
}

char* make_request(char* url, size_t q_headers, char** header_values, char* data){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* handle = curl_easy_init();
    if(handle == NULL)
	return NULL;
    struct ResponseMemory response_memory = {0};
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&response_memory);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, REQUEST_ERROR);

    struct curl_slist *headers = NULL;
    if(header_values != NULL){
	for(size_t i = 0; i < q_headers; i++){
	    if(header_values[i] != NULL){
		headers = curl_slist_append(headers, header_values[i]);
	    }
	}
	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
	if(data != NULL){
	    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
	}
    }
    char *response_data = NULL;
    CURLcode response_status = curl_easy_perform(handle);
    if(response_status == CURLE_OK){
	response_data = response_memory.data;
    }
    curl_slist_free_all(headers);
    curl_easy_cleanup(handle);
    curl_global_cleanup();
    return response_data;;
}


