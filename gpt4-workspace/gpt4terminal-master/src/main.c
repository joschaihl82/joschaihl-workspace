#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "requests.h"
#include "querydata.h"

#define OPENAI_API "https://api.openai.com/v1/chat/completions"
#define KEY_FILENAME "/.gpt4terminal"

FILE* openKeyFile(const char* mode) {
    char key_path[128];
    strcpy(key_path, getenv("HOME"));
    strcat(key_path, KEY_FILENAME);
    return fopen(key_path, mode);
}

int getKey(char* key, int len) {
    FILE* file = openKeyFile("r"); 
    if (file == NULL)
        return 0;
    fread(key, sizeof(char), len, file);
    for (int i = 0; i < len; i++) {
        if (key[i] == '\n') {
            key[i] = '\0';
        }
    }
    fclose(file);
    return 1;
}

int saveKey(char* key, int len) {
    FILE* file = openKeyFile("w+");
    if (file == NULL)
        return 0;
    fwrite(key, 1, len, file);
    fclose(file);
    return 1;
}

char* read_file_contents(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL; // Could not open the file
    }

    // Seek to the end of the file to determine its size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET); // Reset the position to the start of the file

    // Allocate memory for the file contents plus one for null terminator
    char* buffer = (char*)malloc(filesize + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL; // Could not allocate memory
    }

    // Read the file contents into the buffer
    size_t read_size = fread(buffer, 1, filesize, file);
    fclose(file); // Close the file after reading

    // Check if the read was successful
    if (read_size != filesize) {
        perror("Error reading file");
        free(buffer);
        return NULL; // Incomplete read
    }

    buffer[filesize] = '\0'; // Null-terminate the string
    return buffer; // Return the buffer containing the file contents
}

void printUsage() {
    printf("Usage:\n");
    printf("  -c            : Configure OpenAI Key\n");
    printf("  -f \"<file>\"  : Path to the file with the query text\n");
    printf("  -m \"<model>\" : Set the GPT model (the default is gpt-4o-mini)\n");
    printf("  -q \"<query>\" : Query the GPT model\n");
}

int main(int argc, char** argv) {
    int option;
    char *query = NULL; 
    char model[64] = "gpt-4o-mini";
    int configure = 0;

    // Parse command-line arguments
    while ((option = getopt(argc, argv, "f:m:q:c")) != -1) {
        switch (option) {
            case 'c':
                configure = 1;
                break;
	    case 'f':
		query = read_file_contents(optarg);
		break;
	    case 'm':
		strncpy(model, optarg, 64);
		break;
            case 'q':
                query = (char*) malloc(strlen(optarg));
		strcpy(query, optarg);	
                break;
            default:
                printUsage();
                return 1;
        }
    }

    // Handle key configuration
    if (configure) {
        printf("Enter your OpenAI Key: ");
        char key[128];
        scanf("%s", key);
        if (saveKey(key, 128)) {
            printf("Configuration done!\n");
        } else {
            printf("An error has occurred.\n");
        }
        return 0;
    }

    // Ensure a query is provided
    if (query == NULL) {
        printf("No query provided.\n");
        printUsage();
        return 1;
    }

    // Prepare headers and authenticate
    char content_type[64], key[128], auth[256];
    if (getKey(key, 128) == 0) {
        printf("OpenAI Key not found! To continue, enter your Key: ");
        scanf("%s", key);
        if (saveKey(key, 128)) {
            printf("Configuration done!\n");
        } else {
            printf("An error has occurred.\n");
            return 0;
        }
    }

    sprintf(content_type, "Content-Type: application/json");
    sprintf(auth, "Authorization: Bearer %s", key);
    char* headers[] = {content_type, auth}; 

    // Prepare and send query
    char* data = mountQueryData(query, model);
    char* response = make_request(OPENAI_API, 2, headers, data);

    if (response != NULL) {
        char* content = getQueryAnswer(response);
        if (content != NULL) {
            printf("CHATGPT (%s):\n\n%s\n\n", model, content);
            int total_tokens = getQueryTokenUsage(response);
            if (total_tokens >= 0)
                printf("Tokens used: %d\n", total_tokens);
        } else {
            char* error_message = getQueryError(response);
            if (error_message != NULL)
                printf("ERROR:\n\n%s\n", error_message);
            else
                printf("ERROR:\n\n%s\n", response);
        }
    } else {
        printf("%s\n", REQUEST_ERROR);
    }

    free(query);

    return 0;
}
