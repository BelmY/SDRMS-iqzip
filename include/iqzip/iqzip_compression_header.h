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

typedef struct
{
  uint8_t block_size :4;
  uint16_t rsi :11;
  uint8_t endianness :1;
} iqzip_compression_header_t;

class iqzip_compression_header
{

public:

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

  iqzip_compression_header (uint16_t block_size, uint8_t rsi,
                            uint8_t enable_preprocessing, uint8_t endianness,
                            uint8_t version, uint8_t type, uint8_t sec_hdr_flag,
                            uint16_t apid, uint8_t sequence_flags,
                            uint16_t sequence_count, uint_least16_t data_length,
                            uint8_t sample_resolution, uint8_t data_sense,
                            uint8_t restricted_codes);

  virtual
  ~iqzip_compression_header ();

  void
  encode_iqzip_header_block_size (uint16_t size);

  void
  encode_iqzip_header_endianness (uint8_t endianness);

  void
  encode_iqzip_header_reference_sample_interval (uint16_t interval);

  uint16_t
  decode_iqzip_header_block_size (uint8_t code) const;

  uint16_t
  decode_iqzip_header_endianess () const;

  uint8_t
  get_block_size_field () const;

  uint8_t
  get_reference_sample_interval_field () const;

  uint8_t
  get_endianness_field () const;

  iqzip_compression_header_t&
  get_iqzip_compression_header ();

  void
  write_header_to_file (std::string path);

  size_t
  parse_header_from_file (std::string path);

  uint16_t
  get_reference_sample_interval () const;

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

  void
  set_reference_sample_interval (uint8_t interval);

private:
  iqzip_compression_header_t d_iqzip_header;
  ccsds_packet_primary_header *d_primary_header;
  compression_identification_packet *d_cip;

  uint16_t d_block_size;
  uint8_t d_rsi;
  uint8_t d_enable_preprocessing;
  uint8_t d_endianness;
  uint8_t d_version;
  uint8_t d_type;
  uint8_t d_sec_hdr_flag;
  uint8_t d_sequence_flags;
  uint8_t d_data_sense;
  uint8_t d_sample_resolution;
  uint8_t d_restricted_codes;

  uint16_t d_apid;
  uint16_t d_sequence_count;
  uint16_t d_data_length;
};

#endif /* IQZIP_COMPRESSION_HEADER_H_ */
