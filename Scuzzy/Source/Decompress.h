#pragma once
#include <vector>
#include <cstdint>

// Returns decompressed output (empty vector on failure)
std::vector<uint8_t> decompress(int start, const std::vector<uint8_t>& data, std::vector<uint8_t>& output, int read);

// Returns compressed size in bytes, or negative error code
int getCompressedSize(int start, const std::vector<uint8_t>& data);
