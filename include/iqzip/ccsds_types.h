/*
 * ccsds_types.h
 *
 *  Created on: Mar 21, 2020
 *      Author: ctriant
 */

#ifndef INCLUDE_IQZIP_CCSDS_TYPES_H_
#define INCLUDE_IQZIP_CCSDS_TYPES_H_

namespace iqzip {

namespace compression {

namespace header {

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

/*!
 * The default values for the Compression Technique Identification subfield
 *  as defined by the CCSDS Loosless Data Compression Blue Book.
 */
enum class COMPRESSION_TECHNIQUE_IDENTIFICATION {
    NO_COMPRESSION = 0x0, CCSDS_LOSSLESS_COMPRESSION = 0x1
};

/*!
 * The default values for the preprocessor status field of the Preprocessor subfield
 *  as defined by the CCSDS Loosless Data Compression Blue Book.
 */
enum class PREPROCESSOR_STATUS {
    ABSENT = 0x0, PRESENT = 0x1
};

/*!
 * The default values for the predictor type field of the Preprocessor subfield
 *  as defined by the CCSDS Loosless Data Compression Blue Book.
 */
enum class PREPROCESSOR_PREDICTOR_TYPE {
    BYPASS = 0x0, UNIT_DELAY = 0x1, APPLICATION_SPECIFIC = 0x7
};

/*!
 * The default values for the mapper type field of the Preprocessor subfield
 *  as defined by the CCSDS Loosless Data Compression Blue Book.
 */
enum class PREPROCESSOR_MAPPER_TYPE {
    PREDICTION_ERROR = 0x0, APPLICATION_SPECIFIC = 0x3
};

/*!
 * The default values for the block size field of the Preprocessor subfield
 *  as defined by the CCSDS Loosless Data Compression Blue Book.
 */
enum class PREPROCESSOR_BLOCK_SIZE {
    SAMPLES_8 = 0x0,
    SAMPLES_16 = 0x1,
    SAMPLES_32_64 = 0x2,
    APPLICATION_SPECIFIC = 0x3
};

/*!
 * The default values for the data sense field of the Preprocessor subfield
 *  as defined by the CCSDS Loosless Data Compression Blue Book.
 */
enum class PREPROCESSOR_DATA_SENSE {
    TWO_COMPLEMENT = 0x0, POSITIVE = 0x1
};

/*!
 * The default values for the resolution range field of the Entropy Coder subfield
 *  as defined by the CCSDS Loosless Data Compression Blue Book.
 */
enum class ENTROPY_CODER_RESOLUTION_RANGE {
    SPARE = 0x0,
    LESS_EQUAL_8 = 0x1,
    LESS_EQUAL_16 = 0x2,
    LESS_EQUAL_32 = 0x3
};

/*!
 * The default values for the block size field of the Extended Parameters subfield
 *  as defined by the CCSDS Loosless Data Compression Blue Book.
 */
enum class EXTENDED_PARAMETERS_BLOCK_SIZE {
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
enum class EXTENDED_PARAMETERS_RESTRICTED_CODES {
    BASIC = 0x0, RESTRICTED = 0x1
};

/*!
 * The default values for the Source Configuration subfield headers as defined
 *  by the CCSDS Loosless Data Compression Blue Book.
 */
enum SOURCE_CONFIGURATION_SUBFIELD_HEADER {
    PREPROCESSOR = 0x0,
    ENTROPY_CODER = 0x1,
    INSTRUMENT_CONFIGURATION = 0x2,
    EXTENDED_PARAMETERS = 0x3
};

}
}
}

#endif /* INCLUDE_IQZIP_CCSDS_TYPES_H_ */
