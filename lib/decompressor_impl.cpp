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

#include "decompressor_impl.h"

#include <cstring>

namespace iqzip {

namespace compression {

decompressor_impl::decompressor_impl() :
    iqzip_impl(),
    d_iqzip_header_size(0),
    d_tmp_stream(new char[STREAM_CHUNK]),
    d_stream_avail_in(0),
    d_out(new char[CHUNK]),
    d_total_out(0)
{
}

decompressor_impl::~decompressor_impl()
{
}

int
decompressor_impl::decompress_init(const std::string fin,
                                   const std::string fout)
{
    /* Read header and save options to class fields */
    d_iqzip_header_size = d_ccsds_cip_hdr.parse_header_from_file(fin);
    d_version = d_ccsds_cip_hdr.decode_version();
    d_type = d_ccsds_cip_hdr.decode_type();
    d_sec_hdr_flag = d_ccsds_cip_hdr.decode_secondary_header_flag();
    d_apid = d_ccsds_cip_hdr.decode_application_process_identifier();
    d_sequence_flags = d_ccsds_cip_hdr.decode_sequence_flags();
    d_packet_sequence_count = d_ccsds_cip_hdr.decode_packet_sequence_count();
    d_packet_data_length = d_ccsds_cip_hdr.decode_packet_data_length();
    d_grouping_data_length = d_ccsds_cip_hdr.decode_grouping_data_length();
    d_compression_tech_id = d_ccsds_cip_hdr.decode_compression_technique_id();
    d_reference_sample_interval =
        d_ccsds_cip_hdr.decode_reference_sample_interval();
    d_preprocessor_status = d_ccsds_cip_hdr.decode_preprocessor_status();
    d_predictor_type = d_ccsds_cip_hdr.decode_preprocessor_predictor_type();
    d_mapper_type = d_ccsds_cip_hdr.decode_preprocessor_mapper_type();
    d_block_size = d_ccsds_cip_hdr.decode_block_size();
    d_data_sense = d_ccsds_cip_hdr.decode_preprocessor_data_sense();
    d_sample_resolution = d_ccsds_cip_hdr.decode_preprocessor_sample_resolution();
    d_cds_per_packet = 0;
    d_restricted_codes =
        d_ccsds_cip_hdr.decode_extended_parameters_restricted_code_option();
    d_endianness = d_ccsds_cip_hdr.decode_iqzip_header_endianess();

    /* Initialize libaec stream */
    init_aec_stream();

    /* Open input & output file */
    input_stream.open(fin, std::ios::in | std::ios::binary);
    if (!input_stream.is_open()) {
        std::cout << "Error opening input file" << std::endl;
        return -1;
    }
    output_stream.open(fout, std::ios::out | std::ios::binary);
    if (!output_stream.is_open()) {
        std::cout << "Error opening output file" << std::endl;
        return -1;
    }

    /* Skip header from file */
    input_stream.seekg(d_iqzip_header_size);

    /* Initialize libaec stream for decompression */
    int status = aec_decode_init(&d_strm);
    if (status != AEC_OK) {
        std::cout << "Error in initializing stream" << std::endl;
        print_error(status);
    }
    return status;
}

int
decompressor_impl::decompress()
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

        status = aec_decode(&d_strm, AEC_NO_FLUSH);
        if (status != AEC_OK) {
            std::cout << "Error in decoding" << std::endl;
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
    return 0;
}

int
decompressor_impl::stream_decompress(const char *inbuf,
                                     size_t nbytes)
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

        status = aec_decode(&d_strm, AEC_NO_FLUSH);
        if (status != AEC_OK) {
            std::cout << "Error in decoding" << std::endl;
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
        //d_stream_avail_in = d_strm.avail_in;
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
decompressor_impl::decompress_fin()
{
    int status;
    status = aec_decode_end(&d_strm);
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
decompressor_impl::stream_decompress_fin()
{
    int status;

    d_strm.next_out = reinterpret_cast<unsigned char *>(d_out);
    d_strm.next_in = reinterpret_cast<unsigned char *>(d_tmp_stream);
    d_strm.avail_in = d_stream_avail_in;

    status = aec_decode(&d_strm, AEC_NO_FLUSH);
    if (status != AEC_OK) {
        std::cout << "ERROR: while flushing output" << std::endl;
        print_error(status);
        return -1;
    }
    if (d_strm.total_out - d_total_out > 0) {
        output_stream.write(d_out, d_strm.total_out - d_total_out);
    }

    status = aec_decode_end(&d_strm);
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

decompressor_sptr
create_decompressor()
{
    return std::shared_ptr<decompressor>(new decompressor_impl);
}

} // namespace compression
} // namespace iqzip
