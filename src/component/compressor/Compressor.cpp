//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#include "Compressor.h"

shared_ptr<CompressedDataDto> Compressor::compress(string& value) {
    lzma_stream lzmaStream = LZMA_STREAM_INIT;

    if (!initEncoder(&lzmaStream)) {
        return nullptr;
    }
    lzmaStream.next_in = (uint8_t *) value.c_str();
    lzmaStream.avail_in = value.length();
    lzmaStream.next_out = new uint8_t[lzmaStream.avail_in / 2];
    lzmaStream.avail_out = lzmaStream.avail_in / 2;

    auto result = lzma_code(&lzmaStream, LZMA_FINISH);

    lzma_end(&lzmaStream);

    if (lzmaStream.avail_out == 0 || result == LZMA_STREAM_END) {
        auto out = make_shared<CompressedDataDto>();
        out->length = lzmaStream.avail_out;
        out->value = new uint8_t[lzmaStream.avail_out];

        for (uint32_t i = 0; i < lzmaStream.avail_out; ++i) {
            out->value[i] = lzmaStream.next_out[i];
        }
        return out;
    }

    if (result != LZMA_OK) {
        string errorMessage;

        switch (result) {
            case LZMA_MEM_ERROR:
                errorMessage = "Memory allocation failed";
                break;
            case LZMA_DATA_ERROR:
                errorMessage = "File size limits exceeded";
                break;
            default:
                errorMessage = "Unknown error, possibly a bug";
                break;
        }
        logger->error("Could not compress data! Reason: {}", errorMessage);
    }
    return nullptr;
}

bool Compressor::initEncoder(lzma_stream* lzmaStream) {
    lzma_options_lzma lzmaOptions;

    lzma_lzma_preset(&lzmaOptions, LZMA_PRESET_EXTREME);

    lzma_filter filters[] = {
        {LZMA_FILTER_X86, nullptr},
        {LZMA_FILTER_LZMA2, &lzmaOptions},
        {LZMA_VLI_UNKNOWN, nullptr},
    };
    // Initialize the encoder using the custom filter chain.
    auto result = lzma_stream_encoder(lzmaStream, filters, LZMA_CHECK_CRC64);

    if (result == LZMA_OK) {
        return true;
    }
    string errorMessage;

    switch (result) {
        case LZMA_MEM_ERROR:
            errorMessage = "Memory allocation failed";
            break;
        case LZMA_OPTIONS_ERROR:
            errorMessage = "Specified filter chain is not supported";
            break;
        case LZMA_UNSUPPORTED_CHECK:
            errorMessage = "Specified integrity check is not supported";
            break;
        default:
            errorMessage = "Unknown error, possibly a bug";
            break;
    }
    logger->error("Could not initialize LZMA2 encoder! Reason: {}", errorMessage);

    return false;
}

shared_ptr<string> Compressor::decompress(CompressedDataDto& compressedData) {
    lzma_stream lzmaStream = LZMA_STREAM_INIT;

    if (!initDecoder(&lzmaStream)) {
        return nullptr;
    }
    uint32_t outputBufferPredictionSize = compressedData.length * 10;
    lzmaStream.next_in = compressedData.value;
    lzmaStream.avail_in = compressedData.length;
    lzmaStream.next_out = new uint8_t[outputBufferPredictionSize];
    lzmaStream.avail_out = outputBufferPredictionSize;

    auto result = lzma_code(&lzmaStream, LZMA_FINISH);

    lzma_end(&lzmaStream);

    if (lzmaStream.avail_out == 0 || result == LZMA_STREAM_END) {
        auto data = make_shared<string>(string((char *)lzmaStream.next_out));

        return data;
    }

    if (result != LZMA_OK) {
        string errorMessage;

        switch (result) {
            case LZMA_MEM_ERROR:
                errorMessage = "Memory allocation failed";
                break;
            case LZMA_DATA_ERROR:
                errorMessage = "File size limits exceeded";
                break;
            default:
                errorMessage = "Unknown error, possibly a bug";
                break;
        }
        logger->error("Could not compress data! Reason: {}", errorMessage);
    }
    return nullptr;
}

bool Compressor::initDecoder(lzma_stream* lzmaStream) {
    lzma_ret result = lzma_stream_decoder(lzmaStream, UINT64_MAX, LZMA_CONCATENATED);

    if (result == LZMA_OK) {
		return true;
    }
    string errorMessage;

    switch (result) {
        case LZMA_MEM_ERROR:
            errorMessage = "Memory allocation failed";
            break;
        case LZMA_OPTIONS_ERROR:
            errorMessage = "Unsupported decompressor flags";
            break;
        default:
            errorMessage = "Unknown error, possibly a bug";
            break;
	}
    logger->error("Could not initialize LZMA2 decoder! Reason: {}", errorMessage);

    return false;
}
