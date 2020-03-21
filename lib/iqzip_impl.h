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

#ifndef IQZIP_IMPL_H
#define IQZIP_IMPL_H

#include <cmath>
#include <fstream>

#include <libaec.h>
#include <iqzip/iqzip_compression_header.h>

namespace iqzip {

class iqzip_impl {

protected:
    uint32_t CHUNK = 10485760;
    compression::header::iqzip_compression_header d_ccsds_cip_hdr;
    aec_stream d_strm;

    std::ofstream output_stream;
    std::ifstream input_stream;

    uint8_t d_version;
    uint8_t d_type;
    uint8_t d_sec_hdr_flag;
    uint16_t d_apid;
    uint8_t d_sequence_flags;
    uint16_t d_packet_sequence_count;
    uint16_t d_packet_data_length;
    uint16_t d_grouping_data_length;
    uint8_t d_compression_tech_id;
    uint8_t d_reference_sample_interval;
    uint8_t d_preprocessor_status;
    uint8_t d_predictor_type;
    uint8_t d_mapper_type;
    uint16_t d_block_size;
    uint8_t d_data_sense;
    uint8_t d_sample_resolution;
    uint16_t d_cds_per_packet;
    uint8_t d_restricted_codes;
    uint8_t d_endianness;

    /*!
     * Default constructor
     */
    iqzip_impl();

    /*!
     * Constructor with arguments. All necessary variables for compression are
     *  initialized. Called from super classes.
     * @param version CIP packet version.
     * @param type CIP type version.
     * @param sec_hdr_flag CIP secondary header flag.
     * @param apid CIP application ID.
     * @param sequence_flags CIP sequence flags.
     * @param packet_sequence_count CIP sequence count.
     * @param packet_data_length CIP packet data length.
     * @param grouping_data_length CIP grouping data length.
     * @param compression_tech_id CIP compression technique ID.
     * @param reference_sample_interval CIP reference sample interval.
     * @param preprocessor_status Secondary header preprocessor status. 0 if preprocessor absent.
     * @param predictor_type Secondary header predictor type.
     * @param mapper_type Secondary header mapper type.
     * @param block_size Secondary header block size (samples).
     * @param data_sense Secondary header data sense.
     * @param sample_resolution Secondary header sample resolution (bits).
     * @param cds_per_packet Secondary header number of CDSes per packet.
     * @param restricted_codes Secondary header restricted codes.
     * @param endianness Endianness of samples.
     */
    iqzip_impl(uint8_t version, uint8_t type, uint8_t sec_hdr_flag, uint16_t apid,
               uint8_t sequence_flags, uint16_t packet_sequence_count,
               uint16_t packet_data_length, uint16_t grouping_data_length,
               uint8_t compression_tech_id, uint8_t reference_sample_interval,
               uint8_t preprocessor_status, uint8_t predictor_type,
               uint8_t mapper_type, uint16_t block_size, uint8_t data_sense,
               uint8_t sample_resolution, uint16_t cds_per_packet,
               uint8_t restricted_codes, uint8_t endianness);

    /*!
     * Default destructor
     */
    virtual ~iqzip_impl();

    /*!
     * Initializes the aec stream from the class members. Used in init*
     * functions.
     */
    void init_aec_stream(void);

    /*!
     * Virtual function to print error messages from super classes.
     * @param the value of the error
     */
    void print_error(int status);

};

} // namespace iqzip

#endif /* IQZIP_IMPL_H */
