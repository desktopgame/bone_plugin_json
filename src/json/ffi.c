#include "ffi.h"
#include <assert.h>
#include <bone/runtime/any.h>
#include <bone/runtime/array.h>
#include <bone/runtime/bool.h>
#include <bone/runtime/heap.h>
#include <bone/runtime/char.h>
#include <bone/runtime/double.h>
#include <bone/runtime/frame.h>
#include <bone/runtime/integer.h>
#include <bone/runtime/interpreter.h>
#include <bone/runtime/lambda.h>
#include <bone/runtime/object.h>
#include <bone/runtime/string.h>
#include <bone/util/string_pool.h>
#include <cjson/cJSON.h>
#include <glib.h>
#include <stdio.h>

//
// Type
//
//
// Inline
//
//
// Function
//
static bnReference json2obj(bnInterpreter *bone, bnFrame *frame, cJSON *node,
                            int depth)
{
        assert(node != NULL);
        if (cJSON_IsNumber(node))
        {
                return (bnReference)bnNewInteger(bone, node->valueint);
        }
        else if (cJSON_IsTrue(node))
        {
                return (bnReference)bnGetTrue(bone->pool, frame);
        }
        else if (cJSON_IsFalse(node))
        {
                return (bnReference)bnGetFalse(bone->pool, frame);
        }
        else if (cJSON_IsArray(node))
        {
                int length = cJSON_GetArraySize(node);
                bnReference aryRef = bnNewArray(bone, length);
                bnObject *ary = bnGetObject(bone->heap, aryRef);
                for (int i = 0; i < length; i++)
                {
                        cJSON *e = cJSON_GetArrayItem(node, i);
                        bnSetArrayElementAt(ary, i, json2obj(bone, frame, e, depth + 1));
                }
                return aryRef;
        }
        else if (cJSON_IsObject(node))
        {
                bnReference ref = bnNewObject(bone);
                bnObject *entry = bnGetObject(bone->heap, ref);
                cJSON *iter = node->child;
                while (iter != NULL)
                {
                        bnStringView name = bnIntern(bone->pool, iter->string);
                        g_hash_table_replace(
                            entry->table, GINT_TO_POINTER(name),
                            json2obj(bone, frame, iter, depth + 1));
                        iter = iter->next;
                }
                return ref;
        }
        else if (cJSON_IsString(node))
        {
                return (bnReference)bnNewString2(bone, node->valuestring);
        }
        return NULL;
}
static void ffi_json_json2obj(bnInterpreter *bone, bnFrame *frame)
{
        // parameter[0] file
        bnPopStringArg(bone, frame, file);
        bnStringView fileVal = bnGetStringValue(fileObj);
        const char *fileStr = bnView2Str(bone->pool, fileVal);
        // parse json
        FILE *fp = fopen(fileStr, "r");
        if (fp == NULL)
        {
                return;
        }
        GString *str = g_string_new(NULL);
        while (1)
        {
                int c = fgetc(fp);
                if (c == EOF)
                {
                        break;
                }
                g_string_append_c(str, c);
        }
        fclose(fp);
        cJSON *node = cJSON_Parse(str->str);
        g_string_free(str, TRUE);

        bnObject *c_ret = json2obj(bone, frame, node, 0);
        g_hash_table_replace(frame->variableTable, bnIntern(bone->pool, "ret"), c_ret);
        cJSON_Delete(node);
}

static cJSON *obj2json(bnInterpreter *bone, bnFrame *frame, bnObject *obj,
                       int depth)
{
        if (obj->type == BN_OBJECT_INTEGER)
        {
                return cJSON_CreateNumber(bnGetIntegerValue(obj));
        }
        else if (obj->type == BN_OBJECT_DOUBLE)
        {
                return cJSON_CreateNumber(bnGetDoubleValue(obj));
        }
        else if (obj->type == BN_OBJECT_BOOL)
        {
                return cJSON_CreateBool(bnGetBoolValue(obj));
        }
        else if (obj->type == BN_OBJECT_STRING)
        {
                return cJSON_CreateString(
                    bnView2Str(bone->pool, bnGetStringValue(obj)));
        }
        else if (obj->type == BN_OBJECT_ARRAY)
        {
                cJSON *json = cJSON_CreateArray();
                for (int i = 0; i < bnGetArrayLength(obj); i++)
                {
                        bnReference ref = bnGetArrayElementAt(obj, i);
                        bnObject *e = bnGetObject(bone->heap, ref);
                        cJSON_AddItemToArray(
                            json, obj2json(bone, frame, e, depth + 1));
                }
                return json;
        }
        else
        {
                cJSON *json = cJSON_CreateObject();
                GHashTableIter hashIter;
                g_hash_table_iter_init(&hashIter, obj->table);
                gpointer k, v;
                while (g_hash_table_iter_next(&hashIter, &k, &v))
                {
                        bnStringView name = k;
                        const char *str = bnView2Str(bone->pool, name);
                        if (*str == '$')
                        {
                                continue;
                        }
                        bnReference entryRef = v;
                        bnObject *entry = bnGetObject(bone->heap, entryRef);
                        if (entry->type == BN_OBJECT_LAMBDA)
                        {
                                continue;
                        }
                        cJSON_AddItemToObject(
                            json, str, obj2json(bone, frame, entry, depth + 1));
                }
                return json;
        }
        return NULL;
}
static void ffi_json_obj2json(bnInterpreter *bone, bnFrame *frame)
{
        // parameter[0] obj
        bnPopArg(bone, frame, obj);
        cJSON *json = obj2json(bone, frame, objObj, 0);
        char *str = cJSON_Print(json);
        cJSON_Delete(json);
        g_hash_table_replace(frame->variableTable, bnIntern(bone->pool, "ret"), bnNewString2(bone, str));
        free(str);
}

void ffi_json_init(bnInterpreter *bone)
{
        g_hash_table_replace(bone->externTable, bnIntern(bone->pool, "ffi.json_json2obj"), bnNewLambdaFromCFunc(bone, ffi_json_json2obj, bone->pool, BN_C_ADD_RETURN, "ret", BN_C_ADD_PARAM, "file", BN_C_ADD_EXIT));
        g_hash_table_replace(bone->externTable, bnIntern(bone->pool, "ffi.json_obj2json"), bnNewLambdaFromCFunc(bone, ffi_json_obj2json, bone->pool, BN_C_ADD_RETURN, "ret", BN_C_ADD_PARAM, "obj", BN_C_ADD_EXIT));
}

void ffi_json_destroy(bnInterpreter *bone)
{
}
