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

#ifndef COMPRESSION_IDENTIFICATION_PACKET_H_
#define COMPRESSION_IDENTIFICATION_PACKET_H_

#include <stdint.h>
#include <string>

/*
 * Max size of CIP header in bytes excluding Secondary header and Instrument Configuration
 */
#define MAX_CIP_HEADER_SIZE_BYTES               16
#define SOURCE_DATA_FIXED_SIZE                  4
#define PREPROCESSOR_SUBFIELD_SIZE              2
#define ENTROPY_CODER_SUBFIELD_SIZE             2
#define EXTENDED_PARAMETERS_SUBFIELD_SIZE       2
#define INSTRUMENT_CONFIG_SUBFIELD_SIZE         2

namespace iqzip
{

  namespace compression
  {

    /*!
     *
     * \ingroup compression_header
     *
     * The Compression Identification Packet is an optional packet that shall contain
     * information that would allow the Adaptive Entropy decompressor, as described in
     * the CCSDS Lossless Data Compression Blue Book, to be automatically configured
     * to acquire a group of compressed application data packets without the need of
     * a-priori mission specific information.
     *
     * The important mission specific configuration for the Adaptive Entropy Coder refers to:
     *      - l, the number of CDSes that exist in a packet
     *      - r, the reference sample interval
     *      - n, the sample resolution in bits
     *      - J, the number of samples per block
     *      - whether the Basic or Restricted set of codes is used
     *
     * This class represents the header of the CIP packets as it is defined in the
     * CCSDS Lossless Data Compression Blue Book and provides all the appropriate
     * setter and getter functions to access its subfields.
     *
     * TODO: Describe the structure of CIP header and explain why Secondary Header is
     * omitted.
     */
    class compression_identification_packet
    {

    public:

      /*!
       * The default values for the Compression Technique Identification subfield
       *  as defined by the CCSDS Loosless Data Compression Blue Book.
       */
      enum class COMPRESSION_TECHNIQUE_IDENTIFICATION
      {
        NO_COMPRESSION = 0x0, CCSDS_LOSSLESS_COMPRESSION = 0x1
      };

      /*!
       * The default values for the preprocessor status field of the Preprocessor subfield
       *  as defined by the CCSDS Loosless Data Compression Blue Book.
       */
      enum class PREPROCESSOR_STATUS
      {
        ABSENT = 0x0, PRESENT = 0x1
      };

      /*!
       * The default values for the predictor type field of the Preprocessor subfield
       *  as defined by the CCSDS Loosless Data Compression Blue Book.
       */
      enum class PREPROCESSOR_PREDICTOR_TYPE
      {
        BYPASS = 0x0, UNIT_DELAY = 0x1, APPLICATION_SPECIFIC = 0x7
      };

      /*!
       * The default values for the mapper type field of the Preprocessor subfield
       *  as defined by the CCSDS Loosless Data Compression Blue Book.
       */
      enum class PREPROCESSOR_MAPPER_TYPE
      {
        PREDICTION_ERROR = 0x0, APPLICATION_SPECIFIC = 0x3
      };

      /*!
       * The default values for the block size field of the Preprocessor subfield
       *  as defined by the CCSDS Loosless Data Compression Blue Book.
       */
      enum class PREPROCESSOR_BLOCK_SIZE
      {
        SAMPLES_8 = 0x0,
        SAMPLES_16 = 0x1,
        SAMPLES_32_64 = 0x2,
        APPLICATION_SPECIFIC = 0x3
      };

      /*!
       * The default values for the data sense field of the Preprocessor subfield
       *  as defined by the CCSDS Loosless Data Compression Blue Book.
       */
      enum class PREPROCESSOR_DATA_SENSE
      {
        TWO_COMPLEMENT = 0x0, POSITIVE = 0x1
      };

      /*!
       * The default values for the resolution range field of the Entropy Coder subfield
       *  as defined by the CCSDS Loosless Data Compression Blue Book.
       */
      enum class ENTROPY_CODER_RESOLUTION_RANGE
      {
        SPARE = 0x0,
        LESS_EQUAL_8 = 0x1,
        LESS_EQUAL_16 = 0x2,
        LESS_EQUAL_32 = 0x3
      };

      /*!
       * The default values for the block size field of the Extended Parameters subfield
       *  as defined by the CCSDS Loosless Data Compression Blue Book.
       */
      enum class EXTENDED_PARAMETERS_BLOCK_SIZE
      {
        SAMPLES_8 = 0x0,
        SAMPLES_16 = 0x1,
        SAMPLES_32 = 0x2,
        SAMPLES_64 = 0x3,
        APPLICATION_SPECIFIC = 0xF
      };

      /*!
       * The default values for the restricted codes field of the Extended Parameters subfield
       *  as defined by the CCSDS Loosless Data Compression Blue Book.
       */
      enum class EXTENDED_PARAMETERS_RESTRICTED_CODES
      {
        BASIC = 0x0, RESTRICTED = 0x1
      };

