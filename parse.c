#include"http_define.h"


extern struct file * file;

/* 구조체에 저장한 file 정보를 이용하여 request_line, header, body 및 garbage의 형태로
 file을 CRLF를 기준으로 한 줄씩 읽어 파싱하는 함수

* request_line의 경우, 종료 지점 CRLF를 제외하고 그 어떤 곳에서도 CR이나 LF 사용이 금지되어 있다. 

매개변수 : X
반환값 : 성공 시 0, 에러 -1
*/
/* 각 파트에 해당하는 배열을 저장할 때 CRLF 값도 저장해주어야 하는가..! */
int parse_http_request_file(struct file * file)
{
    char * new_element_start = NULL;
    char * request_start_line = NULL;
    char * header_start_line = NULL;
    char * end_of_element = NULL;
    char * temp_pointer = NULL;

    unsigned int changed_file_size = 0;
    unsigned int request_line_length = 0;
    int written_size = 0;
    int temp_size = 0;
    int header_count = 0;
    int result = 0;

    struct http_request http_request;
    struct request_line request_line;	
    struct URI URI;
    struct header_list header_list;

    /* 파싱 시작 */
    printf("start parsing\n");
    request_start_line = file->file_pointer;
    written_size = file->file_size;
    if ( request_start_line == NULL ){
        printf("파일 시작 포인터가 NULL입니다.\n");
        return -1;
    }

    printf("pointer ok\n");

    /*memchr로 CR을 탐색한 뒤, +1 지점에 LF가 있는지 판단*/
    end_of_element = get_parsing_length(request_start_line, &written_size, &request_line_length, CR);

    if ( end_of_element == NULL ){
        printf("잘못된 request_line 형식 입니다.\n");
        return -1; 
    }
    printf("finding CR ok");
    if ( *(end_of_element + 1) != LF ){
        printf("잘못된 request_line 형식 입니다.\n");
        return -1;
    }

    /* request_line 파싱 후 uri 파싱 */
    result = parse_request_line(&request_line, &URI, request_start_line, request_line_length);
    if ( result == -1 ){
        printf("parse_request_line 실행 에러입니다\n");
        return -1;
    }

    /*request_line + CRLF 사이즈를 빼준다.*/
    changed_file_size = written_size -  2; /*CRLF*/
    if ( changed_file_size == 0 ){
        printf("헤더가 없습니다.\n");
        http_request.header = NULL;
        http_request.header_line_length = 0;
        http_request.garbage = NULL;
        http_request.garbage_length = 0;
        goto END_OF_DIVIDING;
    }else if ( changed_file_size < 0 ) {
        printf("파싱 오류입니다.\n");
        return -1;
    }

    /* CR를 지나서 새로운 문단이 시작하는 포인터 값을 구한다.*/
    header_start_line = end_of_element + 2; /*CRLF*/

    while(1){

        end_of_element = get_parsing_length(header_start_line, &changed_file_size, &temp_size, CR);
        if ( end_of_element == NULL ){
            printf("CR not find. 헤더 라인 형식 오류입니다.\n");
            return -1;
        }
        if ( *(end_of_element + 1) != LF ){
            printf("LF not find. 헤더 라인 형식 오류입니다.\n");
            return -1;
        }
        if ( changed_file_size == 0 ){
            printf("헤더 끝처리 없음\n");
            return -1;
        }

        new_element_start = header_start_line + 2; /*CRLF*/
        /*while문을 돌다가 헤더의 끝을 만났다!*/
        if ( *new_element_start == CR && *(new_element_start + 1) == LF ){
            changed_file_size = changed_file_size - 2; /*CRLF*/
            /* 남은 파일 사이즈가 2, 즉 CRLF 두 바이트만 남았다면*/
            if ( changed_file_size == 2 ){

                printf("no garbage\n");
                http_request.header_line_length = temp_size + 2 + 2; /*CRLF CRLF*/
                http_request.header = malloc(http_request.header_line_length);
                if ( http_request.header == NULL ){
                    printf("header 메모리 할당 에러\n");
                    return -1;
                }
                memset(http_request.header, 0, http_request.header_line_length);
                memcpy(http_request.header, temp_pointer, http_request.header_line_length);
                header_list.header_num = header_count + 1;
                http_request.garbage = NULL;
                http_request.garbage_length = 0;
                break;

            }else{
                /* 아니면 garbage 값이 존재하므로 */
                http_request.header_line_length = temp_size + 2 + 2; /*CRLF CRLF*/
                http_request.header = malloc(http_request.header_line_length);
                if ( http_request.header == NULL ){
                    printf("header 메모리 할당 에러\n");
                    return -1;
                }
                memset(http_request.header, 0, http_request.header_line_length);
                memcpy(http_request.header, temp_pointer, http_request.header_line_length);
                header_list.header_num = header_count + 1;

                changed_file_size = changed_file_size - 2; /*CRLF*/
                new_element_start = new_element_start + 2; /*CRLF*/
                
                http_request.garbage_length = changed_file_size;
                http_request.garbage = malloc(http_request.garbage_length + 1);
                if ( http_request.garbage == NULL ){
                    printf("garbage 메모리 할당 에러\n");
                    return -1;
                }
                memset(http_request.garbage, 0, http_request.garbage_length);
                memcpy(http_request.garbage, new_element_start, http_request.garbage_length);
            }
        } 
        changed_file_size = changed_file_size - 2; /*CRLF*/
        http_request.header_line_length += temp_size + 2; /*CRLF*/
        header_count += 1;
    }

    /* parse_header는 첫 헤더의 시작 주소를 가리키는 포인터를 반환 */
    temp_pointer = parse_header(&http_request);
    /*temp_pointer 값을 리스트의 첫 시작 헤드가 가리키게 한다.*/
    header_list.header_head->next = temp_pointer;

    print_all(&http_request,&request_line, &URI, &header_list);
    clean_up(&http_request, &request_line, &URI, &header_list);

END_OF_DIVIDING:   


   
return 0;
}

