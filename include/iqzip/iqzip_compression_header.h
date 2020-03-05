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
#include <iqzip/ccsds_packet_primary_header.h>
#include <iqzip/compression_identification_packet.h>

#define IQZIP_COMPRESSION_HDR_SIZE      2

namespace iqzip {

namespace compression {

/*!
 *
 * \ingroup compression_header
 *
 * The IQzip compression header is an optional extension to the CSSDS Compression
 * Identification Packet that enables non-standard application specific Adaptive
 *  Entropy Coder configuration.
 *
 * More specifically, as the CCSDS Lossless Data Compression Blue Book describes,
 * specific parameters, such as the number of samples per block or the reference
 * sample interval, are bounded to a strict set of values. However, for some of
 * these parameters, the standard describes a reserved value that refers to Application
 * Specific parameterization. IQzip extension header exists to further describe
 * this information.
 *
 * This class wraps the standard Compression Identification Packet header and the
 * IQzip compression header extension. The IQzip compression header is automatically
 * appended, if necessary, after the end of the CIP header.
 */
class iqzip_compression_header {

public:

    /*!
     * A bit-field that defines the structure of the IQzip compression header.
     */
    typedef struct iqzip_compression_header_t {
        uint8_t block_size : 4;
        uint16_t rsi : 11;
        uint8_t endianness : 1;
    } iqzip_compression_header_t;

    /*!
     * The default block size value for the IQzip compression header
     */
    enum class BLOCK_SIZE {
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

    /*!
     * The default endianness value for the IQzip compression header
     */
    enum class ENDIANNESS {
        BIG = 0x0, LITTLE
    };

    iqzip_compression_header(uint8_t version, uint8_t type,
                             uint8_t sec_hdr_flag, uint16_t apid,
                             uint8_t sequence_flags,
                             uint16_t packet_sequence_count,
                             uint16_t packet_data_length,
                             uint16_t grouping_data_length,
                             uint8_t compression_tech_id,
                             uint8_t reference_sample_interval,
                             uint8_t preprocessor_status,
                             uint8_t predictor_type, uint8_t mapper_type,
                             uint16_t block_size, uint8_t data_sense,
                             uint8_t sample_resolution,
                             uint16_t csd_per_packet,
                             uint8_t restricted_codes, uint8_t endianness);

    iqzip_compression_header();

    virtual
    ~iqzip_compression_header();

    /*!
     * Write the IQzip compression header to the file in path. This function automatically
     * appends only the appropriate header segments to the file.
     * \param path The full path to the file
     */
    void
    write_header_to_file(std::string path);

    /*!
     * Parse the IQzip compression header from the file in path and populate the internal
     * bit-fields of the class that describe the header.
     * \param path The full path to the file
     * \return a size_t representing the length of the parsed header in bytes
     */
    size_t
    parse_header_from_file(std::string path);

    /*!
     * Get the appropriate block size by parsing the whole IQzip compression header.
     * For example, in the case that a block size of 64 samples is encoded in the
     * header, the appropriate value is stored in the Extended Parameters subfield.
     * Equivalently, for non-standard application specific block sizes that exceed
     * 64 samples, the desired value should be retrieved from the IQzip extension header.
     * \return a uint16_t representing the block size
     */
    uint16_t
    decode_block_size();

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
     * Get the decoded value of grouping data length subfield.
     * \return a uint16_t representing the grouping data length.
     */
    uint16_t
    decode_grouping_data_length() const;

    /*!
     * Get the decoded value of compression technique identification subfield.
     * \return a uint8_t representing the compression technique identification.
     */
    uint8_t
    decode_compression_technique_id() const;

    /*!
     * Get the decoded value of reference sample interval subfield.
     * \return a uint8_t representing the reference sample interval.
     */
    uint8_t
    decode_reference_sample_interval() const;

    /*!
     * Get the decoded value of preprocessor status subfield.
     * \return a uint8_t representing the preprocessor status.
     */
    uint8_t
    decode_preprocessor_status() const;

    /*!
     * Get the decoded value of preprocessor predictor type subfield.
     * \return a uint8_t representing the preprocessor predictor type.
     */
    uint8_t
    decode_preprocessor_predictor_type() const;

    /*!
     * Get the decoded value of preprocessor mapper type subfield.
     * \return a uint8_t representing the preprocessor mapper type.
     */
    uint8_t
    decode_preprocessor_mapper_type() const;

    /*!
     * Get the decoded value of preprocessor block size subfield.
     * \return a uint16_t representing the preprocessor block size.
     */
    uint16_t
    decode_preprocessor_block_size() const;

    /*!
     * Get the decoded value of preprocessor data sense subfield.
     * \return a uint8_t representing the preprocessor data sense.
     */
    uint8_t
    decode_preprocessor_data_sense() const;

    /*!
     * Get the decoded value of preprocessor sample resolution subfield.
     * \return a uint8_t representing the preprocessor sample resolution.
     */
    uint8_t
    decode_preprocessor_sample_resolution() const;

    /*!
     * Get the decoded value of extended parameters block size subfield.
     * \return a uint16_t representing the extended parameters block size.
     */
    uint16_t
    decode_extended_parameters_block_size() const;

