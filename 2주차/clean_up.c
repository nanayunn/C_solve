#include"config_header.h"

/*free 및 자원 해제를 위한 함수*/
int clean_up(struct http_request * http_request, struct request_line * request_line, struct URI * URI, struct header_list * header_list)
{
	int result = 0;
	int try_num = 0;

	if ( header_list->header_head != NULL ){
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
	if ( URI->fragment != NULL){
		free(URI->fragment);
	}
	if ( URI->uri_pointer != NULL){
		free(URI->uri_pointer);
	}
	if ( http_request->header != NULL ){
		free(http_request->header);
	}
	if ( URI->query != NULL){
		free(URI->query);
	}
	
return 0;	
}


/* 헤더 프리 함수 */
int free_header(struct header_list * header_list)
{
	struct header * header_tail;
	struct header * free_header;
	header_tail = header_list->header_head;
	free_header = header_list->header_head;
	int attemp_num = 0;

	if ( header_list == NULL ){
		printf("구조체 전달 오류 입니다. \n");
		return -1;
	}

	while( header_tail != NULL ){
		attemp_num++;
		if ( attemp_num > header_list->header_num ){
			printf("헤더의 총 갯수를 넘어섰습니다. 무한 루프로 인식합니다. 종료\n");
			return -1;
		}
		free_header = header_tail;
		free(header_tail->name);
		free(header_tail->value);
		header_tail = header_tail->next;
		free(free_header);	
	}


return 0;
}


int free_config_line(struct config_list * config_list)
{
	struct config_line * config_line_tail;
	struct header * free_config_line;
	config_line_tail = config_list->config_line_start;
	free_config_line = config_list->config_line_start;

	int attemp_num = 0;
	int index = 0;

	if ( config_list == NULL ){
		printf("구조체 전달 오류 입니다. \n");
		return -1;
	}

	while( config_line_tail != NULL ){
		attemp_num++;
		if ( attemp_num > config_list->config_total_cnt ){
			printf("config line의 총 갯수를 넘어섰습니다. 무한 루프로 인식합니다. 종료\n");
			return -1;
		}

		free_config_line = config_line_tail;
		free(config_line_tail->type_value);

		for (index = 0; index < config_list->config_total_cnt; index ++){
			free(config_line_tail->server_list[index]->ip);
		}

		config_line_tail = config_line_tail->next;
		free(free_config_line);	
	}

return 0;
}

int clean_up_all(struct http_compare_point * http_compare_point, struct config_list * config_list, struct file *config_file)
{
	if ( http_compare_point->path != NULL ){
		free(http_compare_point->path);
	}
	if ( http_compare_point->host != NULL ){
		free(http_compare_point->host);
	}
	if ( config_list->config_line_start != NULL ){
		free_config_line(config_list);
	}
	if ( config_file->file_pointer != NULL ){
		free(config_file->file_pointer);
	}

return 0;
}
