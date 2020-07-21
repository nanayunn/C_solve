#include"config_header.h"


int main(int argc, char *argv[])
{

	char * file_path = NULL;

	int argv_length = 0;
	int result = 0;

	if ( argc != ARGC_NUM ) {
		printf("프로그램 실행 시 config 파일의 경로를 함께 입력해 주세요.\n");
		return -1;
	}

	argv_length = strlen(argv[1]);
	if ( argv_length <=0 ){
		printf("파일 경로를 제대로 입력해주세요.\n");
		return -1;
	}
	
	file_path = argv[1];
	if ( file_path == NULL ){
		printf("file_path 포인터 오류입니다.");
		return -1;
	}
	printf("입력한 config 경로 : %s\n", file_path);

	result = config_main(file_path);
	if ( result == -1 ){
		printf("parsing_main에서 오류 발생\n");
		return -1;
	}

return 0;	
}



