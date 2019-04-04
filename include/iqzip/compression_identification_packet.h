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

/*
 * Max size of CIP header in bytes excluding Secondary header and Instrument Configuration
 */
#define MAX_CIP_HEADER_SIZE_BYTES               16
#define SOURCE_DATA_FIXED_SIZE                  4
#define PREPROCESSOR_SUBFIELD_SIZE              2
#define ENTROPY_CODER_SUBFIELD_SIZE             2
#define EXTENDED_PARAMETERS_SUBFIELD_SIZE       2
#define INSTRUMENT_CONFIG_SUBFIELD_SIZE         2

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

enum SOURCE_CONFIGURATION_SUBFIELD_HEADER
{
  PREPROCESSOR = 0x0,
  ENTROPY_CODER = 0x1,
  INSTRUMENT_CONFIGURATION = 0x2,
  EXTENDED_PARAMETERS = 0x3
};

typedef struct
{
  uint8_t header :2;
  uint8_t status :1;
  uint8_t predictor_type :3;
  uint8_t mapper_type :2;
  uint8_t block_size :2;
  uint8_t data_sense :1;
  uint8_t sample_resolution :5;
} preprocessor_t;

typedef struct
{
  uint8_t header :2;
  uint8_t resolution_range :2;
  uint16_t cds_num :12;
} entropy_coder_t;

typedef struct
{
  uint8_t header :2;
  uint16_t *params;
} instrument_configuration_t;

typedef struct
{
  uint8_t header :2;
  uint8_t reserved_1 :2 = 0x0;
  uint8_t block_size :4;
  uint8_t reserved_2 :1 = 0x0;
  uint8_t restricted_code_options :1;
  uint8_t reserved_3 :2 = 0x0;
  uint8_t reference_sample_interval :4;
} extended_parameters_t;

typedef struct
{
  uint8_t grouping_data_length_reserved :4;
  uint16_t grouping_data_length :12;
  uint8_t compression_technique_id :8;
  uint8_t reference_sample_interval :8;
} source_data_fixed_t;

typedef struct
{
  preprocessor_t preprocessor;
  entropy_coder_t entropy_coder;
  extended_parameters_t extended_parameters;
  instrument_configuration_t instrument_config;
} source_data_variable_t;

class compression_identification_packet
{

public:

  compression_identification_packet (uint16_t grouping_data_length,
                                     uint8_t compression_tech_id,
                                     uint8_t reference_sample_interval,
                                     uint8_t preprocessor_status,
                                     uint8_t predictor_type,
                                     uint8_t mapper_type,
                                     uint16_t block_size,
                                     uint8_t data_sense,
                                     uint8_t sample_resolution,
                                     uint16_t csd_per_packet,
                                     uint8_t restricted_codes);

  compression_identification_packet ();

  virtual
  ~compression_identification_packet ();

  void
  encode ();

  source_data_fixed_t&
  get_source_data_fixed ();

  source_data_variable_t&
  get_source_data_variable ();

  void
  set_source_data_fixed (source_data_fixed_t hdr);

  void
  set_source_data_variable (source_data_variable_t hdr);

  void
  encode_grouping_data_length (uint16_t length);

  void
  encode_compression_technique_id (uint8_t id);

  void
  encode_reference_sample_interval (uint8_t interval);

  void
  encode_preprocessor_header ();

  void
  encode_entropy_coder_header ();

  void
  encode_instrument_configuration_header ();

  void
  encode_extended_parameters_header ();

  void
  encode_preprocessor_status (uint8_t status);

  void
  encode_preprocessor_predictor_type (uint8_t type);

  void
  encode_preprocessor_mapper_type (uint8_t type);

  void
  encode_preprocessor_block_size (uint16_t size);

  void
  encode_preprocessor_data_sense (uint8_t data_sense);

  void
  encode_preprocessor_sample_resolution (uint8_t resolution);

  void
  encode_entropy_coder_resolution_range (uint8_t resolution);

  void
  encode_entropy_coder_cds_num (uint16_t num);

  void
  encode_extended_parameters_block_size (uint16_t size);

  void
  encode_extended_parameters_restricted_code_option (uint8_t option);

  void
  encode_extended_parameters_reference_sample_interval (uint8_t interval);

  uint16_t
  decode_grouping_data_length () const;

  uint8_t
  decode_compression_technique_id () const;

  uint8_t
  decode_reference_sample_interval () const;

  uint8_t
  decode_preprocessor_status () const;

  uint8_t
  decode_preprocessor_predictor_type () const;

  uint8_t
  decode_preprocessor_mapper_type () const;

  uint16_t
  decode_preprocessor_block_size () const;

  uint8_t
  decode_preprocessor_data_sense () const;

  uint8_t
  decode_preprocessor_sample_resolution () const;

  uint16_t
  decode_extended_parameters_block_size () const;

  uint8_t
  decode_extended_parameters_restricted_code_option () const;

  uint8_t
  decode_extended_parameters_reference_sample_interval () const;

private:
  uint16_t d_grouping_data_length;
  uint8_t d_compression_tech_id;
  uint8_t d_reference_sample_interval;
  uint8_t d_preprocessor_status;
  uint8_t d_predictor_type;
  uint8_t d_mapper_type;
  uint8_t d_sample_resolution;
  uint8_t d_data_sense;
  uint8_t d_restricted_codes;
  uint16_t d_block_size;
  uint16_t d_cds_per_packet;

  source_data_fixed_t d_source_data_fixed;
  source_data_variable_t d_source_data_variable;

  void
  encode_preprocessor ();

  void
  encode_entropy_coder ();

  void
  encode_instrument_configuration ();

  void
  encode_extended_parameters ();

};

#endif /* COMPRESSION_IDENTIFICATION_PACKET_H_ */
