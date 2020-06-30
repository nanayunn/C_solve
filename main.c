#include"http.h"


int get_file_size(char * file_path);
int read_file(char * file_path, file_t * file);
void getchar_clear(void);
int start_parse(file_t * file, length_t * length);
int parse_request_line(request_line_t * request_line, length_t * length,  char * element_start, int temp_size);
int parse_uri(URI_t * uri, request_line_t * request_line ,length_t * length);


int main(int argc, char *argv[])
{	
	char * file_path = NULL;
    char * end_of_file = NULL;

	int argv_length = 0;
	int result = 0;

    file_t * file;
	length_t * length;

    file = malloc(sizeof(file_t));
	length = malloc(sizeof(length_t));
	
	/*argc 값을 기준으로 정해진 값이 아니면 안내 메세지를 출력*/
	if (argc != ARGC_NUM) {
		printf("파싱을 진행할 파일의 경로를 제대로 입력해 주세요.\n");
		return -1;
	}
	
	argv_length = strlen(argv[1]);
	if ( argv_length <= 0 ) {
		printf("파일 이름을 제대로 입력해주세요.\n");
		return -1;
	}

	file_path = argv[1];
	

	/*비즈니스 로직 시작*/

	read_file(file_path, file);

    start_parse(file, length);

	free(length);
	free(file);
	return 0;
}

/*scanf를 안전하게 쓰기 위한 버퍼 클리너*/
void getchar_clear(void)
{
	int temp_val = 0;
	temp_val = getchar();
	while (1) {
		if (temp_val == '\n') {
			break;
		}
	}
	return;
}

/*======================================
file_size를 계산하는 함수
매개 변수 : char * file_path
반환값 : file_size, 에러시 -1*/
int get_file_size(char * file_path)
{
    printf("get file size start\n");
	/*전달된 매개변수 예외 처리*/
	if( file_path == NULL ){
		printf("get_file_size : 매개변수 전달 에러 입니다.\n");
		return -1;
	}

	/*변수 선언 및 초기화*/
	struct stat stat_stat;
	int result = 0;

	/*stat()을 이용
	성공시 0, 실패 시 -1 값을 반환*/
	result = stat(file_path, &stat_stat);

	if( result != 0 ){
        printf("get_file_size : 파일 정보를 읽어오는 것에 실패하였습니다....\n");
		return -1;
	}
    printf("result success: %d\n", result);

	/* 파일 사이즈 Exception */
	if ( stat_stat.st_size < 0 ){
		printf("get_file_size : file내에 읽을 값이 충분하지 않습니다.\n");
		return -1;
	}else if( stat_stat.st_size == 0 ){
		printf("빈 파일입니다.\n");
		return stat_stat.st_size;
	}
	
printf("get file size end\n");
return stat_stat.st_size;
}

/*=======================================
파일을 읽는 함수
매개변수 :  file_path, struct file
반환값 : 0, 에러시 -1*/
int read_file(char * file_path, file_t * file)
{	

    printf("read file start\n");
	/*구조체 포인터 매개변수에 대한 Exception 처리*/
	if( file_path == NULL ){
		printf("read_file : 매개변수 전달 에러입니다.\n");
		return -1;
	}
    if ( file == NULL ){
        printf("구조체 에러 입니다.\n");
        return -1;
    }

	/*사용하는 변수 초기화*/
	int fd = 0;
	int temp_val = 0;

	/*get_file_size()를 이용하여 file 구조체에 사이즈 값을 구해 저장*/
	(file->written_size) = get_file_size(file_path);
	if( file->written_size < 0 ){
		printf("get_file_size 오류입니다.\n");
		return -1;
	}else if ( file->written_size == 0 ){
        printf("빈 파일 입니다. 파싱을 종료합니다.\n");
        return -1;
    }
    
	fd = open ( file_path, O_RDONLY );
	if ( fd < 0 ){
		printf("파일을 열지 못했습니다. fd 반환값 오류\n");

		return -1;
	}
    printf("fd 상태 : %d\n", fd);

	/*open한 파일을 read로 읽어오고, temp_val에 읽은 바이트 수를 임시 저장한 후
	file_size와 비교하여 read가 완전하게 이루어졌는지 체크*/
	temp_val = read(fd, file->file_data, file->written_size);
	if ( temp_val < 0 ){
		printf("read 실패\n");
		close(fd);
		return -1;
	}

	if ( temp_val != file->written_size ){
		printf("read가 정상적으로 이루어지지 않았습니다.\n저장된 파일 사이즈 : %d\n읽어들인 바이트 수 : %d\n", file->written_size, temp_val);
		close(fd);
		return -1;
	}

    printf("저장된 파일 사이즈 : %d\n읽어들인 바이트 수 : %d\n", file->written_size, temp_val);

	/*파일을 닫아준다.*/
	close(fd);
printf("read file end\n");
return 0;
}


