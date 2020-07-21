#include"config_header.h"


/*
 * main에서 호출되며, LB를 위해 필요한 모든 함수를 실행하는 실질적인 main 함수입니다.
 *
 * 매개변수 : char * config file_path
 * 반환값 : 성공시 0, 실패시 -1
 * */
int config_main(char * file_path)
{

	struct file config_file;
	struct config_list config_list;
	struct http_compare_point http_compare_point;

	char http_file_path[MAX_HTTP_FILE_NAME];

	int user_choice = 0;
	int result = 0;

	if ( file_path == NULL ){
		printf("매개 변수 전달 에러입니다.\n");
		return -1;
	}

	memset(&config_file, 0, sizeof(struct file));
	memset(&config_list, 0, sizeof(struct config_list));
	memset(&http_compare_point, 0, sizeof(struct http_compare_point));
	memset(http_file_path, 0, MAX_HTTP_FILE_NAME);

	read_file(file_path, &config_file);

	printf("읽어온 파일 내용 : %s\n", config_file.file_pointer);

	parse_config_line(&config_file, &config_list);

	while(1){
		printf("파싱을 진행할 http 파일의 경로를 입력하세요.\n");

		scanf("%s", http_file_path);
		getchar_clear();

		printf("입력한 http 파일 경로 : %s\n", http_file_path);

		parse_http_request_file(&http_file_path, &http_compare_point);
		
		result = compare_and_lb(&config_list, &http_compare_point);
	    if ( result == SEARCH_FAIL ){
			printf("http 파일 내의 path와 host에 매칭되는 서버가 없습니다.\n");
		}else if ( result == -1 ){
			printf("compare and lb 함수 실행 중 에러 발생\n");
            return -1;
		}else{
			printf("매칭 성공~~~\n");
		}	
		
		memset(&http_compare_point, 0, sizeof(struct http_compare_point)); /* 초기화 */
		
		printf("다른 파일을 입력하려면 1번, 종료하려면 2번을 눌러주세요.\n");

		scanf("%d", &user_choice);
		getchar_clear();

		if ( user_choice == CONTINUE ){
			printf("프로그램을 진행합니다.\n");
			continue;
		}else if ( user_choice == EXIT ){
			printf("프로그램을 종료합니다.\n");
			break;
		}else{
			printf("잘못된 입력값입니다. 프로그램을 종료합니다.\n");
			break;
		}

	}

	clean_up_all(&http_compare_point, &config_list, &config_file);

return 0;
}







/*
 * config file내의 값을 한 줄씩 읽어서 구분된 값을 파싱 및 저장하여 
 * 링크드 리스트로 구현하는 함수입니다.
 * 매개변수 : config_file 구조체 포인터, cofig_list 구조체 포인터(헤드 config의 주소값을 갖고 있을 구조체)
 * 반환값 : 성공시 0, 실패시 -1
 */
