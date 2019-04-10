#ifndef IQZIP_COMPRESSOR_H
#define IQZIP_COMPRESSOR_H

#include <cstdint>
#include <iostream>
#include <string>

#include "iqzip.h"

class Iqzip_compressor : Iqzip{
public:

    Iqzip_compressor ();

    Iqzip_compressor (uint8_t d_version, uint8_t d_type, uint8_t d_sec_hdr_flag,
			uint16_t d_apid, uint8_t d_sequence_flags,
			uint16_t d_sequence_count, uint16_t d_packet_data_length,
			uint16_t d_grouping_data_length, uint8_t d_compression_tech_id,
			uint8_t d_reference_sample_interval, uint8_t d_preprocessor_status,
			uint8_t predictor_type, uint8_t mapper_type, uint16_t d_block_size,
			uint8_t d_data_sense, uint8_t d_sample_resolution,
			uint16_t d_cds_per_packet, uint8_t d_restricted_codes,
			uint8_t d_endianness);

    ~Iqzip_compressor();

    int	iqzip_compress_init(const std::string fin, const std::string fout);

    int iqzip_compress();

    int iqzip_compress_fin();

    void print_error(int status);
};
#endif /* IQZIP_COMPRESSOR_H */
