#include"http_define.h"

extern file_t * file;
extern http_request_t * http_request;
extern request_line_t * request_line;

/* 구조체에 저장한 file 정보를 이용하여 request_line, header, body 및 garbage의 형태로
 file을 CRLF를 기준으로 한 줄씩 읽어 파싱하는 함수
매개변수 : X
반환값 : 성공 시 0, 에러 -1
*/
/* 각 파트에 해당하는 배열을 저장할 때 CRLF 값도 저장해주어야 하는가..! */
int divide_into_req_header(void)
{
    char * file_start_pointer = NULL;
    char * end_of_line = NULL;
    char * header_start_line = NULL;
    char * origin_header_start = NULL;

    int changed_file_size = 0;
    int header_count = 0;

    file_start_pointer = (char)malloc(file->file_size);

    memset(file_start_pointer, 0, file->file_size);
    if( file_start_pointer == NULL ){
        printf("파일 포인터 초기화 에러입니다...");
        return -1;
    }

    /* memcpy는 Exception 핸들링할 것이 없다고.. 한다... */
    memcpy(file_start_pointer, file->file_pointer, file->file_size);
    
    end_of_line = memchr(file_start_pointer, LF, file->file_size);
    if ( end_of_line - 1 != CR ){
        printf("잘못된 request_line 형식 입니다.");
        return -1;
    }
    
    /* request_line이 빠진 file_size를 구해준다. 
    'LF'까지의 포인터 - 첫 포인터 + 1 의 값을 빼준다. */
    changed_file_size = ( file->file_size ) -  (end_of_line - file_start_pointer + 1);

    /* CRLF 2바이트 빼준다. */
    end_of_line = end_of_line - 2;
    /* 끝 - 첫 포인터를 하면 본래값보다 1 작은 값이 나오므로  +1을 해준다. 
    http_request 구조체에 request_line의 포인터 저장
                        request_line의 배열 크기 저장*/
    http_request->request_line_length = (end_of_line - file_start_pointer) + 1;

    /*메모리 할당, 초기화 후 memcpy로 해당되는 버퍼만큼 복사하여 구조체에 넘긴다.*/
    http_request->request_line = (char)malloc(http_request->request_line_length);
    if( http_request->request_line == NULL ){
        printf("메모리 할당 에러 입니다.");
        return -1;
    }
    memset(http_request->request_line, 0, http_request->request_line_length);
    memcpy(http_request->request_line, file_start_pointer, http_request->request_line_length);

    /* LF를 지나서 새로운 문단이 시작하는 포인터 값을 구한다.
    request_line의 맨 끝 배열, CR, LF 이렇게 총 3개의 칸을 이동한다. */
    header_start_line = end_of_line + 3;

    origin_header_start = header_start_line;

    while(1){

        /*헤더 시작 지점 포인터 NULL 체크*/
        if( header_start_line == NULL ){
            printf("헤더 포인터 오류입니다.");
            return -1;
        }
        /* 파일 사이즈 체크 */
        if( changed_file_size < 0 ){
            printf("파일크기 보다 많은 양의 헤더가 계산되었습니다. 파싱 오류");
            return -1;
        }
        
        /*마지막 헤더의 끝을 표시하는 end_of_line에 +1 한 값을 header_start_pointer의 값으로 주고 있으므로
        헤더가 끝난 지점, header_start_pointer가 가르치는 값은 CR 이어야 한다. +-1 위치의 값을 비교하여 무한루프 탈출*/
        if ( header_start_line -1 == LF && header_start_line == CR && header_start_line + 1 == LF ){
            printf("헤더 파트가 끝났습니다. ");
            http_request->header_line_length = ( header_start_line + 1 ) - http_request->header + 1;
            http_request->header_num = header_count;
            
            http_request->header = (char)malloc(http_request->header_line_length);
            if ( http_request->header == NULL ){
                printf("헤더 메모리 할당 에러입니다.");
                return -1;
            }
            memset(http_request->header, 0, http_request->header_line_length);
            memcpy(http_request->header, origin_header_start, http_request->header_line_length);

            /*end_of_line = 마지막 LF를 가리키는 포인터 */
            end_of_line = header_start_line + 1;
            break;
        }

        end_of_line = memchr(header_start_line, LF, changed_file_size);
        header_count ++;

        /*끝 포인터의 앞에 CR 체크*/
        if ( end_of_line - 1 != CR ){
        printf("잘못된 header 형식 입니다.");
        return -1;
        }       

        /* header 한줄을 검사한 뒤, 변경되는 header 사이즈를 받는다.
        'LF'까지의 포인터 - 첫 포인터 + 1 의 값을 빼준다. */
        changed_file_size = changed_file_size -  (end_of_line - header_start_line + 1);

        /* LF를 지나서 새로운 문단이 시작하는 포인터 값을 구한다.
        request_line의 맨 끝 배열, CR, LF 이렇게 총 3개의 칸을 이동한다. */
        header_start_line = end_of_line + 1;
    }

    /*end_of_line은 현재 header 마지막 부분 CRLFCRLF 중 LF를 가리키고 있다.*/
    if ( end_of_line + 1 != NULL ){
        http_request->garbage_length = ( file->file_size ) - ( ( end_of_line - file->file_pointer ) + 1);

        http_request->garbage = (char)malloc(http_request->garbage_length);
        if ( http_request->garbage == NULL ){
            printf("garbage 값 메모리 할당 에러 입니다.");
            return -1;
        }
        memset(http_request->garbage, 0, http_request->garbage_length);
        memcpy(http_request->garbage, end_of_line + 1, http_request->garbage_length);
    }else{

        http_request->garbage = NULL;
        http_request->garbage_length = 0;
    }

    /*clean up*/
    char * end_of_line = NULL;
    char * header_start_line = NULL;
    char * origin_header_start = NULL;

    int changed_file_size = 0;
    int header_count = 0;

    if ( file_start_pointer != NULL ){
    memset(file_start_pointer, 0, file->file_size);
    free(file_start_pointer);
    }else{
        printf("free 시키지 않은 메모리 도망감");
        return -1;
    }

return 0;
}

