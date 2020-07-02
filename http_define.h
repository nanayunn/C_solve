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
#define BREAK_LOOP_NUM 512 /*기본값 256, apache 기준 최댓값 20000.. 두배로 설정*/
#define MAX_TRY_FREE_NUM 5
#define ARGC_NUM 2

#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERROR -1

#define SP ' '
#define HT '\t' 
#define CR '\r'
#define LF "\n"
#define CRLF "\r\n"
#define Q_MARK '?'
#define HASH_MARK '#'
#define COLON ':'
#define NONE 'x'

struct file
{
	char * file_pointer;
	int file_size;

};

struct http_request
{
	struct request_line;
	char * header;
	char * garbage;

	int header_line_length;
	int garbage_length;

};

struct request_line
{
	char *method;
	char *version;
	struct URI;

	int method_length;
	int version_length;

};

struct URI
{	
	char *uri_pointer;
	char *path;
	char *query;
	char *fragment;

	int uri_length;
	int path_length;
	int query_length;
	int fragment_length;

};


struct header{
	char * name;
	char * value;

	int name_length;
	int value_length;
	struct header * next;
};


struct header_list{
	struct header * header_head;
	int header_num;
};

void getchar_clear(void);
int get_file_size(char *file_path);
int read_file(char * file_path, struct file * file); 

char * get_parsing_length(char * start_line, int * full_length, int * parsed_thing_length, int parse_std_word);

int parse_request_line(struct request_line * request_line, struct URI * URI, char * request_line_start, int request_line_length);
int parse_uri(struct URI * URI);
int parse_http_request_file(struct file * file);
char * parse_header( struct http_request * http_request);

int free_header(struct header_list * header_list);
int print_header(struct header_list * header_list);
int print_all(struct http_request * http_request, struct request_line * request_line, struct URI * URI, struct header_list * header_list);
int clean_up(struct http_request * http_request, struct request_line * request_line, struct URI * URI, struct header_list * header_list);