int parse_config_line(struct file * config_file, struct config_list * config_list)
{
	struct config_line * config_line = NULL;
	struct config_line * config_line_tail = NULL;

	char * new_element_start = NULL;
	char * end_of_element = NULL;
	char * end_of_file = NULL;
	char * lf_finder = NULL;
	
	char temp_buff[MAX_TEMP_BUF_SIZE];

	int written_size = 0;
	int check_size = 0;
	int result = 0;
	int index = 0;
	int config_cnt = 0;
	

	config_line_tail = malloc(sizeof(struct config_line));
	if ( config_line_tail == NULL ){
		printf("config line 동적할당 에러입니다.\n");
		return -1;
	}
	memset(config_line_tail, 0, sizeof(struct config_line));
	
	memset(temp_buff, 0, MAX_TEMP_BUF_SIZE);

	new_element_start = config_file->file_pointer;
	written_size = config_file->file_size;

	printf("파일 시작 : %s\n", new_element_start);
	printf("파일 크기 : %d\n", written_size);

	end_of_file = new_element_start + written_size - 1; /* config_file의 끝지점을 알기 위한 포인터 */


	while(1){


		
		/*한줄의 끝*/
		lf_finder = memchr(new_element_start, LF, written_size);
		if ( lf_finder == NULL ){
			printf("잘못된 형식 입니다.\n");
			break;
		}
		printf("LF : %s\n", lf_finder);

		config_cnt ++;
		printf("cnt : %d\n", config_cnt);
		
		config_line = malloc(sizeof(struct config_line));
 	    	if ( config_line == NULL ){
		       	printf("config line 동적할당 에러입니다.\n");
		        return -1;
        	}
        	memset(config_line, 0, sizeof(struct config_line));

		if ( config_line_tail == NULL ){
		    printf("start config_line\n");
		    config_line_tail = config_line;
        	}else{
		    printf("연결\n");
		    config_line_tail->next = config_line;
		    config_line_tail = config_line;
		}

		/* type parse */
		check_size = get_parsing_length(new_element_start, written_size, SP);
	    	if ( check_size == MAX_TYPE_LENGTH ){
			memcpy(temp_buff, new_element_start, check_size);

			printf("%s\n", temp_buff);

			config_line->type = get_type(temp_buff, check_size);

			memset(temp_buff, 0, MAX_TEMP_BUF_SIZE);
			

		}else{
			if ( check_size == -1 ){
				printf("함수 내 에러 발생\n");
				break;
			}else if ( check_size == ERROR_MEMCHR ){
				printf("SP가 없습니다. 잘못된 형식\n");
				break;
			}else{
				config_line->type = ELSE_TYPE;
			}
			
		}
		new_element_start = new_element_start + check_size + 1; /* SP */
		written_size = written_size - (check_size + 1); /* SP */
		
		printf("%d\n", config_line->type);

		/* match parse */
		check_size = get_parsing_length(new_element_start, written_size, SP);
		printf("check match : %d\n", check_size);
		if ( check_size == MIN_MATCH_LENGTH || check_size == MAX_MATCH_LENGTH ){
			memcpy(temp_buff, new_element_start, check_size);

			printf("match : %s\n", temp_buff);

			config_line->match = get_match(temp_buff, check_size);

			memset(temp_buff, 0, MAX_TEMP_BUF_SIZE);
			

		}else{
			if ( check_size == -1 ){
				printf("함수 내 에러 발생\n");
				break;

			}else if ( check_size == ERROR_MEMCHR ){
				printf("SP가 없습니다. 잘못된 형식\n");
				break;

			}else{
				config_line->match = ELSE_MATCH;
			}
			
		}

		new_element_start = new_element_start + check_size + 1; /* SP */
		written_size = written_size - (check_size + 1); /* SP */

		printf("match : %d\n", config_line->match);

		
		/* type_value parse */
		check_size = get_parsing_length(new_element_start, written_size, SP);
		if ( check_size == -1 ){
			printf("함수 내 에러 발생\n");
			break;

		}else if ( check_size == ERROR_MEMCHR ){
			printf("SP가 없습니다. 잘못된 형식\n");
			break;

		}else{
			/* NONE */
		}

		config_line->type_value_length = check_size;
		config_line->type_value = malloc(config_line->type_value_length + 1); /* NULL */
		if ( config_line->type_value == NULL ){
			printf("type_value 값 메모리 할당 오류입니다.\n");
			return -1;

		}
		memset(config_line->type_value, 0, config_line->type_value_length + 1);	/* NULL */
		memcpy(config_line->type_value, new_element_start, config_line->type_value_length);

		new_element_start = new_element_start + check_size + 1; /* SP */
		written_size = written_size - ( check_size + 1 ); /* SP */

		printf("type_value = %s\n", config_line->type_value);

		/* server list parse */
		for ( index = 0; index < MAX_SERVER_NUM; index ++){

			check_size = get_parsing_length(new_element_start, written_size, COLON);
			if ( check_size == -1 ){
				printf("함수 내 에러 발생\n");
				break;

			}else if ( check_size == ERROR_MEMCHR ){
				printf("colon이 없습니다. 잘못된 형식\n");
				break;

			}else{
				/*NONE*/
			}
			config_line->server_list[index] = malloc(sizeof(struct server));
			memset(config_line->server_list[index], 0, sizeof(struct server));
			config_line->server_list[index]->ip_length = check_size;

			config_line->server_list[index]->ip = malloc(config_line->server_list[index]->ip_length + 1); /* NULL */
			memset(config_line->server_list[index]->ip, 0, config_line->server_list[index]->ip_length + 1); /* NULL */
			memcpy(config_line->server_list[index]->ip, new_element_start, config_line->server_list[index]->ip_length);

			printf("ip : %s\n", config_line->server_list[index]->ip);


			new_element_start = new_element_start + check_size + 1; /* COLON */
			written_size = written_size - ( check_size + 1 ); /* COLON */

			
			check_size = get_parsing_length(new_element_start, written_size, COMMA);
			if( check_size == ERROR_MEMCHR ){
				printf("config file의 끝입니다.\n");

				check_size = lf_finder - new_element_start;
				memset(config_line->server_list[index]->port, 0, MAX_PORT_LENGTH);
				memcpy(config_line->server_list[index]->port, new_element_start, check_size);

				printf("last port : %s\n",config_line->server_list[index]->port);
				printf("last : index : %d\n", index);

				config_line->total_server_count = index + 1;
				config_line->server_call_num = 0;

				break;
			}else if( check_size == -1 ){
				printf("함수 내에서 에러 발생\n");
				break;

			}else{

				if ( new_element_start + check_size > lf_finder ){
					printf("끝지점을 지나쳤습니다. 종료된 상황\n");
					check_size = lf_finder - new_element_start; /*while문 시작할 때 구해준 값(한 줄의 끝)*/
					memset(config_line->server_list[index]->port, 0, MAX_PORT_LENGTH);
					memcpy(config_line->server_list[index]->port, new_element_start, check_size);
					printf("last port : %s\n",config_line->server_list[index]->port);
					printf("last : index : %d\n", index);
					config_line->total_server_count = index + 1;
					config_line->server_call_num = 0;

					break;

				}
				memset(config_line->server_list[index]->port, 0, MAX_PORT_LENGTH);
				memcpy(config_line->server_list[index]->port, new_element_start, check_size);

				printf("port : %s\n",config_line->server_list[index]->port);
				
				new_element_start = new_element_start + check_size + 1; /* COMMA */
				written_size = written_size - ( check_size + 1 ); /* COMMA */
				
			}
			

		}

		

		/*서버 파싱 끝났으면 새로운 config_line 한줄 파싱 위해서 새로운 포인터와 길이를 구해준다.*/
		written_size = written_size - ( check_size + 1 ); /* LF */
		printf("written size : %d\n", written_size);
		if ( written_size == 0 ){
			printf("config file 파싱 끝\n");
			config_line_tail = config_line;
			config_line_tail->next = NULL;
			config_list->config_total_cnt = config_cnt;

			break;

		}if ( written_size < 0 ){
			printf("파일값 계산 에러\n");
			return -1;

		}
		new_element_start = new_element_start + check_size + 1; /* LF */

		
		if ( config_cnt == 1 ){
			config_list->config_line_start = config_line;
		}

	}



if ( check_size < 0 ){
	printf("루프문 실행 중 에러 발생\n");
	free_config_line(config_list);
	return -1;
}


return 0;
}


