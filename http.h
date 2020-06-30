#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h> 

/*
헤더 리스트 구현 최대값*/
#define MAX_HEADER_SIZE 1024 /*기본값 256, apache 기준 최댓값 20000..*/
#define MAX_FILE_DATA 2048
#define ARGC_NUM 2

#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERROR -1

#define SP ' '
#define HT '\t' 
#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"
#define Q_MARK '?'
#define HASH_MARK '#'
#define COLON ':'

typedef struct
{
	char file_data[MAX_FILE_DATA];
	int written_size;

}file_t;

typedef struct
{
	struct request_line;
	char * header;
	char * garbage;

}http_request_t;

typedef struct
{	
	char *path;
	char *query;
	char *fragment;

}URI_t;

typedef struct
{
	char *method;
	char *version;
	char *uri_pointer;

}request_line_t;

typedef struct{

	int header_line_length;
	int garbage_length;

	int path_length;
	int query_length;
	int fragment_length;

	int uri_length;
	int method_length;
	int version_length;

}length_t;


typedef struct{
	char * name;
	char * value;

	int name_length;
	int value_length;
	struct header * next;
}header_t;


typedef struct{
	struct header *head_location;
	int list_num;
}header_start_to_end_t;
