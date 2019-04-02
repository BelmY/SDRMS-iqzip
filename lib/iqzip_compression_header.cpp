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

#include "iqzip/iqzip_compression_header.h"

#include <stdexcept>
#include <cstring>
#include <iostream>

iqzip_compression_header::iqzip_compression_header (
    uint16_t block_size, uint8_t rsi, uint8_t enable_preprocessing,
    uint8_t endianness, uint8_t version, uint8_t type, uint8_t sec_hdr_flag,
    uint16_t apid, uint8_t sequence_flags, uint16_t sequence_count,
    uint_least16_t data_length, uint8_t sample_resolution, uint8_t data_sense,
    uint8_t restricted_codes) :
        d_block_size (block_size),
        d_rsi (rsi),
        d_enable_preprocessing (enable_preprocessing),
        d_endianness (endianness),
        d_version (version),
        d_type (type),
        d_sec_hdr_flag (sec_hdr_flag),
        d_apid (apid),
        d_sequence_flags (sequence_flags),
        d_sequence_count (sequence_count),
        d_data_length (data_length),
        d_data_sense (data_sense),
        d_sample_resolution (sample_resolution),
        d_restricted_codes (restricted_codes)
{
  encode_iqzip_header_block_size (block_size);
  d_iqzip_header.rsi = rsi;
  d_iqzip_header.endianness = endianness;

  d_primary_header = new ccsds_packet_primary_header (version, type,
                                                      sec_hdr_flag, apid,
                                                      sequence_flags,
                                                      sequence_count,
                                                      data_length);

  d_cip = new compression_identification_packet (enable_preprocessing,
                                                 block_size, sample_resolution,
                                                 rsi, data_sense,
                                                 restricted_codes);

}

iqzip_compression_header::~iqzip_compression_header ()
{
}

void
iqzip_compression_header::write_header_to_file (std::string path)
{
  FILE* f = fopen (path.c_str (), "wb");
  fwrite (&(d_primary_header->get_primary_header ()),
          sizeof(packet_primary_header_t), 1, f);

  //TODO: This functionality should be enclosed to compression_identification class
  fwrite (&(d_cip->get_source_data_field_fixed ()),
          sizeof(source_data_field_fixed_t), 1, f);
  fwrite (&(d_cip->get_source_configuration ().preprocessor),
          sizeof(preprocessor_t), 1, f);
  fwrite (&(d_cip->get_source_configuration ().entropy_coder),
          sizeof(entropy_coder_t), 1, f);
  //TODO: Add a parameter that indicates the use of Instrument Configuration subfield
  if (d_block_size > 16 || d_rsi > 255 || d_restricted_codes) {
    fwrite (&(d_cip->get_source_configuration ().extended_parameters),
            sizeof(extended_parameters_t), 1, f);
  }
  if (d_block_size > 64) {
    fwrite (&d_iqzip_header, sizeof(iqzip_compression_header_t), 1, f);
  }
  fclose (f);
}

size_t
iqzip_compression_header::parse_header_from_file (std::string path)
{
  size_t result;
  size_t cip_hdr_size;
  size_t max_header_size;
  uint8_t *buffer;
  FILE* f;
  source_configuration_t* hdr_src_cnf;
  source_data_field_fixed_t* hdr_src_cnf_fixed;
  iqzip_compression_header_t* iqzip_hdr;

  cip_hdr_size = CCSDS_PRIMARY_HEADER_SIZE + SOURCE_DATA_FIELD_FIXED_SIZE
      + PREPROCESSOR_SUBFIELD_SIZE + ENTROPY_CODER_SUBFIELD_SIZE;
  max_header_size = MAX_CIP_HEADER_SIZE_BYTES + IQZIP_COMPRESSION_HDR_SIZE;

  buffer = (uint8_t*) malloc (sizeof(uint8_t) * max_header_size);
  f = fopen (path.c_str (), "rb");
  result = fread (buffer, 1, max_header_size, f);
  if (result != max_header_size) {
    throw std::runtime_error ("File reading error");
    exit (3);
  }
  fclose (f);

  hdr_src_cnf = (source_configuration_t*) (&buffer[CCSDS_PRIMARY_HEADER_SIZE
      + SOURCE_DATA_FIELD_FIXED_SIZE]);
  hdr_src_cnf_fixed =
      (source_data_field_fixed_t*) (&buffer[CCSDS_PRIMARY_HEADER_SIZE]);

  if (!(d_block_size = d_cip->decode_preprocessor_block_size (
      hdr_src_cnf->preprocessor.block_size))) {
    d_block_size = d_cip->decode_extended_parameters_block_size (
        hdr_src_cnf->extended_parameters.block_size);
    if (!d_block_size) {
      iqzip_hdr =
          (iqzip_compression_header_t*) (&buffer[CCSDS_PRIMARY_HEADER_SIZE
              + SOURCE_DATA_FIELD_FIXED_SIZE + 6]);
      d_block_size = (int) decode_iqzip_header_block_size(iqzip_hdr->block_size);
    }
  }
  d_data_sense = (uint8_t) hdr_src_cnf->preprocessor.data_sense;
  d_sample_resolution = (uint8_t) hdr_src_cnf->preprocessor.sample_resolution;
  d_enable_preprocessing = (uint8_t) hdr_src_cnf->preprocessor.status;
  //TODO: Add decoder for RSI similar to the encoder
  d_rsi = (uint8_t) hdr_src_cnf_fixed->reference_sample_interval + 1;
  d_restricted_codes =
      (uint8_t) hdr_src_cnf->extended_parameters.restricted_code_options;
  if (d_block_size > 16 || d_rsi > 256) {
    cip_hdr_size += EXTENDED_PARAMETERS_SUBFIELD_SIZE;
    if (d_block_size > 64) {
      cip_hdr_size += IQZIP_COMPRESSION_HDR_SIZE;
    }
  }

  /* FIXME: Take into consideration the CCSDS secondary header and the
   * Instrument Configuration subfield */
  return cip_hdr_size;

}

