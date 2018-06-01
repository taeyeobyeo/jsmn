#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

/*static const char *JSON_STRING =
	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";
*/

/*Reads Json File*/
char *readJSONfile() {
	FILE * file;
	file = fopen("data.json", "r");
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
void jsonNameList(char * jsonstr, jsmntok_t *t, int tokcount, int *nameTokIndex) {
	int i = 0;
	int j = 0;
	int depth = t[1].parent; // 0은 {여서 첫번째를 위해 1을 넣음
	//parent랑 똑같은걸 넣음
	for (i = 0; i < tokcount; i++) {
		if (t[i].parent == depth)
			nameTokIndex[j++] = i;
	}
	nameTokIndex[0] = j;
}

void printNameList(char* jsonstr, jsmntok_t *t, int *nameTokIndex) {
	int j = 0;
	printf("***** Name List *****\n");
	for (j = 0; j < 100; j++) {
		if (nameTokIndex[j] == -1) break;
		printf("[Name %d] %.*s\n", j + 1, t[nameTokIndex[j]].end - t[nameTokIndex[j]].start, jsonstr + t[nameTokIndex[j]].start);
	}
	printf("\n");
}

void selectNameList(char* jsonstr, jsmntok_t *t, int *nameTokIndex) {
	int input;
	while (1) {
		printf("\nSelect Name's no (exit:0) >> ");
		scanf("%d",&input);
		if (input == 0) break;
		printf("[Name %d] %.*s\n", input, t[nameTokIndex[input - 1]].end - t[nameTokIndex[input - 1]].start, jsonstr + t[nameTokIndex[input - 1]].start);
		printf("%.*s\n\n", t[nameTokIndex[input - 1] + 1].end - t[nameTokIndex[input - 1] + 1].start, jsonstr + t[nameTokIndex[input - 1] + 1].start);
	}
}

void printObjectList(char* jsonstr, jsmntok_t *t, int *nameTokIndex) {
	int j = 1;
	printf("***** Object List *****\n");
	for (j = 0; j < 100; j++) {
		if (nameTokIndex[j] == 0) break; //0으로 초기화 되어있고 0은 토큰의 특성상 0을가진 토큰은 없어야됨
		printf("[%.*s] %.*s\n", t[nameTokIndex[j]].end - t[nameTokIndex[j]].start, jsonstr + t[nameTokIndex[j]].start, t[nameTokIndex[j] + 1].end - t[nameTokIndex[j] + 1].start, jsonstr + t[nameTokIndex[j] + 1].start);
	}
	printf("\n");
}

int printFirstValue(char* jsonstr, jsmntok_t *t,  int tokcount, int* firstValue){
	int i, j = 0;
	int depth = 0;
	for (i = 0; i < tokcount; i++) {
		if (t[i].type == JSMN_STRING && t[i].size == 1 && t[i - 1].type == JSMN_OBJECT && t[i - 2].type != JSMN_OBJECT) {
			if(depth == 0){
				depth++;
				firstValue[j] = i;
				j++;
			}
		}
		else if(t[i].type == JSMN_STRING && t[i].size != 1 && t[i + 1].type == JSMN_OBJECT){
			if(depth == 1) depth--;
		}
	}
	j = 1;
	printf("***** Object List *****\n");
	for (j = 0; j < 30; j++) {
		if (firstValue[j] == 0) break; //0으로 초기화 되어있고 0은 토큰의 특성상 0을가진 토큰은 없어야됨
		printf("[Name %d] %.*s\n", j + 1, t[firstValue[j] + 1].end - t[firstValue[j] + 1].start, jsonstr + t[firstValue[j] + 1].start);
	}
	printf("\n");
	return j;
}

void printSelectedObject(char* jsonstr, jsmntok_t *t, int* nameTokIndex, int *firstValue, int count) {
	int input, j;
	char* str = (char*)malloc(sizeof(char)*(strlen(jsonstr)/count));
	int r;
	jsmn_parser p;
	jsmntok_t tok[128];
	int name[100] = {0};
	int first[30] = {0};
	while(1){
		printf("원하는 번호 입력 (Exit: 0)");
		scanf("%d", &input);
		if(input == 0) break;
		input--;
		sprintf(str,"%.*s",t[firstValue[input] - 1].end - t[firstValue[input] - 1].start, jsonstr + t[firstValue[input] - 1].start);
		jsmn_init(&p);
		r = jsmn_parse(&p, str, strlen(str), tok, sizeof(tok)/sizeof(tok[0]));
		if (r < 0) {
			printf("Failed to parse JSON: %d\n", r);
			return 1;
		}
		jsonNameList(str, tok, r, name);
		printObjectList(str, tok, name);

		printf("\n");
	}
	free(str);
}

void printFirst(char* jsonstr, jsmntok_t *t, int* nameTokIndex){
	int j = 1;
	printf("***** Name List *****\n");
	for (j = 0; j < 100; j++) {
		if (nameTokIndex[j] == 0) break; //0으로 초기화 되어있고 0은 토큰의 특성상 0을가진 토큰은 없어야됨
		printf("[Name %d] %.*s\n", j + 1, t[nameTokIndex[j]].end - t[nameTokIndex[j]].start, jsonstr + t[nameTokIndex[j]].start);
	}
	printf("\n");
}

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int main() {
	int i;
	int r;
	jsmn_parser p;
	int nameTokIndex[100] = { -1 };
	for (i = 0; i < 100; i++) nameTokIndex[i] = -1;
	int firstValue[30] = {0};
	jsmntok_t t[128]; /* We expect no more than 128 tokens */
	char* JSON_STRING = readJSONfile();
	//printf("%s\n", JSON_STRING);
#ifdef JSMN_PARENT_LINKS
	printf("Parent\n");
#endif
	

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}
	
	jsonNameList(JSON_STRING, t, r, nameTokIndex);
	printNameList(JSON_STRING, t, nameTokIndex);
	//int c = printFirstValue(JSON_STRING, t, r, firstValue);
	//printSelectedObject(JSON_STRING, t, nameTokIndex, firstValue, c);
	//printObjectList(JSON_STRING, t, nameTokIndex);
	//selectNameList(JSON_STRING, t, nameTokIndex);
	/* Assume the top-level element is an object */
	//printFirst(JSON_STRING,t,nameTokIndex);

	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}
#ifdef nothing
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
