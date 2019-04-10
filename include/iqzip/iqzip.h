#ifndef IQZIP_H
#define IQZIP_H

#include <cmath>
#include <fstream>

#include "libaec.h"
#include "iqzip/iqzip_compression_header.h"

class Iqzip {
protected:
    uint32_t CHUNK = 10485760;
    iqzip_compression_header d_iq_header;
    aec_stream d_strm;

    std::ofstream output_stream;
    std::ifstream input_stream;

    uint8_t d_version;
    uint8_t d_type;
    uint8_t d_sec_hdr_flag;
    uint16_t d_apid;
    uint8_t d_sequence_flags;
    uint16_t d_packet_sequence_count;
    uint16_t d_packet_data_length;
    uint16_t d_grouping_data_length;
    uint8_t d_compression_tech_id;
    uint8_t d_reference_sample_interval;
    uint8_t d_preprocessor_status;
    uint8_t d_predictor_type;
    uint8_t d_mapper_type;
    uint16_t d_block_size;
    uint8_t d_data_sense;
    uint8_t d_sample_resolution;
    uint16_t d_cds_per_packet;
    uint8_t d_restricted_codes;
    uint8_t d_endianness;

    Iqzip ();

	Iqzip (uint8_t version, uint8_t type, uint8_t sec_hdr_flag,
			uint16_t apid, uint8_t sequence_flags,
			uint16_t packet_sequence_count, uint16_t packet_data_length,
			uint16_t grouping_data_length, uint8_t compression_tech_id,
			uint8_t reference_sample_interval, uint8_t preprocessor_status,
			uint8_t predictor_type, uint8_t mapper_type, uint16_t block_size,
			uint8_t data_sense, uint8_t sample_resolution,
			uint16_t cds_per_packet, uint8_t restricted_codes,
			uint8_t endianness);

	virtual ~Iqzip();

    virtual void print_error(int) = 0;

    void init_aec_stream (void);

};
#endif /* IQZIP_H */
