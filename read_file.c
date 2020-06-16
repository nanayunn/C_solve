#include <sys/types.h> 
#include <sys/stat.h> 
#include <stdio.h> 
#include "http_define.h"

/*int stat(const char *path, struct stat * buf);*/
/*char* fgets(char* s , int length , FILE* stream) ;*/

/*읽을 file의 경로를 입력 받고,
매개 변수 file_size와 입력받은 file_path를 file_info 함수에 넘겨주어 
*/
char* get_file_path(int *file_size)
{
	/*변수 선언 및 초기화*/
	char file_path[MAX_MEMORY_SIZE];
	char *file_start_pointer = NULL;
	int file_path_length = 0;
    int attempt_num = 0; 


	memset(file_path, 0, MAX_MEMORY_SIZE);

	/* 매개 변수 exception 처리 */
	if(file_size == NULL)
	{
		printf("받아온 file size가 NULL입니다.\n");
		return -1;
	}

	while(1)

	{
		if( attempt_num == MAX_ATTEMPT ){	
			printf("최대 시도 횟수 범위를 넘었습니다. 파일 경로 확인 바람");
            return -1;
		}

		printf("데이터를 가공할 파일의 경로를 입력해주세요!\n");
        printf("파일의 경로: \n");
		
        /*파일의 경로값을 입력받는다.*/
        scanf("%s", file_path);
        printf("%s", file_path);

        /*NULL 값('\0')을 제외한 문자열의 길이를 반환하는 strlen으로 file_path의 길이를 구해준다.*/
		file_path_length = strlen(file_path);

        /*아무것도 입력이 되지 않았을 시의 exception*/
		if( file_path_length == 0 ){
			return -1;
		}

        /*file_path와 빈 file_size를 넘겨주면 
        get_file_size() 함수를 통해 file_size 계산, 파일이 어디서 시작하는지 포인터를 넘겨준다.*/
		file_start_pointer = file_info(file_path, file_size);

		if( file_start_pointer == NULL ){
			printf("파일을 읽을 수가 없습니다.. 다시 시도는 해봅니다...");
			attempt_num++;
			continue;
		}else{
			goto exit;
		}
		
	}

exit:

	return file_start_pointer;
}

/*파일의 정보를 얻는 함수
입력받은 file_path와 file_size를 매개변수로 받는다.
file_size를 get_file_size로 획득,
완성된 파일 정보로 file_read()실행,
읽은 파일을 파싱하기 위해 첫 시작 포인터를 넘겨받는다.*/
char* file_info(char *file_path, int *file_size)
{
	/*변수 선언 및 초기화*/
	char *file_start_pointer = NULL;

	/* 매개변수 유효값 체크 */
	if( file_path == NULL ){
		printf("file_info : 파일 경로가 NULL 입니다.\n");
		return -1;
	}

	if( file_size == NULL ){
		printf("file_info : 파일 사이즈가 NULL입니다.\n");
		return -1;
	}

    /*file_size의 포인터를 이용하여 변수의 값에 직접 접근,
    get_file_size()를 통해 나온 값을 대입하여 준다.*/
	*file_size = get_file_size(file_path);

    /*파일이 비었을 경우 예외처리*/    
	if( *file_size == 0 ){
		printf("file_info: 파일에 읽을 값이 없는데요..!?\n");
		return -1;
	}
	
    /*file_path와 file_size를 갖고 파일을 읽는다.*/
	file_start_pointer = file_read(file_path, *file_size);
    
	if( file_start_pointer == NULL ){
		printf("file_info :뭔가 잘못되었습니다. 파일 포인터의 값이 이상합니다..NULL\n");
		return -1;
	}

	return file_start_pointer;
}

/*file_size를 계산하는 함수
file_path를 매개변수로 받아 stat 함수를 이용하여 st_size를 받아와 file_size에 대입한다.*/
int get_file_size(char *file_path)
{
	/*변수 선언 및 초기화*/
	struct stat stat_stat;
	int result = 0;

	memset(stat_stat, 0, sizeof(stat_stat));

	result = stat(file_path, &stat_stat);

	if( result != 0 ){
        printf("get_file_size : 파일 정보를 읽어오는 것에 실패하였습니다....");
		return -1;
	}

return stat_stat.st_size;
}


char* file_read(char* file_path, int file_size)
{
	/*변수 선언 및 초기화*/
	FILE *file_pointer = NULL;
	char *file_read_buffer = NULL;
	int real_file_size = 0;

	/* 매개변수 예외처리 */
	if( file_size == 0 ){
		printf("file_read : 파일 사이즈 제대로 전달 안되었어요..\n");
		return -1;
	}

    /*fopen 으로 파일 포인터가 입력된 file_path에 있는 파일을 가리키도록 한다.*/
	file_pointer = fopen(file_path, "rt");

	if( file_pointer == NULL ){
		printf("file_read : 파일 오픈하려는데 포인터 설정이 제대로 안되었네요..");
		return -1;
	}

	file_read_buffer = (char *)malloc(file_size);
	if( file_read_buffer == NULL ){
		printf("file_read : 파일을 읽는 버퍼에 메모리 할당이 제대로 되지 않았습니다...\n");
        printf("file_read : 파일 버퍼를 종료합니다....\n");
		goto file_close;
	}

    /*fread 성공 시 sizeof(char) * file_size 만큼의 바이트 단위를 뽑게된다.
    읽은 내용은 file_read_buffer에 저~~장!!!*/
	real_file_size = fread(file_read_buffer, sizeof(char), file_size, file_pointer);
	if( real_file_size != file_size ){
		printf("file_read : 파일을 읽다 말았나 봅니다... 다시 실행합시다..");
		goto file_close;
	}

	/* clean up */

file_close:

    /*파일 오픈하긴했는데 다음 단계가 제대로 되지 않은 것이니 
    NULL이 아닐 때 닫아주어야함.*/
	if( file_pointer != NULL ){
		fclose(file_pointer);
	}


return file_read_buffer;
}
