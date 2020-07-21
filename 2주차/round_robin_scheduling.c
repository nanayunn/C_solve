#include"config_header.h"

int compare_and_lb(struct config_list * config_list, struct http_compare_point * http_compare_point)
{
    struct config_line * temp_config_line = NULL;

    int server_alloc_success = 0;
    int result = 0;
    int attemp_num = 0;

    if ( config_list == NULL ){
        printf("config_list 매개변수 전달 에러 입니다.\n");
        return -1;
    }
    if ( http_compare_point == NULL ){
        printf("http_compare_point 매개변수 전달 에러 입니다.\n");
        return -1;
    }

    temp_config_line = config_list->config_line_start;
    if ( http_compare_point->host == NULL ){

        while(temp_config_line != NULL){
            
            attemp_num ++;
            if ( attemp_num > config_list->config_total_cnt ){
                printf("this is 무한루프\n");
                return -1;
            }
            if ( temp_config_line->type_value_length > http_compare_point->path_length ){
                printf("이 매치는 애초에 불가능\n");
                temp_config_line = temp_config_line->next;
                continue;
            }
            /*path만 매치*/
            if ( temp_config_line->type == PATH ){

                if ( temp_config_line->type_value_length > http_compare_point->path_length ){
                    printf("이 매치는 애초에 불가능\n");
                    temp_config_line = temp_config_line->next;
                    continue;
                }

                result = is_it_match(temp_config_line->type_value, http_compare_point->path, temp_config_line->type_value_length, http_compare_point->path_length, temp_config_line->match);
                if ( result == 0 ){

                    printf("매칭 서버 발견!!\n");
                    printf("매칭 서버 : %s\n", temp_config_line->server_list[temp_config_line->server_call_num]->ip);
                    printf("포트 번호 : %s\n", temp_config_line->server_list[temp_config_line->server_call_num]->port);
                    temp_config_line->server_call_num = ( temp_config_line->server_call_num + 1 ) % temp_config_line->total_server_count;

                    server_alloc_success = 0;
                    return server_alloc_success;

                }else if(result == SEARCH_FAIL){
                    temp_config_line = temp_config_line->next;

                }else{
                    printf("this is 오류\n");
                    return -1;
                }
            }else{
                temp_config_line = temp_config_line->next;
            }
        }


    }else{

        while(temp_config_line != NULL){

            attemp_num ++;
            if ( attemp_num > config_list->config_total_cnt ){
                printf("this is 무한루프\n");
                return -1;
            }


            if ( temp_config_line->type == PATH ){

                if ( temp_config_line->type_value_length > http_compare_point->path_length ){
                printf("이 매치는 애초에 불가능\n");
                temp_config_line = temp_config_line->next;
                continue;
                }

                result = is_it_match(temp_config_line->type_value, http_compare_point->path, temp_config_line->type_value_length, http_compare_point->path_length, temp_config_line->match);
                if ( result == 0 ){

                    printf("매칭 서버 발견!!\n");
                    printf("http path : %s\n", http_compare_point->path);
                    printf("config path : %s\n", temp_config_line->type_value);
                    printf("매칭 서버 : %s\n", temp_config_line->server_list[temp_config_line->server_call_num]->ip);
                    printf("포트 번호 : %s\n", temp_config_line->server_list[temp_config_line->server_call_num]->port);
                    temp_config_line->server_call_num = ( temp_config_line->server_call_num + 1 ) % temp_config_line->total_server_count;

                    server_alloc_success = 0;
                    return server_alloc_success;

                }else if(result == SEARCH_FAIL){
                    temp_config_line = temp_config_line->next;

                }else{
                    printf("this is 오류\n");
                    return -1;
                }
            }else if ( temp_config_line->type == HOST ){

                if ( temp_config_line->type_value_length > http_compare_point->host_length ){
                    printf("이 매치는 애초에 불가능\n");
                    temp_config_line = temp_config_line->next;
                    continue;
                }

                result = is_it_match(temp_config_line->type_value, http_compare_point->host, temp_config_line->type_value_length, http_compare_point->host_length, temp_config_line->match);
                if ( result == 0 ){

                    printf("매칭 서버 발견!!\n");
                    printf("http host : %s\n", http_compare_point->host);
                    printf("config host : %s\n", temp_config_line->type_value);
                    printf("매칭 서버 : %s\n", temp_config_line->server_list[temp_config_line->server_call_num]->ip);
                    printf("포트 번호 : %s\n", temp_config_line->server_list[temp_config_line->server_call_num]->port);
                    temp_config_line->server_call_num = ( temp_config_line->server_call_num + 1 ) % temp_config_line->total_server_count;

                    server_alloc_success = 0;
                    return server_alloc_success;

                }else if(result == SEARCH_FAIL){
                    temp_config_line = temp_config_line->next;

                }else{
                    printf("this is 오류\n");
                    return -1;
                }

            }else{
                printf("매칭 TYPE 값에 해당 없는 TYPE");
                temp_config_line = temp_config_line->next;
            }
        
        
        }

    }
server_alloc_success = SEARCH_FAIL;

return server_alloc_success;    
}

int is_it_match(char * type_value, char * path_or_host_pointer, int type_value_length, int path_host_length ,int match_word)
{
    char * start_compare = NULL;

    int try_num = 0;
    int index = 0;
    int temp_size = 0;
    int result = 0;
    int success = 0;

    if ( type_value == NULL ){
        printf("type value 매개변수 전달 에러 입니다.\n");
        return -1;
    }
    if ( path_or_host_pointer == NULL ){
        printf("path_of_host_pointer 매개변수 전달 에러 입니다.\n");
        return -1;
    }
    if ( type_value_length == 0 ){
        printf("type_value_length 매개변수 전달 에러 입니다.\n");
        return -1;
    }
    if ( match_word > ELSE_MATCH || match_word < 0 ){
        printf("match_word 매개변수 전달 에러 입니다.\n");
        return -1;
    }

    if ( path_host_length == type_value_length ){
        result = strncmp(type_value, path_or_host_pointer, type_value_length);
        if ( result == 0 ){
            success = 0;
            printf("바로 찾음\n");
            return success;
        }
    }

    if ( match_word == ANY ){
        int try_num = path_host_length - type_value_length + 1;

        for ( index = 0; index < try_num; index ++){

            path_or_host_pointer = path_or_host_pointer + index;
            
			result = strncmp(type_value, path_or_host_pointer, type_value_length);
			if ( result == 0 ){

				success = 0;
				return success;

			}else{

				success = SEARCH_FAIL;
				return success;

			}

        }
    }else if ( match_word == START ){
        result = strncmp(type_value, path_or_host_pointer, type_value_length);
        if ( result == 0 ){

            success = 0;
            return success;

        }else{

			success = SEARCH_FAIL;
			return success;

		}
    }else if ( match_word == END ){
        temp_size = path_host_length - type_value_length;
        start_compare = path_or_host_pointer + temp_size;
        result = strncmp(type_value, start_compare, type_value_length);
        if ( result == 0 ){

            success = 0;
            return success;

        }else{

			success = SEARCH_FAIL;
			return success;
        }
	}else{
        printf("매칭 값에 없는 매칭 방법입니다.\n");
        success = SEARCH_FAIL;
        return success;
    }
           
return success;    
}