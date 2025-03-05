#ifndef __APP_DATA_H__
#define __APP_DATA_H__


void app_data_init(void);

void app_data_set(char* key, void* value);

void* app_data_get(char* key);

void app_data_update(void);


#endif