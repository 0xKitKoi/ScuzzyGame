#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <array>

// Constants matching JS implementation
constexpr int MINIMUM_INDEX = 0;
constexpr int MAXIMUM_INDEX = 326;

// Decompression command types
constexpr int UNCOMPRESSED_BLOCK = 0;
constexpr int RUN_LENGTH_ENCODED_BYTE = 1;
constexpr int RUN_LENGTH_ENCODED_SHORT = 2;
constexpr int INCREMENTAL_SEQUENCE = 3;
constexpr int REPEAT_PREVIOUS_DATA = 4;
constexpr int REVERSE_BITS = 5;
constexpr int UNKNOWN_1 = 6;
constexpr int UNKNOWN_2 = 7;

// Generate reversed bytes lookup table (bit reversal)
std::array<int16_t, 256> generateReversedBytes();

// Export the reversed bytes table
extern std::array<int16_t, 256> REVERSED_BYTES;

// ROM data management
class ROM {
public:
    static bool loadFromFile(const std::string& path);
    static const std::vector<uint8_t>& data();
    static size_t size();
};

// Helper to create a Block from an absolute file offset
class Block;
Block readBlock(size_t location);

// SNES <-> file offset conversion (matches JS implementation)
// Throws exceptions like JS implementation does
size_t snesToHex(uint32_t address, bool header = true);
uint32_t hexToSnes(size_t address, bool header = true);

// Decompression functions
// Returns the decompressed data, or nullptr if decompression failed
// out_size receives the number of bytes of compressed data read
std::vector<uint8_t>* decompress(size_t start, const std::vector<uint8_t>& data, 
                                 std::vector<uint8_t>& output, size_t& out_size);

// Get the size of compressed data
// Returns the decompressed size, or a negative error code if there's an error
int getCompressedSize(size_t start, const std::vector<uint8_t>& data);
