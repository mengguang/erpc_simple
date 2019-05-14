/*
 * erpc.h
 *
 *  Created on: May 13, 2019
 *      Author: mengguang
 */

#ifndef ERPC_H_
#define ERPC_H_
#include <stdint.h>
#include <stdbool.h>

#define ERPC_MAX_DATA_LENGTH    260    // 1 + 1 + 255 + 2 + 1

typedef struct {
    uint8_t data[ERPC_MAX_DATA_LENGTH];
    uint16_t length;
} erpc_packet_t;

//user defined function
bool erpc_send_request(erpc_packet_t *request);


//erpc interface
bool erpc_init_packet(erpc_packet_t *packet, uint32_t request_id, uint32_t function);
bool erpc_set_param_array(erpc_packet_t *packet, uint8_t n, uint8_t *param,
                          uint8_t length);
bool erpc_update_packet_length(erpc_packet_t *packet, uint8_t n_params);
bool erpc_finish_packet(erpc_packet_t *packet);
bool erpc_dump_packet(erpc_packet_t *packet);

bool erpc_feed_packet(erpc_packet_t *packet, uint8_t c);
bool erpc_process_request(erpc_packet_t *request, erpc_packet_t *response);
#endif /* ERPC_H_ */
