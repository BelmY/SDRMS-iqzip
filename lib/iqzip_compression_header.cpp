/* -*- c++ -*- */
/*
 *  IQzip
 *
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

#include <iqzip/iqzip_compression_header.h>
#include <sigmf/annotation.h>
#include <stdexcept>
#include <cstring>
#include <iostream>

namespace iqzip {

namespace compression {

iqzip_compression_header::iqzip_compression_header(
    uint8_t version, uint8_t type, uint8_t sec_hdr_flag, uint16_t apid,
    uint8_t sequence_flags, uint16_t packet_sequence_count,
    uint16_t packet_data_length, uint16_t grouping_data_length,
    uint8_t compression_tech_id, uint8_t reference_sample_interval,
    uint8_t preprocessor_status, uint8_t predictor_type,
    uint8_t mapper_type, uint16_t block_size, uint8_t data_sense,
    uint8_t sample_resolution, uint16_t csd_per_packet,
    uint8_t restricted_codes, uint8_t endianness) :
    d_block_size(block_size),
    d_rsi(reference_sample_interval),
    d_enable_preprocessing(preprocessor_status),
    d_endianness(endianness),
    d_version(version),
    d_type(type),
    d_sec_hdr_flag(sec_hdr_flag),
    d_apid(apid),
    d_sequence_flags(sequence_flags),
    d_sequence_count(packet_sequence_count),
    d_data_length(packet_data_length),
    d_data_sense(data_sense),
    d_sample_resolution(sample_resolution),
    d_restricted_codes(restricted_codes)
{

    encode();

    d_primary_header = new ccsds_packet_primary_header(version, type,
            sec_hdr_flag, apid,
            sequence_flags,
            packet_sequence_count,
            packet_data_length);

    d_cip = new compression_identification_packet(grouping_data_length,
            compression_tech_id,
            reference_sample_interval,
            preprocessor_status,
            predictor_type,
            mapper_type, block_size,
            data_sense,
            sample_resolution,
            csd_per_packet,
            restricted_codes);

}

iqzip_compression_header::iqzip_compression_header() :
    d_block_size(0),
    d_rsi(0),
    d_enable_preprocessing(0),
    d_endianness(0),
    d_version(0),
    d_type(0),
    d_sec_hdr_flag(0),
    d_apid(0),
    d_sequence_flags(0),
    d_sequence_count(0),
    d_data_length(0),
    d_data_sense(0),
    d_sample_resolution(0),
    d_restricted_codes(0)
{
    d_primary_header = new ccsds_packet_primary_header();
    d_cip = new compression_identification_packet();
}

iqzip_compression_header::~iqzip_compression_header()
{
}

void
iqzip_compression_header::write_header_to_file(std::string path)
{
    FILE *f = fopen(path.c_str(), "wb");
    fwrite(&(d_primary_header->get_primary_header()),
           sizeof(ccsds_packet_primary_header::packet_primary_header_t), 1,
           f);
    fwrite(&(d_cip->get_source_data_fixed()),
           sizeof(compression_identification_packet::source_data_fixed_t), 1,
           f);
    fwrite(&(d_cip->get_source_data_variable().preprocessor),
           sizeof(compression_identification_packet::preprocessor_t), 1, f);
    fwrite(&(d_cip->get_source_data_variable().entropy_coder),
           sizeof(compression_identification_packet::entropy_coder_t), 1, f);
    //TODO: Add a parameter that indicates the use of Instrument Configuration subfield
    if (d_block_size > 16 || d_rsi > 255 || d_restricted_codes) {
        fwrite(
            &(d_cip->get_source_data_variable().extended_parameters),
            sizeof(compression_identification_packet::extended_parameters_t), 1,
            f);
    }
    if (d_block_size > 64) {
        fwrite(&d_iqzip_header, sizeof(iqzip_compression_header_t), 1, f);
    }
    fclose(f);
}

size_t
iqzip_compression_header::parse_header_from_file(std::string path)
{
    size_t result;
    size_t hdr_size;
    size_t max_header_size;
    uint8_t *buffer;
    FILE *f;
    ccsds_packet_primary_header::packet_primary_header_t *hdr_primary;
    compression_identification_packet::source_data_variable_t *hdr_src_cnf;
    compression_identification_packet::source_data_fixed_t *hdr_src_cnf_fixed;
    iqzip_compression_header_t *iqzip_hdr;

    hdr_size = CCSDS_PRIMARY_HEADER_SIZE + SOURCE_DATA_FIXED_SIZE
               + PREPROCESSOR_SUBFIELD_SIZE + ENTROPY_CODER_SUBFIELD_SIZE;
    max_header_size = MAX_CIP_HEADER_SIZE_BYTES + IQZIP_COMPRESSION_HDR_SIZE;

    buffer = (uint8_t *) malloc(sizeof(uint8_t) * max_header_size);
    f = fopen(path.c_str(), "rb");
    result = fread(buffer, 1, max_header_size, f);
    if (result != max_header_size) {
        throw std::runtime_error("File reading error");
        exit(3);
    }
    fclose(f);

    hdr_primary =
        (ccsds_packet_primary_header::packet_primary_header_t *)(&buffer[0]);
    hdr_src_cnf =
        (compression_identification_packet::source_data_variable_t *)(
            &buffer[CCSDS_PRIMARY_HEADER_SIZE
                    + SOURCE_DATA_FIXED_SIZE]);
    hdr_src_cnf_fixed =
        (compression_identification_packet::source_data_fixed_t *)(
            &buffer[CCSDS_PRIMARY_HEADER_SIZE]);
    // FIXME: Offset should be changed if Instrument Configuration subfiled is used
    iqzip_hdr =
        (iqzip_compression_header_t *)(&buffer[CCSDS_PRIMARY_HEADER_SIZE
                                       + SOURCE_DATA_FIXED_SIZE + PREPROCESSOR_SUBFIELD_SIZE
                                       + ENTROPY_CODER_SUBFIELD_SIZE + EXTENDED_PARAMETERS_SUBFIELD_SIZE]);
    d_primary_header->set_primary_header(*hdr_primary);
    d_cip->set_source_data_fixed(*hdr_src_cnf_fixed);
    d_cip->set_source_data_variable(*hdr_src_cnf);
    set_iqzip_compression_header(*iqzip_hdr);

    /* Retrieve header size */
    if (!(d_block_size = decode_preprocessor_block_size())) {
        d_block_size = decode_extended_parameters_block_size();
        hdr_size += EXTENDED_PARAMETERS_SUBFIELD_SIZE;
        if (!d_block_size) {
            hdr_size += IQZIP_COMPRESSION_HDR_SIZE;
        }
    }

    /* FIXME: Take into consideration the CCSDS secondary header and the
     * Instrument Configuration subfield */
    return hdr_size;

}

