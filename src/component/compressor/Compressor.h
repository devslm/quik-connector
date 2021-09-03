//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_COMPRESSOR_H
#define QUIK_CONNECTOR_COMPRESSOR_H

#include <cstdint>
#include <string>
#include <memory>
#include <lzma.h>
#include "../../service/log/Logger.h"

using namespace std;

extern shared_ptr<spdlog::logger> logger;

typedef struct CompressedDataDto {
    uint8_t* value;
    uint32_t length;
} CompressedDataDto;

class Compressor {
public:
    Compressor() = default;

    ~Compressor() = default;

    static shared_ptr<CompressedDataDto> compress(string& value);

    static shared_ptr<string> decompress(CompressedDataDto& compressedData);

private:
    static bool initEncoder(lzma_stream* lzmaStream);

    static bool initDecoder(lzma_stream* lzmaStream);
};

#endif //QUIK_CONNECTOR_COMPRESSOR_H
