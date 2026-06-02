/*
 * cJSON - Ultralightweight JSON parser in ANSI C
 * Source: https://github.com/DaveGamble/cJSON/blob/master/cJSON.c
 * Version: 1.7.x
 * Author: Dave Gamble
 * License: MIT
 *
 * Verbatim copy of the core parsing functions. The parse_number(),
 * parse_string(), parse_array(), and parse_object() functions are
 * among the most-recognisable patterns in JFrog's Catalog — they have
 * a distinctive logical structure that survives reformatting.
 */

#include "cjson.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

static void *(*cJSON_malloc)(size_t sz) = malloc;
static void  (*cJSON_free)(void *ptr)   = free;

static char *cJSON_strdup(const char *str) {
    size_t len;
    char *copy;
    len = strlen(str) + 1;
    if (!(copy = (char*)cJSON_malloc(len))) return NULL;
    memcpy(copy, str, len);
    return copy;
}

void cJSON_InitHooks(cJSON_Hooks *hooks) {
    if (!hooks) {
        cJSON_malloc = malloc;
        cJSON_free   = free;
        return;
    }
    cJSON_malloc = (hooks->malloc_fn) ? hooks->malloc_fn : malloc;
    cJSON_free   = (hooks->free_fn)   ? hooks->free_fn   : free;
}

static cJSON *cJSON_New_Item(void) {
    cJSON *node = (cJSON*)cJSON_malloc(sizeof(cJSON));
    if (node) memset(node, 0, sizeof(cJSON));
    return node;
}

void cJSON_Delete(cJSON *c) {
    cJSON *next;
    while (c) {
        next = c->next;
        if (!(c->type & cJSON_IsReference) && c->child)
            cJSON_Delete(c->child);
        if (!(c->type & cJSON_IsReference) && c->valuestring)
            cJSON_free(c->valuestring);
        if (!(c->type & cJSON_StringIsConst) && c->string)
            cJSON_free(c->string);
        cJSON_free(c);
        c = next;
    }
}

/* Parse the input text to generate a number, and populate the result
   into item. */
static const char *parse_number(cJSON *item, const char *num) {
    double n = 0, sign = 1, scale = 0;
    int subscale = 0, signsubscale = 1;

    if (*num == '-') sign = -1, num++;       /* Has sign? */
    if (*num == '0') num++;                  /* is zero */
    if (*num >= '1' && *num <= '9')
        do n = (n * 10.0) + (*num++ - '0');
        while (*num >= '0' && *num <= '9');  /* Number? */
    if (*num == '.' && num[1] >= '0' && num[1] <= '9') {
        num++;
        do n = (n * 10.0) + (*num++ - '0'), scale--;
        while (*num >= '0' && *num <= '9');  /* Fractional part? */
    }
    if (*num == 'e' || *num == 'E') {        /* Exponent? */
        num++;
        if (*num == '+') num++;
        else if (*num == '-') signsubscale = -1, num++;
        while (*num >= '0' && *num <= '9')
            subscale = (subscale * 10) + (*num++ - '0'); /* Number? */
    }

    /* number = +/- number.fraction * 10^+/- exponent */
    n = sign * n * pow(10.0, (scale + subscale * signsubscale));

    item->valuedouble = n;
    item->valueint    = (int)n;
    item->type        = cJSON_Number;
    return num;
}

static unsigned parse_hex4(const char *str) {
    unsigned h = 0;
    if      (*str >= '0' && *str <= '9') h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
    else return 0;
    h = h << 4; str++;
    if      (*str >= '0' && *str <= '9') h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
    else return 0;
    h = h << 4; str++;
    if      (*str >= '0' && *str <= '9') h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
    else return 0;
    h = h << 4; str++;
    if      (*str >= '0' && *str <= '9') h += (*str) - '0';
    else if (*str >= 'A' && *str <= 'F') h += 10 + (*str) - 'A';
    else if (*str >= 'a' && *str <= 'f') h += 10 + (*str) - 'a';
    else return 0;
    return h;
}

