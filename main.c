#include <stdio.h>
#include <libserialport.h>
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "erpc.h"

struct sp_port *port = NULL;

bool erpc_send_request(erpc_packet_t *request) {
    enum sp_return ret;
    ret = sp_blocking_write(port, request->data, request->length, 3000);
    if (ret < 0) {
        printf("sp_blocking_write error: %d\n", ret);
        return false;
    }
    return true;
}

int main() {
    enum sp_return ret;

    const char *port_name = "COM11";

    ret = sp_get_port_by_name(port_name, &port);
    if (ret != SP_OK) {
        printf("sp_get_port_by_name error: %d\n", ret);
        return ret;
    }
    printf("port found: %s, description: %s\n", port_name, sp_get_port_description(port));

    ret = sp_open(port, SP_MODE_READ_WRITE);
    if (ret != SP_OK) {
        printf("sp_open error: %d\n", ret);
        return ret;
    }

    ret = sp_set_baudrate(port, 921600);
    if (ret != SP_OK) {
        printf("sp_set_baudrate error: %d\n", ret);
        return ret;
    }

    ret = sp_set_bits(port, 8);
    if (ret != SP_OK) {
        printf("sp_set_bits error: %d\n", ret);
        return ret;
    }

    ret = sp_set_parity(port, SP_PARITY_NONE);
    if (ret != SP_OK) {
        printf("sp_set_parity error: %d\n", ret);
        return ret;
    }

    ret = sp_set_stopbits(port, 1);
    if (ret != SP_OK) {
        printf("sp_set_stopbits error: %d\n", ret);
        return ret;
    }

    erpc_packet_t request;
    erpc_packet_t response;

    char hello[128];
    for (int i = 0; i < 1000000; i++) {
        erpc_init_packet(&request, i, 1);
        snprintf(hello, sizeof(hello), "Hello, %d", i * i);
        printf("%d\n", i);
        erpc_set_param_array(&request, 0, (uint8_t *) hello, strlen(hello));
        erpc_update_packet_length(&request, 1);
        erpc_finish_packet(&request);
        //erpc_dump_packet(&request);
        erpc_send_request(&request);
        bool result = false;
        uint8_t buf[1024];

        memset(&response, 0, sizeof(response));
        while (true) {
            ret = sp_blocking_read(port, buf, sizeof(buf), 1);
            if (ret < 0) {
                break;
            }
            for (int j = 0; j < ret; j++) {
                result = erpc_feed_packet(&response, buf[j]);
                if (result == true) {
                    //erpc_dump_packet(&response);
                    break;
                }
            }
            if (result == true) {
                break;
            }
        }
    }

    sp_close(port);

    return 0;
}