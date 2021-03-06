#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */


static const char *JSON_STRING =
	"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
	"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

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
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));

#ifdef DEBUG_MODE
	printf("Token count: %d\n", r);
#endif

	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "user") == 0) {

#ifdef DEBUG_MODE
			printf("%d %d\n", t[i+1].start, t[i+1].end);
			printf("[%d] ", i+1);
#endif

			/* We may use strndup() to fetch string value */
#ifdef JSMN_PARENT_LINKS
			printf("- User: %.*s parent:%d\n", t[i+1].end-t[i+1].start, JSON_STRING + t[i+1].start, t[i+1].parent);
#else
			printf("- User: %.*s\n", t[i+1].end-t[i+1].start, JSON_STRING + t[i+1].start);
#endif
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {

#ifdef DEBUG_MODE
			printf("%d %d\n", t[i+1].start, t[i+1].end);
			printf("[%d] ", i+1);
#endif

			/* We may additionally check if the value is either "true" or "false" */
#ifdef JSMN_PARENT_LINKS
			printf("- Admin: %.*s parent:%d\n", t[i+1].end-t[i+1].start, JSON_STRING + t[i+1].start,t[i+1].parent);
#else
			printf("- Admin: %.*s\n", t[i+1].end-t[i+1].start, JSON_STRING + t[i+1].start);
#endif
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {

#ifdef DEBUG_MODE
			printf("%d %d\n", t[i+1].start, t[i+1].end);
			printf("[%d] ", i+1);
#endif

			/* We may want to do strtol() here to get numeric value */
#ifdef JSMN_PARENT_LINKS
			printf("- UID: %.*s parent:%d\n", t[i+1].end-t[i+1].start, JSON_STRING + t[i+1].start,t[i+1].parent);
#else
			printf("- UID: %.*s\n", t[i+1].end-t[i+1].start, JSON_STRING + t[i+1].start);
#endif
			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) {

#ifdef DEBUG_MODE
			printf("%d %d\n", t[i+1].start, t[i+1].end);
			printf("[%d] ", i+1);
#endif
			int j;
#ifdef JSMN_PARENT_LINKS
			printf("- Groups: parent:%d\n",t[i+1].parent);
#else
			printf("- Groups:\n");
#endif
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
#ifdef DEBUG_MODE
			printf("%d %d\n", g->start, g->end);
			printf("[%d] ", i+j+2);
#endif
#ifdef JSMN_PARENT_LINKS
			printf("  * %.*s	parent:%d\n", g->end - g->start, JSON_STRING + g->start,g->parent);
#else
			printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
#endif
			}
			i += t[i+1].size + 1;
		} else {
			printf("Unexpected key: %.*s\n", t[i].end-t[i].start,
					JSON_STRING + t[i].start);
		}
	}
	return EXIT_SUCCESS;
}
