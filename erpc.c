/*
 * erpc.c
 *
 *  Created on: May 13, 2019
 *      Author: mengguang
 */


#include "erpc.h"
#include "stdio.h"
#include "string.h"

#define ERPC_MAGIC_BEGIN        0xAA
#define ERPC_MAGIC_END          0x55
#define ERPC_DATA_TYPE_ARRAY    0x01
#define ERPC_DATA_TYPE_UINT32   0x02
#define ERPC_DATA_TYPE_UINT64   0x03
#define ERPC_DATA_TYPE_INT32    0x04
#define ERPC_DATA_TYPE_INT64    0x05

//https://www.modbustools.com/modbus_crc16.html
uint16_t erpc_crc16(const uint8_t *nData, uint16_t wLength) {
    static const uint16_t wCRCTable[] = {
            0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
            0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
            0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
            0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
            0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
            0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
            0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
            0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
            0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
            0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
            0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
            0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
            0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
            0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
            0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
            0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
            0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
            0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
            0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
            0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
            0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
            0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
            0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
            0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
            0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
            0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
            0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
            0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
            0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
            0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
            0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
            0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040};

    uint8_t nTemp;
    uint16_t wCRCWord = 0xFFFF;

    while (wLength--) {
        nTemp = *nData++ ^ wCRCWord;
        wCRCWord >>= 8;
        wCRCWord ^= wCRCTable[nTemp];
    }
    return wCRCWord;
}

bool erpc_packet_is_valid(erpc_packet_t *packet) {
    if (packet->length < 5) {
        return false;
    }
    if (packet->data[0] != ERPC_MAGIC_BEGIN) {
        return false;
    }
    uint8_t length = packet->data[1];
    if (packet->data[length + 2 + 2] != ERPC_MAGIC_END) {
        return false;
    }
    uint16_t crc = *(uint16_t *) (packet->data + 2 + length);
    uint16_t crc_local = erpc_crc16(packet->data + 2, length);
    if (crc != crc_local) {
        printf("crc invalid!\n");
        printf("crc remote: %u\n", crc);
        printf("crc local: %u\n", crc_local);
        return false;
    }
    return true;
}

bool erpc_get_request_id(erpc_packet_t *packet, uint32_t *id) {
    uint8_t type = packet->data[2];
    if (type != ERPC_DATA_TYPE_UINT32) {
        return false;
    }
    *id = *(uint32_t *) (packet->data + 3);
    return true;
}

bool erpc_set_request_id(erpc_packet_t *packet, uint32_t id) {
    packet->data[2] = ERPC_DATA_TYPE_UINT32;
    *(uint32_t *) (packet->data + 3) = id;
    return true;
}

bool erpc_get_function(erpc_packet_t *packet, uint32_t *function) {
    uint8_t type = packet->data[7];
    if (type != ERPC_DATA_TYPE_UINT32) {
        return false;
    }
    *function = *(uint32_t *) (packet->data + 8);
    return true;
}

bool erpc_set_function(erpc_packet_t *packet, uint32_t function) {
    packet->data[7] = ERPC_DATA_TYPE_UINT32;
    *(uint32_t *) (packet->data + 8) = function;
    return true;
}

uint8_t erpc_get_param_position(erpc_packet_t *packet, uint8_t n) {
    uint8_t position = 0;
    position += 1; //magic begin;
    position += 1; //data length;
    position += 1 + 4; //request_id, uint32_t
    position += 1 + 4; //function, uint32_t
    uint8_t type;
    uint8_t length;
    for (uint8_t i = 0; i < n; i++) {
        type = packet->data[position];
        position += 1;
        switch (type) {
            case ERPC_DATA_TYPE_UINT32:
                position += 4;
                break;
            case ERPC_DATA_TYPE_UINT64:
                position += 8;
                break;
            case ERPC_DATA_TYPE_INT32:
                position += 4;
                break;
            case ERPC_DATA_TYPE_INT64:
                position += 8;
                break;
            case ERPC_DATA_TYPE_ARRAY:
                length = packet->data[position];
                position += length + 1;
                break;
            default:
                return 0; //invalid data type.
        }
    }
    return position;
}

