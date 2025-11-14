// kv_store.h
#ifndef KV_STORE_H
#define KV_STORE_H

#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 256
#define MAX_ITEMS 1024

void kv_init();
int kv_put(const char* key, const char* value);  // return 0 on success
const char* kv_get(const char* key);             // return NULL if not found
int kv_del(const char* key);                     // return 0 on success, -1 if not found

#endif
