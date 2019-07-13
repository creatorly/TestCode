#include "ztest.h"

static struct ubus_context *g_ubus_ctx;
struct blob_buf g_bbuf;

static void ztest_ubus_reconnect_timer(struct uloop_timeout *timeout)
{
    static struct uloop_timeout retry = {
        .cb = ztest_ubus_reconnect_timer,
    };
    int t = 2;

    if (ubus_reconnect(g_ubus_ctx, NULL) != 0) {
        ULOG_ERR("failed to reconnect, trying again in %d seconds\n", t);
        uloop_timeout_set(&retry, t * 1000);
        return;
    }

    ULOG_INFO("reconnected to ubus, new id: %08x\n", g_ubus_ctx->local_id);
    ubus_add_uloop(g_ubus_ctx);
}

static void ztest_ubus_connection_lost(struct ubus_context *ctx)
{
    ztest_ubus_reconnect_timer(NULL);
}

enum {
    ZTEST_STATUS,
    ZTEST_PORT,
    ZTEST_MAX
};

static const struct blobmsg_policy zboard_policy[ZTEST_MAX] = {
    [ZTEST_STATUS] = { .name = "status", .type = BLOBMSG_TYPE_INT32 },
    [ZTEST_PORT] = { .name = "port", .type = BLOBMSG_TYPE_INT32 },
};

static uint32_t g_wan_status;
static uint32_t g_wan_port;

static int ztest_set_test(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
    struct blob_attr *tb[ZTEST_MAX];

    ULOG_INFO("ztest_set_test\n");
    blobmsg_parse(zboard_policy, ZTEST_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[ZTEST_STATUS] || !tb[ZTEST_PORT])
        return UBUS_STATUS_INVALID_ARGUMENT;

    g_wan_status = blobmsg_get_u32(tb[ZTEST_STATUS]);
    g_wan_port = blobmsg_get_u32(tb[ZTEST_PORT]);

    return 0;
}

static int ztest_get_test(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
    ULOG_INFO("ztest_get_test\n");
    blob_buf_init(&g_bbuf, 0);
    blobmsg_add_u32(&g_bbuf, "status", g_wan_status);
    blobmsg_add_u32(&g_bbuf, "port", g_wan_port);
    ubus_send_reply(ctx, req, g_bbuf.head);

    return 0;
}

static const struct ubus_method ztest_methods[] = {
    { .name = "set_test", .handler = ztest_set_test },
    { .name = "get_test", .handler = ztest_get_test },
};

static struct ubus_object_type ztest_object_type =
    UBUS_OBJECT_TYPE("ztest", ztest_methods);

static struct ubus_object ztest_object = {
    .name = "ztest",
    .type = &ztest_object_type,
    .methods = ztest_methods,
    .n_methods = ARRAY_SIZE(ztest_methods),
};

static void receive_event(struct ubus_context *ctx, struct ubus_event_handler *ev,
                          const char *type, struct blob_attr *msg)
{
    struct blob_attr *tb[ZTEST_MAX];

    ULOG_INFO("receive_event\n");
    blobmsg_parse(zboard_policy, ZTEST_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[ZTEST_STATUS] || !tb[ZTEST_PORT])
        return;

    g_wan_status = blobmsg_get_u32(tb[ZTEST_STATUS]);
    g_wan_port = blobmsg_get_u32(tb[ZTEST_PORT]);
}

int ztest_ubus_event_init(void)
{
    int ret;
    static struct ubus_event_handler listener;
    
    memset(&listener, 0, sizeof(listener));
    listener.cb = receive_event;
    ret = ubus_register_event_handler(g_ubus_ctx, &listener, "test_event");
    if (ret){
        ULOG_ERR("ubus_register_event_handler error %s", ubus_strerror(ret));
    }
    return ret;
}

int ztest_ubus_init(void)
{
    int ret;

    g_ubus_ctx = ubus_connect(NULL);
    if (!g_ubus_ctx) {
        ULOG_ERR("Failed to connect to ubus\n");
        return -1;
    }

    g_ubus_ctx->connection_lost = ztest_ubus_connection_lost;
    ubus_add_uloop(g_ubus_ctx);

    ret = ubus_add_object(g_ubus_ctx, &ztest_object);
    if (ret)
        ULOG_ERR("Failed to add zboard object: %s\n", ubus_strerror(ret));

    return ret;
}

void ztest_ubus_clean(void)
{
    if (g_ubus_ctx) {
        ubus_remove_object(g_ubus_ctx, &ztest_object);
        ubus_free(g_ubus_ctx);
    }
}

