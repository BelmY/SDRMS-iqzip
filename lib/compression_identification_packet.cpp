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

#include "iqzip/compression_identification_packet.h"
#include <math.h>
#include <stdexcept>
#include <cstring>

compression_identification_packet::compression_identification_packet (
    uint8_t enable_preprocessing, uint16_t block_size, uint8_t endianess,
    uint8_t sample_resolution, uint8_t reference_sample_interval,
    uint8_t data_sense, uint8_t restricted_codes, uint8_t version, uint8_t type,
    uint8_t sec_hdr_flag, uint16_t apid, uint8_t sequence_flags,
    uint16_t sequence_count, uint_least16_t data_length) :
        d_enable_preprocessing (enable_preprocessing),
        d_block_size (block_size),
        d_endiannes (endianess),
        d_sample_resolution (sample_resolution),
        d_reference_sample_interval (reference_sample_interval),
        d_data_sense (data_sense),
        d_restricted_code (restricted_codes),
        d_version (version),
        d_type (type),
        d_sec_hdr_flag (sec_hdr_flag),
        d_apid (apid),
        d_sequence_flags (sequence_flags),
        d_sequence_count (sequence_count),
        d_data_length (data_length)

{
  d_primary_header = new ccsds_packet_primary_header (version, type,
                                                      sec_hdr_flag, apid,
                                                      sequence_flags,
                                                      sequence_count,
                                                      data_length);
}

compression_identification_packet::~compression_identification_packet ()
{
}

void
compression_identification_packet::write_header_to_file (std::string path)
{
  FILE* f = fopen (path.c_str (), "wb+");
  fwrite (&(get_ccsds_primary_header ()), sizeof(packet_primary_header_t), 1,
          f);
  fclose (f);
}

packet_primary_header_t&
compression_identification_packet::get_ccsds_primary_header ()
{
  return d_primary_header->get_primary_header ();
}

void
compression_identification_packet::set_source_data_field_fixed (
    source_data_field_fixed_t data)
{
  d_src_data_field_fixed = data;
}

void
compression_identification_packet::set_source_configuration (
    source_configuration_t data)
{
  d_source_configuration = data;
}

void
compression_identification_packet::set_grouping_data_length (uint16_t length)
{
  d_src_data_field_fixed.grouping_data_length = (length - 1) % 4096;
}

void
compression_identification_packet::set_compression_technique_id (uint8_t id)
{
  d_src_data_field_fixed.compression_technique_id = id;
}

void
compression_identification_packet::set_reference_sample_interval (
    uint8_t interval)
{
  d_src_data_field_fixed.reference_sample_interval = (interval - 1) * 256;
}

void
compression_identification_packet::set_preprocessor_status (uint8_t status)
{
  switch (status)
    {
    case (int) (PREPROCESSOR_STATUS::ABSENT):
    case (int) (PREPROCESSOR_STATUS::PRESENT):
      d_source_configuration.preprocessor.status = status;
      break;
    default:
      std::runtime_error ("Invalid preprocessor status");
    }
}

void
compression_identification_packet::set_preprocessor_predictor_type (
    uint8_t type)
{
  switch (type)
    {
    case (int) (PREPROCESSOR_PREDICTOR_TYPE::APPLICATION_SPECIFIC):
    case (int) (PREPROCESSOR_PREDICTOR_TYPE::BYPASS):
    case (int) (PREPROCESSOR_PREDICTOR_TYPE::UNIT_DELAY):
      d_source_configuration.preprocessor.predictor_type = type;
      break;
    default:
      std::runtime_error ("Invalid preprocessor predictor type");
    }
}

void
compression_identification_packet::set_preprocessor_mapper_type (uint8_t type)
{
  switch (type)
    {
    case (int) (PREPROCESSOR_MAPPER_TYPE::APPLICATION_SPECIFIC):
    case (int) (PREPROCESSOR_MAPPER_TYPE::PREDICTION_ERROR):
      d_source_configuration.preprocessor.mapper_type = type;
      break;
    default:
      std::runtime_error ("Invalid preprocessor mapper type");
    }
}

void
compression_identification_packet::set_preprocessor_block_size (uint8_t size)
{
  if (size <= 0) {
    std::runtime_error ("Invalid preprocessor block size");
  }
  else if (size == 8) {
    d_source_configuration.preprocessor.block_size = (uint8_t) (
        PREPROCESSOR_BLOCK_SIZE::SAMPLES_8);
  }
  else if (size == 16) {
    d_source_configuration.preprocessor.block_size = (uint8_t) (
        PREPROCESSOR_BLOCK_SIZE::SAMPLES_16);
  }
  else if (size == 32 || size == 64) {
    d_source_configuration.preprocessor.block_size = (uint8_t) (
        PREPROCESSOR_BLOCK_SIZE::SAMPLES_32_64);
  }
  else {
    d_source_configuration.preprocessor.block_size = (uint8_t) (
        PREPROCESSOR_BLOCK_SIZE::APPLICATION_SPECIFIC);
  }
}

void
compression_identification_packet::set_preprocessor_data_sense (
    uint8_t data_sense)
{
  switch (data_sense)
    {
    case (int) (PREPROCESSOR_DATA_SENSE::POSITIVE):
    case (int) (PREPROCESSOR_DATA_SENSE::TWO_COMPLEMENT):
      d_source_configuration.preprocessor.data_sense = data_sense;
      break;
    default:
      std::runtime_error ("Invalid preprocessor data sense");
    }
}

void
compression_identification_packet::set_preprocessor_sample_resolution (
    uint8_t resolution)
{
  if (resolution >= 1 && resolution <= 32) {
    d_source_configuration.preprocessor.sample_resolution = resolution;
  }
  else {
    std::runtime_error ("Invalid preprocessor sample resolution");
  }
}

