#include"http_define.h"

/* 지정 문자를 넣어주면 그 지정 문자 지점의 포인터를 반환하고, 
첫 시작부터 지정 문자까지의 길이 변수에 저장

사용 API :
memchr(읽을 버퍼 시작 포인터, 검색할 문자, 버퍼 총 길이), 

매개 변수 : char * start_line, int full_length, int parsed_thing_length, int parse_std_word
변경되는 변수 : parsed_thing_length에 값 대입, full_length는 파싱된 값 만큼 빠진 값으로 변경
반환값 : char * end_of_element , 실패, 문자 체크 실패 시 NULL*/
char * get_parsing_length(char * start_line, int * full_length, int * parsed_thing_length, int parse_std_word)
{
    char * end_of_element = NULL;
    char * error_msg = "ERROR";

    if ( start_line == NULL ){
        printf("매개 변수 전달 에러입니다.(start_line)\n");
        return NULL;
    }
    if ( full_length <= 0 ){
        printf("매개변수 전달 에러입니다.(full_length)\n");
        return NULL;
    }
    if ( parsed_thing_length <= 0 ){
        printf("매개 변수 전달 에러입니다.(parsed_thing_length)\n");
        return NULL;
    }
    if ( parse_std_word <= 0 ){
        printf("매개 변수 전달 에러입니다.(parse_std_word)\n");
        return NULL;
        
    }else if ( parse_std_word == NONE){
        printf("기준문자가 없는 파싱입니다\n");
        *parsed_thing_length = *full_length;
        printf("남은 길이가 없습니다. 여기가 끝\n");
        printf("end_of_element = end_of_file");
            return end_of_element;    
    }

    end_of_element = memchr(start_line, parse_std_word, *full_length);
    if ( end_of_element ==  NULL ){
        printf("지정문자를 찾을 수 없습니다. 함수 종료.\n");
        return NULL;
    } 

    *parsed_thing_length = end_of_element - start_line;
    
    *full_length = *full_length - *parsed_thing_length; 
    if ( *full_length == 0 ){
        printf("남은 길이가 없습니다. 여기가 끝\n");
        printf("end_of_element = end_of_file");
        return end_of_element;
    }else if ( *full_length < 0 ){
        printf("길이 파싱 에러\n");
        return NULL;
    }
printf("end of finding word\n");
return end_of_element;    
}
