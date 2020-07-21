#include"config_header.h"

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

/*file_size를 계산하는 함수
매개 변수 : char * file_path
반환값 : file_size, 에러시 -1*/
int get_file_size(char *file_path)
{
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

	/* 파일 사이즈 Exception */
	if ( stat_stat.st_size < 0 ){
		printf("get_file_size : file내에 읽을 값이 충분하지 않습니다.\n");
		return -1;
	}else if( stat_stat.st_size == 0 ){
		printf("빈 파일입니다.\n");
		return stat_stat.st_size;
	}


return stat_stat.st_size;
}



/*파일을 읽는 함수
매개변수 :  file_path
반환값 : 0, 에러시 -1*/
int read_file(char * file_path, struct file * file)
{	

	/*구조체 포인터 매개변수에 대한 Exception 처리*/
	if( file_path == NULL ){
		printf("read_file : 매개변수 전달 에러입니다.\n");
		return -1;
	}
	if ( file == NULL ){
		printf("file : 매개 변수 전달 에러입니다.");
		return -1;
	}


	/*사용하는 변수 초기화*/
	int fd = 0;
	int temp_val = 0;
	/*get_file_size()를 이용하여 file 구조체에 사이즈 값을 구해 저장*/
	
	file->file_size = get_file_size(file_path);
	if( file->file_size < 0 ){
		printf("get_file_size 오류입니다.");
		return -1;
	}

	/*파일을 읽어서 메모리에 저장할 버퍼는 file 구조체 안에 있습니다.
	file_size만큼 동적할당 합니다.*/

	file->file_pointer = malloc(file->file_size);
	if( file-> file_pointer == NULL ){
		printf("file_pointer에 메모리 할당 실패!\n");
		return -1;
	}

	fd = open ( file_path, O_RDONLY );
	if ( fd < 0 ){
		printf("파일을 열지 못했습니다. fd 반환값 오류\n");
		free(file->file_pointer);
		return -1;
	}

	/*open한 파일을 read로 읽어오고, temp_val에 읽은 바이트 수를 임시 저장한 후
	file_size와 비교하여 read가 완전하게 이루어졌는지 체크*/
	temp_val = read(fd, file->file_pointer, file->file_size);
	if ( temp_val < 0 ){
		printf("read 실패\n");
		free(file->file_pointer);
		close(fd);
		return -1;
	}
	if ( temp_val != file->file_size ){
		printf("read가 정상적으로 이루어지지 않았습니다.\n저장된 파일 사이즈 : %d\n읽어들인 바이트 수 : %d\n", file->file_size, temp_val);
		free(file->file_pointer);
		close(fd);
		return -1;
	}

	/*파일을 닫아준다.*/
	close(fd);	

return 0;
}

/*헤더 프린팅 함수*/ 
int print_header(struct header_list * header_list)
{	
	struct header * header_tail;
	header_tail = header_list->header_head;

	int attemp_num = 0;
	if ( header_list == NULL ){
		printf("구조체 전달 오류 입니다. \n");
		return -1;
	}
	while( header_tail != NULL ){
		attemp_num++;
		if ( attemp_num == BREAK_LOOP_NUM ){
			printf("헤더 출력 허용 범위를 넘어섰습니다. 512번째의 헤더까지만 출력합니다.\n");
			return 0;
		}
		printf("name : %s, ", header_tail->name);
		printf("value : %s\n", header_tail->value);
		header_tail = header_tail->next;

	}

return 0;
}

/*config 프린팅 함수*/ 
int print_config(struct config_list * config_list)
{	
	struct config_line * config_list_tail;
	config_list_tail = config_list->config_line_start;

	int attemp_num = 0;
	if ( config_list == NULL ){
		printf("구조체 전달 오류 입니다. \n");
		return -1;
	}
	while( config_list_tail != NULL ){
		attemp_num++;
		if ( attemp_num == BREAK_LOOP_NUM ){
			printf("헤더 출력 허용 범위를 넘어섰습니다. 512번째의 헤더까지만 출력합니다.\n");
			return 0;
		}
		printf("type : %d, ", config_list_tail->type);
		printf("match : %d\n", config_list_tail->match);
		printf("type_value : %s\n", config_list_tail->type_value);
		config_list_tail = config_list_tail->next;

	}

return 0;
}