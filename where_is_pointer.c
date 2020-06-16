#include"http_define.h"

/*
추출하고 싶은 문자열의 끝 지점 포인터를 반환하는 함수

매개변수 : 
file_start_pointer : 체크를 원하는 문자열의 첫번째 주소와, 
check_word : 문자열이 끝처리 되는 특정 문자와, 
check_str_size : 체크하는 문자열이 들어간 전체 버퍼의 사이즈

반환값 : 추출을 원하는 문자열의 끝 지점 포인터*/
char* where_is_pointer(char * file_start_pointer, char check_word, int check_str_size)
{
	/* 변수 선언 및 초기화 */
	char * where_is_checked = NULL;


	/* 매개변수 Exception 체크 */
	if( file_start_pointer == NULL ){
		printf("where_is_pointer : 파일 주소 실종..NULL\n");
		return -1;
	}
	if( check_word < 0 ){
		printf("where_is_pointer : memchr을 멈춰줄 브레이크가 없군요...\n");
		return -1;
	}
	if( check_str_size <= 0 ){
		printf("where_is_pointer : 내용이 있어야 분석도 하겠죠,,?\n");
		return -1;
	}

    /*주어진 포인터와 브레이크 문자, 분석할 버퍼의 사이즈를 넘겨 받아서 
    브레이크 문자에 따라 해당 문자가 존재하는 곳의 주소값을 받는다.*/
	where_is_checked = memchr(file_start_pointer, check_word, check_str_size);

	if( where_is_checked == NULL ){
		printf("where_is_pointer : memchr로 분석한게 없어요...\n");
		return -1;
	}
	
    /*브레이크 문자에 따라 if else if로 처리한다.*/
	if( check_word == LF ){
        return where_is_checked -1;   /*브레이크 문자 지점의 포인터 주소를 알고 있기 때문에, 
                                        브레이크 문자에서 하나 줄어든 값이 주소가 되어야한다.*/ 
    }else if( check_word == CRLF ){
		return where_is_checked-2; /* CR과 LF를 처리해주므로 -2*/
	}else{
		return where_is_checked;
	}

	/* clean up */
}