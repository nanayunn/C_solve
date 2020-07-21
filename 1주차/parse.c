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
    char * lf_finder = NULL;

    int changed_file_size = 0;
    int request_line_length = 0;
    int written_size = 0;
    int temp_size = 0;
    int header_count = 0;
    int result = 0;

    struct http_request http_request;
    struct request_line request_line;	
    struct URI URI;
    struct header_list header_list;

    memset(&http_request, 0, sizeof(struct http_request));
    memset(&request_line, 0, sizeof(struct request_line));
    memset(&URI, 0, sizeof(struct URI));
    memset(&header_list, 0, sizeof(struct header_list));

    /* 파싱 시작 */
    printf("start parsing\n");
    request_start_line = file->file_pointer;
    written_size = file->file_size;
    if ( request_start_line == NULL ){
        printf("파일 시작 포인터가 NULL입니다.\n");
        return -1;
    }

    printf("pointer ok\n");

    /*get_parsing_length 함수에서 memchr을 사용 후 얻어낸 길이를 기준으로 request_line의 끝 지점과 길이를 구한다.*/
    request_line_length = get_parsing_length(request_start_line, written_size, CR);
    if ( request_line_length == -1 ){
        printf("잘못된 request_line 형식 입니다.\n");
        return -1; 
    }else if ( request_line_length == ERROR_MEMCHR ){
        printf("잘못된 request_line 형식 입니다. no CR\n");
        return -1;
    }else{
        /* nothing */
    }
    
    end_of_element = request_start_line + request_line_length;
    lf_finder = memchr(request_start_line, LF, written_size);
    if ( lf_finder == NULL ){
        printf("no LF\n");
    }
    if (( end_of_element + 1)!= lf_finder){
        printf("request line 형식 오류 입니다.\n");
        return -1;
    }


    /* request_line을 파싱하는 함수 속에서 uri 파싱 */
    result = parse_request_line(&request_line, &URI, request_start_line, request_line_length);
    if ( result == -1 ){
        printf("parse_request_line 실행 에러입니다\n");
        return -1;
    }


    /**/
    new_element_start = request_start_line + request_line_length + 2; /*CRLF*/
    header_start_line = new_element_start;
    written_size = written_size - ( request_line_length + 2 ); /*CRLF*/

    if ( written_size > 0 ){

        while(1){
            temp_size = get_parsing_length(new_element_start, written_size, CR);
            if ( temp_size == -1 ){
                printf("CR not find. 헤더 라인 형식 오류입니다.\n");
                return -1;
            }
            lf_finder = memchr(new_element_start, LF, written_size);
            /*CRLF 처리*/
            if ( new_element_start + temp_size + 1  != lf_finder ){
                printf("LF not find. 헤더 라인 형식 오류입니다.\n");
                return -1;
            }
             /*CRLFCRLF 없이 끝나는지 검사*/
            if ( ( written_size - (temp_size + 2) ) == 0 ){
                printf("헤더 끝처리 없음\n");
                return -1;
            }
            new_element_start = new_element_start + temp_size + 2; /*CRLF*/
            written_size = written_size - ( temp_size + 2 ); /*CRLF*/
            /*만약 새로운 헤더의 시작 포인터가 CR의 값을 가지고 있고, 그 다음이 LF 라면 헤더가 끝남 */
            if ( *new_element_start == CR && *( new_element_start + 1 ) == LF){
                header_list.header_num = header_count + 1;
                printf("헤더 끝!\n");
                written_size = written_size - 2; /*CRLF*/
                end_of_element = new_element_start + 1; /*헤더의 마지막 끝부분 LF를 가리키는 포인터*/
                http_request.header_line_length = end_of_element - header_start_line + 1;
                http_request.header = malloc(http_request.header_line_length);
                memset(http_request.header, 0, http_request.header_line_length);
                memcpy(http_request.header, header_start_line, http_request.header_line_length);
                if ( written_size > 0 ){
                    printf("garbage 값 존재\n");
                    http_request.garbage_length = written_size;
                    http_request.garbage = malloc(http_request.garbage_length);
                    memset(http_request.garbage, 0, http_request.garbage_length);
                    memcpy(http_request.garbage, end_of_element + 1, http_request.garbage_length);
                    break;
                }else{
                    printf("no garbage\n");
                    http_request.garbage_length = 0;
                    http_request.garbage = NULL;
                    break;
                }
                
            }
            header_count ++;
        }

     /* parse_header는 첫 헤더의 시작 주소를 가리키는 포인터를 반환 */
    parse_header(&http_request, &header_list);
    /*temp_pointer 값을 리스트의 첫 시작 헤드가 가리키게 한다.*/
    

    }else if ( written_size == 0 ){
        printf("헤더가 없습니다.\n");
        http_request.header = NULL;
        http_request.header_line_length = 0;
        http_request.garbage = NULL;
        http_request.garbage_length = 0;
    }else{
        printf("파싱 오류입니다.\n");
        return -1;
    }

    

    print_all(&http_request,&request_line, &URI, &header_list);

    clean_up(&http_request, &request_line, &URI, &header_list);




   
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