/*request_line의 구조체에 http_request에 저장된 정보를 바탕으로 파싱하는 함수( methond, uri, version )
http_request에 저장된 정보를 임시로 저장할 변수 생성 및 동적 할당, (정보 복사 후 free)
모든 파싱 항목은 
memchr -> malloc()-> memset->memcpy의 단계를 거쳐서 파싱
매개변수 : X
반환값 : 성공시 0, 실패시 -1;
*/
int parse_request_line(void)
{
    char * request_line_start = NULL;
    char * end_of_element = NULL;
    char * new_element_start = NULL;

    int changed_size = 0;

    request_line_start = (char)malloc(http_request->request_line_length);

    memset(request_line_start, 0, http_request->request_line_length);
    if( request_line_start == NULL ){
        printf("request line 포인터 초기화 에러입니다...");
        return -1;
    }
    memcpy(request_line_start, http_request->request_line, http_request->request_line_length);

    /*request_line의 맨 끝 값: CRLF 바로 이전의 배열 값*/
    
    end_of_element = memchr(request_line_start, SP, http_request->request_line_length);
    if ( end_of_element == NULL ){
        printf("올바르지 않은 request_line 입니다.");
        return -1;
    }

    /*method의 마지막 배열 값 = end_of_element -1*/
    end_of_element = end_of_element - 1;

    request_line->method_length = end_of_element - request_line_start + 1;
    request_line->method = (char) malloc(request_line->method_length);
    memset(request_line->method, 0, request_line->method_length);
    if( request_line->method == NULL ){
        printf("request_line->method 포인터 초기화 에러입니다...");
        return -1;
    }
    memcpy(request_line->method, request_line_start, request_line->method_length);

    /* method 마지막 배열 + SP + URI의 시작 */
    new_element_start = end_of_element + 2;
    changed_size = ( http_request->request_line_length ) - ( request_line->method_length + 1);

    end_of_element = memcpy(new_element_start, SP, changed_size);
    if ( end_of_element == NULL ){
        printf("올바르지 않은 request_line 입니다.");
        return -1;
    }

    end_of_element = end_of_element - 1;
    request_line->uri_length = end_of_element - new_element_start;
    request_line->uri = (char)malloc(request_line->uri_length);

    memset(request_line->uri, 0, request_line->uri_length);
    if( request_line->uri == NULL ){
        printf("request_line->uri 포인터 초기화 에러입니다...");
        return -1;
    }
    memcpy(request_line->uri, new_element_start, request_line->uri_length);

    /* URI의 마지막 배열 + SP + version의 시작과 끝 */
    new_element_start = end_of_element + 2;
    changed_size = changed_size - ( request_line->uri_length + 1 );

    /*2 더해준건 SP 두 개 값*/
    if ( request_line->method_length + request_line->uri_length + changed_size + 2 != http_request->request_line_length){
        printf("오버플로우 혹은 파싱 오류입니다.");
        return -1;
    }
    request_line->version_length = changed_size;
    request_line->version = (char)malloc(changed_size);
    memset(request_line->version, 0, changed_size);
    if( request_line->version == NULL ){
        printf("request_line->version 포인터 초기화 에러입니다...");
        return -1;
    }
    memcpy(request_line->version, new_element_start, changed_size);    


    /*clean up*/
    char * end_of_element = NULL;
    char * new_element_start = NULL;
    int changed_size = 0;

    if(request_line_start != NULL){
        memset(request_line_start, 0, http_request->request_line_length);
        free(request_line_start);
    }else{
        printf("free 안시켜준 request_line_start 도망");
        return -1;
    }


return 0;
}