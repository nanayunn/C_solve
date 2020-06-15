#include<stdio.h>
#include<string.h>

#define MIN_MEMORY_SIZE 0
#define MAX_MEMORY_SIZE 1024

#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define FAIL 1
#define ERROR -1

#define SP ' '
#define CRLF '\r\n'
#define Q_MARK '?'
#define HASH_MARK '#'
#define COLON ': '
#define END_OF_HEADER '\r\n\r\n'

struct http_request
{
    char *request_line;
    char *header;
    char *garbage;
};

struct request_line
{
    char *method;
    char *URI;
    char *version;
};

struct URI
{
    char *path;
    char *query;
    char *fragment;
};