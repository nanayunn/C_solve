#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h> 


#define MAX_PHONE_BOOK_SIZE 512
#define MAX_FILE_NAME 512
#define MAX_USER_NUM 10
#define MAX_ID_LEN 16
#define MAX_NAME_LEN 16
#define MAX_PHONE_NUM_LEN 16

#define CRLF "\r\n"
#define CR '\r'
#define LF '\n'

#define TRUE 1
#define FALSE 0

#define SUCCESS 0
#define ERROR -1

typedef struct
{
    char file_data[MAX_PHONE_BOOK_SIZE];
    int written_size;

}file_t;

typedef struct
{   
    char ID[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    char phone_num[MAX_PHONE_NUM_LEN];
}phone_book_t;



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
    printf("result success: %d\n", result);

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


/*=======================================
파일을 읽는 함수
매개변수 :  file_path, struct file
반환값 : 0, 에러시 -1*/
int read_file(char * file_path, file_t * file)
{	
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
	}
    if ( file->written_size >MAX_PHONE_BOOK_SIZE ){
        printf("파일 내용을 모두 가져올 수 없습니다. 종료합니다.\n");
        return -1;
    }

	fd = open ( file_path, O_RDONLY | O_APPEND);
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

return 0;
}

/* ==========================================
전화번호부를 저장할 파일을 만들어주는 함수
사용자에게 입력받은 파일 경로 자리에 파일이 없다면 만들어주고, 이미 존재하는 파일이라면 프로그램 종료
매개변수 : 파일 경로, 구조체 파일
반환값 : 성공시 0, 실패시 -1
*/
int create_file(char * file_path, file_t * file)
{
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

	fd = open ( file_path, O_WRONLY | O_CREAT | O_EXCL, 0644 );
	if ( fd < 0 ){
		printf("파일을 열지 못했습니다. 파일 생성이 실패했거나 이미 존재하는 파일입니다.\n");
		return -1;
	}
    printf("fd 상태 : %d\n", fd);

	printf("파일 생성이 완료되었습니다.\n");

	close(fd);
return 0;
}

/*==========================================
파일 오픈, 생성 후 이용할 서비스를 안내해주는 함수
user_choice에 따라 이용할 수 있는 서비스가 달라진다.
매개변수 : user_choice, file 구조체
반환값 : 성공시 0, 실패시 -1
*/
int phone_book_func(char * file_path, int user_choice, file_t * file, phone_book_t * user)
{
	int menu_num = 0;
	int result = 0;
	
	if ( user_choice == 0 ){
		printf("매개변수 전달 오류 입니다.\n");
		return -1;
	} 
	if ( file == NULL ){
		printf("구조체 전달 에러입니다.\n");
		return -1;
	}

	while(1){
		if ( user_choice == 1 ){
			printf("이용하실 서비스를 입력해주세요\n");
			printf("1. 추가하기 2. 삭제하기 3. 검색하기 4. 종료하기\n");

			scanf("%d", &menu_num);
			getchar_clear();

			printf("선택하신 메뉴 : %d\n", menu_num);

			if ( menu_num == 1 ){
				result = add_phone_num( file_path, user, file);
				if ( result == -1 ){
					printf("추가하기 실패\n");
					return -1;
				} 
				printf("추가하기가 완료되었습니다. 파일에 저장됩니다.\n");
				break;
			}else if( menu_num == 2 ){

				break;
			}else if( menu_num == 3 ){

				break;
			}else if( menu_num == 4 ){

				break;
			}else{
				printf("메뉴를 잘못 선택하셨습니다.\n");
				continue;
			}


		}else if( user_choice == 2 ){

			printf("이용하실 서비스를 입력해주세요\n");
			printf("1. 추가하기 2. 종료하기\n");

			scanf("%d", &menu_num);
			getchar_clear();

			printf("선택하신 메뉴 : %d\n", menu_num);

			if ( menu_num == 1 ){
				result = add_phone_num( file_path, user, file);
				if ( result == -1 ){
					printf("추가하기 실패\n");
					return -1;
				} 
				printf("추가하기가 완료되었습니다. 파일에 저장됩니다.\n");
				break;

			}else if( menu_num == 2 ){
				printf("종료합니다.\n");
				break;
			}else{
				printf("메뉴를 잘못 선택하셨습니다.\n");
				continue;
			}

		}else{
			printf("유효하지 않은 입력입니다.\n");
			return -1;
		}
	}
return 0;	
}

