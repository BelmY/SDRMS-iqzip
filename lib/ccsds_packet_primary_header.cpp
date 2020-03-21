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

#include <iqzip/ccsds_packet_primary_header.h>
#include <stdexcept>
#include <cstring>
#include <iostream>

namespace iqzip {

namespace compression {

namespace header {

ccsds_packet_primary_header::ccsds_packet_primary_header(
    uint8_t version, uint8_t type, uint8_t sec_hdr_flag, uint16_t apid,
    uint8_t sequence_flags, uint16_t packet_sequence_count,
    uint16_t packet_data_length) :
    d_version(version),
    d_type(type),
    d_sec_hdr_flag(sec_hdr_flag),
    d_apid(apid),
    d_sequence_flags(sequence_flags),
    d_packet_sequence_count(packet_sequence_count),
    d_packet_data_length(packet_data_length)
{
    encode();
}

ccsds_packet_primary_header::ccsds_packet_primary_header() :
    d_version(0),
    d_type(0),
    d_sec_hdr_flag(0),
    d_apid(0),
    d_sequence_flags(0),
    d_packet_sequence_count(0),
    d_packet_data_length(0)
{
}

ccsds_packet_primary_header::~ccsds_packet_primary_header()
{
    // TODO Auto-generated destructor stub
}

void
ccsds_packet_primary_header::encode()
{
    encode_type(d_type);
    encode_version(d_version);
    encode_sequence_flags(d_sequence_flags);
    encode_secondary_header_flag(d_sec_hdr_flag);
    encode_packet_sequence_count(d_packet_sequence_count);
    encode_application_process_identifier(d_apid);
    encode_packet_data_length(d_packet_data_length);

}

ccsds_packet_primary_header::packet_primary_header_t &
ccsds_packet_primary_header::get_primary_header()
{
    return d_primary_header;
}

void
ccsds_packet_primary_header::set_primary_header(
    packet_primary_header_t *hdr)
{
    memcpy(&d_primary_header, hdr, sizeof(packet_primary_header_t));
}

uint8_t
ccsds_packet_primary_header::decode_version() const
{
    return (d_primary_header[0] & PACKET_VERSION_MASK) >> 5;
}

uint8_t
ccsds_packet_primary_header::decode_type() const
{
    return (d_primary_header[0] & PACKET_TYPE_MASK) >> 4;
}

uint8_t
ccsds_packet_primary_header::decode_secondary_header_flag() const
{
    return (d_primary_header[0] & SECONDARY_HEADER_FLAG_MASK) >> 3;
}

uint16_t
ccsds_packet_primary_header::decode_application_process_identifier() const
{
    uint16_t apid = 0;
    apid |= d_primary_header[1];
    apid |= ((d_primary_header[0] & (APPLICATION_PROCESS_ID_MASK >> 8)) << 8);
    return apid;
}

uint8_t
ccsds_packet_primary_header::decode_sequence_flags() const
{
    return (d_primary_header[2] & SEQUENCE_FLAGS_MASK) >> 6;
}

uint16_t
ccsds_packet_primary_header::decode_packet_sequence_count() const
{
    uint16_t seq = 0;
    seq |= d_primary_header[3];
    seq |= ((d_primary_header[2] & 0x3F) << 8);
    return seq;
}

uint16_t
ccsds_packet_primary_header::decode_packet_data_length() const
{
    uint16_t len = 0;
    len |= d_primary_header[5];
    len |= d_primary_header[4] << 8;
    return len;
}

void
ccsds_packet_primary_header::encode_application_process_identifier(
    uint16_t apid)
{
    /**
     * TODO: Explain what's going on here
     */
    d_primary_header[0] |= ((apid >> 8) & (APPLICATION_PROCESS_ID_MASK >> 8));
    d_primary_header[1] = (uint8_t)apid;
}

void
ccsds_packet_primary_header::encode_packet_data_length(
    uint16_t data_length)
{
    /**
     * TODO: Explain what's going on here
     */
    d_primary_header[4] |= (data_length >> 8);
    d_primary_header[5] = data_length;
}

void
ccsds_packet_primary_header::encode_secondary_header_flag(uint8_t flag)
{
    d_primary_header[0] |= flag << 3;
}

void
ccsds_packet_primary_header::encode_packet_sequence_count(
    uint16_t sequence_count)
{
    /**
     * TODO: Explain what's going on here
     */
    sequence_count = sequence_count % 16384;
    d_primary_header[2] |= (sequence_count >> 8) & 0x3f;
    d_primary_header[3] = sequence_count;
}

void
ccsds_packet_primary_header::encode_sequence_flags(uint8_t sequence_flags)
{
    d_primary_header[2] |= (sequence_flags & 0xc0) << 6;
}

void
ccsds_packet_primary_header::encode_type(uint8_t type)
{
    d_primary_header[0] |= (type & 0x1) << 4;
}

void
ccsds_packet_primary_header::encode_version(uint8_t version)
{
    d_primary_header[0] |= (version & 0x7) << 5;
}

} // namespace header
} // namespace compression
} // namespace iqzip
