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

#ifndef CCSDS_PACKET_PRIMARY_HEADER_H_
#define CCSDS_PACKET_PRIMARY_HEADER_H_

#include <stdint.h>

typedef struct
{
  uint8_t version :3;
  uint8_t type :1;
  uint8_t sec_hdr_flag :1;
  uint16_t apid :11;
  uint8_t sequence_flags :2;
  uint16_t sequence_count :14;
  uint16_t data_length :16;
} packet_primary_header_t;

class ccsds_packet_primary_header
{

public:

  enum class PACKET_VERSION
  {
    CCSDS_PACKET_VERSION_1 = 0x0,
  };

  enum class PACKET_TYPE
  {
    CCSDS_TELECOMMAND = 0x0, CCSDS_TELEMETRY = 0x1,
  };

  enum class PACKET_SECONDARY_HEADER_FLAG
  {
    IDLE_PACKET = 0x0, SEC_HDR_NOT_PRESENT = 0x0, SEC_HDR_PRESENT = 0x1,
  };

  enum class PACKET_APPLICATION_PROCESS_IDENTIFIER
  {
    IDLE_PACKET = 0x0
  };

  enum class PACKET_SEQUENCE_FLAGS
  {
    CONTINUATION_SEGMENT = 0x0,
    FIRST_SEGMENT = 0x1,
    LAST_SEGMENT = 0x2,
    UNSEGMENTED = 0x3
  };

  ccsds_packet_primary_header (uint8_t version, uint8_t type,
                               uint8_t sec_hdr_flag, uint16_t apid,
                               uint8_t sequence_flags, uint16_t sequence_count,
                               uint16_t data_length);

  virtual
  ~ccsds_packet_primary_header ();

  uint8_t
  get_version () const;

  uint8_t
  get_type () const;

  uint8_t
  get_secondary_header_flag () const;

  uint16_t
  get_apid () const;

  uint8_t
  get_sequence_flags () const;

  uint16_t
  get_sequence_count () const;

  uint16_t
  get_data_length () const;

  void
  set_apid (uint16_t apid);

  void
  set_data_length (uint16_t data_length);

  void
  set_sec_hdr_flag (uint8_t flag);

  void
  set_sequence_count (uint16_t sequence_count);

  void
  set_sequence_flags (uint8_t sequence_flags);

  void
  set_type (uint8_t type);

  void
  set_version (uint8_t version);

  packet_primary_header_t&
  get_primary_header ();

private:
  packet_primary_header_t d_primary_header;

};

#endif /* CCSDS_PACKET_PRIMARY_HEADER_H_ */
