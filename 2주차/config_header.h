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

#define MAX_HTTP_FILE_NAME 1024
#define MAX_TYPE_LENGTH 4
#define MAX_MATCH_LENGTH 5
#define MIN_MATCH_LENGTH 3
#define MAX_SERVER_NUM 2048
#define MAX_PORT_LENGTH 6
#define MAX_TEMP_BUF_SIZE 6


#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERROR -1
#define ERROR_MEMCHR -2
#define SEARCH_FAIL -2

#define CONTINUE 1
#define EXIT 2

#define SP ' '
#define HT '\t' 
#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"
#define Q_MARK '?'
#define HASH_MARK '#'
#define COLON ':'
#define COMMA ','

#define UPPER_TO_LOWER 32

enum type{
	PATH = 0,
	HOST = 1,
	ELSE_TYPE = 3
};

enum match{
	ANY = 0,
	START = 1,
	END = 2,
	ELSE_MATCH = 3
};

struct file
{
	char * file_pointer;
	int file_size;

};



struct config_line
{
    char * type_value;

    int type;
    int match;

    int type_value_length;
    int server_call_num;
    int total_server_count;
    
    struct server * server_list[MAX_SERVER_NUM];
    struct config_line * next;

};


struct server
{
    char * ip;
    char port[MAX_PORT_LENGTH];

    int ip_length;
};

struct  config_list
{
	struct config_line * config_line_start;

	int config_total_cnt;
};

struct http_compare_point
{
	char * path;
	char * host;

	int path_length;
	int host_length;
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


int config_main(char * file_path);
int parse_config_line(struct file * config_file, struct config_list * config_list);
int get_type(char * temp_buff, int check_size);
int get_match(char * temp_buff, int check_size);

int get_parsing_length(char * start_line, int full_length, int parse_std_word);

int parse_http_request_file(char * file_path, struct http_compare_point * http_compare_point);
int parse_header( struct http_request * http_request, struct header_list * header_list );
int parse_request_line(struct request_line * request_line, struct URI * URI, char * request_line_start, int request_line_length);
int parse_uri(struct URI * URI);

int clean_up(struct http_request * http_request, struct request_line * request_line, struct URI * URI, struct header_list * header_list);
int free_header(struct header_list * header_list);
int free_config_line(struct config_list * config_list);
int clean_up_all(struct http_compare_point * http_compare_point, struct config_list * config_list, struct file *config_file);

int print_header(struct header_list * header_list);
int print_config(struct config_list * config_list);

int compare_and_lb(struct config_list * config_list, struct http_compare_point * http_compare_point);
int is_it_match(char * type_value, char * path_of_host_pointer, int type_value_length, int path_host_length ,int match_word);





