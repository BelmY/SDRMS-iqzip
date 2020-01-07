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

#include <iostream>

#include <iqzip/iqzip.h>

Iqzip::Iqzip() :
    d_version(0),
    d_type(0),
    d_sec_hdr_flag(0),
    d_apid(0),
    d_sequence_flags(0),
    d_packet_sequence_count(0),
    d_packet_data_length(0),
    d_grouping_data_length(0),
    d_compression_tech_id(0),
    d_reference_sample_interval(0),
    d_preprocessor_status(0),
    d_predictor_type(0),
    d_mapper_type(0),
    d_block_size(0),
    d_data_sense(0),
    d_sample_resolution(0),
    d_cds_per_packet(0),
    d_restricted_codes(0),
    d_endianness(0)
{
    d_iq_header = iqzip::compression::iqzip_compression_header();
}
;

Iqzip::Iqzip(uint8_t version, uint8_t type, uint8_t sec_hdr_flag,
             uint16_t apid, uint8_t sequence_flags,
             uint16_t packet_sequence_count, uint16_t packet_data_length,
             uint16_t grouping_data_length, uint8_t compression_tech_id,
             uint8_t reference_sample_interval, uint8_t preprocessor_status,
             uint8_t predictor_type, uint8_t mapper_type, uint16_t block_size,
             uint8_t data_sense, uint8_t sample_resolution,
             uint16_t cds_per_packet, uint8_t restricted_codes,
             uint8_t endianness) :
    d_version(version),
    d_type(type),
    d_sec_hdr_flag(sec_hdr_flag),
    d_apid(apid),
    d_sequence_flags(sequence_flags),
    d_packet_sequence_count(packet_sequence_count),
    d_packet_data_length(packet_data_length),
    d_grouping_data_length(grouping_data_length),
    d_compression_tech_id(compression_tech_id),
    d_reference_sample_interval(reference_sample_interval),
    d_preprocessor_status(preprocessor_status),
    d_predictor_type(predictor_type),
    d_mapper_type(mapper_type),
    d_block_size(block_size),
    d_data_sense(data_sense),
    d_sample_resolution(sample_resolution),
    d_cds_per_packet(cds_per_packet),
    d_restricted_codes(restricted_codes),
    d_endianness(endianness)
{
    /* Initialize IQ CCSDS header */
    d_iq_header = iqzip::compression::iqzip_compression_header(
                      d_version, d_type, d_sec_hdr_flag, d_apid, d_sequence_flags,
                      d_packet_sequence_count, d_packet_data_length, d_grouping_data_length,
                      d_compression_tech_id, d_reference_sample_interval, d_preprocessor_status,
                      d_predictor_type, d_mapper_type, d_block_size, d_data_sense,
                      d_sample_resolution, d_cds_per_packet, d_restricted_codes, d_endianness);
}

Iqzip::~Iqzip()
{
}

void
Iqzip::init_aec_stream(void)
{
    d_strm.avail_in = 0;
    d_strm.avail_out = CHUNK;
    d_strm.bits_per_sample = d_sample_resolution;
    d_strm.block_size = d_block_size;
    d_strm.flags = 0;
    /* Always use 3 bytes for 24bits samples and don't enforce */
    d_strm.flags |= AEC_DATA_3BYTE;
    d_strm.flags |= AEC_NOT_ENFORCE;
    /* Shift option bit to corresponding bit in flag to initialize flags */
    d_strm.flags |= (~(d_data_sense << (uint8_t) log2(AEC_DATA_SIGNED)))
                    & AEC_DATA_SIGNED;
    d_strm.flags |= (~(d_endianness << (uint8_t) log2(AEC_DATA_MSB)))
                    & AEC_DATA_MSB;
    d_strm.flags |= d_preprocessor_status << (uint8_t) log2(AEC_DATA_PREPROCESS);
    d_strm.flags |= d_restricted_codes << (uint8_t) log2(AEC_RESTRICTED);
    //d_strm.flags |= AEC_PAD_RSI;
    d_strm.next_in = nullptr;
    d_strm.next_out = nullptr;
    d_strm.rsi = d_reference_sample_interval;
    d_strm.state = nullptr;
    d_strm.total_in = 0;
    d_strm.total_out = 0;
}

uint32_t
Iqzip::getChunk() const
{
    return CHUNK;
}

void
Iqzip::setChunk(uint32_t chunk = 10485760)
{
    CHUNK = chunk;
}

uint16_t
Iqzip::getApid() const
{
    return d_apid;
}

void
Iqzip::setApid(uint16_t apid)
{
    d_apid = apid;
}

