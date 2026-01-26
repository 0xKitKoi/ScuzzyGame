#include "Rom.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include "Block.h"

static std::vector<uint8_t> s_data;

// Generate reversed bytes lookup table (reverses bits in a byte)
std::array<int16_t, 256> generateReversedBytes() {
    std::array<int16_t, 256> reversedBytes;
    for (int i = 0; i < 256; ++i) {
        int reversed = 0;
        int value = i;
        for (int j = 0; j < 8; ++j) {
            reversed = (reversed << 1) | (value & 1);
            value >>= 1;
        }
        reversedBytes[i] = static_cast<int16_t>(reversed);
    }
    return reversedBytes;
}

// Initialize the reversed bytes table
std::array<int16_t, 256> REVERSED_BYTES = generateReversedBytes();

bool ROM::loadFromFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (!in) return false;
    auto sz = in.tellg();
    in.seekg(0, std::ios::beg);
    s_data.resize((size_t)sz);
    in.read(reinterpret_cast<char*>(s_data.data()), (std::streamsize)sz);
    return true;
}

const std::vector<uint8_t>& ROM::data() { 
    return s_data; 
}

size_t ROM::size() { 
    return s_data.size(); 
}

Block readBlock(size_t location) {
    return Block(location);
}

size_t snesToHex(uint32_t address, bool header) {
    uint32_t newAddress = address;
    // Standard SNES mapping used by the JS port
    if (newAddress >= 0x400000 && newAddress < 0x600000) {
        newAddress -= 0x0;
    } else if (newAddress >= 0xC00000 && newAddress < 0x1000000) {
        newAddress -= 0xC00000;
    } else {
        // Fallback: pointer may already be a file offset (some ROM dumps/use cases)
        // In that case, just treat it as an offset and warn.
        std::cerr << "WARN: snesToHex: address 0x" << std::hex << address << std::dec 
                  << " not in SNES ranges; treating as file offset\n";
        return (size_t)address;
    }
    if (header) newAddress += 0x200;
    size_t result = (size_t)(newAddress - 0xA0200);
    return result;
}

uint32_t hexToSnes(size_t address, bool header) {
    size_t newAddress = address;
    if (header) newAddress -= 0x200;
    if (newAddress >= 0 && newAddress < 0x400000) {
        return (uint32_t)(newAddress + 0xC00000);
    } else if (newAddress >= 0x400000 && newAddress < 0x600000) {
        return (uint32_t)newAddress;
    } else {
        // JS implementation throws an error
        char buf[128];
        snprintf(buf, sizeof(buf), "File offset out of range: 0x%zx", address);
        throw std::runtime_error(buf);
    }
}

