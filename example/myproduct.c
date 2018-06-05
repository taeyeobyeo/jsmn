#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../jsmn.h"

typedef enum {
	cocacola,
	lotte,
	Tamsaa,
	DongaOtsuka
}company_t;

typedef struct {
	company_t company;
	char name[20];
	int price;
	bool sparkling;
}product_t;

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
	file = fopen("data3.json", "r");
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

	

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}
	
	jsonNameList(JSON_STRING, t, r, nameTokIndex);
	//printNameList(JSON_STRING, t, nameTokIndex);
	//selectNameList(JSON_STRING, t, nameTokIndex);
	int c = printFirstValue(JSON_STRING, t, r, firstValue);
	printSelectedObject(JSON_STRING, t, nameTokIndex, firstValue, c);
	//printObjectList(JSON_STRING, t, nameTokIndex);
	/* Assume the top-level element is an object */
	//printFirst(JSON_STRING,t,nameTokIndex);
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
