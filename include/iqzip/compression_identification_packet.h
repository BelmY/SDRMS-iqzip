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

#ifndef COMPRESSION_IDENTIFICATION_PACKET_H_
#define COMPRESSION_IDENTIFICATION_PACKET_H_

#include <stdint.h>
#include <string>
#include "iqzip/ccsds_packet_primary_header.h"

enum class COMPRESSION_TECHNIQUE_IDENTIFICATION
{
  NO_COMPRESSION = 0x0, CCSDS_LOSSLESS_COMPRESSION = 0x1
};

enum class SOURCE_CONFIGURATION_SUBHEADER
{
  PREPROCESSOR = 0x0,
  ENTROPY_CODER = 0x1,
  INSTRUMENT_CONFIGURATION = 0x2,
  EXTENDED_PARAMETERS = 0x3
};

enum class PREPROCESSOR_STATUS
{
  ABSENT = 0x0, PRESENT = 0x1
};

enum class PREPROCESSOR_PREDICTOR_TYPE
{
  BYPASS = 0x0, UNIT_DELAY = 0x1, APPLICATION_SPECIFIC = 0x7
};

enum class PREPROCESSOR_MAPPER_TYPE
{
  PREDICTION_ERROR = 0x0, APPLICATION_SPECIFIC = 0x3
};

enum class PREPROCESSOR_BLOCK_SIZE
{
  SAMPLES_8 = 0x0,
  SAMPLES_16 = 0x1,
  SAMPLES_32_64 = 0x2,
  APPLICATION_SPECIFIC = 0x3
};

enum class PREPROCESSOR_DATA_SENSE
{
  TWO_COMPLEMENT = 0x0, POSITIVE = 0x1
};

enum class ENTROPY_CODER_RESOLUTION_RANGE
{
  SPARE = 0x0, LESS_EQUAL_8 = 0x1, LESS_EQUAL_16 = 0x2, LESS_EQUAL_32 = 0x3
};

enum class EXTENDED_PARAMETERS_BLOCK_SIZE
{
  SAMPLES_8 = 0x0,
  SAMPLES_16 = 0x1,
  SAMPLES_32 = 0x2,
  SAMPLES_64 = 0x3,
  APPLICATION_SPECIFIC = 0xF
};

enum class EXTENDED_PARAMETERS_RESTRICTED_CODES
{
  BASIC = 0x0, RESTRICTED = 0x1
};

typedef struct
{
  uint8_t grouping_data_length_reserved :4;
  uint16_t grouping_data_length :12;
  uint8_t compression_technique_id :8;
  uint8_t reference_sample_interval :8;
} source_data_field_fixed_t;

typedef struct
{
  uint8_t header :2 = (uint8_t) SOURCE_CONFIGURATION_SUBHEADER::PREPROCESSOR;
  uint8_t status :1;
  uint8_t predictor_type :3;
  uint8_t mapper_type :2;
  uint8_t block_size :2;
  uint8_t data_sense :1;
  uint8_t sample_resolution :5;
} preprocessor_t;

typedef struct
{
  uint8_t header :2 = (uint8_t) SOURCE_CONFIGURATION_SUBHEADER::ENTROPY_CODER;
  uint8_t resolution_range :2;
  uint16_t cds_num :12;
} entropy_coder_t;

typedef struct
{
  uint8_t header :2 =
      (uint8_t) SOURCE_CONFIGURATION_SUBHEADER::INSTRUMENT_CONFIGURATION;
  uint16_t *params;
} instrument_configuration_t;

typedef struct
{
  uint8_t header :2 =
      (uint8_t) SOURCE_CONFIGURATION_SUBHEADER::EXTENDED_PARAMETERS;
  uint8_t reserved_1 :2 = 0x0;
  uint8_t block_size :4;
  uint8_t reserved_2 :1 = 0x0;
  uint8_t restricted_code_options :1;
  uint8_t reserved_3 :2 = 0x0;
  uint8_t reference_sample_interval :4;
} extended_parameters_t;

