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

#include "compressor_impl.h"
#include <cstring>

namespace iqzip {

namespace compression {

compressor_impl::compressor_impl(uint8_t version, uint8_t type,
                                 uint8_t sec_hdr_flag, uint16_t apid,
                                 uint8_t sequence_flags,
                                 uint16_t sequence_count,
                                 uint16_t packet_data_length,
                                 uint16_t grouping_data_length,
                                 uint8_t compression_tech_id,
                                 uint8_t reference_sample_interval,
                                 uint8_t preprocessor_status,
                                 uint8_t predictor_type,
                                 uint8_t mapper_type,
                                 uint16_t block_size,
                                 uint8_t data_sense,
                                 uint8_t sample_resolution,
                                 uint16_t cds_per_packet,
                                 uint8_t restricted_codes,
                                 uint8_t endianness) :
    iqzip_impl(version, type, sec_hdr_flag, apid, sequence_flags,
               sequence_count, packet_data_length,
               grouping_data_length, compression_tech_id,
               reference_sample_interval, preprocessor_status,
               predictor_type, mapper_type, block_size, data_sense,
               sample_resolution, cds_per_packet, restricted_codes,
               endianness),
    d_tmp_stream(new char[sample_resolution * STREAM_CHUNK]),
    d_reference_samples_bytes((reference_sample_interval + 1) *
                              sample_resolution / 8 * block_size),
    d_stream_avail_in(0),
    d_out(new char[CHUNK]),
    d_total_out(0)

{
}

compressor_impl::~compressor_impl()
{
    delete[] d_tmp_stream;
}

int
compressor_impl::compress_init(const std::string fin,
                               const std::string fout)
{
    /* Initialize libaec stream */
    init_aec_stream();
    /* Initialize libaec stream for compression */
    int status = aec_encode_init(&d_strm);
    if (status != AEC_OK) {
        std::cout << "Error in initializing stream" << std::endl;
        print_error(status);
    }

    /* Write header to compressed file */
    d_ccsds_cip_hdr.write_header_to_file(fout);

    /* Open input & output file */
    input_stream.open(fin, std::ios::in | std::ios::binary);
    if (!input_stream.is_open()) {
        std::cout << "Error opening input file" << std::endl;
        return -1;
    }
    output_stream.open(fout,
                       std::ios::out | std::ios::app | std::ios::binary);
    if (!output_stream.is_open()) {
        std::cout << "Error opening output file" << std::endl;
        return -1;
    }

    return status;
}

int
compressor_impl::compress()
{
    int total_out = 0;
    int input_avail = 1;
    int output_avail = 1;
    int status;
    char *in = new char[CHUNK];
    char *out = new char[CHUNK];

    d_strm.next_out = reinterpret_cast<unsigned char *>(out);

    while (input_avail || output_avail) {
        if (d_strm.avail_in == 0 && input_avail) {
            input_stream.read(in, CHUNK);
            d_strm.avail_in = input_stream.gcount();
            if (d_strm.avail_in != CHUNK) {
                input_avail = 0;
            }
            d_strm.next_in = reinterpret_cast<const unsigned char *>(in);
        }

        status = aec_encode(&d_strm, AEC_NO_FLUSH);
        if (status != AEC_OK) {
            std::cout << "Error in encoding" << std::endl;
            print_error(status);
            return status;
        }

        if (d_strm.total_out - total_out > 0) {
            output_stream.write(out, d_strm.total_out - total_out);
            total_out = d_strm.total_out;
            output_avail = 1;
            d_strm.next_out = reinterpret_cast<unsigned char *>(out);
            d_strm.avail_out = CHUNK;
        }
        else {
            output_avail = 0;
        }
    }

    status = aec_encode(&d_strm, AEC_FLUSH);
    if (status != AEC_OK) {
        std::cout << "ERROR: while flushing output" << std::endl;
        print_error(status);
        return -1;
    }
    if (d_strm.total_out - total_out > 0) {
        output_stream.write(out, d_strm.total_out - total_out);
    }

    return 0;
}

int
compressor_impl::stream_compress_init(const std::string fout)
{
    /* Initialize libaec stream */
    init_aec_stream();
    /* Initialize libaec stream for compression */
    int status = aec_encode_init(&d_strm);
    if (status != AEC_OK) {
        std::cout << "Error in initializing stream" << std::endl;
        print_error(status);
    }

    /* Write header to compressed file */
    d_ccsds_cip_hdr.write_header_to_file(fout);

    /* Open output file */
    output_stream.open(fout, std::ios::out | std::ios::app | std::ios::binary);
    if (!output_stream.is_open()) {
        std::cout << "Error opening output file" << std::endl;
        return -1;
    }

    return status;
}

int
compressor_impl::stream_compress(const char *inbuf, size_t nbytes)
{
    int status;
    /* Save input buffer to internal buffer */
    if (d_stream_avail_in + nbytes < STREAM_CHUNK) {
        std::memcpy(&d_tmp_stream[d_stream_avail_in], inbuf, nbytes);
        d_stream_avail_in += nbytes;
        d_strm.avail_in = d_stream_avail_in;
        return AEC_OK;
    }
    /* d_stream_avail_in + nbytes >= STREAM_CHUNK */
    long int remainder = d_stream_avail_in + nbytes - STREAM_CHUNK;
    while (remainder >= 0) {
        std::memcpy(&d_tmp_stream[d_stream_avail_in], inbuf,
                    STREAM_CHUNK - d_stream_avail_in);
        d_stream_avail_in = STREAM_CHUNK;
        /* Ready aec_stream */
        d_strm.avail_in = d_stream_avail_in;
        d_strm.next_out = reinterpret_cast<unsigned char *>(d_out);
        d_strm.next_in = reinterpret_cast<unsigned char *>(d_tmp_stream);

        status = aec_encode(&d_strm, AEC_NO_FLUSH);
        if (status != AEC_OK) {
            std::cout << "Error in encoding" << std::endl;
            print_error(status);
            return status;
        }
        if (d_strm.total_out - d_total_out > 0) {
            /* Write encoded output to file */
            output_stream.write(d_out, d_strm.total_out - d_total_out);
            d_strm.avail_out = CHUNK;
            d_total_out = d_strm.total_out;
        }
        inbuf += nbytes - remainder;
        nbytes -= nbytes - remainder;
        d_stream_avail_in = 0;
        d_strm.avail_in = d_stream_avail_in;
        remainder = d_stream_avail_in + nbytes - STREAM_CHUNK;
    }
    /* If we have leftover samples copy them to stream */
    if (nbytes) {
        std::memcpy(&d_tmp_stream[d_stream_avail_in], inbuf, nbytes);
        d_stream_avail_in = nbytes;
        return AEC_OK;
    }

    return AEC_OK;
}

int
compressor_impl::compress_fin()
{
    int status;

    status = aec_encode_end(&d_strm);
    if (status != AEC_OK) {
        std::cout << "Error finishing stream" << std::endl;
        print_error(status);
        return status;
    }
    d_strm.next_in = nullptr;
    d_strm.next_out = nullptr;
    d_strm.state = nullptr;

    input_stream.close();
    output_stream.close();

    return 0;
}

int
compressor_impl::stream_compress_fin()
{
    int status;

    d_strm.next_out = reinterpret_cast<unsigned char *>(d_out);
    d_strm.next_in = reinterpret_cast<unsigned char *>(d_tmp_stream);
    d_strm.avail_in = d_stream_avail_in;

    status = aec_encode(&d_strm, AEC_FLUSH);
    if (status != AEC_OK) {
        std::cout << "ERROR: while flushing output" << std::endl;
        print_error(status);
        return -1;
    }
    if (d_strm.total_out - d_total_out > 0) {
        output_stream.write(d_out, d_strm.total_out - d_total_out);
    }

    status = aec_encode_end(&d_strm);
    if (status != AEC_OK) {
        std::cout << "Error finishing stream" << std::endl;
        print_error(status);
        return status;
    }

    d_strm.next_in = nullptr;
    d_strm.next_out = nullptr;
    d_strm.state = nullptr;

    input_stream.close();
    output_stream.close();

    return 0;
}

compressor_sptr
create_compressor(uint8_t version, uint8_t type,
                  uint8_t sec_hdr_flag, uint16_t apid,
                  uint8_t sequence_flags,
                  uint16_t sequence_count,
                  uint16_t packet_data_length,
                  uint16_t grouping_data_length,
                  uint8_t compression_tech_id,
                  uint8_t reference_sample_interval,
                  uint8_t preprocessor_status,
                  uint8_t predictor_type,
                  uint8_t mapper_type,
                  uint16_t block_size,
                  uint8_t data_sense,
                  uint8_t sample_resolution,
                  uint16_t cds_per_packet,
                  uint8_t restricted_codes,
                  uint8_t endianness)
{
    return std::shared_ptr<compressor>(new compressor_impl(version, type,
                                       sec_hdr_flag, apid, sequence_flags,
                                       sequence_count, packet_data_length,
                                       grouping_data_length, compression_tech_id,
                                       reference_sample_interval, preprocessor_status,
                                       predictor_type, mapper_type, block_size, data_sense,
                                       sample_resolution, cds_per_packet, restricted_codes,
                                       endianness));
}

} // namespace compression
} // namespace iqzip
