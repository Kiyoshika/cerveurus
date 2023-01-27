#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string.h>
#include <stddef.h>

#define HTTP_STATUS_CODE_LEN 51
#define HTTP_RESPONSE_BODY_LEN 4096
#define HTTP_URL_LEN 501

// forward declaration
struct SortedArray;
struct Route;

// note: please keep these in the same order as _status_code_text
// as that's the corresponding lookup table
enum http_status_code_e
{
	OK = 0,
	CREATED,
	BAD_REQUEST,
	UNAUTHORIZED,
	FORBIDDEN,
	NOT_FOUND,
	INTERNAL_ERROR,
	NOT_IMPLEMENTED
};

enum http_request_type_e
{
	POST,
	GET,
	DELETE
};

// defined in HTTP_Server.c
extern const char _status_code_text[8][50];

typedef struct HTTP_Server {
	int client_socket;
	int socket;
	int port;
	enum http_request_type_e request_type;
	char request_url[HTTP_URL_LEN];
	char status_code[HTTP_STATUS_CODE_LEN];
	char* response_body;
	char* request_body;
	struct SortedArray* params;
	struct SortedArray* headers;
	struct Route* routes;
} HTTP_Server;

void http_init(HTTP_Server* http_server, int port);

// main loop to listen to client requests.
// server can be killed with Ctrl+C and memory
// will be deallocated
void http_listen(HTTP_Server* http_server);

void http_free(HTTP_Server* http_server);

void http_set_status_code(
		HTTP_Server* const http_server, 
		const enum http_status_code_e);

void http_set_response_body(
		HTTP_Server* const http_server,
		const char* body);


#endif
