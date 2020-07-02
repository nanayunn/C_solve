#include"http_define.h"

int main(int argc, char *argv[])
{	
	char * file_path = NULL;

	
	struct file file;

	int argv_length = 0;
	int result = 0;
	
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

	printf("file path : %s\n", argv[1]);

	file_path = argv[1];
	if ( file_path == NULL ){
		printf("file_path 포인터 오류입니다.");
		return -1;
	}

	/*비즈니스 로직 시작*/

	result = read_file(file_path, &file);
	if ( result == -1 ){
		printf("read_fild 에러, 프로그램 종료\n");
		return -1;
	}

	printf("http request start\n");

	result = parse_http_request_file(&file);
	if ( result == -1 ){
		printf("divide_into_req_header 에러, 프로그램 종료\n");
		return -1;
	}
	if ( file.file_pointer != NULL ){
		free(file.file_pointer);
	}

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
	printf("get file size end\n");

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
		close(fd);
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


/*free 및 자원 해제를 위한 함수*/
int clean_up(struct http_request * http_request, struct request_line * request_line, struct URI * URI, struct header_list * header_list)
{
	int result = 0;
	int try_num = 0;

	if ( http_request->header_line_length != 0 ){
		free_header(header_list);
	}
	if ( http_request->garbage != NULL ){
		free(http_request->garbage);
	}
	if ( request_line->method != NULL){
		free(request_line->method);
	}
	if ( request_line->version != NULL){
		free(request_line->version);
	}
	if ( URI->path != NULL){
		free(URI->path);
	}
	if ( URI->query != NULL){
		free(URI->query);
	}
	if ( URI->fragment != NULL){
		free(URI->fragment);
	}
	if ( URI->uri_pointer != NULL){
		free(URI->uri_pointer);
	}

	
	
return 0;	
}


/*파싱 값 프린팅 함수*/ 
int print_all(struct http_request * http_request, struct request_line * request_line, struct URI * URI, struct header_list * header_list)
{	

	print("Method : %s\n", request_line->method);
	print("path : %s\n", URI->path);
	if ( URI->query != NULL ){
		print("query : %s\n", URI->query);
	}else{
		print("query : none(nothing exist)\n");
	}
	if ( URI->fragment != NULL ){
		print("fragment : %s\n", URI->fragment);
	}else{
		print("fragment : none(nothing exist)\n");
	}
	print("version : %s\n", request_line->version);
	print("header : \n");
	if ( header_list->header_head != NULL ){
		print_header(header_list);
	}else{
		printf("header : none(nothing)");
		
	}

	if ( http_request->garbage != NULL ){
		printf("garbage : none(nothing)");
	}else{
		print("garbage : %s", http_request->garbage);
	}

	
return 0;
}

/*헤더 프린팅 함수*/ 
int print_header(struct header_list * header_list)
{

	char * temp_pointer = NULL;
	temp_pointer = header_list->header_head->next;

	int attemp_num = 0;

	if ( header_list == NULL ){
		printf("구조체 전달 오류 입니다. \n");
		return -1;
	}

	while( temp_pointer != NULL ){
		if ( attemp_num == BREAK_LOOP_NUM ){
			printf("헤더 출력 허용 범위를 넘어섰습니다. 512번째의 헤더까지만 출력합니다.\n");
			return 0;
		}
		printf("name : %s, ", temp_pointer->name);
		printf("value : %s", temp_pointer->value);
		temp_pointer = temp_pointer->next;
		attemp_num ++;
	}
return 0;
}


/* 헤더 프리 함수 */
int free_header(struct header_list * header_list)
{


	char * temp_pointer = NULL;
	temp_pointer = header_list->header_head->next;

	int attemp_num = 0;

	if ( header_list == NULL ){
		printf("구조체 전달 오류 입니다. \n");
		return -1;
	}

	while( temp_pointer != NULL ){
		if ( attemp_num > header_list->header_num ){
			printf("헤더의 총 갯수를 넘어섰습니다. 무한 루프로 인식합니다. 종료\n");
			return -1;
		}
		free(temp_pointer->name);
		free(temp_pointer->value);
		temp_pointer = temp_pointer->next;
		attemp_num ++;
	}
	if ( header_list->header_head != NULL ){
		free(header_list->header_head);
	}


return 0;
}