uint16_t
Iqzip::getBlockSize() const
{
    return d_block_size;
}

void
Iqzip::setBlockSize(uint16_t blockSize)
{
    d_block_size = blockSize;
}

uint16_t
Iqzip::getCdsPerPacket() const
{
    return d_cds_per_packet;
}

void
Iqzip::setCdsPerPacket(uint16_t cdsPerPacket)
{
    d_cds_per_packet = cdsPerPacket;
}

uint8_t
Iqzip::getCompressionTechId() const
{
    return d_compression_tech_id;
}

void
Iqzip::setCompressionTechId(uint8_t compressionTechId)
{
    d_compression_tech_id = compressionTechId;
}

uint8_t
Iqzip::getDataSense() const
{
    return d_data_sense;
}

void
Iqzip::setDataSense(uint8_t dataSense)
{
    d_data_sense = dataSense;
}

uint8_t
Iqzip::getEndianness() const
{
    return d_endianness;
}

void
Iqzip::setEndianness(uint8_t endianness)
{
    d_endianness = endianness;
}

uint16_t
Iqzip::getGroupingDataLength() const
{
    return d_grouping_data_length;
}

void
Iqzip::setGroupingDataLength(uint16_t groupingDataLength)
{
    d_grouping_data_length = groupingDataLength;
}

const iqzip::compression::iqzip_compression_header &
Iqzip::getIqHeader() const
{
    return d_iq_header;
}

void
Iqzip::setIqHeader(
    const iqzip::compression::iqzip_compression_header &iqHeader)
{
    d_iq_header = iqHeader;
}

uint8_t
Iqzip::getMapperType() const
{
    return d_mapper_type;
}

void
Iqzip::setMapperType(uint8_t mapperType)
{
    d_mapper_type = mapperType;
}

uint16_t
Iqzip::getPacketDataLength() const
{
    return d_packet_data_length;
}

void
Iqzip::setPacketDataLength(uint16_t packetDataLength)
{
    d_packet_data_length = packetDataLength;
}

uint16_t
Iqzip::getPacketSequenceCount() const
{
    return d_packet_sequence_count;
}

void
Iqzip::setPacketSequenceCount(uint16_t packetSequenceCount)
{
    d_packet_sequence_count = packetSequenceCount;
}

uint8_t
Iqzip::getPredictorType() const
{
    return d_predictor_type;
}

void
Iqzip::setPredictorType(uint8_t predictorType)
{
    d_predictor_type = predictorType;
}

uint8_t
Iqzip::getPreprocessorStatus() const
{
    return d_preprocessor_status;
}

void
Iqzip::setPreprocessorStatus(uint8_t preprocessorStatus)
{
    d_preprocessor_status = preprocessorStatus;
}

uint8_t
Iqzip::getReferenceSampleInterval() const
{
    return d_reference_sample_interval;
}

void
Iqzip::setReferenceSampleInterval(uint8_t referenceSampleInterval)
{
    d_reference_sample_interval = referenceSampleInterval;
}

uint8_t
Iqzip::getRestrictedCodes() const
{
    return d_restricted_codes;
}

void
Iqzip::setRestrictedCodes(uint8_t restrictedCodes)
{
    d_restricted_codes = restrictedCodes;
}

uint8_t
Iqzip::getSampleResolution() const
{
    return d_sample_resolution;
}

void
Iqzip::setSampleResolution(uint8_t sampleResolution)
{
    d_sample_resolution = sampleResolution;
}

uint8_t
Iqzip::getSecHdrFlag() const
{
    return d_sec_hdr_flag;
}

void
Iqzip::setSecHdrFlag(uint8_t secHdrFlag)
{
    d_sec_hdr_flag = secHdrFlag;
}

uint8_t
Iqzip::getSequenceFlags() const
{
    return d_sequence_flags;
}

void
Iqzip::setSequenceFlags(uint8_t sequenceFlags)
{
    d_sequence_flags = sequenceFlags;
}

const aec_stream &
Iqzip::getStrm() const
{
    return d_strm;
}

void
Iqzip::setStrm(const aec_stream &strm)
{
    d_strm = strm;
}

uint8_t
Iqzip::getType() const
{
    return d_type;
}

void
Iqzip::setType(uint8_t type)
{
    d_type = type;
}

uint8_t
Iqzip::getVersion() const
{
    return d_version;
}

void
Iqzip::setVersion(uint8_t version)
{
    d_version = version;
}

const std::ifstream &
Iqzip::getInputStream() const
{
    return input_stream;
}

const std::ofstream &
Iqzip::getOutputStream() const
{
    return output_stream;
}

