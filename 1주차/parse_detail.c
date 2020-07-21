#include"http_define.h"

/* 
 * 
사용 API :
memchr(읽을 버퍼 시작 포인터, 검색할 문자, 버퍼 총 길이), 

memchr을 이용하여 첫 포인터부터 기준문자까지의 길이를 구해 반환하는 함수

매개 변수 : char * start_line, int full_length,  int parse_std_word
반환값 : int parsed_thing_length , 실패 시 -1, 문자 체크 NULL값은 -2로 체크*/
int get_parsing_length(char * start_line, int full_length, int parse_std_word)
{
    char * end_of_element = NULL;
    char * error_msg = "ERROR";
    int parsed_thing_length = 0;

    if ( start_line == NULL ){
        printf("매개 변수 전달 에러입니다.(start_line)\n");
        return -1;
    }
    if ( full_length <= 0 ){
        printf("매개변수 전달 에러입니다.(full_length)\n");
        return -1;
    }
    if ( parse_std_word <= 0 ){
        printf("매개 변수 전달 에러입니다.(parse_std_word)\n");
        return -1;
        
    }
    end_of_element = memchr(start_line, parse_std_word, full_length);
    if ( end_of_element ==  NULL ){
        printf("지정문자를 찾을 수 없습니다. 함수 종료.\n");
        return ERROR_MEMCHR;
    }
    
    parsed_thing_length = end_of_element - start_line;

return parsed_thing_length;    
}