int start_parse(file_t * file, length_t * length)
{	
    http_request_t * http_request;
    request_line_t * request_line;
	URI_t * uri;
    header_start_to_end_t * header_list;

    char * element_start = NULL;
    char * end_of_element = NULL;
    char * end_of_file = NULL;
    char * word_finder = NULL;

    int temp_size = 0;
    int changed_size = 0;

    if ( file == NULL ){
        printf("start_parse : file 구조체 전달 에러 입니다.\n");
        return -1;
    }
	if ( length == NULL ){
        printf("start_parse : length 구조체 전달 에러 입니다.\n");
        return -1;
    }


    end_of_file = file->file_data + file->written_size - 1; /* 파일의 끝 지점을 가리키는 포인터 */

    element_start = file->file_data;

    end_of_element = memchr(element_start, CR , file->written_size); //CR/
    if ( end_of_element == NULL ){
        printf("cannot find CR..\n");
 		// return -1;
    }

    word_finder = memchr(element_start, LF, file->written_size); /*LF*/
    if ( word_finder == NULL ){
        printf("cannot find LF..\n");
        return -1;
    }

    /* 형식 오류 Exception 처리 */
    // if ( end_of_element + 1 != word_finder ){
    //     printf("파싱 파일 형식 오류입니다. 프로그램 종료\n");
    //    // return -1;
    // }
    if ( word_finder == end_of_file ){
        printf("헤더가 없습니다. 프로그램 종료\n");
        return -1;
    }

    /*request line 파싱*/
    temp_size = word_finder - element_start; /*CR 지점에서 첫 지점 빼서 request_line 크기 구함*/


    /*메모리 할당*/
    request_line = malloc(sizeof(request_line));
	
    if ( request_line == NULL ){
        printf("request_line 메모리 할당 오류\n");
        return -1;
    }



    parse_request_line(request_line, length, element_start, temp_size);
	printf("aa\n");
	printf("uri length : %d\n", length->uri_length);

	/*uri 메모리 할당*/

	uri = malloc(sizeof(uri));
	if ( uri == NULL ){
	 	printf("uri 메모리 할당 실패\n");
	 	return -1;
	 }

	parse_uri(uri, request_line, length);



//free(uri);
free(request_line);
return 0;

}