int parse_header( struct http_request * http_request, struct header_list * header_list )
{

    char * end_of_element = NULL;
    char * new_element_start = NULL;
    char * check_whitespace = NULL;
    char * first_header = NULL;
    char * sp_finder = NULL;
    char * ht_finder = NULL;
    char * lf_finder = NULL;
    char * error_msg = "ERROR";

    int header_length = 0;
    int cnt = 0;
    
    struct header * header = NULL;
    struct header * header_tail = NULL;


    if ( http_request == NULL ){
        printf("구조체 매개변수 에러입니다.\n");
        return -1;
    }
    if ( header_list == NULL ){
        printf("구조체 매개변수 에러입니다.\n");
        return -1;
    }

    header_tail = malloc(sizeof(struct header));
    if ( header_tail == NULL ){
        printf("header 구조체 메모리 할당 에러");
        return -1;
    }
    memset(header_tail, 0, sizeof(struct header));


    header_length = http_request->header_line_length;
    new_element_start = http_request->header;

    while(1){

        header = malloc(sizeof(struct header));
        if ( header == NULL ){
            printf("header 구조체 메모리 할당 에러");
            return -1;
        }

        memset(header, 0, sizeof(struct header));


        /* 헤더 name과 콜론 사이 공백 예외 처리를 위한 값 */    
        check_whitespace = memchr(new_element_start, SP, header_length);

        header->name_length = get_parsing_length(new_element_start, header_length, COLON);
        if ( header->name_length == -1 ){
            printf("함수 내 에러 발생\n");
            return -1;
        }
        if ( header->name_length == ERROR_MEMCHR ){
            printf("지정 문자가 발견되지 않았습니다, header colon, 잘못된 형식이므로 종료\n");
            return -1;
        }
        end_of_element = new_element_start + header->name_length; /*COLON의 위치*/
        if ( end_of_element == check_whitespace + 1 ){
            printf("name과 colon 사이에 공백이 있습니다. 잘못된 헤더 형식이므로 종료\n");
            return -1;
        }
        
        header->name = malloc(header->name_length + 1);
        if ( header->name == NULL ){
            printf("header name 메모리 할당 에러\n");
            return -1;
        }
        memset(header->name, 0, header->name_length + 1);
        memcpy(header->name, new_element_start, header->name_length);

        sp_finder = memchr(new_element_start, SP, header_length);
        ht_finder = memchr(new_element_start, HT, header_length);
        /* 헤더의 name과 value는 ':' 콜론으로 구분이 되고, value의 값은 ' ' 혹은 '\t'의 공백 이후에 덧붙여진다.
        (가독성 문제라서 필수 사항이 아니다. 따라서 if 문으로 구별하여 진행합니다.*/
        if ( end_of_element + 1 == sp_finder || end_of_element + 1 == ht_finder ){
            new_element_start = end_of_element + 2; /* 콜론이랑 스페이스 */
            header_length = header_length - ( header->name_length + 2 ); /* 콜론이랑 스페이스 */
        }else{
            new_element_start = end_of_element + 1; /* 콜론 */
            header_length = header_length - ( header->name_length + 1 ); /* 콜론 */
        }
        header->value_length = get_parsing_length(new_element_start, header_length, CR);
        if ( header->value_length == ERROR_MEMCHR ){
            printf("value CR 찾던 중 에러 발생\n");
            return -1;
        }
        header->value = malloc(header->value_length + 1);
        if ( header->value == NULL ){
            printf("header value 메모리 할당 에러\n");
            return -1;
        }
        memset(header->value, 0, header->value_length + 1);
        memcpy(header->value, new_element_start, header->value_length);

        if ( header_tail == NULL ){
	     header_tail = header;
	}else{
	     header_tail->next = header;	
	}
	

        end_of_element = new_element_start + header->value_length;
        new_element_start = end_of_element + 2; /* CRLF */
        header_length = header_length - ( header->value_length + 2); /* CRLF */

        if ( header_length == 2 ){
            printf("헤더가 끝났씁니다\n");
            header_tail = header;
            header_tail->next = NULL;

            return 0;
        }

        if ( cnt == 0 ){
            header_list->header_head = header; 
        }
        cnt ++;
    }
 
return 0;
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
    request_line->method_length = get_parsing_length(request_line_start, changed_size, SP);
    if ( request_line->method_length == -1 ){
        printf("함수 내 에러 발생");
        return -1;
    }
    if ( request_line->method_length == ERROR_MEMCHR ){
        printf("지정 문자가 발견되지 않았습니다, method");
        return -1;
    }


    request_line->method = malloc(request_line->method_length + 1);
    if ( request_line->method == NULL ){
        printf("method 메모리 할당 오류입니다.");
    }


    memset(request_line->method, 0, request_line->method_length + 1);
    memcpy(request_line->method, request_line_start, request_line->method_length);

    /*포인터, 길이 조정*/
    new_element_start = request_line_start + request_line->method_length + 1; /*SP*/
    changed_size = changed_size - ( request_line->method_length + 1 ); /*SP*/

    /*URI 파싱*/
    URI->uri_length = get_parsing_length(new_element_start, changed_size, SP);
    if ( URI->uri_length == -1 ){
        printf("함수 내 에러 발생");
        return -1;
    }
    if ( URI->uri_length == ERROR_MEMCHR ){
        printf("지정 문자가 발견되지 않았습니다, uri");
        return -1;
    }



    URI->uri_pointer = malloc(URI->uri_length + 1);
    if ( URI->uri_pointer == NULL ){
        printf("method 메모리 할당 오류입니다.");
    }


    memset(URI->uri_pointer, 0, URI->uri_length + 1); 
    memcpy(URI->uri_pointer, new_element_start, URI->uri_length);

    parse_uri(URI);

    /*포인터, 길이 조정*/
    new_element_start = new_element_start + URI->uri_length + 1; /*SP*/
    changed_size = changed_size - ( URI->uri_length + 1 ) ; /*SP*/

    /*version 파싱*/
    request_line->version_length =  changed_size;


    request_line->version = malloc(request_line->version_length + 1);
    if ( request_line->version == NULL ){
        printf("method 메모리 할당 오류입니다.");
    }
    memset(request_line->version, 0, request_line->version_length + 1);
    memcpy(request_line->version, new_element_start, request_line->version_length);
    

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
    URI->path_length = get_parsing_length(URI->uri_pointer, changed_size, Q_MARK);
    if ( URI->path_length == ERROR_MEMCHR ){

        /*1. 쿼리가 없다*/
        URI->path_length = get_parsing_length(URI->uri_pointer, changed_size, HASH_MARK);
        if ( URI->path_length == ERROR_MEMCHR ){

            /*fragment가 없다*/
            
            URI->path_length = URI->uri_length;

            URI->path = malloc(URI->path_length + 1);
            if ( URI->path == NULL){
                printf("메모리 할당 에러 입니다.\n");
                return -1;
            }


            memset(URI->path, 0, URI->path_length + 1);
            memcpy(URI->path, URI->uri_pointer, URI->path_length);


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
            changed_size = changed_size - ( URI->path_length + 1); /* # mark */
            
            URI->fragment_length = changed_size;
            if ( changed_size == 0 ){
                URI->fragment = NULL;
                URI->query = NULL;
                URI->query_length = 0;
                return 0;
            }
            new_element_start = URI->uri_pointer + URI->path_length + 1; /* # mark */

            URI->fragment = malloc(URI->fragment_length  + 1);
            if ( URI->fragment == NULL){
                printf("메모리 할당 에러 입니다.\n");
                return -1;
            }
            memset(URI->fragment, 0, URI->fragment_length + 1);
            memcpy(URI->fragment, new_element_start, URI->fragment_length);
            
            URI->query = NULL;
            URI->query_length = 0;
            return 0;
    }else {
    /* 쿼리가 있다 */

        URI->path = malloc(URI->path_length + 1);
        if ( URI->path == NULL){
            printf("메모리 할당 에러 입니다.\n");
            return -1;
        }
        memset(URI->path, 0, URI->path_length + 1);
        memcpy(URI->path, URI->uri_pointer, URI->path_length);

        
        changed_size = changed_size - ( URI->path_length + 1 ); /* ? mark */
        /*기호는 존재하지만 내용이 없는 경우 exception 처리*/
		if ( changed_size == 0 ){
				printf("query 길이가 0 입니다\n");
				
                URI->query = NULL;
				URI->query_length = 0;
				URI->fragment_length = 0;
				URI->fragment = NULL;
				return 0;
			}

        new_element_start = URI->uri_pointer + URI->path_length + 1; /* ? mark */
        /*fragment 조사*/
        URI->query_length = get_parsing_length(new_element_start, changed_size, HASH_MARK);
        if ( URI->query_length == -1 ){
            printf("함수 내 에러 발생");
            return -1;
        }
        if ( end_of_element == ERROR_MEMCHR ){
            /* fragment가 없다 */
            URI->query_length = changed_size;
            if ( changed_size == 0 ){
                URI->query = NULL;
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
        
        
        changed_size = changed_size - ( URI->path_length + 1 ); /* # mark */
        if ( changed_size == 0 ){
                    URI->fragment = NULL;
                    URI->fragment_length = 0;
                    return 0;
                }
        URI->fragment_length = changed_size;
        URI->fragment = malloc(URI->fragment_length + 1);
        if ( URI->fragment == NULL){
            printf("메모리 할당 에러 입니다.\n");
            return -1;
        }
        new_element_start =new_element_start + URI->query_length + 1; /* # mark */
        memset(URI->fragment, 0, URI->fragment_length + 1);
        memcpy(URI->fragment, new_element_start, URI->fragment_length);
        return 0;
    }

return 0;
}