void
compression_identification_packet::set_entropy_coder_resolution_range (
    uint8_t resolution)
{
  if (resolution <= 0) {
    std::runtime_error ("Invalid entropy coder resolution range");
  }
  else if (resolution <= 8) {
    d_source_configuration.entropy_coder.resolution_range = (uint8_t) (
        ENTROPY_CODER_RESOLUTION_RANGE::LESS_EQUAL_8);
  }
  else if (resolution <= 16) {
    d_source_configuration.entropy_coder.resolution_range = (uint8_t) (
        ENTROPY_CODER_RESOLUTION_RANGE::LESS_EQUAL_16);
  }
  else if (resolution <= 32) {
    d_source_configuration.entropy_coder.resolution_range = (uint8_t) (
        ENTROPY_CODER_RESOLUTION_RANGE::LESS_EQUAL_32);
  }
  else {
    d_source_configuration.preprocessor.block_size = (uint8_t) (
        ENTROPY_CODER_RESOLUTION_RANGE::SPARE);
  }
}

void
compression_identification_packet::set_entropy_coder_cds_num (uint16_t num)
{
  if (num >= 1 && num <= 4096) {
    d_source_configuration.entropy_coder.cds_num = num;
  }
  else {
    std::runtime_error ("Invalid entropy coder resolution range");
  }
}

void
compression_identification_packet::set_extended_parameters_block_size (
    uint8_t size)
{
  if (size <= 0) {
    std::runtime_error ("Invalid extended parameters block size");
  }
  else if (size == 8) {
    d_source_configuration.extended_parameters.block_size = (uint8_t) (
        EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_8);
  }
  else if (size == 16) {
    d_source_configuration.extended_parameters.block_size = (uint8_t) (
        EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_16);
  }
  else if (size == 32) {
    d_source_configuration.extended_parameters.block_size = (uint8_t) (
        EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_32);
  }
  else if (size == 64) {
    d_source_configuration.extended_parameters.block_size = (uint8_t) (
        EXTENDED_PARAMETERS_BLOCK_SIZE::SAMPLES_64);
  }
  else {
    d_source_configuration.extended_parameters.block_size = (uint8_t) (
        EXTENDED_PARAMETERS_BLOCK_SIZE::APPLICATION_SPECIFIC);
  }
}

void
compression_identification_packet::set_extended_parameters_restricted_code_option (
    uint8_t option)
{
  switch (option)
    {
    case (int) (EXTENDED_PARAMETERS_RESTRICTED_CODES::BASIC):
    case (int) (EXTENDED_PARAMETERS_RESTRICTED_CODES::RESTRICTED):
      d_source_configuration.extended_parameters.restricted_code_options =
          option;
      break;
    default:
      std::runtime_error ("Invalid extended parameters restricted code option");
    }
}

void
compression_identification_packet::set_extended_parameters_reference_sample_interval (
    uint8_t interval)
{
  d_source_configuration.extended_parameters.reference_sample_interval = floor (
      (interval - 1) % 256);
}

source_data_field_fixed_t
compression_identification_packet::get_source_data_field_fixed () const
{
  return d_src_data_field_fixed;
}

source_configuration_t
compression_identification_packet::get_source_configuration () const
{
  return d_source_configuration;
}

uint16_t
compression_identification_packet::get_grouping_data_length () const
{
  return d_src_data_field_fixed.grouping_data_length;
}

uint8_t
compression_identification_packet::get_compression_technique_id () const
{
  return d_src_data_field_fixed.compression_technique_id;
}

uint8_t
compression_identification_packet::get_reference_sample_interval () const
{
  return d_src_data_field_fixed.reference_sample_interval;
}

uint8_t
compression_identification_packet::get_preprocessor_status () const
{
  return d_source_configuration.preprocessor.status;
}

uint8_t
compression_identification_packet::get_preprocessor_predictor_type () const
{
  return d_source_configuration.preprocessor.predictor_type;
}

uint8_t
compression_identification_packet::get_preprocessor_mapper_type () const
{
  return d_source_configuration.preprocessor.mapper_type;
}

uint8_t
compression_identification_packet::get_preprocessor_block_size () const
{
  return d_source_configuration.preprocessor.block_size;
}

uint8_t
compression_identification_packet::get_preprocessor_data_sense () const
{
  return d_source_configuration.preprocessor.data_sense;
}

uint8_t
compression_identification_packet::get_preprocessor_sample_resolution () const
{
  return d_source_configuration.preprocessor.sample_resolution;
}

void
compression_identification_packet::initialize_cip_header ()
{
  memset (&d_src_data_field_fixed, 0, sizeof(source_data_field_fixed_t));
  memset (&d_source_configuration, 0, sizeof(source_configuration_t));
  d_source_configuration.preprocessor.block_size = d_block_size;
  d_source_configuration.preprocessor.data_sense = d_data_sense;
  // FIXME
  d_source_configuration.preprocessor.mapper_type = 0;
  d_source_configuration.preprocessor.predictor_type = 0;
  d_source_configuration.preprocessor.sample_resolution = d_sample_resolution;
  d_source_configuration.preprocessor.status = d_enable_preprocessing;
  //FIXME
  d_source_configuration.entropy_coder.cds_num = 0;
  d_source_configuration.entropy_coder.resolution_range = d_sample_resolution;
  d_source_configuration.extended_parameters.block_size = d_block_size;
  d_source_configuration.extended_parameters.reference_sample_interval =
      d_reference_sample_interval;
  d_source_configuration.extended_parameters.restricted_code_options =
      d_restricted_code;
}
