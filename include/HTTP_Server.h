#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string.h>
#include <stddef.h>

#define HTTP_STATUS_CODE_LEN 51
#define HTTP_RESPONSE_BODY_LEN 4096
#define HTTP_URL_LEN 501

// forward declaration
struct SortedArray;
struct CallbackArgs;

// note: please keep these in the same order as _status_code_text
// as that's the corresponding lookup table
enum http_status_code_e
{
	OK = 0,
	CREATED,
	ACCEPTED,
	NO_CONTENT,
	FOUND,
	NOT_MODIFIED,
	BAD_REQUEST,
	UNAUTHORIZED,
	FORBIDDEN,
	NOT_FOUND,
	METHOD_NOT_ALLOWED,
	NOT_ACCEPTABLE,
	REQUEST_TIMEOUT,
	UNSUPPORTED_MEDIA_TYPE,
	INTERNAL_ERROR,
	NOT_IMPLEMENTED,
	INSUFFICIENT_STORAGE
};

enum http_request_type_e
{
	POST,
	GET,
	DELETE
};

// defined in HTTP_Server.c
extern const char _status_code_text[17][50];

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

// Prepare the response body to send to client
void http_prepare_response(
		HTTP_Server* const http_server);

// Add a route that renders a template
// (always a GET request)
void http_add_route_template(
		HTTP_Server* const http_server,
		char* route_path,
		char* template_file_name);

// Add a route that renders a static file (e.g., css or js)
// (always a GET request and not meant to be fetched explicitly by user)
void http_add_route_static(
		HTTP_Server* const http_server,
		char* static_file_name);

// Add a route that handles a custom callback
// One route can have multiple request types at once (GET/POST).
void http_add_route_api(
		HTTP_Server* const http_server,
		char* route_path,
		void* user_data,
		void (*user_data_dealloc)(void*),
		char* (*get_callback)(struct CallbackArgs * const args),
		void (*post_callback)(struct CallbackArgs * const args),
		void (*delete_callback)(struct CallbackArgs * const args));

// Add route that handles GET request
void http_add_route_GET(
		HTTP_Server* const http_server,
		char* route_path,
		void* user_data,
		void (*user_data_dealloc)(void*),
		char* (*get_callback)(struct CallbackArgs * const args));

// Add route that handles POST request
void http_add_route_POST(
		HTTP_Server* const http_server,
		char* route_path,
		void* user_data,
		void (*user_data_dealloc)(void*),
		void (*post_callback)(struct CallbackArgs * const args));

// Add route that handles DELETE request
void http_add_route_DELETE(
		HTTP_Server* const http_server,
		char* route_path,
		void* user_data,
		void (*user_data_dealloc)(void*),
		void (*delete_callback)(struct CallbackArgs * const args));

// cleanup memory allocated from server
void http_free(HTTP_Server* http_server);

#endif