typedef struct
{
  preprocessor_t preprocessor;
  entropy_coder_t entropy_coder;
  extended_parameters_t extended_parameters;
  instrument_configuration_t instrument_config;
} source_configuration_t;

class compression_identification_packet
{

public:

  compression_identification_packet (uint8_t enable_preprocessing,
                                     uint16_t block_size, uint8_t endianess,
                                     uint8_t sample_resolution,
                                     uint8_t reference_sample_interval,
                                     uint8_t data_sense,
                                     uint8_t restricted_codes, uint8_t version,
                                     uint8_t type, uint8_t sec_hdr_flag,
                                     uint16_t apid, uint8_t sequence_flags,
                                     uint16_t sequence_count,
                                     uint_least16_t data_length);

  virtual
  ~compression_identification_packet ();

  void
  set_source_data_field_fixed (source_data_field_fixed_t data);

  void
  set_source_configuration (source_configuration_t data);

  void
  set_grouping_data_length (int length);

  void
  set_compression_technique_id (uint8_t id);

  void
  set_reference_sample_interval (uint8_t interval);

  void
  set_preprocessor_status (uint8_t status);

  void
  set_preprocessor_predictor_type (uint8_t type);

  void
  set_preprocessor_mapper_type (uint8_t type);

  void
  set_preprocessor_block_size (uint8_t size);

  void
  set_preprocessor_data_sense (uint8_t data_sense);

  void
  set_preprocessor_sample_resolution (uint8_t resolution);

  void
  set_entropy_coder_resolution_range (uint8_t resolution);

  void
  set_entropy_coder_cds_num (uint16_t num);

  void
  set_extended_parameters_block_size (uint8_t size);

  void
  set_extended_parameters_restricted_code_option (uint8_t option);

  void
  set_extended_parameters_reference_sample_interval (uint8_t interval);

  packet_primary_header_t&
  get_ccsds_primary_header ();

  source_data_field_fixed_t&
  get_source_data_field_fixed ();

  source_configuration_t&
  get_source_configuration ();

  uint16_t
  get_grouping_data_length () const;

  uint8_t
  get_compression_technique_id () const;

  uint8_t
  get_reference_sample_interval () const;

  uint8_t
  get_preprocessor_status () const;

  uint8_t
  get_preprocessor_predictor_type () const;

  uint8_t
  get_preprocessor_mapper_type () const;

  uint8_t
  get_preprocessor_block_size () const;

  uint8_t
  get_preprocessor_data_sense () const;

  uint8_t
  get_preprocessor_sample_resolution () const;

  void
  initialize_cip_header ();

  void
  write_header_to_file (std::string path);

  uint16_t
  get_block_size () const;

  void
  set_block_size (uint16_t block_size);

  uint8_t
  get_data_sense () const;

  void
  set_data_sense (uint8_t data_sense);

  uint8_t
  get_enable_preprocessing () const;

  void
  set_enable_preprocessing (uint8_t enable_preprocessing);

  uint8_t
  get_endiannes () const;

  void
  set_endiannes (uint8_t endiannes);

  uint8_t
  get_restricted_codes () const;

  void
  set_restricted_codes (uint8_t restricted_codes);

  uint8_t
  get_sample_resolution () const;

  void
  set_sample_resolution (uint8_t sample_resolution);

  ccsds_packet_primary_header *d_primary_header;
  source_data_field_fixed_t d_src_data_field_fixed;
  source_configuration_t d_source_configuration;

private:
  uint8_t d_enable_preprocessing;
  uint16_t d_block_size;
  uint8_t d_endiannes;
  uint8_t d_sample_resolution;
  uint8_t d_reference_sample_interval;
  uint8_t d_data_sense;
  uint8_t d_restricted_codes;
  uint8_t d_version;
  uint8_t d_type;
  uint8_t d_sec_hdr_flag;
  uint16_t d_apid;
  uint8_t d_sequence_flags;
  uint16_t d_sequence_count;
  uint16_t d_data_length;
};

#endif /* COMPRESSION_IDENTIFICATION_PACKET_H_ */