// int save_into_file_struct(phone_book_t * phone_book, file_t * file)
// {
// 	if ( phone_book == NULL ){
// 		printf("save_into_file_struct : 구조체 전달 에러입니다.\n");
// 		return -1;
// 	}
// 	if ( file == NULL ){
// 		printf("save_into_file_struct : 구조체 전달 에러입니다.\n");
// 		return -1;
// 	}


// }

// int parse_user_info( phone_book_t * phone_book, file_t * file, phone_book_t * user )
// {
// 	if ( phone_book == NULL ){
// 		printf("save_into_file_struct : 구조체 전달 에러입니다.\n");
// 		return -1;
// 	}
// 	if ( file == NULL ){
// 		printf("save_into_file_struct : 구조체 전달 에러입니다.\n");
// 		return -1;
// 	}
// 	if ( user == NULL ){
// 		printf("save_into_file_struct : 구조체 전달 에러입니다.\n");
// 		return -1;
// 	}

/*====================================
파일에 저장된 유저의 명수를 알아오는 함수
매개변수 : file 구조체, 유저 명수를 저장하는 user_index
반환값 : user_index 값, 실패시 -1
*/
int get_user_num(file_t * file, int * user_index)
{

	if ( file == NULL ){
		printf("파일 구조체 오류입니다.\n");
		return -1;
	}
	if ( user_index == NULL ){
		printf("user_index 포인터 오류입니다.\n");
		return -1;
	}

	char * start_user_info = NULL;
	char * end_of_element = NULL;
	char * end_of_file = NULL;
	char * lf_finder = NULL;

	int changed_size = 0;
	int user_num = 0;

	start_user_info = file->file_data;
	changed_size = file->written_size;
	end_of_file = start_user_info + changed_size - 1;/*파일의 끝 지점, 한 문장의 끝처리 CRLF 중 LF를 가르킴*/
	printf("%d\n", changed_size);

	while(1){

		end_of_element = memchr(start_user_info, CR, changed_size);
		if ( end_of_element == NULL ){
			printf("notfound CR, 파일 형식 오류입니다. 수정하지 말하주세요\n");
			return -1;
		}
		lf_finder = memchr(start_user_info, LF, changed_size);
		if ( lf_finder == NULL ){
			printf("notfound LF, 파일 형식 오류입니다. 수정하지 말하주세요\n");
			return -1;
		}
		if ( end_of_element + 1 != lf_finder ){
			printf("파일 형식 오류입니다. 수정하지 말하주세요\n");
			return -1;
		}
		if ( end_of_element + 1 == end_of_file ){
			printf("파일의 끝입니다. 루프를 종료합니다\n");
			user_num++;
			break;
		}
		user_num++;

		changed_size = changed_size - (end_of_element - start_user_info + 2); /*CRLF*/
		if ( changed_size < 0 ){
			printf("파일 사이즈 변환 오류입니다\n");
			return -1;
		}
		printf("user_index : %d, changed_size: %d\n", user_num, changed_size);

		start_user_info = end_of_element + 2; /*CRLF*/

	}
	
printf("user_num : %d\n", user_num);
*user_index = user_num;

printf("user_index : %d\n", *user_index);
return *user_index;	
}

/*======================================================
파일에 정보를 쓰는 함수
매개변수 : fd, 쓸 값을 담은 버퍼, 버퍼의 사이즈
반환값 :  성공시 0, 실패 -1
*/
int write_file(int fd, char * write_buf, int size)
{

	int written_size = 0;
	if ( fd < 0 ){
		printf("fd 파일 디스크립터 오류 입니다.\n");
		return -1;
	}
	if ( write_buf == NULL ){
		printf("write_buf 오류 입니다.\n");
		return -1;
	}
	if ( size == 0 ){
		printf("쓸 값의 크기가 0입니다. 쓸게 없음 \n");
		return -1;
	}


	written_size = write(fd, write_buf, size);

	if ( written_size < 0 ){
		printf("파일 쓰기 에러입니다.\n");
		close(fd);
		return -1;
	}else if( written_size == 0 ){
		printf("파일에 아무것도 쓰이지 않았습니다.\n");
		close(fd);
		return -1;
	}else if ( written_size > 0 && written_size != size ){
		printf("쓰기를 모두 완료하지 못하였습니다. 파일에 쓰기 공간이 부족합니다.\n");
		close(fd);
		return -1;
	}else{
		/*nothing*/
	}
return 0;	
}



