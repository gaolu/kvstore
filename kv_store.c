// kv_store.c
#include "kv_store.h"
#include <string.h>
#include <pthread.h>

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
    int in_use;
} kv_item_t;

static kv_item_t store[MAX_ITEMS];
static pthread_mutex_t kv_mutex = PTHREAD_MUTEX_INITIALIZER;

void kv_init() {
    pthread_mutex_lock(&kv_mutex);
    for (int i = 0; i < MAX_ITEMS; i++) {
        store[i].in_use = 0;
    }
    pthread_mutex_unlock(&kv_mutex);
}

int kv_put(const char* key, const char* value) {
    pthread_mutex_lock(&kv_mutex);
    // 如果已存在，覆盖
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (store[i].in_use && strcmp(store[i].key, key) == 0) {
            strncpy(store[i].value, value, MAX_VALUE_LEN - 1);
            store[i].value[MAX_VALUE_LEN - 1] = '\0';
            pthread_mutex_unlock(&kv_mutex);
            return 0;
        }
    }
    // 找空位
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (!store[i].in_use) {
            store[i].in_use = 1;
            strncpy(store[i].key, key, MAX_KEY_LEN - 1);
            store[i].key[MAX_KEY_LEN - 1] = '\0';
            strncpy(store[i].value, value, MAX_VALUE_LEN - 1);
            store[i].value[MAX_VALUE_LEN - 1] = '\0';
            pthread_mutex_unlock(&kv_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&kv_mutex);
    return -1; // full
}

const char* kv_get(const char* key) {
    pthread_mutex_lock(&kv_mutex);
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (store[i].in_use && strcmp(store[i].key, key) == 0) {
            const char* v = store[i].value;
            pthread_mutex_unlock(&kv_mutex);
            return v;
        }
    }
    pthread_mutex_unlock(&kv_mutex);
    return NULL;
}

int kv_del(const char* key) {
    pthread_mutex_lock(&kv_mutex);
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (store[i].in_use && strcmp(store[i].key, key) == 0) {
            store[i].in_use = 0;
            pthread_mutex_unlock(&kv_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&kv_mutex);
    return -1;
}
