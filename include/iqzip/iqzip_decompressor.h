#ifndef IQZIP_DECOMPRESSOR_H
#define IQZIP_DECOMPRESSOR_H

#include <cstdint>
#include <iostream>
#include <string>

#include "iqzip.h"

class Iqzip_decompressor : Iqzip {
private:
	size_t d_iqzip_header_size;

public:

    Iqzip_decompressor ();

    ~Iqzip_decompressor();

    int	iqzip_decompress_init(const std::string fin, const std::string fout);

    int iqzip_decompress();

    int iqzip_decompress_fin();

    void print_error(int status);
};
#endif /* IQZIP_DECOMPRESSOR_H */
