#ifndef QUERY_DATA_H
#define QUERY_DATA_H

char* mountQueryData(char* content_value,  char* model_value);
char* getQueryAnswer(char* response_data);
char* getQueryError(char* response_data);
int getQueryTokenUsage(char* response_data);

#endif