/*request_line의 method, uri, version을 파싱하는 함수 
매개변수 : request_line, length 구조체 포인터, element_start(request_line 시작 지점 포인터), 
temp_size(CRLF를 제외한 request_line의 길이)
리턴값 : 성공시 0, 실패시 -1
*/
int parse_request_line(request_line_t * request_line, length_t * length,  char * element_start, int temp_size)
{	
	printf("requestline parse\n");

    char * end_of_element = NULL;

	int changed_size = 0;
	int parse_element_size = 0;

    if ( request_line == NULL ){
        printf("parse_request_line : request_line 구조체 전달 오류입니다.");
		return -1;
    }
	if ( length == NULL ){
        printf("parse_request_line : length 구조체 전달 오류입니다.");
		return -1;
    }
	if ( element_start == NULL ){
        printf("parse_request_line : element_start 구조체 전달 오류입니다.");
		return -1;
    }
	if ( temp_size == 0 ){
        printf("parse_request_line : temp_size 값 전달 오류입니다.");
		return -1;
    }

	printf("method parse\n");
	/* method 파싱 */
	end_of_element = memchr(element_start, SP, temp_size);
	if ( end_of_element == NULL ){
        printf("cannot find SP..\n");
 		return -1;
    }

	parse_element_size = end_of_element - element_start;

	request_line->method = malloc(parse_element_size +1 );
	length->method_length = parse_element_size;
	memset(request_line->method, 0, length->method_length + 1);

	printf("%d\n", parse_element_size);

	snprintf(request_line->method, length->method_length + 1 , "%s", element_start);
	printf("%s\n", request_line->method);


	changed_size = temp_size - (parse_element_size + 1); /*SP*/
	element_start = end_of_element + 1;

	printf("uri parse\n");
	/*uri 파싱*/
	end_of_element = memchr(element_start, SP, changed_size);
	if ( end_of_element == NULL ){
        printf("cannot find SP..\n");
 		return -1;
    }

	parse_element_size = end_of_element - element_start;

	request_line->uri_pointer = malloc(parse_element_size +1 );
	length->uri_length = parse_element_size;
	memset(request_line->uri_pointer, 0, length->uri_length + 1);

	printf("%d\n", parse_element_size);

	snprintf(request_line->uri_pointer, length->uri_length + 1 , "%s", element_start);
	printf("%s\n", request_line->uri_pointer);

	
    
	printf("version parse\n");
	/*version*/
	changed_size = changed_size - (parse_element_size + 1); /*SP*/
	element_start = end_of_element + 1;

	request_line->version = malloc(changed_size +1 );
	length->version_length = changed_size;
	memset(request_line->version, 0, length->version_length + 1);

	printf("%d\n", changed_size);
	snprintf(request_line->version, changed_size + 1, "%s", element_start);

	printf("%s\n", request_line->version);
	
	
printf("end of request line parsing\n");

return 0;
}



