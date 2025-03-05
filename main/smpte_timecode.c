#include "smpte_timecode.h"

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

uint8_t reverse_bits(uint8_t n) {
    n = ((n & 0xF0) >> 4) | ((n & 0x0F) << 4); // Swap nibbles
    n = ((n & 0xCC) >> 2) | ((n & 0x33) << 2); // Swap pairs
    n = ((n & 0xAA) >> 1) | ((n & 0x55) << 1); // Swap individual bits
    return n;
}

void create_bits_from_frame(uint8_t bits[10], ltc_frame frame) {
    two_digits temp_digits_frame;
    convert_digits_to_single(&temp_digits_frame, frame.frame);

    two_digits temp_digits_user;
    convert_digits_to_single(&temp_digits_user, frame.user);

    bits[0] = reverse_bits((temp_digits_user.lsb & 0x0F) << 4) | reverse_bits(temp_digits_frame.lsb & 0x0F);
    bits[1] = reverse_bits((temp_digits_user.msb & 0x0F) << 4) | reverse_bits(temp_digits_frame.msb & 0x0F);

    two_digits temp_digits_frame1;
    convert_digits_to_single(&temp_digits_frame1, frame.second);

    bits[2] = reverse_bits((temp_digits_user.lsb & 0x0F) << 4) | reverse_bits(temp_digits_frame1.lsb & 0x0F);
    bits[3] = reverse_bits((temp_digits_user.msb & 0x0F) << 4) | reverse_bits(temp_digits_frame1.msb & 0x0F);

    two_digits temp_digits_frame2;
    convert_digits_to_single(&temp_digits_frame2, frame.minute);

    bits[4] = reverse_bits((temp_digits_user.lsb & 0x0F) << 4) | reverse_bits(temp_digits_frame2.lsb & 0x0F);
    bits[5] = reverse_bits((temp_digits_user.msb & 0x0F) << 4) | reverse_bits(temp_digits_frame2.msb & 0x0F);
    
    two_digits temp_digits_frame3;
    convert_digits_to_single(&temp_digits_frame3, frame.hour);

    bits[6] = reverse_bits((temp_digits_user.lsb & 0x0F) << 4) | reverse_bits(temp_digits_frame3.lsb & 0x0F);
    bits[7] = reverse_bits((temp_digits_user.msb & 0x0F) << 4) | reverse_bits(temp_digits_frame3.msb & 0x0F);

    uint8_t firsthalf = 0x3f;
    uint8_t secondhalf = 0xfd;

    bits[8] = firsthalf;
    bits[9] = secondhalf;
    //This part is for the 27th bit, it needs to be set or cleared to always give the frame a even number of 0's
    int num_zeros = 0;

    for (int i = 0; i < 10; i++) {
        for (int j = 7; j >= 0; j--) {
            if (!((bits[i] >> j) & 1)) {
                num_zeros++;
            }
        }
    }

    if((num_zeros % 2) == 1) {
        bits[27/8] |= (1<<(27%8));
    }
}