/*
헤더를 파싱하는 함수. 한줄 씩 파싱해 나가고, CRLFCRLF를 만나면 종료하고 반환한다.
* 헤더 파싱 특징 : 
    1. name과 콜론 사이에 어떤 SP도 허용하지 않는다.
    2. 콜론 뒤에 붙는 공백은 가독성을 위해 권유만 되는 부분이다.
    3. 공백이 아닌 첫번째 바이트와 마지막 바이트에는 지속적인 공백 값이 붙을 수 있다.
    4. value의 값은 SP혹은 HT를 붙인 뒤 이어지고 있다. 
    5. http 미디어 타입을 제외하고는 value 값 도중 LF 사용이 절대적으로 금지되어있다.

매개변수 : header_start_line, changed_size
반환값 : 헤더 종료 지점의 포인터, 에러시 ERROR 반환
*/

char * parse_header( struct http_request * http_request )
{

    char * end_of_element = NULL;
    char * new_element_start = NULL;
    char * check_whitespace = NULL;
    char * first_header = NULL;
    char * error_msg = "ERROR";

    int header_length = 0;
    
    struct header * header = NULL;
    struct header * temp_header = NULL;
    struct header * header_start = NULL;


    if ( http_request == NULL ){
        printf("구조체 매개변수 에러입니다.\n");
        return ERROR;
    }

    temp_header = malloc(sizeof(struct header));
    if ( temp_header == NULL ){
        printf("header 구조체 메모리 할당 에러");
        return error_msg;
    }
    memset(temp_header, 0, sizeof(struct header));

    header_start = malloc(sizeof(struct header));
    if ( header_start == NULL ){
        printf("header 구조체 메모리 할당 에러");
        return error_msg;
    }
    memset(header_start, 0, sizeof(struct header));

    header_length = http_request->header_line_length;
    new_element_start = http_request->header;

    while(1){
        header = malloc(sizeof(struct header));
        if ( header == NULL ){
            printf("header 구조체 메모리 할당 에러");
            return error_msg;
        }

        memset(header, 0, sizeof(struct header));
        if ( temp_header != NULL ){
            temp_header->next = header;
        }
        if ( header_start->next == NULL ){
            printf("헤더 파싱 첫번째\n");
            header_start->next = header;
            
        }


        /* 헤더 name과 콜론 사이 공백 예외 처리를 위한 값 */    
        check_whitespace = memchr(new_element_start, SP, header_length);

        end_of_element = get_parsing_length(new_element_start, &header_length, &header->name_length, COLON);
        if ( *end_of_element == error_msg ){
            printf("함수 내 에러 발생\n");
            return error_msg;
        }
        if ( end_of_element == NULL ){
            printf("지정 문자가 발견되지 않았습니다, header colon, 잘못된 형식이므로 종료\n");
            return error_msg;
        }
        if ( end_of_element == check_whitespace + 1 ){
            printf("name과 colon 사이에 공백이 있습니다. 잘못된 헤더 형식이므로 종료\n");
            return error_msg;
        }
        
        header->name = malloc(header->name_length + 1);
        if ( header->name == NULL ){
            printf("header name 메모리 할당 에러\n");
            return error_msg;
        }
        memset(header->name, 0, header->name_length + 1);
        memcpy(header->name, new_element_start, header->name_length);

        /* 헤더의 name과 value는 ':' 콜론으로 구분이 되고, value의 값은 ' ' 혹은 '\t'의 공백 이후에 덧붙여진다.
        (가독성 문제라서 필수 사항이 아니다. 따라서 if 문으로 구별하여 진행합니다.*/
        if ( *end_of_element + 1 == SP || *end_of_element + 1 == HT ){
            new_element_start = end_of_element + 2; /* 콜론이랑 스페이스 */
            header_length = header_length - 2; /* 스페이스 or 탭 */
        }else{
            new_element_start = end_of_element + 1; /* 콜론 */
            header_length = header_length - 1; /* 콜론 */
        }
        end_of_element = get_parsing_length(new_element_start, &header_length, &header->value_length, CR);
        if ( end_of_element == NULL ){
            printf("value CR 찾던 중 에러 발생\n");
             return error_msg;
        }
        header->value = malloc(header->value_length + 1);
        if ( header->value == NULL ){
            printf("header value 메모리 할당 에러\n");
            return error_msg;
        }
        memset(header->value, 0, header->value_length + 1);
        memcpy(header->value, new_element_start, header->value_length);

        temp_header = header;

        if ( *end_of_element + 2 == CR ){
            printf("헤더가 끝났씁니다\n");
            temp_header = header;
            temp_header->next = NULL;
            return first_header;
        }

    }

    
return first_header;
}



