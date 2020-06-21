#include"http_define.h"



file_t * file;


int main(int argc, char *argv[])
{
	/*argc 값을 기준으로 정해진 값이 아니면 안내 메세지를 출력*/
	if (argc != ARGC_NUM) {
		printf("파싱을 진행할 파일의 경로를 제대로 입력해 주세요.\n");
		return -1;
	}
	
	int argv_lenghth = 0;

	argv_lenghth = strlen(argv[1]);

	if ( argv_lenghth == 0 ) {
		printf("파일 이름을 제대로 입력해주세요.\n");
		return -1;
	}
	if ( sizeof(argv[1]) > MAX_FILE_NAME_SIZE ){
		printf("파일 이름이 지나치게 깁니다.\n");
		return -1;
	}


	get_file_size(argv[1], file);
	read_file(file);

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

/*file_size를 계산하는 함수
매개 변수 : char * file_path
반환값 : file_size, 에러시 -1*/
int get_file_size(char *argv_val, file_t * file)
{
	/*전달된 매개변수 예외 처리*/
	if( argv_val == NULL ){
		printf("get_file_size : 매개변수 전달 에러 입니다.\n");
		return -1;
	}

	/*변수 선언 및 초기화*/
	struct stat stat_stat;
	int result = 0;

	/*stat()을 이용
	성공시 0, 실패 시 -1 값을 반환*/
	result = stat(argv_val, &stat_stat);
	if( result != 0 ){
        printf("get_file_size : 파일 정보를 읽어오는 것에 실패하였습니다....\n");
		return -1;
	}

	/* 파일 사이즈 Exception */
	if ( stat_stat.st_size <= 0 ){
		printf("get_file_size : file내에 읽을 값이 충분하지 않습니다.\n");
	}

	/*매개변수로 받아왔던 argv 값을 file 구조체 내에 저장해줍니다.
	오버플로우 이슈는 argv[1] 값을 매개변수로 전달받기 전에 체크*/
	memcpy(file->file_path, *argv_val, sizeof(argv_val));
	
	/*stat으로 구한 파일 사이즈를 file 구조체 내에 저장해줍니다.*/
	file->file_size = stat_stat.st_size;

return 0;
}


/*파일을 읽는 함수
매개변수 : 전역변수 구조체 file
반환값 : 0, 에러시 -1*/
int read_file(file_t * file)
{	
	/*구조체 포인터 매개변수에 대한 Exception 처리*/
	if( file == NULL ){
		printf("read_file : 매개변수 전달 에러입니다.\n");
		return -1;
	}

	/*사용하는 변수 초기화*/
	int fd = 0;
	int temp_val = 0;

	/*파일을 읽어서 메모리에 저장할 버퍼는 file 구조체 안에 있습니다.
	file_size만큼 동적할당 합니다.*/
	file->file_pointer = malloc(file->file_size);
	if( file-> file_pointer == NULL ){
		printf("file_pointer에 메모리 할당 실패!\n");
		return -1;
	}

	fd = open ( file->file_path, O_RDONLY );
	if ( fd < 0 ){
		printf("파일을 열지 못했습니다. fd 반환값 오류\n");
		return -1;
	}

	/*open한 파일을 read로 읽어오고, temp_val에 읽은 바이트 수를 임시 저장한 후
	file_size와 비교하여 read가 완전하게 이루어졌는지 체크*/
	temp_val = read(fd, file->file_pointer, file->file_size);
	if ( temp_val < 0 ){
		printf("read 실패\n");
		return -1;
	}
	if ( temp_val != file->file_size ){
		printf("read가 정상적으로 이루어지지 않았습니다.\n저장된 파일 사이즈 : %d\n읽어들인 바이트 수 : %d\n", file->file_size, temp_val);
		return -1;
	}

	/*파일을 닫아준다.*/
	close(fd);

return 0;
}


