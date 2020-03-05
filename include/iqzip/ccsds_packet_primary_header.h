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

#define CCSDS_PRIMARY_HEADER_SIZE               6

/**
 * CCSDS Packet Primary Header field masks
 */

#define PACKET_VERSION_MASK                     0xe0
#define PACKET_TYPE_MASK                        0x10
#define SECONDARY_HEADER_FLAG_MASK              0x8
#define APPLICATION_PROCESS_ID_MASK             0x7ff
#define SEQUENCE_FLAGS_MASK                     0xc0
#define PACKET_SEQUENCE_COUNT_MASK              0x3fff
#define PACKET_LENGTH_MASK                      0xffff


namespace iqzip {

namespace compression {

/*!
 *
 * \ingroup compression_header
 *
 * The Packet Primary Header is the leading mandatory segment of the Space Packet,
 * and its structure shall conform to the CCSDS Space Packet Protocol Blue Book.
 *
 * This class defines the packet_primary_header_t type that fully represents the
 * Packet Primary Header and all of its fields. The appropriate setter and getter
 * functions to access the header fields are provided.
 *
 *
 *          +-------------------------------------------------------------------------+
 *          |                          PACKET PRIMARY HEADER                          |
 *  +-------|---------+-----------------------------+------------------------+--------+
 *  |       |         |                             |                        |        |
 *  |       |         |    PACKET IDENTIFICATION    |         PACKET         |        |
 *  |       |         |                             |        SEQUENCE        |        |
 *  |       |         |                             |         CONTROL        |        |
 *  |       |  PACKET +--------+------+-------------+----------+-------------+ PACKET |
 *  |       | VERSION |        |      |             |          |             |  DATA  |
 *  | FIELD |  NUMBER |        | SEC. | APPLICATION |          |    PACKET   | LENGTH |
 *  |       |         | PACKET | HDR. |   PROCESS   | SEQUENCE |   SEQUENCE  |        |
 *  |       |         |  TYPE  | FLAG |  IDENTIFIER |   FLAGS  |   COUNT OR  |        |
 *  |       |         |        |      |             |          | PACKET NAME |        |
 *  +-------+---------+--------+------+-------------+----------+-------------+--------+
 *  | BIT   |   0-2   |    3   |   4  |     5-15    |   16-17  |    18-31    | 32-47  |
 *  +-------+---------+--------+------+-------------+----------+-------------+--------+
 *  | MASK  |   0xE0  |  0x10  |  0x8 |    0x7FF    |   0xC0   |     0x3F    | 0xFF   |
 *  +-------+---------+--------+------+-------------+----------+-------------+--------+
 *
 */

class ccsds_packet_primary_header {

public:

    /*!
     * The CCSDS packet primary header
     */

    typedef uint8_t packet_primary_header_t[CCSDS_PRIMARY_HEADER_SIZE];

    /*!
     * The default version value as defined by the CCSDS Space Packet Protocol Blue Book.
     */
    enum class PACKET_VERSION {
        CCSDS_PACKET_VERSION_1 = 0x0,
    };

    /*!
     * The default type values as defined by the CCSDS Space Packet Protocol Blue Book.
     */
    enum class PACKET_TYPE {
        CCSDS_TELEMETRY = 0x0, CCSDS_TELECOMMAND = 0x1,
    };

    /*!
     * The default secondary header flag value as defined by the CCSDS Space Packet
     * Protocol Blue Book.
     */
    enum class PACKET_SECONDARY_HEADER_FLAG {
        IDLE_PACKET = 0x0, SEC_HDR_NOT_PRESENT = 0x0, SEC_HDR_PRESENT = 0x1,
    };

    /*!
     * The default application process identifier value as defined by the CCSDS Space Packet
     * Protocol Blue Book.
     */
    enum class PACKET_APPLICATION_PROCESS_IDENTIFIER {
        IDLE_PACKET = 0x7FF,
    };

    /*!
     * The default packet sequence flags as defined by the CCSDS Space Packet
     * Protocol Blue Book.
     */
    enum class PACKET_SEQUENCE_FLAGS {
        CONTINUATION_SEGMENT = 0x0,
        FIRST_SEGMENT = 0x1,
        LAST_SEGMENT = 0x2,
        UNSEGMENTED = 0x3
    };

    ccsds_packet_primary_header(uint8_t version, uint8_t type,
                                uint8_t sec_hdr_flag, uint16_t apid,
                                uint8_t sequence_flags,
                                uint16_t packet_sequence_count,
                                uint16_t packet_data_length);

    ccsds_packet_primary_header();

    virtual
    ~ccsds_packet_primary_header();

    /*!
     * Get the inner bit-field that represents the packet primary header
     * \return a packet_primary_header_t representing the packet primary header
     */
    packet_primary_header_t &
    get_primary_header();

    void
    set_primary_header(packet_primary_header_t *hdr);

    /*!
     * Get the decoded value of the version subfield.
     * \return a uint8_t representing the header version.
     */
    uint8_t
    decode_version() const;

    /*!
     * Get the decoded value of the type subfield.
     * \return a uint8_t representing the header type.
     */
    uint8_t
    decode_type() const;

    /*!
     * Get the decoded value of the secondary header flag subfield.
     * \return a uint8_t representing the secondary header flag.
     */
    uint8_t
    decode_secondary_header_flag() const;

    /*!
     * Get the decoded value of the application process identifier subfield.
     * \return a uint16_t representing the application process identifier.
     */
    uint16_t
    decode_application_process_identifier() const;

    /*!
     * Get the decoded value of the sequence flags subfield.
     * \return a uint8_t representing the sequence flags.
     */
    uint8_t
    decode_sequence_flags() const;

    /*!
     * Get the decoded value of the packet sequence count subfield.
     * \return a uint16_t representing the packet sequence count.
     */
    uint16_t
    decode_packet_sequence_count() const;

    /*!
     * Get the decoded value of the packet data length subfield.
     * \return a uint16_t representing the packet data length.
     */
    uint16_t
    decode_packet_data_length() const;

    /*!
     * Encode the application process identifier into the appropriate header subfield.
     * \param apid The application process identifier
     */
    void
    encode_application_process_identifier(uint16_t apid);

    /*!
     * Encode the packet data length into the appropriate header subfield.
     * \param data_length The packet data length
     */
    void
    encode_packet_data_length(uint16_t data_length);

    /*!
     * Encode the secondary header flag into the appropriate header subfield.
     * \param flag The secondary header flag
     */
    void
    encode_secondary_header_flag(uint8_t flag);

    /*!
     * Encode the packet sequence count into the appropriate header subfield.
     * \param sequence_count The packet sequence count
     */
    void
    encode_packet_sequence_count(uint16_t sequence_count);

    /*!
     * Encode the sequence flags into the appropriate header subfield.
     * \param sequence_flags The sequence flags
     */
    void
    encode_sequence_flags(uint8_t sequence_flags);

    /*!
     * Encode the type into the appropriate header subfield.
     * \param type The type of the packet
     */
    void
    encode_type(uint8_t type);

    /*!
     * Encode the version into the appropriate header subfield.
     * \param version The version of the packet
     */
    void
    encode_version(uint8_t version);

private:
    void
    encode();

    uint8_t d_version;
    uint8_t d_type;
    uint8_t d_sec_hdr_flag;
    uint8_t d_sequence_flags;
    uint16_t d_apid;
    uint16_t d_packet_sequence_count;
    uint16_t d_packet_data_length;
    packet_primary_header_t d_primary_header;

};
}
}

#endif /* CCSDS_PACKET_PRIMARY_HEADER_H_ */