iqzip_compression_header::iqzip_compression_header_t &
iqzip_compression_header::get_iqzip_compression_header()
{
    return d_iqzip_header;
}

void
iqzip_compression_header::set_iqzip_compression_header(
    iqzip_compression_header_t hdr)
{
    d_iqzip_header = hdr;
}

void
iqzip_compression_header::encode()
{
    encode_iqzip_header_block_size(d_block_size);
    encode_iqzip_header_endianness(d_endianness);
    encode_iqzip_header_reference_sample_interval(d_rsi);
}

uint16_t
iqzip_compression_header::decode_block_size()
{
    uint16_t block_size;
    if (!(block_size = decode_preprocessor_block_size())) {
        block_size = decode_extended_parameters_block_size();
        if (!block_size) {
            return decode_iqzip_header_block_size();
        }
    }
    return block_size;
}

uint8_t
iqzip_compression_header::decode_version() const
{
    return d_primary_header->decode_version();
}

uint8_t
iqzip_compression_header::decode_type() const
{
    return d_primary_header->decode_type();
}

uint8_t
iqzip_compression_header::decode_secondary_header_flag() const
{
    return d_primary_header->decode_secondary_header_flag();
}

uint16_t
iqzip_compression_header::decode_application_process_identifier() const
{
    return d_primary_header->decode_application_process_identifier();
}

uint8_t
iqzip_compression_header::decode_sequence_flags() const
{
    return d_primary_header->decode_sequence_flags();
}

uint16_t
iqzip_compression_header::decode_packet_sequence_count() const
{
    return d_primary_header->decode_packet_sequence_count();
}

uint16_t
iqzip_compression_header::decode_packet_data_length() const
{
    return d_primary_header->decode_packet_data_length();
}

uint16_t
iqzip_compression_header::decode_grouping_data_length() const
{
    return d_cip->decode_grouping_data_length();
}

uint8_t
iqzip_compression_header::decode_compression_technique_id() const
{
    return d_cip->decode_compression_technique_id();
}

uint8_t
iqzip_compression_header::decode_reference_sample_interval() const
{
    return d_cip->decode_reference_sample_interval();
}

uint8_t
iqzip_compression_header::decode_preprocessor_status() const
{
    return d_cip->decode_preprocessor_status();
}

uint8_t
iqzip_compression_header::decode_preprocessor_predictor_type() const
{
    return d_cip->decode_preprocessor_predictor_type();
}

uint8_t
iqzip_compression_header::decode_preprocessor_mapper_type() const
{
    return d_cip->decode_preprocessor_mapper_type();
}

uint16_t
iqzip_compression_header::decode_preprocessor_block_size() const
{
    return d_cip->decode_preprocessor_block_size();
}

