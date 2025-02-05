#include "smpte_timecode.h"
#include <stdio.h>

void convert_digits_to_single(two_digits* digits, uint8_t value) {
    digits->lsb = value % 10;
    digits->msb = (value / 10) % 10;
}

void create_frame_from_timecode(ltc_frame* frame, uint8_t frames, uint8_t seconds, uint8_t minutes, uint8_t hours) {
    frame->color_flag = false;
    frame->drop_frame_flag = false;
    frame->ending_frame = 0x3ffd;
    frame->ext_sync_flag = false;
    frame->user = 0;
    frame->frame = frames;
    frame->second = seconds;
    frame->minute = minutes;
    frame->hour = hours;
}

void create_bits_from_frame(uint8_t* bits[10], ltc_frame frame) {
    two_digits temp_digits;
    convert_digits_to_single(&temp_digits, frame.frame);

}