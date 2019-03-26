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

namespace iqzip
{

  ccsds_packet_primary_header::ccsds_packet_primary_header (
      uint8_t version, uint8_t type, uint8_t sec_hdr_flag, uint16_t apid,
      uint8_t sequence_flags, uint16_t sequence_count, uint16_t data_length)
  {
    d_primary_header.version = version;
    d_primary_header.type = type;
    d_primary_header.sec_hdr_flag = sec_hdr_flag;
    d_primary_header.apid = apid;
    d_primary_header.sequence_flags = sequence_flags;
    d_primary_header.sequence_count = sequence_count;
    d_primary_header.data_length = data_length;

  }

  ccsds_packet_primary_header::~ccsds_packet_primary_header ()
  {
    // TODO Auto-generated destructor stub
  }

  packet_primary_header_t&
  ccsds_packet_primary_header::get_primary_header ()
  {
    return d_primary_header;
  }

  uint8_t
  ccsds_packet_primary_header::get_version () const
  {
    return d_primary_header.version;
  }

  uint8_t
  ccsds_packet_primary_header::get_type () const
  {
    return d_primary_header.type;
  }

  uint8_t
  ccsds_packet_primary_header::get_secondary_header_flag () const
  {
    return d_primary_header.sec_hdr_flag;
  }

  uint16_t
  ccsds_packet_primary_header::get_apid () const
  {
    return d_primary_header.apid;
  }

  uint8_t
  ccsds_packet_primary_header::get_sequence_flags () const
  {
    return d_primary_header.sequence_flags;
  }

  uint16_t
  ccsds_packet_primary_header::get_sequence_count () const
  {
    return d_primary_header.sequence_count;
  }

  uint16_t
  ccsds_packet_primary_header::get_data_length () const
  {
    return d_primary_header.data_length;
  }

  void
  ccsds_packet_primary_header::set_apid (uint16_t apid)
  {
    d_primary_header.apid = apid;
  }

  void
  ccsds_packet_primary_header::set_data_length (uint16_t data_length)
  {
    d_primary_header.data_length = data_length;
  }

  void
  ccsds_packet_primary_header::set_sec_hdr_flag (uint8_t flag)
  {
    d_primary_header.sec_hdr_flag = flag;
  }

  void
  ccsds_packet_primary_header::set_sequence_count (uint16_t sequence_count)
  {
    d_primary_header.sequence_count = (sequence_count - 1) % 16384;
  }

  void
  ccsds_packet_primary_header::set_sequence_flags (uint8_t sequence_flags)
  {
    d_primary_header.sequence_flags = sequence_flags;
  }

  void
  ccsds_packet_primary_header::set_type (uint8_t type)
  {
    d_primary_header.type = type;
  }

  void
  ccsds_packet_primary_header::set_version (uint8_t version)
  {
    d_primary_header.version = version;
  }

} /* namespace iqzip */
