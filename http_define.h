#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>

/*할당하는 메모리 최소, 최대값*/
#define MIN_MEMORY_SIZE 0
#define MAX_MEMORY_SIZE 1024

/*
헤더 리스트 구현 최대값
파일 호출 최대 시도 횟수*/
#define MAX_LIST_SIZE 100
#define MAX_ATTEMPT 10

#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define FAIL 1
#define ERROR -1

#define SP ' '
#define CR '\r'
#define LF '\n'
#define CRLF '\r\n'
#define Q_MARK '?'
#define HASH_MARK '#'
#define COLON ': '
#define END_OF_HEADER '\r\n\r\n'



/*
typedef struct file
{
    unsigned char file;

    int file_size;
}file_t;
*/

typedef struct temp_data
{
    unsigned char empty_char;

    int empty_char_length;

}temp_data_t;

typedef struct http_request
{
    struct request_line request_line;
    list header;
    unsigned char *garbage;

    int request_line_length;
    int garbage_length;
    
}http_request_t;

typedef struct request_line
{
    unsigned char *method;
    unsigned char *URI;
    unsigned char *version;

    int method_length;
    int URI_length;
    int version_length;

}request_line_t;

typedef struct URI
{
    unsigned char *path;
    unsigned char *query;
    unsigned char *fragment;

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


/*함수의 원형 선언 부분*/

/*http request data의 각 부분 초기화*/
int init(struct http_request *http_request_t);

/*파일의 경로를 입력받고, 읽기까지 필요한 함수.*/
char * input_file_path(int *file_size);
char * set_file_read(char *file_path, int *file_size);
int get_file_size(char *file_path);
char * file_read(char *file_path, int file_size);

/*파싱에 필요한 함수*/
/* request_line, header, garbage를 구분하기 위한 함수 */
int seperate_http_request(char *file_start_pointer, struct http_request *http_request_t, int file_size);
char * this_is_request_line(char *file_start_pointer, struct http_request *http_request_t, int file_size);
char * this_is_header(char *file_start_pointer, struct http_request *http_request_t, int file_size);
char * this_is_garbage(char *file_start_pointer, struct http_request *http_request_t, int file_size);

/* req_line_parsing.c */
int parse_requset_line(char *start, struct http_request *http_request_value, int line_length);
char* parse_to_method(char *start, struct http_request *http_request_value, int *line_length);
char* parse_to_uri(char *start, struct http_request *http_request_value, int *line_length);
char* parse_to_path(char *start, struct http_request *http_request_value, int *line_length, int *uri_parsing_flag);
char* parth_to_query(char *start, struct http_request *http_request_value, int *line_length, int *uri_parsing_flag);
char* parse_to_fragment(char *start, struct http_request *http_request_value, int *line_length, int *uri_parsing_flag);
char* parse_to_version(char *start, struct http_request *http_request_value, int *line_length);