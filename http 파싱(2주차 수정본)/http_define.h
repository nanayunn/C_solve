#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

/*할당하는 메모리 최소, 최대값*/
#define MIN_MEMORY_SIZE 0
#define MAX_MEMORY_SIZE 1024

/*
헤더 리스트 구현 최대값
파일 호출 최대 시도 횟수*/
#define MAX_LIST_SIZE 100
#define MAX_ATTEMPT 10
#define MAX_FILE_NAME_SIZE 64

#define ARGC_NUM 2

#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define FAIL 1
#define ERROR -1

#define SP ' '
#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"
#define Q_MARK '?'
#define HASH_MARK '#'
#define COLON ':'


typedef struct file
{
	char * file_path[MAX_FILE_NAME_SIZE];
	char * file_pointer;

	int file_size;

}file_t;

typedef struct http_request
{
	char * request_line;
	char * header;
	char * garbage;

	int request_line_length;
	int header_line_length;
	int header_num;
	int garbage_length;

}http_request_t;

typedef struct request_line
{
	char *method;
	char *version;
	char *uri;

	int method_length;
	int uri_length;
	int version_length;

}request_line_t;

typedef struct URI
{
	char *path;
	char *query;
	char *fragment;

	int path_length;
	int query_length;
	int fragment_length;

}URI_t;

struct header
{
	char * name;
	char * value;

	int name_length;
	int value_length;

	struct header * next;
};

typedef struct List {
	struct header *head;
	unsigned int list_size;
} list;


file_t * file;
http_request_t * http_request;
request_line_t * request_line;
