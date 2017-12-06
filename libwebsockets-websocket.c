#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h>

static int callback_dumb_increment(struct lws *wsi,
                                   enum lws_callback_reasons reason,
                                   void *user, void *in, size_t len)
{
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED:
            printf("connection established\n");
            break;
        case LWS_CALLBACK_RECEIVE: {
            unsigned char *buf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + len +
                                                         LWS_SEND_BUFFER_POST_PADDING);

            int i;

            for (i=0; i < len; i++) {
                buf[LWS_SEND_BUFFER_PRE_PADDING + (len - 1) - i ] = ((char *) in)[i];
            }

            printf("received data: %s, replying: %.*s\n", (char *) in, (int) len,
                   buf + LWS_SEND_BUFFER_PRE_PADDING);

            lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);

            free(buf);
            break;
        }
        default:
            break;
    }

    return 0;
}

int main(void) {
    int port = 9000;
    struct lws_context *context;

    struct lws_protocols protocol;
    memset(&protocol, 0, sizeof protocol);
    protocol.name = "dumb-increment-protocol";
    protocol.callback = (lws_callback_function*)callback_dumb_increment;
    protocol.rx_buffer_size = 0;

    struct lws_context_creation_info context_data;
    memset(&context_data, 0, sizeof context_data);
    context_data.port = port;
    context_data.protocols = &protocol;

    context = lws_create_context(&context_data);

    if (context == NULL) {
        fprintf(stderr, "lws init failed\n");
        return -1;
    }

    printf("starting server...\n");

    while (1) {
        lws_service(context, 50);
    }

    lws_context_destroy(context);

    return 0;
}