/* Parse the input text into an unescaped cstring, and populate item */
static const unsigned char firstByteMark[7] =
    { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static const char *parse_string(cJSON *item, const char *str) {
    const char *ptr = str + 1; char *ptr2; char *out;
    int len = 0; unsigned uc, uc2;
    if (*str != '\"') return NULL; /* not a string! */

    while (*ptr != '\"' && *ptr && ++len)
        if (*ptr++ == '\\') ptr++;  /* Skip escaped quotes. */

    out = (char*)cJSON_malloc(len + 1); /* Allocate space for the unescaped string */
    if (!out) return NULL;

    ptr = str + 1; ptr2 = out;
    while (*ptr != '\"' && *ptr) {
        if (*ptr != '\\') *ptr2++ = *ptr++;
        else {
            ptr++;
            switch (*ptr) {
                case 'b': *ptr2++ = '\b'; break;
                case 'f': *ptr2++ = '\f'; break;
                case 'n': *ptr2++ = '\n'; break;
                case 'r': *ptr2++ = '\r'; break;
                case 't': *ptr2++ = '\t'; break;
                case 'u': /* Transcode UTF16 to UTF8. */
                    uc = parse_hex4(ptr + 1); ptr += 4;
                    if ((uc >= 0xDC00 && uc <= 0xDFFF) || uc == 0) break;
                    if (uc >= 0xD800 && uc <= 0xDBFF) {
                        if (ptr[1] != '\\' || ptr[2] != 'u') break;
                        uc2 = parse_hex4(ptr + 3); ptr += 6;
                        if (uc2 < 0xDC00 || uc2 > 0xDFFF) break;
                        uc = 0x10000 + (((uc & 0x3FF) << 10) | (uc2 & 0x3FF));
                    }
                    len = 4;
                    if      (uc  < 0x80)   len = 1;
                    else if (uc  < 0x800)  len = 2;
                    else if (uc  < 0x10000) len = 3;
                    ptr2 += len;
                    switch (len) {
                        case 4: *--ptr2 = ((uc | 0x80) & 0xBF); uc >>= 6;
                        case 3: *--ptr2 = ((uc | 0x80) & 0xBF); uc >>= 6;
                        case 2: *--ptr2 = ((uc | 0x80) & 0xBF); uc >>= 6;
                        case 1: *--ptr2 =  (uc | firstByteMark[len]);
                    }
                    ptr2 += len;
                    break;
                default: *ptr2++ = *ptr; break;
            }
            ptr++;
        }
    }
    *ptr2 = 0;
    if (*ptr == '\"') ptr++;
    item->valuestring = out;
    item->type        = cJSON_String;
    return ptr;
}

static const char *skip(const char *in) {
    while (in && *in && (unsigned char)*in <= 32) in++;
    return in;
}

static const char *parse_value(cJSON *item, const char *value);
static const char *parse_array(cJSON *item, const char *value);
static const char *parse_object(cJSON *item, const char *value);

/* Build an array from input text */
static const char *parse_array(cJSON *item, const char *value) {
    cJSON *child;
    if (*value != '[') return NULL; /* not an array! */

    item->type  = cJSON_Array;
    value = skip(value + 1);
    if (*value == ']') return value + 1; /* empty array */

    item->child = child = cJSON_New_Item();
    if (!item->child) return NULL;
    value = skip(parse_value(child, skip(value)));
    if (!value) return NULL;

    while (*value == ',') {
        cJSON *new_item;
        if (!(new_item = cJSON_New_Item())) return NULL;
        child->next     = new_item;
        new_item->prev  = child;
        child           = new_item;
        value = skip(parse_value(child, skip(value + 1)));
        if (!value) return NULL;
    }

    if (*value == ']') return value + 1;
    return NULL;
}

/* Build an object from the input text */
static const char *parse_object(cJSON *item, const char *value) {
    cJSON *child;
    if (*value != '{') return NULL; /* not an object! */

    item->type  = cJSON_Object;
    value = skip(value + 1);
    if (*value == '}') return value + 1; /* empty object */

    item->child = child = cJSON_New_Item();
    if (!item->child) return NULL;
    value = skip(parse_string(child, skip(value)));
    if (!value) return NULL;
    child->string = child->valuestring;
    child->valuestring = NULL;
    if (*value != ':') return NULL;
    value = skip(parse_value(child, skip(value + 1)));
    if (!value) return NULL;

    while (*value == ',') {
        cJSON *new_item;
        if (!(new_item = cJSON_New_Item())) return NULL;
        child->next     = new_item;
        new_item->prev  = child;
        child           = new_item;
        value = skip(parse_string(child, skip(value + 1)));
        if (!value) return NULL;
        child->string = child->valuestring;
        child->valuestring = NULL;
        if (*value != ':') return NULL;
        value = skip(parse_value(child, skip(value + 1)));
        if (!value) return NULL;
    }

    if (*value == '}') return value + 1;
    return NULL;
}

/* Parser core - when encountering text, process appropriately */
static const char *parse_value(cJSON *item, const char *value) {
    if (!value) return NULL;
    if (!strncmp(value, "null",  4)) { item->type = cJSON_NULL;  return value + 4; }
    if (!strncmp(value, "false", 5)) { item->type = cJSON_False; return value + 5; }
    if (!strncmp(value, "true",  4)) { item->type = cJSON_True; item->valueint = 1; return value + 4; }
    if (*value == '\"') return parse_string(item, value);
    if (*value == '-' || (*value >= '0' && *value <= '9')) return parse_number(item, value);
    if (*value == '[') return parse_array(item, value);
    if (*value == '{') return parse_object(item, value);
    return NULL; /* failure */
}

cJSON *cJSON_Parse(const char *value) {
    cJSON *c = cJSON_New_Item();
    if (!c) return NULL;
    if (!parse_value(c, skip(value))) {
        cJSON_Delete(c);
        return NULL;
    }
    return c;
}

int cJSON_GetArraySize(const cJSON *array) {
    cJSON *c = array->child;
    int i = 0;
    while (c) i++, c = c->next;
    return i;
}

cJSON *cJSON_GetArrayItem(const cJSON *array, int item) {
    cJSON *c = array ? array->child : NULL;
    while (c && item > 0) item--, c = c->next;
    return c;
}

cJSON *cJSON_GetObjectItem(const cJSON * const object, const char * const string) {
    cJSON *c = object ? object->child : NULL;
    while (c && strcasecmp(c->string, string)) c = c->next;
    return c;
}

int cJSON_IsInvalid(const cJSON * const item) { return item ? (item->type & 0xFF) == cJSON_Invalid : 0; }
int cJSON_IsFalse  (const cJSON * const item) { return item ? (item->type & 0xFF) == cJSON_False   : 0; }
int cJSON_IsTrue   (const cJSON * const item) { return item ? (item->type & 0xFF) == cJSON_True    : 0; }
int cJSON_IsNull   (const cJSON * const item) { return item ? (item->type & 0xFF) == cJSON_NULL    : 0; }
int cJSON_IsNumber (const cJSON * const item) { return item ? (item->type & 0xFF) == cJSON_Number  : 0; }
int cJSON_IsString (const cJSON * const item) { return item ? (item->type & 0xFF) == cJSON_String  : 0; }
int cJSON_IsArray  (const cJSON * const item) { return item ? (item->type & 0xFF) == cJSON_Array   : 0; }
int cJSON_IsObject (const cJSON * const item) { return item ? (item->type & 0xFF) == cJSON_Object  : 0; }
