#ifndef TNT_KAFKA_COMMON_H
#define TNT_KAFKA_COMMON_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <librdkafka/rdkafka.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>

#include <tarantool/module.h>

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

/**
 * You may use likely()/unlikely() to provide the compiler with branch
 * prediction information.
 *
 * @sa https://en.cppreference.com/w/cpp/language/attributes/likely
 */
#if __has_builtin(__builtin_expect) || defined(__GNUC__)
# define likely(x)    __builtin_expect(!! (x),1)
# define unlikely(x)  __builtin_expect(!! (x),0)
#else
# define likely(x)    (x)
# define unlikely(x)  (x)
#endif

/**
 * An x* variant of a memory allocation function calls the original function
 * and panics if it fails (i.e. it should never return NULL).
 */
#define xalloc_impl(size, func, args...)                                        \
    ({                                                                          \
        void *ret = func(args);                                                 \
        if (unlikely(ret == NULL)) {                                            \
            fprintf(stderr, "Can't allocate %zu bytes at %s:%d",                \
                    (size_t)(size), __FILE__, __LINE__);                        \
            exit(EXIT_FAILURE);                                                 \
        }                                                                       \
        ret;                                                                    \
    })

#define xmalloc(size)           xalloc_impl((size), malloc, (size))
#define xcalloc(n, size)        xalloc_impl((n) * (size), calloc, (n), (size))
#define xrealloc(ptr, size)     xalloc_impl((size), realloc, (ptr), (size))

extern const char* const consumer_label;
extern const char* const consumer_msg_label;
extern const char* const producer_label;

int
lua_librdkafka_version(struct lua_State *L);

int
lua_librdkafka_dump_conf(struct lua_State *L, rd_kafka_t *rk);

int
lua_librdkafka_metadata(struct lua_State *L, rd_kafka_t *rk, rd_kafka_topic_t *only_rkt, int timeout_ms);

int
lua_librdkafka_list_groups(struct lua_State *L, rd_kafka_t *rk, const char *group, int timeout_ms);

/**
 * Push native lua error with code -3
 */
int
lua_push_error(struct lua_State *L);

void
set_thread_name(const char *name);

rd_kafka_resp_err_t
kafka_pause(rd_kafka_t *rk);

rd_kafka_resp_err_t
kafka_resume(rd_kafka_t *rk);

#endif //TNT_KAFKA_COMMON_H
