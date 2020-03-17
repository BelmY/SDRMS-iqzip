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

#include <iqzip/compression_identification_packet.h>
#include <math.h>
#include <stdexcept>
#include <cstring>
#include <byteswap.h>
#include <iostream>

namespace iqzip {

namespace compression {

compression_identification_packet::compression_identification_packet(
    uint16_t grouping_data_length, uint8_t compression_tech_id,
    uint8_t reference_sample_interval, uint8_t preprocessor_status,
    uint8_t predictor_type, uint8_t mapper_type, uint16_t block_size,
    uint8_t data_sense, uint8_t sample_resolution, uint16_t csd_per_packet,
    uint8_t restricted_codes) :
    d_grouping_data_length(grouping_data_length),
    d_compression_tech_id(compression_tech_id),
    d_reference_sample_interval(reference_sample_interval),
    d_preprocessor_status(preprocessor_status),
    d_predictor_type(predictor_type),
    d_mapper_type(mapper_type),
    d_block_size(block_size),
    d_data_sense(data_sense),
    d_sample_resolution(sample_resolution),
    d_cds_per_packet(csd_per_packet),
    d_restricted_codes(restricted_codes)
{
    encode();
}

compression_identification_packet::compression_identification_packet() :
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
    d_restricted_codes(0)
{
}

compression_identification_packet::~compression_identification_packet()
{
}

void
compression_identification_packet::encode()
{
    encode_grouping_data_length(d_grouping_data_length);
    encode_compression_technique_id(d_compression_tech_id);
    encode_reference_sample_interval(d_reference_sample_interval);
    encode_preprocessor();
    encode_entropy_coder();
    encode_instrument_configuration();
    encode_extended_parameters();
}

void
compression_identification_packet::encode_preprocessor()
{
    encode_preprocessor_header();
    encode_preprocessor_status(d_preprocessor_status);
    encode_preprocessor_predictor_type(d_predictor_type);
    encode_preprocessor_mapper_type(d_mapper_type);
    encode_preprocessor_block_size(d_block_size);
    encode_preprocessor_data_sense(d_data_sense);
    encode_preprocessor_sample_resolution(d_sample_resolution);
    //TODO: This line should be enclosed in a function for every subfield in d_source_data_variable
}

void
compression_identification_packet::encode_entropy_coder()
{
    encode_entropy_coder_header();
    encode_entropy_coder_resolution_range(d_sample_resolution);
    encode_entropy_coder_cds_num(d_cds_per_packet);
}

void
compression_identification_packet::encode_instrument_configuration()
{
    encode_instrument_configuration_header();
}

void
compression_identification_packet::encode_extended_parameters()
{
    // Set reserved fields to zero
    memset(&d_source_data_variable.extended_parameters, 0,
           sizeof(extended_parameters_t));
    encode_extended_parameters_header();
    encode_extended_parameters_block_size(d_block_size);
    encode_extended_parameters_restricted_code_option(d_restricted_codes);
    encode_extended_parameters_reference_sample_interval(
        d_reference_sample_interval);
}

compression_identification_packet::source_data_fixed_t &
compression_identification_packet::get_source_data_fixed()
{
    return d_source_data_fixed;
}

compression_identification_packet::source_data_variable_t &
compression_identification_packet::get_source_data_variable()
{
    return d_source_data_variable;
}

void
compression_identification_packet::set_source_data_fixed(
    source_data_fixed_t *hdr)
{
    memcpy(&d_source_data_fixed, hdr, sizeof(source_data_fixed_t));
}

void
compression_identification_packet::set_source_data_variable(
    source_data_variable_t *hdr)
{
    memcpy(&d_source_data_variable, hdr, sizeof(source_data_variable_t));
}

void
compression_identification_packet::encode_grouping_data_length(
    uint16_t length)
{
    //TODO: Fix length limitations
    length = length % 4096;
    d_source_data_fixed[0] |= length >> 8;
    d_source_data_fixed[1] = length;
}

void
compression_identification_packet::encode_compression_technique_id(
    uint8_t id)
{
    d_source_data_fixed[2] = id;
}

void
compression_identification_packet::encode_reference_sample_interval(
    uint8_t interval)
{
    d_source_data_fixed[3] = (interval - 1) % 256;
}

void
compression_identification_packet::encode_preprocessor_header()
{

    d_source_data_variable.preprocessor[0] |=
        SOURCE_CONFIGURATION_SUBFIELD_HEADER::PREPROCESSOR << 6;
}

void
compression_identification_packet::encode_preprocessor_status(
    uint8_t status)
{
    switch (status) {
    case 0:
        d_source_data_variable.preprocessor[0] |= (uint8_t)(PREPROCESSOR_STATUS::ABSENT)
                << 5;
        break;
    case 1:
        d_source_data_variable.preprocessor[0] |= (uint8_t)(
                    PREPROCESSOR_STATUS::PRESENT) << 5;
        break;
    default:
        std::runtime_error("Invalid preprocessor status");
    }
}

void
compression_identification_packet::encode_preprocessor_predictor_type(
    uint8_t type)
{
    switch (type) {
    case 7:
        d_source_data_variable.preprocessor[0] |= (uint8_t)(
                    PREPROCESSOR_PREDICTOR_TYPE::APPLICATION_SPECIFIC) << 2;
        break;
    case 0:
        d_source_data_variable.preprocessor[0] |= (uint8_t)(
                    PREPROCESSOR_PREDICTOR_TYPE::BYPASS) << 2;
        break;
    case 1:
        d_source_data_variable.preprocessor[0] |= (uint8_t)(
                    PREPROCESSOR_PREDICTOR_TYPE::UNIT_DELAY) << 2;
        break;
        break;
    default:
        std::runtime_error("Invalid preprocessor predictor type");
    }
}

void
compression_identification_packet::encode_preprocessor_mapper_type(
    uint8_t type)
{
    switch (type) {
    case 3:
        d_source_data_variable.preprocessor[0] |= ((uint8_t)(
                    PREPROCESSOR_MAPPER_TYPE::APPLICATION_SPECIFIC) &
                PREPROCESSOR_MAPPER_TYPE_MASK);
        break;
    case 0:
        d_source_data_variable.preprocessor[0] |= ((uint8_t)(
                    PREPROCESSOR_MAPPER_TYPE::PREDICTION_ERROR) & PREPROCESSOR_MAPPER_TYPE_MASK);
        break;
    default:
        std::runtime_error("Invalid preprocessor mapper type");
    }
}

void
compression_identification_packet::encode_preprocessor_block_size(
    uint16_t size)
{
    if (size <= 0) {
        std::runtime_error("Invalid preprocessor block size");
    }
    else if (size == 8) {
        d_source_data_variable.preprocessor[1] |= (uint8_t)(
                    PREPROCESSOR_BLOCK_SIZE::SAMPLES_8) << 6;
    }
    else if (size == 16) {
        d_source_data_variable.preprocessor[1] |= (uint8_t)(
                    PREPROCESSOR_BLOCK_SIZE::SAMPLES_16) << 6;
    }
    else if (size == 32 || size == 64) {
        d_source_data_variable.preprocessor[1] |= (uint8_t)(
                    PREPROCESSOR_BLOCK_SIZE::SAMPLES_32_64) << 6;
    }
    else {
        d_source_data_variable.preprocessor[1] |= (uint8_t)(
                    PREPROCESSOR_BLOCK_SIZE::APPLICATION_SPECIFIC) << 6;
    }
}

void
compression_identification_packet::encode_preprocessor_data_sense(
    uint8_t data_sense)
{
    switch (data_sense) {
    case 1:
        d_source_data_variable.preprocessor[1] |= (uint8_t)(
                    PREPROCESSOR_DATA_SENSE::POSITIVE) << 5;
        break;
    case 0:
        d_source_data_variable.preprocessor[1] |= (uint8_t)(
                    PREPROCESSOR_DATA_SENSE::TWO_COMPLEMENT) << 5;
        break;
    default:
        std::runtime_error("Invalid preprocessor data sense");
    }
}

void
compression_identification_packet::encode_preprocessor_sample_resolution(
    uint8_t resolution)
{
    if (resolution >= 1 && resolution <= 32) {
        d_source_data_variable.preprocessor[1] |= (resolution & 0x1f);
    }
    else {
        std::runtime_error("Invalid preprocessor sample resolution");
    }

}

void
compression_identification_packet::encode_entropy_coder_resolution_range(
    uint8_t resolution)
{
    if (resolution <= 0) {
        std::runtime_error("Invalid entropy coder resolution range");
    }
    else if (resolution <= 8) {
        d_source_data_variable.entropy_coder[0] |=
            (uint8_t)(ENTROPY_CODER_RESOLUTION_RANGE::LESS_EQUAL_8) << 4;
    }
    else if (resolution <= 16) {
        d_source_data_variable.entropy_coder[0] |=
            (uint8_t)(ENTROPY_CODER_RESOLUTION_RANGE::LESS_EQUAL_16) << 4;
    }
    else if (resolution <= 32) {
        d_source_data_variable.entropy_coder[0] |=
            (uint8_t)(ENTROPY_CODER_RESOLUTION_RANGE::LESS_EQUAL_32) << 4;
    }
    else {
        d_source_data_variable.entropy_coder[0] |=
            (uint8_t)(ENTROPY_CODER_RESOLUTION_RANGE::SPARE) << 4;
    }
}

void
compression_identification_packet::encode_entropy_coder_cds_num(
    uint16_t num)
{
    if (num >= 1 && num <= 4096) {
        d_source_data_variable.entropy_coder[0] |= ((num >> 8) &
                (ENTROPY_CODER_CDSES_NUM_MASK >> 8));
        d_source_data_variable.entropy_coder[1] = (uint8_t)num;
    }
    else {
        std::runtime_error("Invalid entropy coder resolution range");
    }
}

void
compression_identification_packet::encode_instrument_configuration_header()
{

    d_source_data_variable.instrument_config.header =
        (uint8_t)(SOURCE_CONFIGURATION_SUBFIELD_HEADER::INSTRUMENT_CONFIGURATION);
}

void
compression_identification_packet::encode_extended_parameters_header()
{

    d_source_data_variable.extended_parameters[0] |=
        (uint8_t)(SOURCE_CONFIGURATION_SUBFIELD_HEADER::EXTENDED_PARAMETERS) << 6;
}

void
compression_identification_packet::encode_extended_parameters_block_size(
    uint16_t size)
{
    if (size <= 0) {
        std::runtime_error("Invalid extended parameters block size");
    }
    else if (size == 8) {
        d_source_data_variable.extended_parameters[0] |=
            (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_8);
    }
    else if (size == 16) {
        d_source_data_variable.extended_parameters[0] |=
            (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_16);
    }
    else if (size == 32) {
        d_source_data_variable.extended_parameters[0] |=
            (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_32);
    }
    else if (size == 64) {
        d_source_data_variable.extended_parameters[0] |=
            (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_64);
    }
    else {
        d_source_data_variable.extended_parameters[0] |=
            (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::APPLICATION_SPECIFIC);
    }
}

void
compression_identification_packet::encode_entropy_coder_header()
{

    d_source_data_variable.entropy_coder[0] |=
        (uint8_t)(SOURCE_CONFIGURATION_SUBFIELD_HEADER::ENTROPY_CODER) << 6;
}

void
compression_identification_packet::encode_extended_parameters_restricted_code_option(
    uint8_t option)
{
    switch (option) {
    case 0:
        d_source_data_variable.extended_parameters[1] |=
            (uint8_t)(EXTENDED_PARAMETERS_RESTRICTED_CODES::BASIC) << 6;
        break;
    case 1:
        d_source_data_variable.extended_parameters[1] |=
            (uint8_t)(EXTENDED_PARAMETERS_RESTRICTED_CODES::RESTRICTED) << 6;
        break;
    default:
        std::runtime_error(
            "Invalid extended parameters restricted code option");
    }
}

void
compression_identification_packet::encode_extended_parameters_reference_sample_interval(
    uint8_t interval)
{
    d_source_data_variable.extended_parameters[1] |=
        (uint8_t)floor((interval - 1) % 256) &
        EXTENDED_PARAMETERS_REFERENCE_SAMPLE_INTERVAL_MASK;
}

uint16_t
compression_identification_packet::decode_grouping_data_length() const
{
    uint16_t len = 0;
    len |= d_source_data_fixed[1];
    len |= d_source_data_fixed[0] << 8;
    return len;
}

uint8_t
compression_identification_packet::decode_compression_technique_id() const
{
    return d_source_data_fixed[2];
}

uint8_t
compression_identification_packet::decode_reference_sample_interval() const
{
    return d_source_data_fixed[3] + 1;
}

uint8_t
compression_identification_packet::decode_preprocessor_status() const
{
    return (d_source_data_variable.preprocessor[0] & PREPROCESSOR_STATUS_MASK) >> 5;
}

uint8_t
compression_identification_packet::decode_preprocessor_predictor_type() const
{
    return (d_source_data_variable.preprocessor[0] &
            PREPROCESSOR_PREDICTOR_TYPE_MASK) >> 2;
}

uint8_t
compression_identification_packet::decode_preprocessor_mapper_type() const
{
    return (d_source_data_variable.preprocessor[0] & PREPROCESSOR_MAPPER_TYPE_MASK);
}

uint8_t
compression_identification_packet::decode_preprocessor_data_sense() const
{
    return (d_source_data_variable.preprocessor[1] & PREPROCESSOR_DATA_SENSE_MASK)
           >> 5;
}

uint8_t
compression_identification_packet::decode_preprocessor_sample_resolution() const
{
    return (d_source_data_variable.preprocessor[1] &
            PREPROCESSOR_SAMPLE_RESOLUTION_MASK);
}

uint16_t
compression_identification_packet::decode_preprocessor_block_size() const
{
    uint8_t code = (d_source_data_variable.preprocessor[1] &
                    PREPROCESSOR_BLOCK_SIZE_MASK) >> 6;
    switch (code) {
    case (uint8_t)(PREPROCESSOR_BLOCK_SIZE::SAMPLES_8):
        return 8;
    case (uint8_t)(PREPROCESSOR_BLOCK_SIZE::SAMPLES_16):
        return 16;
    case (uint8_t)(PREPROCESSOR_BLOCK_SIZE::SAMPLES_32_64):
    case (uint8_t)(PREPROCESSOR_BLOCK_SIZE::APPLICATION_SPECIFIC):
        return 0;
    default:
        std::runtime_error("Invalid preprocessor block size code");
    }
    return 0;
}

uint16_t
compression_identification_packet::decode_extended_parameters_block_size() const
{
    uint8_t code = d_source_data_variable.extended_parameters[0] &
                   EXTENDED_PARAMETERS_BLOCK_SIZE_MASK;
    switch (code) {
    case (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_8):
        return 8;
    case (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_16):
        return 16;
    case (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_32):
        return 32;
    case (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_64):
        return 64;
    case (uint8_t)(EXTENDED_PARAMETERS_BLOCK_SIZE::APPLICATION_SPECIFIC):
        return 0;
    default:
        std::runtime_error("Invalid extended parameters block size code");
    }
    return 0;
}

uint8_t
compression_identification_packet::decode_extended_parameters_reference_sample_interval()
const
{
    return d_source_data_variable.extended_parameters[1] &
           EXTENDED_PARAMETERS_REFERENCE_SAMPLE_INTERVAL_MASK;
}

uint8_t
compression_identification_packet::decode_extended_parameters_restricted_code_option()
const
{
    return d_source_data_variable.extended_parameters[1] &
           EXTENDED_PARAMETERS_RESTRICTED_CODES_MASK;
}

} // namespace compression
} // namespace iqzip