/*uri의 path, query, fragment를 파싱하는 함수 
매개변수 : uri, requset_line, length 구조체 포인터
리턴값 : 성공시 0, 실패시 -1
*/
int parse_uri(URI_t * uri, request_line_t * request_line, length_t * length)
{
	if ( uri == NULL ){
		printf("uri 구조체 전달 에러\n");
		return -1;
	}
	if ( request_line ==  NULL ){
		printf("request_line 구조체 전달 에러");
		return -1;
	}

	char * new_element_start = NULL;
	char * end_of_element = NULL;
	char * word_finder = NULL;

	int changed_size = 0;
	int parse_element_size = 0;

	/*query 탐색*/

	word_finder = memchr(request_line->uri_pointer, Q_MARK, length->uri_length);
	if ( word_finder == NULL ){
		printf("query 없음\n");
		word_finder = memchr(request_line->uri_pointer, HASH_MARK, length->uri_length);
		if ( word_finder == NULL  ){
			printf("fragment 없음\n");
			/*다 path 꺼*/
			uri->path = malloc(length->uri_length + 1);
			if ( uri->path == NULL ){
				printf("동적할당 에러\n");
				return -1;
			}

			memset(uri->path, 0, length->uri_length + 1);
			snprintf(uri->path, length->uri_length + 1, "%s", request_line->uri_pointer);

			uri->query = NULL;
			length->query_length = 0;
			
			uri->fragment = NULL;
			length->fragment_length = 0;

			return 0;
		}else{
			printf("fragment 존재\n");
			/*path 파싱*/
			end_of_element = word_finder;
			parse_element_size = end_of_element - ( request_line->uri_pointer );
			length->path_length = parse_element_size;
			uri->path = malloc(parse_element_size + 1);
			if ( uri->path == NULL ){
				printf("동적할당 에러\n");
				return -1;
			}

			memset(uri->path, 0, length->path_length + 1);
			snprintf(uri->path, length->uri_length + 1, "%s", request_line->uri_pointer);

			
			/*fragment 파싱*/
			new_element_start = end_of_element + 1;/*SP*/
			changed_size = length->uri_length - ( parse_element_size + 1 ); /*SP*/

			/*기호는 존재하지만 내용이 없는 경우 exception 처리*/
			if ( changed_size == 0 ){
				printf("fragment 길이가 0 입니다\n");
				uri->query = NULL;
				length->query_length = 0;

				length->fragment_length = 0;
				uri->fragment = "";
				return 0;
			}

			length->fragment_length = changed_size;
			uri->fragment = malloc(length->fragment_length + 1);
			if ( uri->fragment == NULL ){
				printf("동적할당 에러\n");
				return -1;
			}
			memset(uri->fragment, 0, length->fragment_length + 1);
			snprintf(uri->fragment, length->fragment_length + 1, "%s", new_element_start);

			

			/*uri NULL 처리*/
			length->query_length = 0;
			uri->query = NULL;

			return 0;
		}
	}else{
		printf("query 존재\n");

		/*path 파싱*/
		end_of_element = word_finder;
		parse_element_size = end_of_element - ( request_line->uri_pointer );
		length->path_length = parse_element_size;
		uri->path = malloc(parse_element_size + 1);
		if ( uri->path == NULL ){
			printf("동적할당 에러\n");
			return -1;
		}

		memset(uri->path, 0, length->path_length + 1);
		snprintf(uri->path, length->path_length + 1, "%s", request_line->uri_pointer);

		printf("path : %s\n", uri->path);

		changed_size = length->uri_length - ( parse_element_size + 1 );

		printf("%d\n", changed_size);
		/*기호는 존재하지만 내용이 없는 경우 exception 처리*/
		if ( changed_size == 0 ){
				printf("query 길이가 0 입니다\n");
				uri->query = "";
				length->query_length = 0;
				printf("query : %s\n", uri->query);
				length->fragment_length = 0;
				uri->fragment = NULL;
				return 0;
			}

		
		new_element_start = end_of_element + 1;	

		word_finder = memchr(new_element_start, HASH_MARK, changed_size);

		if ( word_finder == NULL ){
			printf("fragment 없음\n");
			/*query 파싱*/
			length->query_length = changed_size;
			
			uri->query = malloc(length->query_length + 1);
			if ( uri->query == NULL ){
			printf("동적할당 에러\n");
			return -1;
			}
			memset(uri->query, 0, length->query_length + 1);
			snprintf(uri->query, length->query_length + 1, "%s", new_element_start);

			length->fragment_length = 0;
			uri->fragment = NULL;
			return 0;
			
		}else{
			printf("fragment 있음\n");
			/*query 파싱*/
			end_of_element = word_finder;
			parse_element_size = end_of_element - new_element_start;

			length->query_length = parse_element_size;
			uri->query = malloc(length->query_length + 1);
			if ( uri->query == NULL ){
			printf("동적할당 에러\n");
			return -1;
			}
			memset(uri->query, 0, length->query_length + 1);
			snprintf(uri->query, length->query_length + 1, "%s", new_element_start);

			
			changed_size = changed_size - ( parse_element_size + 1 ); /*# mark*/

			if ( changed_size == 0 ){
				printf("fragment 길이가 0 입니다\n");

				length->fragment_length = 0;
				uri->fragment ="";
				return 0;
			}
			new_element_start = end_of_element + 1; /*# mark*/

			/*fragment 파싱*/
			length->fragment_length = changed_size;
			uri->fragment = malloc(length->fragment_length + 1);
			if ( uri->fragment == NULL ){
				printf("동적할당 에러\n");
				return -1;
			}
			memset(uri->fragment, 0, length->fragment_length + 1);
			snprintf(uri->fragment, length->fragment_length + 1, "%s", new_element_start);
			return 0;
		}	
	}
return 0;
}