      /*!
       * The default values for the Source Configuration subfield headers as defined
       *  by the CCSDS Loosless Data Compression Blue Book.
       */
      enum SOURCE_CONFIGURATION_SUBFIELD_HEADER
      {
        PREPROCESSOR = 0x0,
        ENTROPY_CODER = 0x1,
        INSTRUMENT_CONFIGURATION = 0x2,
        EXTENDED_PARAMETERS = 0x3
      };

      /*!
       * A bit-field that defines the structure of the Preprocessor subfield.
       */
      typedef struct
      {
        uint8_t header :2;
        uint8_t status :1;
        uint8_t predictor_type :3;
        uint8_t mapper_type :2;
        uint8_t block_size :2;
        uint8_t data_sense :1;
        uint8_t sample_resolution :5;
      } preprocessor_t;

      /*!
       * A bit-field that defines the structure of the Entropy Coder subfield.
       */
      typedef struct
      {
        uint8_t header :2;
        uint8_t resolution_range :2;
        uint16_t cds_num :12;
      } entropy_coder_t;

      /*!
       * A bit-field that defines the structure of the Instrument Configuration subfield.
       */
      typedef struct
      {
        uint8_t header :2;
        uint16_t *params;
      } instrument_configuration_t;

      /*!
       * A bit-field that defines the structure of the Extended Parameters subfield.
       */
      typedef struct
      {
        uint8_t header :2;
        uint8_t reserved_1 :2;
        uint8_t block_size :4;
        uint8_t reserved_2 :1;
        uint8_t restricted_code_options :1;
        uint8_t reserved_3 :2;
        uint8_t reference_sample_interval :4;
      } extended_parameters_t;

      /*!
       * A bit-field that defines the structure of the fixed part of the Source Data
       * Field.
       */
      typedef struct
      {
        uint8_t grouping_data_length_reserved :4;
        uint16_t grouping_data_length :12;
        uint8_t compression_technique_id :8;
        uint8_t reference_sample_interval :8;
      } source_data_fixed_t;

      /*!
       * A bit-field that defines the structure of the Source Configuration Field.
       */
      typedef struct
      {
        preprocessor_t preprocessor;
        entropy_coder_t entropy_coder;
        extended_parameters_t extended_parameters;
        instrument_configuration_t instrument_config;
      } source_data_variable_t;

      compression_identification_packet (uint16_t grouping_data_length,
                                         uint8_t compression_tech_id,
                                         uint8_t reference_sample_interval,
                                         uint8_t preprocessor_status,
                                         uint8_t predictor_type,
                                         uint8_t mapper_type,
                                         uint16_t block_size,
                                         uint8_t data_sense,
                                         uint8_t sample_resolution,
                                         uint16_t csd_per_packet,
                                         uint8_t restricted_codes);

      compression_identification_packet ();

      virtual
      ~compression_identification_packet ();

      /*!
       * Get the inner bit-field that represents the fixed part of the Source Data subfield
       * of the Compression Identification Packet header
       * \return a source_data_fixed_t representing the fixed part of the Source Data subfield
       */
      source_data_fixed_t&
      get_source_data_fixed ();

      /*!
       * Get the inner bit-field that represents the Source Configuration subfield
       * of the Compression Identification Packet header
       * \return a source_data_variable_t representing the Source Configuration subfield
       */
      source_data_variable_t&
      get_source_data_variable ();

      void
      set_source_data_fixed (source_data_fixed_t hdr);

      void
      set_source_data_variable (source_data_variable_t hdr);

      /*!
       * Encode the grouping data length into the appropriate header subfield.
       * \param length The grouping data length
       */
      void
      encode_grouping_data_length (uint16_t length);

      /*!
       * Encode the compression technique identification into the appropriate header subfield.
       * \param id The compression technique identification
       */
      void
      encode_compression_technique_id (uint8_t id);

      /*!
       * Encode the reference sample interval into the appropriate header subfield.
       * \param interval The reference sample interval
       */
      void
      encode_reference_sample_interval (uint8_t interval);

      /*!
       * Encode the preprocessor header into the appropriate header subfield.
       */
      void
      encode_preprocessor_header ();

      /*!
       * Encode the entropy coder header into the appropriate header subfield.
       */
      void
      encode_entropy_coder_header ();

      /*!
       * Encode the instrument configuration header into the appropriate header subfield.
       */
      void
      encode_instrument_configuration_header ();

      /*!
       * Encode the extended parameters header into the appropriate header subfield.
       */
      void
      encode_extended_parameters_header ();

      /*!
       * Encode the preprocessor status into the appropriate header subfield.
       * \param status The preprocessor status
       */
      void
      encode_preprocessor_status (uint8_t status);

      /*!
       * Encode the preprocessor predictor type into the appropriate header subfield.
       * \param type The preprocessor predictor type
       */
      void
      encode_preprocessor_predictor_type (uint8_t type);