/*======================================================
전화번호부를 추가하는 함수
매개 변수 : int user_choice, 
*/
int add_phone_num(char * file_path , phone_book_t * user, file_t * file)
{	

	char * id = "ID: ";
	char * name = " NAME: ";
	char * phone_num = " PHONE_NUM: ";


	int written_size = 0;
	int fd = 0;

	if ( file == NULL ){
		printf("add_phone_num : 구조체 전달 에러입니다.\n");
		return -1;
	}
	if ( user == NULL ){
		printf("add_phone_num : 구조체 전달 에러입니다.\n");
		return -1;
	}
	if ( file_path == NULL ){
		printf("add_phone_num : 구조체 전달 에러입니다.\n");
		return -1;
	}

	printf("%s\n", file_path);
	
	printf("추가하기 서비스를 선택하셨습니다.\n");
	printf("추가할 ID를 입력해주세요.(최대 8자)\n");
	printf("ID : ");
	scanf("%s", user->ID);
	getchar_clear();

	printf("입력하신 ID : %s\n", user->ID);
	printf("%ld\n", strlen(user->ID));


	printf("이름을 입력해주세요.\n");
	printf("NAME : ");
	scanf("%s", user->name);
	getchar_clear();

	printf("입력하신 NAME : %s\n", user->name);

	printf("전화번호를 입력해주세요.(000-0000-0000의 형식으로 입력해주세요)\n");
	printf("PHONE_NUM : ");
	scanf("%s", user->phone_num);
	getchar_clear();

	printf("입력하신 NAME : %s\n", user->phone_num);


	fd = open ( file_path, O_WRONLY | O_APPEND, 0644 );
	if ( fd < 0 ){
		printf("파일을 열지 못했습니다. fd 반환값 오류\n");
		fprintf(stderr, "파일을 open도중 오류 발생: %s\n", strerror(errno));
		return -1;
	}
    printf("fd 상태 : %d\n", fd);

	write_file(fd, id, strlen(id));
	write_file(fd, (user->ID), strlen(user->ID));
	write_file(fd, name, strlen(name));
	write_file(fd, (user->name), strlen(user->name));
	write_file(fd, phone_num, strlen(phone_num));
	write_file(fd, (user->phone_num), strlen(user->phone_num));
	write_file(fd, '\n', 1);

close(fd);
return 0;
}


// int check_ID_valid(phone_book_t * user, file_t * file, int user_index)
// {
// 	pass;
// }

int main(void)
{
    char * file_path = NULL;
    char user_input[MAX_FILE_NAME];

    int filename_len = 0;
    int menu_num = 0;
    int user_choice = 0;
    file_t * file;
    phone_book_t * user;

	phone_book_t phone_book[MAX_USER_NUM];

	int user_index = 0; /* 전화번호부 인원 */

    memset(user_input, 0, MAX_FILE_NAME);

    file = malloc(sizeof(file_t));
	user = malloc(sizeof(phone_book_t));


    while(1){

        printf("전화번호부 프로그램입니다.\n");
        printf("메뉴를 선택해주세요.\n");
        printf("1. 파일 불러오기 2. 파일 새로 만들기\n");

        scanf("%d", &user_choice);
        getchar_clear();

        if ( user_choice == 1 ){
            printf("이전 파일을 불러옵니다.\n");
            printf("불러올 파일 경로를 입력해주세요.\n파일 경로 : ");

            scanf("%s", user_input);
            getchar_clear();

            file_path = user_input;
            printf("%s\n", file_path);

            read_file(file_path, file);

            printf("file read successful\n");

			get_user_num(file, &user_index);

			phone_book_func(file_path, user_choice, file, user);
			printf("%s\n", file_path);


            break;
        }else if ( user_choice == 2 ){
            printf("새로운 전화번호부 파일을 생성합니다.\n");
			printf("파일을 생성할 경로를 입력해주세요.\n파일 경로 : ");

            scanf("%s", user_input);
            getchar_clear();

            file_path = user_input;
            printf("%s\n", file_path);

			create_file(file_path, file);

			printf("file creation successful\n");

			phone_book_func(file_path, user_choice, file, user);
			printf("%s\n", file_path);

            break;

        }else{
            printf("메뉴를 잘못 선택하셨습니다.\n 다시 선택해주세요.\n");
            continue;
        }
    }


    free(file);
    free(user);
    return 0;
}