iqzip_compression_header_t&
iqzip_compression_header::get_iqzip_compression_header ()
{
  return d_iqzip_header;
}

void
iqzip_compression_header::encode_iqzip_header_block_size (uint16_t size)
{
  if (size > 65535) {
    std::runtime_error ("Invalid IQzip header block size");
  }
  else if (size == 128) {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_128);
  }
  else if (size == 256) {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_256);
  }
  else if (size == 512) {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_512);
  }
  else if (size == 1024) {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_1024);
  }
  else if (size == 2048) {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_2048);
  }
  else if (size == 4096) {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_4096);
  }
  else if (size == 8192) {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_8192);
  }
  else if (size == 16384) {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_16384);
  }
  else if (size == 32768) {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_32768);
  }
  else {
    d_iqzip_header.block_size = (uint8_t) (BLOCK_SIZE::SAMPLES_65535);
  }
}

void
iqzip_compression_header::encode_iqzip_header_endianness (uint8_t endianness)
{
  switch (endianness)
    {
    case 1:
      d_iqzip_header.endianness = (uint8_t) (ENDIANNESS::LITTLE);
      break;
    case 0:
      d_iqzip_header.endianness = (uint8_t) (ENDIANNESS::BIG);
      break;
    default:
      std::runtime_error ("Invalid IQzip header endianness");
    }
}

uint16_t
iqzip_compression_header::decode_iqzip_header_block_size (uint8_t code) const
{
  switch (code)
    {
    case (uint8_t) (BLOCK_SIZE::SAMPLES_128):
      return 128;
    case (uint8_t) (BLOCK_SIZE::SAMPLES_256):
      return 256;
    case (uint8_t) (BLOCK_SIZE::SAMPLES_512):
      return 512;
    case (uint8_t) (BLOCK_SIZE::SAMPLES_1024):
      return 1024;
    case (uint8_t) (BLOCK_SIZE::SAMPLES_2048):
      return 2048;
    case (uint8_t) (BLOCK_SIZE::SAMPLES_4096):
      return 4096;
    case (uint8_t) (BLOCK_SIZE::SAMPLES_8192):
      return 8192;
    case (uint8_t) (BLOCK_SIZE::SAMPLES_16384):
      return 16384;
    case (uint8_t) (BLOCK_SIZE::SAMPLES_32768):
      return 32768;
    case (uint8_t) (BLOCK_SIZE::SAMPLES_65535):
      return 65535;
    default:
      std::runtime_error ("Invalid IQzip header block size code");
    }
  return 0;
}

uint16_t
iqzip_compression_header::decode_iqzip_header_endianess () const
{
  uint8_t code = d_iqzip_header.endianness;
  switch (code)
    {
    case (uint8_t) (ENDIANNESS::BIG):
      return 0;
    case (uint8_t) (ENDIANNESS::LITTLE):
      return 1;
    default:
      std::runtime_error ("Invalid IQzip header endianness code");
    }
  return 0;
}

void
iqzip_compression_header::encode_iqzip_header_reference_sample_interval (
    uint16_t interval)
{
  d_iqzip_header.rsi = (interval - 1) % 2048;
}

uint8_t
iqzip_compression_header::get_block_size_field () const
{
  return d_iqzip_header.block_size;
}

uint8_t
iqzip_compression_header::get_reference_sample_interval_field () const
{
  return d_iqzip_header.rsi;
}

uint8_t
iqzip_compression_header::get_endianness_field () const
{
  return d_iqzip_header.endianness;
}

uint16_t
iqzip_compression_header::get_block_size () const
{
  return d_block_size;
}

uint16_t
iqzip_compression_header::get_reference_sample_interval () const
{
  return d_rsi;
}

void
iqzip_compression_header::set_block_size (uint16_t block_size)
{
  d_block_size = block_size;
}

uint8_t
iqzip_compression_header::get_data_sense () const
{
  return d_data_sense;
}

void
iqzip_compression_header::set_data_sense (uint8_t data_sense)
{
  d_data_sense = data_sense;
}

uint8_t
iqzip_compression_header::get_enable_preprocessing () const
{
  return d_enable_preprocessing;
}

void
iqzip_compression_header::set_enable_preprocessing (
    uint8_t enable_preprocessing)
{
  d_enable_preprocessing = enable_preprocessing;
}

uint8_t
iqzip_compression_header::get_restricted_codes () const
{
  return d_restricted_codes;
}

void
iqzip_compression_header::set_restricted_codes (uint8_t restricted_codes)
{
  d_restricted_codes = restricted_codes;
}

uint8_t
iqzip_compression_header::get_sample_resolution () const
{
  return d_sample_resolution;
}

void
iqzip_compression_header::set_sample_resolution (uint8_t sample_resolution)
{
  d_sample_resolution = sample_resolution;
}

void
iqzip_compression_header::set_reference_sample_interval (uint8_t interval)
{
  d_rsi = interval;
}
