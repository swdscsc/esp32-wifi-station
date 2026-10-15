
#include <stdio.h>
#include "cjson/cJSON.h"

// 示例 05：用 cJSON 构造 / 解析 JSON（无需联网）
void app_main(void)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "esp32");
    cJSON_AddNumberToObject(root, "temp", 26.5);

    char *s = cJSON_Print(root);
    printf("json: %s\n", s);

    cJSON *p = cJSON_Parse(s);
    cJSON *t = cJSON_GetObjectItem(p, "temp");
    printf("temp = %.1f\n", t->valuedouble);

    free(s);
    cJSON_Delete(p);
    cJSON_Delete(root);
}