uint8_t
iqzip_compression_header::decode_preprocessor_data_sense() const
{
    return d_cip->decode_preprocessor_data_sense();
}

uint8_t
iqzip_compression_header::decode_preprocessor_sample_resolution() const
{
    return d_cip->decode_preprocessor_sample_resolution();
}

uint16_t
iqzip_compression_header::decode_extended_parameters_block_size() const
{
    return d_cip->decode_extended_parameters_block_size();
}

uint8_t
iqzip_compression_header::decode_extended_parameters_restricted_code_option()
const
{
    return d_cip->decode_extended_parameters_restricted_code_option();
}

uint8_t
iqzip_compression_header::decode_extended_parameters_reference_sample_interval()
const
{
    return d_cip->decode_extended_parameters_reference_sample_interval();
}

void
iqzip_compression_header::encode_version(uint8_t version)
{
    d_version = version;
}

void
iqzip_compression_header::encode_type(uint8_t type)
{
    d_type = type;
}

void
iqzip_compression_header::encode_secondary_header_flag(uint8_t flag)
{
    d_sec_hdr_flag = flag;
}

void
iqzip_compression_header::encode_application_process_identifier(
    uint16_t apid)
{
    d_apid = apid;
}

void
iqzip_compression_header::encode_sequence_flags(uint8_t flags)
{
    d_sequence_flags = flags;
}

void
iqzip_compression_header::encode_packet_sequence_count(uint16_t count)
{
    d_sequence_count = count;
}

void
iqzip_compression_header::encode_packet_data_length(uint16_t length)
{
    d_data_length = length;
}

void
iqzip_compression_header::encode_iqzip_header_block_size(size_t size)
{
    if (size > 65535) {
        std::runtime_error("Invalid IQzip header block size");
    }
    else if (size == 128) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_128);
    }
    else if (size == 256) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_256);
    }
    else if (size == 512) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_512);
    }
    else if (size == 1024) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_1024);
    }
    else if (size == 2048) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_2048);
    }
    else if (size == 4096) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_4096);
    }
    else if (size == 8192) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_8192);
    }
    else if (size == 16384) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_16384);
    }
    else if (size == 32768) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_32768);
    }
    else if (size == 65536) {
        d_iqzip_header.block_size = (uint8_t)(BLOCK_SIZE::SAMPLES_65535);
    }
    else {
        d_iqzip_header.block_size = 0;
    }
}

void
iqzip_compression_header::encode_iqzip_header_endianness(
    uint8_t endianness)
{
    switch (endianness) {
    case 1:
        d_iqzip_header.endianness = (uint8_t)(ENDIANNESS::LITTLE);
        break;
    case 0:
        d_iqzip_header.endianness = (uint8_t)(ENDIANNESS::BIG);
        break;
    default:
        std::runtime_error("Invalid IQzip header endianness");
    }
}

void
iqzip_compression_header::encode_iqzip_header_reference_sample_interval(
    uint16_t interval)
{
    d_iqzip_header.rsi = (interval - 1) % 2048;
}

uint16_t
iqzip_compression_header::decode_iqzip_header_block_size() const
{
    uint8_t code = d_iqzip_header.block_size;
    switch (code) {
    case (uint8_t)(BLOCK_SIZE::SAMPLES_128):
        return 128;
    case (uint8_t)(BLOCK_SIZE::SAMPLES_256):
        return 256;
    case (uint8_t)(BLOCK_SIZE::SAMPLES_512):
        return 512;
    case (uint8_t)(BLOCK_SIZE::SAMPLES_1024):
        return 1024;
    case (uint8_t)(BLOCK_SIZE::SAMPLES_2048):
        return 2048;
    case (uint8_t)(BLOCK_SIZE::SAMPLES_4096):
        return 4096;
    case (uint8_t)(BLOCK_SIZE::SAMPLES_8192):
        return 8192;
    case (uint8_t)(BLOCK_SIZE::SAMPLES_16384):
        return 16384;
    case (uint8_t)(BLOCK_SIZE::SAMPLES_32768):
        return 32768;
    case (uint8_t)(BLOCK_SIZE::SAMPLES_65535):
        return 65535;
    default:
        std::runtime_error("Invalid IQzip header block size code");
    }
    return 0;
}

uint16_t
iqzip_compression_header::decode_iqzip_header_endianess() const
{
    uint8_t code = d_iqzip_header.endianness;
    switch (code) {
    case (uint8_t)(ENDIANNESS::BIG):
        return 0;
    case (uint8_t)(ENDIANNESS::LITTLE):
        return 1;
    default:
        std::runtime_error("Invalid IQzip header endianness code");
    }
    return 0;
}

uint16_t
iqzip_compression_header::decode_iqzip_header_reference_sample_interval() const
{
    return d_iqzip_header.rsi;
}
}
}