bool erpc_get_param_uint32(erpc_packet_t *packet, uint8_t n, uint32_t *param) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    uint8_t type = packet->data[position];
    if (type != ERPC_DATA_TYPE_UINT32) {
        return false;
    }
    *param = *(uint32_t *) (packet->data + position + 1);

    return true;
}

bool erpc_set_param_uint32(erpc_packet_t *packet, uint8_t n, uint32_t param) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    packet->data[position] = ERPC_DATA_TYPE_UINT32;
    *(uint32_t *) (packet->data + position + 1) = param;
    return true;
}

bool erpc_get_param_uint64(erpc_packet_t *packet, uint8_t n, uint64_t *param) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    uint8_t type = packet->data[position];
    if (type != ERPC_DATA_TYPE_UINT64) {
        return false;
    }
    *param = *(uint64_t *) (packet->data + position + 1);

    return true;
}

bool erpc_set_param_uint64(erpc_packet_t *packet, uint8_t n, uint64_t param) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    packet->data[position] = ERPC_DATA_TYPE_UINT64;
    *(uint64_t *) (packet->data + position + 1) = param;
    return true;
}

bool erpc_get_param_int32(erpc_packet_t *packet, uint8_t n, int32_t *param) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    uint8_t type = packet->data[position];
    if (type != ERPC_DATA_TYPE_INT32) {
        return false;
    }
    *param = *(int32_t *) (packet->data + position + 1);

    return true;
}

bool erpc_set_param_int32(erpc_packet_t *packet, uint8_t n, int32_t param) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    packet->data[position] = ERPC_DATA_TYPE_INT32;
    *(int32_t *) (packet->data + position + 1) = param;
    return true;
}

bool erpc_get_param_int64(erpc_packet_t *packet, uint8_t n, int64_t *param) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    uint8_t type = packet->data[position];
    if (type != ERPC_DATA_TYPE_INT64) {
        return false;
    }
    *param = *(int64_t *) (packet->data + position + 1);

    return true;
}

bool erpc_set_param_int64(erpc_packet_t *packet, uint8_t n, int64_t param) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    packet->data[position] = ERPC_DATA_TYPE_INT64;
    *(int64_t *) (packet->data + position + 1) = param;
    return true;
}

bool erpc_get_param_array(erpc_packet_t *packet, uint8_t n, uint8_t **param,
                          uint8_t *length) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    uint8_t type = packet->data[position];
    if (type != ERPC_DATA_TYPE_ARRAY) {
        return false;
    }
    position += 1;
    *length = packet->data[position];
    position += 1;
    *param = packet->data + position;

    return true;
}

bool erpc_set_param_array(erpc_packet_t *packet, uint8_t n, uint8_t *param,
                          uint8_t length) {
    uint8_t position = erpc_get_param_position(packet, n);
    if (position == 0) {
        return false;
    }
    packet->data[position] = ERPC_DATA_TYPE_ARRAY;
    position += 1;
    packet->data[position] = length;
    position += 1;
    memcpy(packet->data + position, param, length);
    return true;
}

bool erpc_update_packet_length(erpc_packet_t *packet, uint8_t n_params) {
    uint8_t position = erpc_get_param_position(packet, n_params);
    position -= 2;
    packet->data[1] = position;
    return true;
}

bool erpc_process_echo(erpc_packet_t *response, uint8_t *data, uint8_t length) {
    erpc_set_param_array(response, 0, data, length);
    erpc_update_packet_length(response, 1);
    return true;
}

bool erpc_init_packet(erpc_packet_t *packet, uint32_t request_id, uint32_t function) {
    memset(packet->data, 0, ERPC_MAX_DATA_LENGTH);
    packet->length = 0;
    erpc_set_request_id(packet, request_id);
    erpc_set_function(packet, function);
    return true;
}

