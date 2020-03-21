/* -*- c++ -*- */
/*
 *  Copyright (C) 2019, Libre Space Foundation <https://libre.space/>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "iqzip_impl.h"

namespace iqzip {

iqzip_impl::iqzip_impl() :
    d_version(0),
    d_type(0),
    d_sec_hdr_flag(0),
    d_apid(0),
    d_sequence_flags(0),
    d_packet_sequence_count(0),
    d_packet_data_length(0),
    d_grouping_data_length(0),
    d_compression_tech_id(0),
    d_reference_sample_interval(0),
    d_preprocessor_status(0),
    d_predictor_type(0),
    d_mapper_type(0),
    d_block_size(0),
    d_data_sense(0),
    d_sample_resolution(0),
    d_cds_per_packet(0),
    d_restricted_codes(0),
    d_endianness(0)
{
    d_ccsds_cip_hdr = compression::header::iqzip_compression_header();
}

iqzip_impl::iqzip_impl(uint8_t version, uint8_t type, uint8_t sec_hdr_flag,
                       uint16_t apid, uint8_t sequence_flags,
                       uint16_t packet_sequence_count, uint16_t packet_data_length,
                       uint16_t grouping_data_length, uint8_t compression_tech_id,
                       uint8_t reference_sample_interval, uint8_t preprocessor_status,
                       uint8_t predictor_type, uint8_t mapper_type, uint16_t block_size,
                       uint8_t data_sense, uint8_t sample_resolution,
                       uint16_t cds_per_packet, uint8_t restricted_codes,
                       uint8_t endianness) :
    d_version(version),
    d_type(type),
    d_sec_hdr_flag(sec_hdr_flag),
    d_apid(apid),
    d_sequence_flags(sequence_flags),
    d_packet_sequence_count(packet_sequence_count),
    d_packet_data_length(packet_data_length),
    d_grouping_data_length(grouping_data_length),
    d_compression_tech_id(compression_tech_id),
    d_reference_sample_interval(reference_sample_interval),
    d_preprocessor_status(preprocessor_status),
    d_predictor_type(predictor_type),
    d_mapper_type(mapper_type),
    d_block_size(block_size),
    d_data_sense(data_sense),
    d_sample_resolution(sample_resolution),
    d_cds_per_packet(cds_per_packet),
    d_restricted_codes(restricted_codes),
    d_endianness(endianness)
{
    /* Initialize IQ CCSDS header */
    d_ccsds_cip_hdr = compression::header::iqzip_compression_header(
                          d_version, d_type, d_sec_hdr_flag, d_apid, d_sequence_flags,
                          d_packet_sequence_count, d_packet_data_length, d_grouping_data_length,
                          d_compression_tech_id, d_reference_sample_interval, d_preprocessor_status,
                          d_predictor_type, d_mapper_type, d_block_size, d_data_sense,
                          d_sample_resolution, d_cds_per_packet, d_restricted_codes, d_endianness);
}

iqzip_impl::~iqzip_impl()
{
}

void
iqzip_impl::init_aec_stream(void)
{
    d_strm.avail_in = 0;
    d_strm.avail_out = CHUNK;
    d_strm.bits_per_sample = d_sample_resolution;
    d_strm.block_size = d_block_size;
    d_strm.flags = 0;
    /* Always use 3 bytes for 24bits samples and don't enforce */
    d_strm.flags |= AEC_DATA_3BYTE;
    d_strm.flags |= AEC_NOT_ENFORCE;
    /* Shift option bit to corresponding bit in flag to initialize flags */
    d_strm.flags |= (~(d_data_sense << (uint8_t) log2(AEC_DATA_SIGNED)))
                    & AEC_DATA_SIGNED;
    d_strm.flags |= (~(d_endianness << (uint8_t) log2(AEC_DATA_MSB)))
                    & AEC_DATA_MSB;
    d_strm.flags |= d_preprocessor_status << (uint8_t) log2(AEC_DATA_PREPROCESS);
    d_strm.flags |= d_restricted_codes << (uint8_t) log2(AEC_RESTRICTED);
    //d_strm.flags |= AEC_PAD_RSI;
    d_strm.next_in = nullptr;
    d_strm.next_out = nullptr;
    d_strm.rsi = d_reference_sample_interval;
    d_strm.state = nullptr;
    d_strm.total_in = 0;
    d_strm.total_out = 0;
}

void
iqzip_impl::print_error(int status)
{
    switch (status) {
    case AEC_CONF_ERROR:
        std::cout << "Compressor: Configuration Error" << std::endl;
        break;
    case AEC_STREAM_ERROR:
        std::cout << "Compressor: Streaming Error" << std::endl;
        break;
    case AEC_DATA_ERROR:
        std::cout << "Compressor: Data Error" << std::endl;
        break;
    case AEC_MEM_ERROR:
        std::cout << "Compressor: Memory allocation Error" << std::endl;
        break;
    default:
        std::cout << "Compressor: Unknown Error" << std::endl;
    }
}

} // namespace iqzip
