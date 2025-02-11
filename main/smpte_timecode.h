#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "driver/rmt_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t frame;
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    bool drop_frame_flag;
    bool color_flag;
    bool ext_sync_flag;
    uint16_t user;
    uint16_t ending_frame;
} ltc_frame;

typedef struct {
    uint8_t lsb;
    uint8_t msb;
} two_digits;

typedef struct simple_frame{
    uint8_t frame;
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    // simple_frame(){};
} simple_frame;

uint8_t reverse_bits(uint8_t n);

void convert_digits_to_single(two_digits* digits, uint8_t value);

void create_frame_from_timecode(ltc_frame* frame, uint8_t frames, uint8_t seconds, uint8_t minutes, uint8_t hours);

void create_bits_from_frame(uint8_t bits[10], ltc_frame frame);

#ifdef __cplusplus
}
#endif
