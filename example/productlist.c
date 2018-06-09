#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"
#include "productlist.h"

typedef struct {
	char company[20], name[20], price[20], count[20];
}Ramen;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

/*Reads Json File*/
char *readJSONfile() {
	FILE * file;
	file = fopen("data4.json", "r");
	if (file == NULL) {
		printf("Can't open file\n");
		exit(1);
	}
	char oneline[255];
	char * JSON_STRING = (char*)malloc(sizeof(oneline));
	JSON_STRING[0] = '\0';
	while (1) {
		fgets(oneline, sizeof(oneline), file);
		JSON_STRING = (char*)realloc(JSON_STRING, sizeof(char)*(strlen(JSON_STRING) + strlen(oneline) + 1));
		strcat(JSON_STRING, oneline);
		if (feof(file)) break;
	}
	fclose(file);
	return JSON_STRING;
}

/**/
void jsonNameList(char * jsonstr, jsmntok_t *t, int tokcount, NameTokenInfo *nameTokIndex) {
	int i = 0, j = 0, k = 0;
	int depth1 = 0, depth2 = 0;
	//Ramen항목 아래 array, object순으로 되어있다. 
	//이 특성을 이용해 array와 object가 연달아 나왔을때 parent값을 저장
	for (i = 0; i < tokcount; i++) {
		if (i > 0 && t[i - 1].type == JSMN_ARRAY && t[i].type == JSMN_OBJECT) {
			depth1 = t[i].parent; //array를 가르킴
			//printf("%d\n", depth1);
			depth2 = t[i + 1].parent; //object를 가르킴
			//printf("%d\n", depth2);
			break;
		}
	}
	for (i = 0; i< tokcount; i++) {
		if (t[i].parent == depth1) { //parent가 array이면 depth2값을 조정
			depth2 = i;
		}
		if (t[i].parent == depth2) { //parent가 object일때
			nameTokIndex[j].objectList = depth2;
			nameTokIndex[j++].tokindex = i;
			//printf("%d %.*s\n", j,t[i].end - t[i].start, jsonstr + t[i].start);
			//printf("%d, ", nameTokIndex[j - 1].objectList);
			//printf("%d\n", nameTokIndex[j - 1].tokindex);
		}
	}
}
void printToken(char * jsonstr, jsmntok_t *t, NameTokenInfo *nameTokIndex) {
	int index = -1, savePreviousObject = -1;//index: +1을 더했을때 초기화 되도록 sPO는 바로 바뀌도록
	int i = 0;
	//다 따로따로하면 너무 지저분해서 하나로 합침
	Ramen ramen[25]; //25인 이유는 nameTokIndex를 100으로 지정했는데 라면객체는 4개씩 담당하고 있어서
	for (i = 0; i < 25; i++) {
		ramen[i].company[0] = '\0';
		ramen[i].name[0] = '\0';
		ramen[i].count[0] = '\0';
		ramen[i].price[0] = '\0';
	}
	i = 0;
	printf("**************************************\n");
	printf("번호	제품명	제조사	가격	개수\n");
	printf("**************************************\n");
	while (nameTokIndex[i].objectList != -1) {
		if (nameTokIndex[i].objectList != savePreviousObject) {
			savePreviousObject = nameTokIndex[i].objectList;
			index++;
		}
		//t[nameTokIndex[i].tokindex].end - t[nameTokIndex[i].tokindex].start, jsonstr + t[nameTokIndex[i].tokindex].start string 프린트하는 양식
		if (jsoneq(jsonstr, &t[nameTokIndex[i].tokindex], "name") == 0) {
			sprintf(ramen[index].name, "%.*s", t[nameTokIndex[i].tokindex + 1].end - t[nameTokIndex[i].tokindex + 1].start, jsonstr + t[nameTokIndex[i].tokindex + 1].start);
		}
		else if (jsoneq(jsonstr, &t[nameTokIndex[i].tokindex], "company") == 0) {
			sprintf(ramen[index].company, "%.*s", t[nameTokIndex[i].tokindex + 1].end - t[nameTokIndex[i].tokindex + 1].start, jsonstr + t[nameTokIndex[i].tokindex + 1].start);
		}
		else if (jsoneq(jsonstr, &t[nameTokIndex[i].tokindex], "price") == 0) {
			sprintf(ramen[index].price, "%.*s", t[nameTokIndex[i].tokindex + 1].end - t[nameTokIndex[i].tokindex + 1].start, jsonstr + t[nameTokIndex[i].tokindex + 1].start);
		}
		else if (jsoneq(jsonstr, &t[nameTokIndex[i].tokindex], "count") == 0) {
			sprintf(ramen[index].count, "%.*s", t[nameTokIndex[i].tokindex + 1].end - t[nameTokIndex[i].tokindex + 1].start, jsonstr + t[nameTokIndex[i].tokindex + 1].start);
		}
		i++;
	}
	/*
	for (i = 0; i < index + 1; i++) {
		printf("%d	%s	%s	%s	%s\n", i + 1, ramen[i].name, ramen[i].company, ramen[i].price, ramen[i].count);
	}*/
	i = 0;
	while(ramen[i].name[0] != '\0'){
		printf("%d	%s	%s	%s	%s\n", i + 1, ramen[i].name, ramen[i].company, ramen[i].price, ramen[i].count);
		i++;
	}
}

int main() {
	int i;
	int r;
	jsmn_parser p;
	NameTokenInfo nameTokIndex[100];
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	char* JSON_STRING = readJSONfile();
	//printf("%s\n", JSON_STRING);

	

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}
	jsonNameList(JSON_STRING, t, r, nameTokIndex);
	printToken(JSON_STRING,t,nameTokIndex);
#ifdef nothing
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}
	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "name") == 0) {
			/* We may use strndup() to fetch string value */
			printf("- name: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "keywords") == 0) {
			/* We may additionally check if the value is either "true" or "false" */
			printf("- keywords: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "description") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- description: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "frameworks") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- frameworks: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "platforms") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- platforms: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "exclude") == 0) {
			/* We may want to do strtol() here to get numeric value */
			printf("- exclude: %.*s\n", t[i+1].end-t[i+1].start,
					JSON_STRING + t[i+1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "examples") == 0) {
			int j;
			printf("- examples:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i+1].size + 1;
		}
		else if(jsoneq(JSON_STRING, &t[i], "repository") == 0){
			int j, k;
			printf("- repository:\n");
			if (t[i+1].type != JSMN_OBJECT) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				int g = i+j+2;
				//printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
				if (jsoneq(JSON_STRING, &t[g], "type") == 0) {
					/* We may want to do strtol() here to get numeric value */
					printf("	- type: %.*s\n", t[g+1].end-t[g+1].start,
							JSON_STRING + t[g+1].start);
					
				}
				else if (jsoneq(JSON_STRING, &t[i], "url") == 0) {
					/* We may want to do strtol() here to get numeric value */
					printf("	- url: %.*s\n", t[i+1].end-t[i+1].start,
							JSON_STRING + t[i+1].start);

				}
			i += t[i+1].size + 1;
			}
		}
		else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
					JSON_STRING + t[i].start);
		}
	}
#endif
	free(JSON_STRING);
	return EXIT_SUCCESS;
}
