#include"http_define.h"

extern file_t * file;
http_request_t * http_request;


int divide_into_req_header(void)
{
    char * file_start_pointer = NULL;
    char * end_of_line = NULL;
    char * header_start_line = NULL;

    int result = 0;
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
    http_request->request_line = file_start_pointer;

    /*memcpy(http_request->request_line, file_start_pointer, http_request->request_line_length);*/
    /* LF를 지나서 새로운 문단이 시작하는 포인터 값을 구한다.
    request_line의 맨 끝 배열, CR, LF 이렇게 총 3개의 칸을 이동한다. */
    header_start_line = end_of_line + 3;

    http_request->header = header_start_line;

    while(1){

        if( header_start_line == NULL ){
            printf("헤더 포인터 오류입니다.");
            return -1;
        }
        
        if( changed_file_size < 0 ){
            printf("파일크기 보다 많은 양의 헤더가 계산되었습니다. 파싱 오류");
            return -1;
        }
        
        if ( header_start_line -1 == LF && header_start_line == CR && header_start_line + 1 == LF ){
            printf("헤더 파트가 끝났습니다. ");
            http_request->header_line_length = ( header_start_line + 1 ) - http_request->header + 1;
            http_request->header_num = header_count;
            /*end_of_line = 마지막 LF를 가리키는 포인터 */
            end_of_line = header_start_line + 1;
            break;
        }

        end_of_line = memchr(header_start_line, LF, changed_file_size);
        header_count ++;

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
    
    
}