/* TYPE의 텍스트를 받아 비교하는 함수
* 매개변수 : type 부분의 텍스트를 임시 저장한 temp buff, type 버퍼의 길이
* 반환값 : config_line->type에 들어갈 int 값
*/
int get_type(char * temp_buff, int check_size)
{
		char * host = NULL;
        char * path = NULL;

		host = "HOST";
        path = "PATH";
	
		int result = 0;
		int type = 0;

	if ( temp_buff == NULL ){
		printf("temp buff 매개변수 전달 에러입니다.\n");
		return -1;	
	}
	if ( check_size <= 0 ){
		printf("비교할 값이 없습니다.\n");
		return -1;
	}
	
	result = strncmp(temp_buff, host, check_size);
	if ( result == 0 ){
		type = HOST;
		return type;
	}else{
		result = strncmp(temp_buff, path, check_size);
		if ( result == 0 ){
			type = PATH;
			return type;
		}else{
			type = ELSE_TYPE;
			return type;
		}
	}

return type;
}

char * upper_to_lower(char * temp_buffer, int check_size)
{
	char * temp_pointer = NULL;
	int index = 0;

	if ( temp_buffer == NULL ){
		printf("temp_buff 매개변수 전달 에러입니다.\n");
		return NULL;
	}
	if ( check_size == 0 ){
		printf("check_size 매개변수 전달 에러입니다.\n");
		return NULL;
	}

	for ( index = 0; index < check_size; index++ ) {

		if (temp_buffer[index] >= 'A'&& temp_buffer[index] <= 'Z') {
			temp_buffer[index] = temp_buffer[index] + UPPER_TO_LOWER;
		}
		else {
			/*소문자면 PASS!*/
			continue;
		}

	}
	temp_pointer = temp_buffer;

return temp_pointer;

}

/* MATCH의 텍스트를 받아 비교하는 함수
* 매개변수 : match 부분의 텍스트를 임시 저장한 temp buff, match 버퍼의 길이
* 반환값 : config_line->match에 들어갈 int 값
*/
int get_match(char * temp_buff, int check_size)
{
        char * any = NULL;
        char * start = NULL;
        char * end = NULL;
		char * empty_pointer = NULL;

        any = "any";
        start = "start";
        end = "end";
	
		int result = 0;
		int match = 0;

	if ( temp_buff == NULL ){
		printf("temp buff 매개변수 전달 에러입니다.\n");
		return -1;	
	}
	if ( check_size <= 0 ){
		printf("비교할 값이 없습니다.\n");
		return -1;
	}
	
	empty_pointer = upper_to_lower(temp_buff, check_size);

	if ( check_size == 3 ){

		result = strncmp(empty_pointer, any, check_size);
		if ( result == 0 ){
			match = ANY;
			return match;
		}else{
			result = strncmp(empty_pointer, end, check_size);
			if ( result == 0 ){
				match = END;
				return match;
			}else{
				match = ELSE_MATCH;
				return match;
			}
		}
		
	}else if ( check_size == 5 ){

		result = strncmp(empty_pointer, start, check_size);
		if ( result == 0 ){
			match = START;
			return match;
		}else{
			match = ELSE_MATCH;
			return match;
		}

	}else{
		/* none */
	}
	
return match;
}

















