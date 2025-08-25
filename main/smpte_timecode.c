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

void create_bits_from_frame(uint8_t bits[10], ltc_frame frame, int framerate) {
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

uint8_t get_bit_from_all(uint8_t bits[10], int index) {
    int byte_index = index / 8;
    int bit_index = index % 8;
    return (bits[byte_index] >> bit_index) & 1;
}

// Helper to decode BCD from arbitrary bit positions
int bcd_decode(uint8_t bits[10], int bit_indices[], int count) {
    int value = 0;
    for (int i = 0; i < count; i++) {
        value |= get_bit_from_all(bits, bit_indices[i]) << i;
    }
    return value;
}

void log_frame_from_bits(uint8_t bits[10]) {
    uint8_t reversed_bits[10];
    for (int i = 0; i < 10; i++) {
        reversed_bits[i] = reverse_bits(bits[i]);
    }

    // Use reversed_bits instead of bits
    int frame_units = bcd_decode(reversed_bits, (int[]){0, 1, 2, 3}, 4);
    int frame_tens  = bcd_decode(reversed_bits, (int[]){8, 9}, 2);
    int sec_units   = bcd_decode(reversed_bits, (int[]){16, 17, 18, 19}, 4);
    int sec_tens    = bcd_decode(reversed_bits, (int[]){24, 25, 26}, 3);
    int min_units   = bcd_decode(reversed_bits, (int[]){32, 33, 34, 35}, 4);
    int min_tens    = bcd_decode(reversed_bits, (int[]){40, 41, 42}, 3);
    int hour_units  = bcd_decode(reversed_bits, (int[]){48, 49, 50, 51}, 4);
    int hour_tens   = bcd_decode(reversed_bits, (int[]){56, 57}, 2);

    int frames  = frame_tens * 10 + frame_units;
    int seconds = sec_tens * 10 + sec_units;
    int minutes = min_tens * 10 + min_units;
    int hours   = hour_tens * 10 + hour_units;

    ESP_LOGI("TC", "Decoded Timecode: %02d:%02d:%02d:%02d", hours, minutes, seconds, frames);
}