      /*!
       * Encode the preprocessor mapper type into the appropriate header subfield.
       * \param type The preprocessor mapper type
       */
      void
      encode_preprocessor_mapper_type (uint8_t type);

      /*!
       * Encode the preprocessor block size into the appropriate header subfield.
       * \param size The preprocessor block size
       */
      void
      encode_preprocessor_block_size (uint16_t size);

      /*!
       * Encode the preprocessor data sense into the appropriate header subfield.
       * \param data_sense The preprocessor data sense
       */
      void
      encode_preprocessor_data_sense (uint8_t data_sense);

      /*!
       * Encode the preprocessor sample resolution into the appropriate header subfield.
       * \param resolution The preprocessor sample resolution
       */
      void
      encode_preprocessor_sample_resolution (uint8_t resolution);

      /*!
       * Encode the entropy coder resolution range into the appropriate header subfield.
       * \param resolution The preprocessor sample resolution
       */
      void
      encode_entropy_coder_resolution_range (uint8_t resolution);

      /*!
       * Encode the entropy coder number of cdses into the appropriate header subfield.
       * \param num The entropy coder number of cdses
       */
      void
      encode_entropy_coder_cds_num (uint16_t num);

      /*!
       * Encode the extended parameters block size into the appropriate header subfield.
       * \param size The extended parameters block size
       */
      void
      encode_extended_parameters_block_size (uint16_t size);

      /*!
       * Encode the extended parameters restricted code option into the appropriate header subfield.
       * \param option The extended parameters restricted code option
       */
      void
      encode_extended_parameters_restricted_code_option (uint8_t option);

      /*!
       * Encode the extended parameters reference sample interval into the appropriate header subfield.
       * \param interval The extended parameters reference sample interval
       */
      void
      encode_extended_parameters_reference_sample_interval (uint8_t interval);

      /*!
       * Get the decoded value of grouping data length subfield.
       * \return a uint16_t representing the grouping data length.
       */
      uint16_t
      decode_grouping_data_length () const;

      /*!
       * Get the decoded value of compression technique identification subfield.
       * \return a uint8_t representing the compression technique identification.
       */
      uint8_t
      decode_compression_technique_id () const;

      /*!
       * Get the decoded value of reference sample interval subfield.
       * \return a uint8_t representing the reference sample interval.
       */
      uint8_t
      decode_reference_sample_interval () const;

      /*!
       * Get the decoded value of preprocessor status subfield.
       * \return a uint8_t representing the preprocessor status.
       */
      uint8_t
      decode_preprocessor_status () const;

      /*!
       * Get the decoded value of preprocessor predictor type subfield.
       * \return a uint8_t representing the preprocessor predictor type.
       */
      uint8_t
      decode_preprocessor_predictor_type () const;

      /*!
       * Get the decoded value of preprocessor mapper type subfield.
       * \return a uint8_t representing the preprocessor mapper type.
       */
      uint8_t
      decode_preprocessor_mapper_type () const;

      /*!
       * Get the decoded value of preprocessor block size subfield.
       * \return a uint16_t representing the preprocessor block size.
       */
      uint16_t
      decode_preprocessor_block_size () const;

      /*!
       * Get the decoded value of preprocessor data sense subfield.
       * \return a uint8_t representing the preprocessor data sense.
       */
      uint8_t
      decode_preprocessor_data_sense () const;

      /*!
       * Get the decoded value of preprocessor sample resolution subfield.
       * \return a uint8_t representing the preprocessor sample resolution.
       */
      uint8_t
      decode_preprocessor_sample_resolution () const;

      /*!
       * Get the decoded value of extended parameters block size subfield.
       * \return a uint16_t representing the extended parameters block size.
       */
      uint16_t
      decode_extended_parameters_block_size () const;

      /*!
       * Get the decoded value of extended parameters restricted codes option subfield.
       * \return a uint8_t representing the extended parameters restricted codes option.
       */
      uint8_t
      decode_extended_parameters_restricted_code_option () const;

      /*!
       * Get the decoded value of extended parameters reference sample interval subfield.
       * \return a uint8_t representing the extended parameters reference sample interval option.
       */
      uint8_t
      decode_extended_parameters_reference_sample_interval () const;

    private:
      uint16_t d_grouping_data_length;
      uint8_t d_compression_tech_id;
      uint8_t d_reference_sample_interval;
      uint8_t d_preprocessor_status;
      uint8_t d_predictor_type;
      uint8_t d_mapper_type;
      uint8_t d_sample_resolution;
      uint8_t d_data_sense;
      uint8_t d_restricted_codes;
      uint16_t d_block_size;
      uint16_t d_cds_per_packet;

      source_data_fixed_t d_source_data_fixed;
      source_data_variable_t d_source_data_variable;

      void
      encode ();

      void
      encode_preprocessor ();

      void
      encode_entropy_coder ();

      void
      encode_instrument_configuration ();

      void
      encode_extended_parameters ();

    };
  }
}

#endif /* COMPRESSION_IDENTIFICATION_PACKET_H_ */