/*request_line의 구조체에 http_request에 저장된 정보를 바탕으로 파싱하는 함수( methond, uri, version )
매개변수 : char * request_line_start, 
          int request_line_length
반환값 : 성공시 0, 실패시 -1;
*/
int parse_request_line(struct request_line * request_line, struct URI * URI, char * request_line_start, int request_line_length)
{
    char * end_of_element = NULL;
    char * new_element_start = NULL;
    char * error_msg = "ERROR";

    int temp_size = 0;
    int changed_size = 0;


    if ( request_line == NULL ){
        printf("매개 변수 전달 에러입니다.\n");
        return -1;
    }
    if ( URI == NULL ){
        printf("매개 변수 전달 에러입니다.\n");
        return -1;
    }
    if ( request_line_start == NULL ){
        printf("매개 변수 전달 에러입니다.\n");
        return -1;
    }
    if ( request_line_length == 0 ){
        printf("매개변수 전달 에러 입니다.\n");
        return -1;
    }

    changed_size = request_line_length;
   
    /* method 파싱 */
    end_of_element = get_parsing_length(request_line_start, &changed_size, &request_line->method_length, SP);
    if ( end_of_element == error_msg ){
        printf("함수 내 에러 발생");
        return -1;
    }
    if ( end_of_element == NULL ){
        printf("지정 문자가 발견되지 않았습니다, method");
        return -1;
    }
    printf("method alloc\n");

    printf("%d\n", request_line->method_length);

    request_line->method = malloc(request_line->method_length + 1);
    if ( request_line->method == NULL ){
        printf("method 메모리 할당 오류입니다.");
    }
    printf("%s", request_line->method);


    printf("end of alloc\n");
    memset(request_line->method, 0, request_line->method_length + 1);
    memcpy(request_line->method, request_line_start, request_line->method_length);

    printf("end of memcpy\n");

    /*포인터, 길이 조정*/
    new_element_start = end_of_element + 1; /*SP*/
    changed_size = changed_size - 1; /*SP*/

    /*URI 파싱*/
    end_of_element = get_parsing_length(new_element_start, &changed_size, &URI->uri_length, SP);
    if ( end_of_element == error_msg ){
        printf("함수 내 에러 발생");
        return -1;
    }
    if ( end_of_element == NULL ){
        printf("지정 문자가 발견되지 않았습니다, uri");
        return -1;
    }

    printf("uri alloc\n");

    URI->uri_pointer = malloc(URI->uri_length + 1);
    if ( URI->uri_pointer == NULL ){
        printf("method 메모리 할당 오류입니다.");
    }

        printf("uri end\n");

    memset(URI->uri_pointer, 0, URI->uri_length + 1); 
    memcpy(URI->uri_pointer, new_element_start, URI->uri_length);

    printf("uri memcpy\n");

    
    
    parse_uri(URI);

    /*포인터, 길이 조정*/
    new_element_start = end_of_element + 1; /*SP*/
    changed_size = changed_size - 1; /*SP*/

    /*version 파싱*/
    end_of_element =  get_parsing_length(new_element_start, &changed_size, &request_line->version_length, NONE);
    if ( end_of_element == error_msg ){
        printf("함수 내 에러 발생");
        return -1;
    }

    printf("version alloc\n");
    request_line->version = malloc(request_line->version_length + 1);
    if ( request_line->version == NULL ){
        printf("method 메모리 할당 오류입니다.");
    }
    memset(request_line->version, 0, request_line->version_length + 1);
    memcpy(request_line->version, new_element_start, request_line->version_length);
    
printf("this is end of request_likne\n");

return 0;
}


