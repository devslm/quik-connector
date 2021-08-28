//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_COMPRESSOR_H
#define QUIK_CONNECTOR_COMPRESSOR_H

class Compressor {
public:
    Compressor() = default;

    ~Compressor() = default;

    static void compress();

    static void uncompress();
};

#endif //QUIK_CONNECTOR_COMPRESSOR_H
