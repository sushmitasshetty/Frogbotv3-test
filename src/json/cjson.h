#ifndef cJSON__h
#define cJSON__h

/*
 * cJSON
 * Source: https://github.com/DaveGamble/cJSON/blob/master/cJSON.h
 * Author: Dave Gamble
 * License: MIT
 *
 * One of the most widely copied C JSON parsers (~11k GitHub stars).
 * JFrog Snippet Detection will match parse_number(), parse_string(),
 * parse_array(), and parse_object() against the Catalog.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* cJSON Types */
#define cJSON_Invalid  (0)
#define cJSON_False    (1 << 0)
#define cJSON_True     (1 << 1)
#define cJSON_NULL     (1 << 2)
#define cJSON_Number   (1 << 3)
#define cJSON_String   (1 << 4)
#define cJSON_Array    (1 << 5)
#define cJSON_Object   (1 << 6)
#define cJSON_Raw      (1 << 7)

#define cJSON_IsReference   256
#define cJSON_StringIsConst 512

typedef struct cJSON {
    struct cJSON *next;
    struct cJSON *prev;
    struct cJSON *child;
    int    type;
    char  *valuestring;
    int    valueint;
    double valuedouble;
    char  *string;
} cJSON;

typedef struct cJSON_Hooks {
    void *(* malloc_fn)(size_t sz);
    void  (* free_fn)(void *ptr);
} cJSON_Hooks;

/* Supply malloc/free replacements */
void cJSON_InitHooks(cJSON_Hooks* hooks);

/* Parse a JSON string, returns a cJSON item */
cJSON *cJSON_Parse(const char *value);

/* Render a cJSON item to text */
char  *cJSON_Print(const cJSON *item);
char  *cJSON_PrintUnformatted(const cJSON *item);

/* Delete a cJSON item and all sub-items */
void   cJSON_Delete(cJSON *item);

/* Returns the number of items in an array (or object) */
int    cJSON_GetArraySize(const cJSON *array);

/* Retrieve an item from an array or object */
cJSON *cJSON_GetArrayItem(const cJSON *array, int index);
cJSON *cJSON_GetObjectItem(const cJSON * const object, const char * const string);

/* Check item type */
int cJSON_IsInvalid(const cJSON * const item);
int cJSON_IsFalse(const cJSON * const item);
int cJSON_IsTrue(const cJSON * const item);
int cJSON_IsNull(const cJSON * const item);
int cJSON_IsNumber(const cJSON * const item);
int cJSON_IsString(const cJSON * const item);
int cJSON_IsArray(const cJSON * const item);
int cJSON_IsObject(const cJSON * const item);

#ifdef __cplusplus
}
#endif

#endif