/*uri 파싱 정규식
^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?
*/
/*The path is terminated by 
the first question mark ("?") or number sign ("#") character, 
or by the end of the URI. 
path는 ? # EOL로 끝남

The query component is 
indicated by the first question mark ("?") character and 
terminated by a number sign ("#") character or by the end of the URI.
query의 시작은 ?, 끝은 # 또는 EOL

The characters slash ("/") and question mark ("?") may represent data within the query component.
쿼리의 첫 시작 ? 뒤에서 나오는 /와 ?는 데이터를 의미

A fragment identifier component is 
indicated by the presence of a number sign ("#") character and 
terminated by the end of the URI.
fragment는 #에서 시작하여 EOL로 끝남.
*/

/* URI 구조체 내의 path, query, fragment를 
request_line 내의 URI 정보를 바탕으로 파싱하는 함수
매개변수 : X
반환값 : 성공시 0, 에러시 -1

request_line_parse에서 URI의 길이는 이미 문자열 처리가 되어있다. 원래 길이 + 1(NULL)
*/
int parse_uri(struct URI * URI)
{   

    printf("parse_uri start\n");
    char * end_of_element = NULL;
    char * new_element_start = NULL;

    int changed_size = 0;
    int temp_size = 0;

    if ( URI == NULL ){
        printf("parse_uri : 매개변수 전달 에러입니다\n");
        return -1;
    }

    changed_size = URI->uri_length;
    end_of_element = get_parsing_length(URI->uri_pointer, &changed_size, &URI->path_length, Q_MARK);
    if ( end_of_element == NULL ){
            printf("no query start\n");

        /*1. 쿼리가 없다*/
        end_of_element = get_parsing_length(URI->uri_pointer, &changed_size, &URI->path_length, HASH_MARK);
        if ( end_of_element ==  NULL ){
            
            /*fragment가 없다*/
            
            URI->path_length = URI->uri_length;
                printf("path start\n");

            URI->path = malloc(URI->path_length + 1);
            if ( URI->path == NULL){
                printf("메모리 할당 에러 입니다.\n");
                return -1;
            }
                printf("path memset start\n");

            memset(URI->path, 0, URI->path_length + 1);
            memcpy(URI->path, URI->uri_pointer, URI->path_length);
                printf("path memcpy start\n");

            URI->query = NULL;
            URI->fragment = NULL;
            URI->query_length =  0;
            URI->fragment_length = 0;
            return 0;
        }
            
            /*fragment가 있다*/
            /*path 파싱*/
            URI->path = malloc(URI->path_length + 1);
            if ( URI->path == NULL){
                printf("메모리 할당 에러 입니다.\n");
                return -1;
            }
            memset(URI->path, 0, URI->path_length + 1);
            memcpy(URI->path, URI->uri_pointer, URI->path_length);

            /*fragment 파싱*/
            new_element_start = end_of_element + 1;
            changed_size = changed_size - 1; /* # mark */
            
            URI->fragment_length = changed_size;
            if ( changed_size == 0 ){
                URI->fragment = malloc(1);
                URI->fragment = "";
                URI->query = NULL;
                URI->query_length = 0;
                return 0;
            }
            URI->fragment = malloc(URI->fragment_length  + 1);
            if ( URI->fragment == NULL){
                printf("메모리 할당 에러 입니다.\n");
                return -1;
            }
            memset(URI->fragment, 0, URI->fragment_length + 1);
            memcpy(URI->fragment, new_element_start, URI->fragment_length);
            return 0;
    }else {
    /* 쿼리가 있다 */
    printf("yes query");
        URI->path = malloc(URI->path_length + 1);
        if ( URI->path == NULL){
            printf("메모리 할당 에러 입니다.\n");
            return -1;
        }
        printf("yes query path start");
        memset(URI->path, 0, URI->path_length + 1);
        memcpy(URI->path, URI->uri_pointer, URI->path_length);
printf("yes query path end");
        changed_size = changed_size - 1;
        /*기호는 존재하지만 내용이 없는 경우 exception 처리*/
		if ( changed_size == 0 ){
				printf("query 길이가 0 입니다\n");
				URI->query = "";
				URI->query_length = 0;
				printf("query : %s\n", URI->query);
				URI->fragment_length = 0;
				URI->fragment = NULL;
				return 0;
			}
        new_element_start = end_of_element + 1;
        /*fragment 조사*/
        end_of_element = get_parsing_length(new_element_start, &changed_size, &URI->query_length, HASH_MARK);
        if ( end_of_element == ERROR ){
            printf("함수 내 에러 발생");
            return -1;
        }
        if ( end_of_element == NULL ){
            /* fragment가 없다 */
            URI->query_length = changed_size;
            if ( changed_size == 0 ){
                URI->query = malloc(1);
                URI->query = "";
                URI->fragment = NULL;
                URI->fragment_length = 0;
                return 0;
            }
            URI->query = malloc(URI->query_length + 1);
            if ( URI->query == NULL){
                printf("메모리 할당 에러 입니다.\n");
                return -1;
            }
            memset(URI->query, 0, URI->query_length + 1);
            memcpy(URI->query, new_element_start, URI->query_length);

            URI->fragment = NULL;
            URI->fragment_length = 0;

            return 0;
        }
        /* fragment가 있다 */
        
        URI->query = malloc(URI->query_length + 1);
        if ( URI->query == NULL){
            printf("메모리 할당 에러 입니다.\n");
            return -1;
        }
        memset(URI->query, 0, URI->query_length + 1);
        memcpy(URI->query, new_element_start, URI->query_length);
        
        
        changed_size = changed_size - 1;
        if ( changed_size == 0 ){
                    URI->fragment = malloc(1);
                    URI->fragment_length = "";
                    return 0;
                }
        URI->fragment_length = changed_size;
        URI->fragment = malloc(URI->fragment_length + 1);
        if ( URI->fragment == NULL){
            printf("메모리 할당 에러 입니다.\n");
            return -1;
        }
        new_element_start = end_of_element + 1;
        memset(URI->fragment, 0, URI->fragment_length + 1);
        memcpy(URI->fragment, new_element_start, URI->fragment_length);
        return 0;
    }

return 0;
}