// Decompression function matching JS implementation
// Do not try to understand what this is doing. It will hurt you.
// The only documentation for this decompression routine is a 65816
// disassembly.
std::vector<uint8_t>* decompress(size_t start, const std::vector<uint8_t>& data, 
                                 std::vector<uint8_t>& output, size_t& out_size) {
    const size_t maxLength = output.size();
    size_t pos = start;
    size_t bpos = 0;
    size_t bpos2 = 0;
    uint8_t tmp;
    
    while (pos < data.size() && data[pos] != 0xFF) {
        // Data overflow before end of compressed data
        if (pos >= data.size()) {
            out_size = pos - start + 1;
            return nullptr;
        }
        
        int commandType = data[pos] >> 5;
        int len = (data[pos] & 0x1F) + 1;
        
        if (commandType == 7) {
            commandType = (data[pos] & 0x1C) >> 2;
            len = ((data[pos] & 3) << 8) + data[pos + 1] + 1;
            ++pos;
        }
        
        // Error: block length would overflow maxLength
        if (bpos + len > maxLength) {
            out_size = pos - start + 1;
            return nullptr;
        }
        
        ++pos;
        
        if (commandType >= 4) {
            if (pos + 1 >= data.size()) {
                out_size = pos - start + 1;
                return nullptr;
            }
            bpos2 = (data[pos] << 8) + data[pos + 1];
            if (bpos2 >= maxLength) {
                out_size = pos - start + 1;
                return nullptr;
            }
            pos += 2;
        }
        
        switch (commandType) {
            case UNCOMPRESSED_BLOCK:
                while (len-- != 0 && pos < data.size()) {
                    output[bpos++] = data[pos++];
                }
                break;
                
            case RUN_LENGTH_ENCODED_BYTE:
                if (pos >= data.size()) {
                    out_size = pos - start + 1;
                    return nullptr;
                }
                while (len-- != 0) {
                    output[bpos++] = data[pos];
                }
                ++pos;
                break;
                
            case RUN_LENGTH_ENCODED_SHORT:
                if (bpos + 2 * len > maxLength || pos + 1 >= data.size()) {
                    out_size = pos - start + 1;
                    return nullptr;
                }
                while (len-- != 0) {
                    output[bpos++] = data[pos];
                    output[bpos++] = data[pos + 1];
                }
                pos += 2;
                break;
                
            case INCREMENTAL_SEQUENCE:
                if (pos >= data.size()) {
                    out_size = pos - start + 1;
                    return nullptr;
                }
                tmp = data[pos++];
                while (len-- != 0) {
                    output[bpos++] = tmp++;
                }
                break;
                
            case REPEAT_PREVIOUS_DATA:
                if (bpos2 + len > maxLength) {
                    out_size = pos - start + 1;
                    return nullptr;
                }
                for (int i = 0; i < len; ++i) {
                    output[bpos++] = output[bpos2 + i];
                }
                break;
                
            case REVERSE_BITS:
                if (bpos2 + len > maxLength) {
                    out_size = pos - start + 1;
                    return nullptr;
                }
                while (len-- != 0) {
                    output[bpos++] = REVERSED_BYTES[output[bpos2++] & 0xFF];
                }
                break;
                
            case UNKNOWN_1:
                if (bpos2 - len + 1 < 0) {
                    out_size = pos - start + 1;
                    return nullptr;
                }
                while (len-- != 0) {
                    output[bpos++] = output[bpos2--];
                }
                break;
                
            case UNKNOWN_2:
            default:
                out_size = pos - start + 1;
                return nullptr;
        }
    }
    
    out_size = pos - start + 1;
    return &output;
}

// Get the size of compressed data
// Returns the decompressed size, or a negative error code
int getCompressedSize(size_t start, const std::vector<uint8_t>& data) {
    int bpos = 0;
    size_t pos = start;
    int bpos2 = 0;
    
    while (pos < data.size() && data[pos] != 0xFF) {
        // Data overflow before end of compressed data
        if (pos >= data.size()) {
            return -8;
        }
        
        int commandType = data[pos] >> 5;
        int length = (data[pos] & 0x1F) + 1;
        
        if (commandType == 7) {
            commandType = (data[pos] & 0x1C) >> 2;
            length = ((data[pos] & 3) << 8) + data[pos + 1] + 1;
            ++pos;
        }
        
        if (bpos + length < 0) {
            return -1;
        }
        
        ++pos;
        
        if (commandType >= 4) {
            if (pos + 1 >= data.size()) {
                return -2;
            }
            bpos2 = (data[pos] << 8) + data[pos + 1];
            if (bpos2 < 0) {
                return -2;
            }
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
                if (bpos < 0) {
                    return -3;
                }
                bpos += 2 * length;
                pos += 2;
                break;
                
            case INCREMENTAL_SEQUENCE:
                bpos += length;
                ++pos;
                break;
                
            case REPEAT_PREVIOUS_DATA:
                if (bpos2 < 0) {
                    return -4;
                }
                bpos += length;
                break;
                
            case REVERSE_BITS:
                if (bpos2 < 0) {
                    return -5;
                }
                bpos += length;
                break;
                
            case UNKNOWN_1:
                if (bpos2 - length + 1 < 0) {
                    return -6;
                }
                bpos += length;
                break;
                
            case UNKNOWN_2:
            default:
                return -7;
        }
    }
    
    return bpos;
}
