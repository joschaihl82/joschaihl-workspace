#include "querydata.h"
#include <stdlib.h>
#include <cjson/cJSON.h>

char* mountQueryData(char* content_value,  char* model_value){
    if(content_value == NULL ||  model_value == NULL)
	return NULL;
    cJSON* data = cJSON_CreateObject();
    cJSON* model = cJSON_CreateString(model_value);
    cJSON_AddItemToObject(data, "model", model);
    cJSON* messages = cJSON_CreateArray();
    cJSON_AddItemToObject(data, "messages", messages);
    cJSON* message = cJSON_CreateObject();
    cJSON_AddItemToArray(messages, message);
    cJSON* role = cJSON_CreateString("user");
    cJSON* content = cJSON_CreateString(content_value);
    cJSON_AddItemToObject(message, "role", role);
    cJSON_AddItemToObject(message, "content", content);
    return cJSON_Print(data);
}

int getQueryTokenUsage(char *response_data){
    cJSON* response = cJSON_Parse(response_data);
    cJSON* usage = cJSON_GetObjectItem(response, "usage");
    cJSON* total_tokens = cJSON_GetObjectItem(usage, "total_tokens");
    if(total_tokens == NULL){
	return -1;
    }
    return total_tokens->valueint;
}

char* getQueryAnswer(char *response_data){
    cJSON* response = cJSON_Parse(response_data);
    cJSON* choices = cJSON_GetObjectItem(response, "choices");
    cJSON* item = cJSON_GetArrayItem(choices, 0);  
    cJSON* message = cJSON_GetObjectItem(item, "message");
    cJSON* content = cJSON_GetObjectItem(message, "content");
    if(content == NULL)
	return NULL;
    return content->valuestring;
}	

char* getQueryError(char* response_data){
    cJSON* response = cJSON_Parse(response_data);
    cJSON* error = cJSON_GetObjectItem(response, "error");
    cJSON* message = cJSON_GetObjectItem(error, "message");
    if(message == NULL)
	return NULL;
    return message->valuestring;
}


