#include <stdio.h>
#include <glib.h>
#include <bone/runtime/interpreter.h>
#include <bone/config.h>
#include "ffi.h"

void json_Init(bnInterpreter *bone)
{
    printf("json Init");
    ffi_json_init(bone);
}

void json_Destroy(bnInterpreter *bone)
{
    printf("json Destroy");
}

const char *json_GetTargetVersion()
{
    return bnGetBuildVersion();
}
