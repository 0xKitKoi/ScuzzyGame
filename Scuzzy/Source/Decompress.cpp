#include "Decompress.h"
#include <stdexcept>

static const int UNCOMPRESSED_BLOCK = 0;
static const int RUN_LENGTH_ENCODED_BYTE = 1;
static const int RUN_LENGTH_ENCODED_SHORT = 2;
static const int INCREMENTAL_SEQUENCE = 3;
static const int REPEAT_PREVIOUS_DATA = 4;
static const int REVERSE_BITS = 5;
static const int UNKNOWN_1 = 6;
static const int UNKNOWN_2 = 7;

// generate reversed byte table
static std::vector<uint8_t> generateReversedBytes() {
    std::vector<uint8_t> reversed(256);
    for (int i = 0; i < 256; ++i) {
        uint8_t v = 0;
        for (int b = 0; b < 8; ++b) {
            if (i & (1 << b)) v |= (1 << (7 - b));
        }
        reversed[i] = v;
    }
    return reversed;
}
static const std::vector<uint8_t> REVERSED_BYTES = generateReversedBytes();

int getCompressedSize(int start, const std::vector<uint8_t>& data) {
    int bpos = 0;
    int pos = start;
    int bpos2 = 0;
    while (data[pos] != 0xFF) {
        if (pos >= (int)data.size()) return -8;
        int commandType = data[pos] >> 5;
        int length = (data[pos] & 0x1F) + 1;
        if (commandType == 7) {
            commandType = (data[pos] & 0x1C) >> 2;
            length = ((data[pos] & 3) << 8) + (data[pos + 1]) + 1;
            ++pos;
        }
        if (bpos + length < 0) return -1;
        ++pos;
        if (commandType >= 4) {
            bpos2 = (data[pos] << 8) + (data[pos + 1]);
            if (bpos2 < 0) return -2;
            pos += 2;
        }
        switch (commandType) {
            case UNCOMPRESSED_BLOCK:
                bpos += length;
                pos += length;
                break;
            case RUN_LENGTH_ENCODED_BYTE:
                bpos += length;
                ++pos;
                break;
            case RUN_LENGTH_ENCODED_SHORT:
                if (bpos < 0) return -3;
                bpos += 2 * length;
                pos += 2;
                break;
            case INCREMENTAL_SEQUENCE:
                bpos += length;
                ++pos;
                break;
            case REPEAT_PREVIOUS_DATA:
                if (bpos2 < 0) return -4;
                bpos += length;
                break;
            case REVERSE_BITS:
                if (bpos2 < 0) return -5;
                bpos += length;
                break;
            case UNKNOWN_1:
                if (bpos2 - length + 1 < 0) return -6;
                bpos += length;
                break;
            default:
            case UNKNOWN_2:
                return -7;
        }
    }
    return bpos;
}

std::vector<uint8_t> decompress(int start, const std::vector<uint8_t>& data, std::vector<uint8_t>& output, int /*read*/) {
    int maxLength = (int)output.size();
    int pos = start;
    int bpos = 0;
    int bpos2 = 0;
    int tmp;
    while (data[pos] != 0xFF) {
        if (pos >= (int)data.size()) return {}; // null in JS
        int commandType = data[pos] >> 5;
        int len = (data[pos] & 0x1F) + 1;
        if (commandType == 7) {
            commandType = (data[pos] & 0x1C) >> 2;
            len = ((data[pos] & 3) << 8) + data[pos + 1] + 1;
            ++pos;
        }
        if (bpos + len > maxLength || bpos + len < 0) return {};
        ++pos;
        if (commandType >= 4) {
            bpos2 = (data[pos] << 8) + data[pos + 1];
            if (bpos2 >= maxLength || bpos2 < 0) return {};
            pos += 2;
        }
        switch (commandType) {
            case UNCOMPRESSED_BLOCK:
                while (len-- != 0) output[bpos++] = data[pos++];
                break;
            case RUN_LENGTH_ENCODED_BYTE:
                while (len-- != 0) output[bpos++] = data[pos];
                ++pos;
                break;
            case RUN_LENGTH_ENCODED_SHORT:
                if (bpos + 2 * len > maxLength || bpos < 0) return {};
                while (len-- != 0) {
                    output[bpos++] = data[pos];
                    output[bpos++] = data[pos + 1];
                }
                pos += 2;
                break;
            case INCREMENTAL_SEQUENCE:
                tmp = data[pos++];
                while (len-- != 0) output[bpos++] = (uint8_t)(tmp++);
                break;
            case REPEAT_PREVIOUS_DATA:
                if (bpos2 + len > maxLength || bpos2 < 0) return {};
                for (int i = 0; i < len; ++i) output[bpos++] = output[bpos2 + i];
                break;
            case REVERSE_BITS:
                if (bpos2 + len > maxLength || bpos2 < 0) return {};
                while (len-- != 0) output[bpos++] = REVERSED_BYTES[output[bpos2++] & 0xFF];
                break;
            case UNKNOWN_1:
                if (bpos2 - len + 1 < 0) return {};
                while (len-- != 0) output[bpos++] = output[bpos2--];
                break;
            default:
            case UNKNOWN_2:
                return {};
        }
    }
    return output;
}