    /*!
     * Get the decoded value of extended parameters restricted codes option subfield.
     * \return a uint8_t representing the extended parameters restricted codes option.
     */
    uint8_t
    decode_extended_parameters_restricted_code_option() const;

    /*!
     * Get the decoded value of extended parameters reference sample interval subfield.
     * \return a uint8_t representing the extended parameters reference sample interval option.
     */
    uint8_t
    decode_extended_parameters_reference_sample_interval() const;

    /*!
     * Get the decoded value of iqzip extension header block size field.
     * \return a uint8_t representing the iqzip extension header block size.
     */
    uint16_t
    decode_iqzip_header_block_size() const;

    /*!
     * Get the decoded value of iqzip extension header endianness field.
     * \return a uint8_t representing the iqzip extension header endianness.
     */
    uint16_t
    decode_iqzip_header_endianess() const;

    /*!
     * Get the decoded value of iqzip extension header reference sample interval field.
     * \return a uint8_t representing the iqzip extension header reference sample interval.
     */
    uint16_t
    decode_iqzip_header_reference_sample_interval() const;

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

    /*!
     * Encode the grouping data length into the appropriate header subfield.
     * \param length The grouping data length
     */
    void
    encode_grouping_data_length(uint16_t length);

    /*!
     * Encode the compression technique identification into the appropriate header subfield.
     * \param id The compression technique identification
     */
    void
    encode_compression_technique_id(uint8_t id);

    /*!
     * Encode the reference sample interval into the appropriate header subfield.
     * \param interval The reference sample interval
     */
    void
    encode_reference_sample_interval(uint8_t interval);

    /*!
     * Encode the preprocessor header into the appropriate header subfield.
     */
    void
    encode_preprocessor_header();

    /*!
     * Encode the entropy coder header into the appropriate header subfield.
     */
    void
    encode_entropy_coder_header();

    /*!
     * Encode the instrument configuration header into the appropriate header subfield.
     */
    void
    encode_instrument_configuration_header();

    /*!
     * Encode the extended parameters header into the appropriate header subfield.
     */
    void
    encode_extended_parameters_header();

    /*!
     * Encode the preprocessor status into the appropriate header subfield.
     * \param status The preprocessor status
     */
    void
    encode_preprocessor_status(uint8_t status);

    /*!
     * Encode the preprocessor predictor type into the appropriate header subfield.
     * \param type The preprocessor predictor type
     */
    void
    encode_preprocessor_predictor_type(uint8_t type);

    /*!
     * Encode the preprocessor mapper type into the appropriate header subfield.
     * \param type The preprocessor mapper type
     */
    void
    encode_preprocessor_mapper_type(uint8_t type);

    /*!
     * Encode the preprocessor block size into the appropriate header subfield.
     * \param size The preprocessor block size
     */
    void
    encode_preprocessor_block_size(uint16_t size);

    /*!
     * Encode the preprocessor data sense into the appropriate header subfield.
     * \param data_sense The preprocessor data sense
     */
    void
    encode_preprocessor_data_sense(uint8_t data_sense);

    /*!
     * Encode the preprocessor sample resolution into the appropriate header subfield.
     * \param resolution The preprocessor sample resolution
     */
    void
    encode_preprocessor_sample_resolution(uint8_t resolution);

    /*!
     * Encode the entropy coder resolution range into the appropriate header subfield.
     * \param resolution The preprocessor sample resolution
     */
    void
    encode_entropy_coder_resolution_range(uint8_t resolution);

    /*!
     * Encode the entropy coder number of cdses into the appropriate header subfield.
     * \param num The entropy coder number of cdses
     */
    void
    encode_entropy_coder_cds_num(uint16_t num);

    /*!
     * Encode the extended parameters block size into the appropriate header subfield.
     * \param size The extended parameters block size
     */
    void
    encode_extended_parameters_block_size(uint16_t size);

    /*!
     * Encode the extended parameters restricted code option into the appropriate header subfield.
     * \param option The extended parameters restricted code option
     */
    void
    encode_extended_parameters_restricted_code_option(uint8_t option);

    /*!
     * Encode the extended parameters reference sample interval into the appropriate header subfield.
     * \param interval The extended parameters reference sample interval
     */
    void
    encode_extended_parameters_reference_sample_interval(uint8_t interval);

    /*!
     * Encode the block size into the appropriate iqzip extension header field.
     * \param size The iqzip extension header block size
     */
    void
    encode_iqzip_header_block_size(size_t size);

    /*!
     * Encode the endianness into the appropriate iqzip extension header field.
     * \param endianness The iqzip extension header endianness
     */
    void
    encode_iqzip_header_endianness(uint8_t endianness);

    /*!
     * Encode the reference sample interval into the appropriate iqzip extension header field.
     * \param interval The iqzip extension header reference sample interval
     */
    void
    encode_iqzip_header_reference_sample_interval(uint16_t interval);

private:
    iqzip_compression_header_t d_iqzip_header;
    ccsds_packet_primary_header *d_primary_header;
    compression_identification_packet *d_cip;

    uint16_t d_block_size;
    uint16_t d_rsi;
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

    iqzip_compression_header_t &
    get_iqzip_compression_header();

    void
    set_iqzip_compression_header(iqzip_compression_header_t *hdr);

    void
    encode();
};
}
}

#endif /* IQZIP_COMPRESSION_HEADER_H_ */
