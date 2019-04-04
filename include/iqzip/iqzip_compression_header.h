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

#ifndef IQZIP_COMPRESSION_HEADER_H_
#define IQZIP_COMPRESSION_HEADER_H_

#include <stdint.h>
#include "iqzip/ccsds_packet_primary_header.h"
#include "iqzip/compression_identification_packet.h"

#define IQZIP_COMPRESSION_HDR_SIZE      2

typedef struct iqzip_compression_header_t
{
  uint8_t block_size :4;
  uint16_t rsi :11;
  uint8_t endianness :1;
} iqzip_compression_header_t;

class iqzip_compression_header
{

public:

  enum class ERROR_CODES
  {
    INIT_ERROR = -1
  };

  enum class BLOCK_SIZE
  {
    SAMPLES_128 = 0x000,
    SAMPLES_256,
    SAMPLES_512,
    SAMPLES_1024,
    SAMPLES_2048,
    SAMPLES_4096,
    SAMPLES_8192,
    SAMPLES_16384,
    SAMPLES_32768,
    SAMPLES_65535
  };

  enum class ENDIANNESS
  {
    BIG = 0x0, LITTLE
  };

  iqzip_compression_header (uint8_t version, uint8_t type, uint8_t sec_hdr_flag,
                            uint16_t apid, uint8_t sequence_flags,
                            uint16_t packet_sequence_count,
                            uint16_t packet_data_length,
                            uint16_t grouping_data_length,
                            uint8_t compression_tech_id,
                            uint8_t reference_sample_interval,
                            uint8_t preprocessor_status, uint8_t predictor_type,
                            uint8_t mapper_type, uint16_t block_size,
                            uint8_t data_sense, uint8_t sample_resolution,
                            uint16_t csd_per_packet, uint8_t restricted_codes,
                            uint8_t endianness);

  iqzip_compression_header ();

  virtual
  ~iqzip_compression_header ();

  void
  write_header_to_file (std::string path);

  size_t
  parse_header_from_file (std::string path);

  uint16_t
  decode_block_size ();

  uint8_t
  decode_version () const;

  uint8_t
  decode_type () const;

  uint8_t
  decode_secondary_header_flag () const;

  uint16_t
  decode_appplication_process_identifier () const;

  uint8_t
  decode_sequence_flags () const;

  uint16_t
  decode_packet_sequence_count () const;

  uint16_t
  decode_packet_data_length () const;

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

  void
  encode_version (uint8_t version);

  void
  encode_type (uint8_t type);

  void
  encode_secondary_header_flag (uint8_t flag);

  void
  encode_appplication_process_identifier (uint16_t size);

  void
  encode_sequence_flags (uint8_t flags);

  void
  encode_packet_sequence_count (uint16_t count);

  void
  encode_packet_data_length (uint16_t length);

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

  void
  encode_iqzip_header_block_size (size_t size);

  void
  encode_iqzip_header_endianness (uint8_t endianness);

  void
  encode_iqzip_header_reference_sample_interval (uint16_t interval);

  uint16_t
  decode_iqzip_header_block_size () const;

  uint16_t
  decode_iqzip_header_endianess () const;

  uint16_t
  decode_iqzip_header_reference_sample_interval () const;

private:
  iqzip_compression_header_t d_iqzip_header;
  ccsds_packet_primary_header *d_primary_header;
  compression_identification_packet *d_cip;

  uint16_t d_block_size;
  uint8_t d_rsi;
  uint8_t d_enable_preprocessing;
  uint8_t d_endianness;
  int8_t d_version;
  int8_t d_type;
  int8_t d_sec_hdr_flag;
  int8_t d_sequence_flags;
  uint8_t d_data_sense;
  uint8_t d_sample_resolution;
  uint8_t d_restricted_codes;

  int16_t d_apid;
  int16_t d_sequence_count;
  int16_t d_data_length;

  iqzip_compression_header_t&
  get_iqzip_compression_header ();

  void
  set_iqzip_compression_header (iqzip_compression_header_t hdr);

  void
  encode ();
};

#endif /* IQZIP_COMPRESSION_HEADER_H_ */
