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

#include "iqzip/ccsds_packet_primary_header.h"
#include <stdexcept>
#include <cstring>

namespace iqzip
{

  namespace compression
  {

    ccsds_packet_primary_header::ccsds_packet_primary_header (
        uint8_t version, uint8_t type, uint8_t sec_hdr_flag, uint16_t apid,
        uint8_t sequence_flags, uint16_t packet_sequence_count,
        uint16_t packet_data_length) :
            d_version (version),
            d_type (type),
            d_sec_hdr_flag (sec_hdr_flag),
            d_apid (apid),
            d_sequence_flags (sec_hdr_flag),
            d_packet_sequence_count (packet_sequence_count),
            d_packet_data_length (packet_data_length)
    {
      encode ();
    }

    ccsds_packet_primary_header::ccsds_packet_primary_header () :
            d_version (0),
            d_type (0),
            d_sec_hdr_flag (0),
            d_apid (0),
            d_sequence_flags (0),
            d_packet_sequence_count (0),
            d_packet_data_length (0)
    {
    }

    ccsds_packet_primary_header::~ccsds_packet_primary_header ()
    {
      // TODO Auto-generated destructor stub
    }

    void
    ccsds_packet_primary_header::encode ()
    {
      encode_version (d_version);
      encode_type (d_type);
      encode_sequence_flags (d_sequence_flags);
      encode_secondary_header_flag (d_sec_hdr_flag);
      encode_packet_sequence_count (d_packet_sequence_count);
      encode_application_process_identifier (d_apid);
      encode_packet_data_length (d_packet_data_length);

    }

    ccsds_packet_primary_header::packet_primary_header_t&
    ccsds_packet_primary_header::get_primary_header ()
    {
      return d_primary_header;
    }

    void
    ccsds_packet_primary_header::set_primary_header (
        packet_primary_header_t hdr)
    {
      d_primary_header = hdr;
    }

    uint8_t
    ccsds_packet_primary_header::decode_version () const
    {
      return d_primary_header.version;
    }

    uint8_t
    ccsds_packet_primary_header::decode_type () const
    {
      return d_primary_header.type;
    }

    uint8_t
    ccsds_packet_primary_header::decode_secondary_header_flag () const
    {
      return d_primary_header.sec_hdr_flag;
    }

    uint16_t
    ccsds_packet_primary_header::decode_application_process_identifier () const
    {
      return d_primary_header.apid;
    }

    uint8_t
    ccsds_packet_primary_header::decode_sequence_flags () const
    {
      return d_primary_header.sequence_flags;
    }

    uint16_t
    ccsds_packet_primary_header::decode_packet_sequence_count () const
    {
      return d_primary_header.sequence_count;
    }

    uint16_t
    ccsds_packet_primary_header::decode_packet_data_length () const
    {
      return d_primary_header.data_length;
    }

    void
    ccsds_packet_primary_header::encode_application_process_identifier (
        uint16_t apid)
    {
      d_primary_header.apid = apid;
    }

    void
    ccsds_packet_primary_header::encode_packet_data_length (
        uint16_t data_length)
    {
      d_primary_header.data_length = data_length;
    }

    void
    ccsds_packet_primary_header::encode_secondary_header_flag (uint8_t flag)
    {
      d_primary_header.sec_hdr_flag = flag;
    }

    void
    ccsds_packet_primary_header::encode_packet_sequence_count (
        uint16_t sequence_count)
    {
      d_primary_header.sequence_count = (sequence_count - 1) % 16384;
    }

    void
    ccsds_packet_primary_header::encode_sequence_flags (uint8_t sequence_flags)
    {
      d_primary_header.sequence_flags = sequence_flags;
    }

    void
    ccsds_packet_primary_header::encode_type (uint8_t type)
    {
      d_primary_header.type = type;
    }

    void
    ccsds_packet_primary_header::encode_version (uint8_t version)
    {
      d_primary_header.version = version;
    }

  }
}