bool erpc_packet_fill_magic(erpc_packet_t *packet) {
    packet->data[0] = ERPC_MAGIC_BEGIN;
    uint8_t length = packet->data[1];
    if (length == 0) {
        return false;
    }
    packet->data[2 + length + 2] = ERPC_MAGIC_END;
    packet->length = length + 5;
    return true;
}

bool erpc_packet_fill_crc(erpc_packet_t *packet) {
    uint8_t length = packet->data[1];
    if (length == 0) {
        return false;
    }
    uint16_t crc = erpc_crc16(packet->data + 2, length);
    *(uint16_t *) (packet->data + 2 + length) = crc;
    return true;
}

bool erpc_finish_packet(erpc_packet_t *packet) {
    erpc_packet_fill_magic(packet);
    erpc_packet_fill_crc(packet);
    return true;
}

void erpc_hex_dump(uint8_t *data, uint16_t length) {
    printf("Hex data:\n");
    for (int i = 0; i < length; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

bool erpc_dump_packet(erpc_packet_t *packet) {
    erpc_hex_dump(packet->data, packet->length);
    uint8_t position = 0;
    uint8_t data_length = 0;
    printf("magic begin: %02X\n", packet->data[position]);
    position += 1;
    data_length = packet->data[position];
    printf("data length: %u\n", data_length);
    position += 1;

    uint8_t type;
    uint8_t length;
    uint8_t n = 0;
    while (position < data_length + 1 + 1) {
        type = packet->data[position];
        position += 1;
        switch (type) {
            case ERPC_DATA_TYPE_UINT32:
                printf("params %d : %lu\n", n, *(uint32_t *) (packet->data + position));
                position += 4;
                n++;
                break;
            case ERPC_DATA_TYPE_UINT64:
                printf("params %d : %llu\n", n, *(uint64_t *) (packet->data + position));
                position += 8;
                n++;
                break;
            case ERPC_DATA_TYPE_INT32:
                printf("params %d : %ld\n", n, *(int32_t *) (packet->data + position));
                position += 4;
                n++;
                break;
            case ERPC_DATA_TYPE_INT64:
                printf("params %d : %lld\n", n, *(int64_t *) (packet->data + position));
                position += 8;
                n++;
                break;
            case ERPC_DATA_TYPE_ARRAY:
                length = packet->data[position];
                printf("params %d length %d : \n", n, length);
                for (int k = 0; k < length; k++) {
                    printf("%02X", packet->data[position + 1 + k]);
                }
                printf("\n");
                position += length + 1;
                n++;
                break;
            default:
                return 0; //invalid data type.
        }
    }

    uint16_t crc = *(uint16_t *) (packet->data + position);
    printf("crc: %d\n", crc);
    position += 2;
    printf("magic end: %02X\n", packet->data[position]);

    return true;
}

bool erpc_process_request(erpc_packet_t *request, erpc_packet_t *response) {
    erpc_dump_packet(request);
    if (erpc_packet_is_valid(request) == false) {
        return false;
    }
    uint32_t function = 0;

    if (erpc_get_function(request, &function) == false) {
        return false;
    }

    uint32_t request_id = 0;
    if (erpc_get_request_id(request, &request_id) == false) {
        return false;
    }

    if (function == 0x01) {
        uint8_t *param0;
        uint8_t length;
        if (erpc_get_param_array(request, 0, &param0,
                                 &length) == false) {
            return false;
        }
        erpc_init_packet(response, request_id, function);
        if (erpc_process_echo(response, param0, length) == false) {
            return false;
        }
        erpc_finish_packet(response);
        erpc_dump_packet(response);
        return true;
    }

    return false;
}

bool erpc_feed_packet(erpc_packet_t *packet, uint8_t c) {
    packet->data[packet->length] = c;
    packet->length++;
    if (erpc_packet_is_valid(packet)) {
        return true;
    } else {
        return false;
    }
}
