#ifndef IQZIP_COMPRESSOR_H
#define IQZIP_COMPRESSOR_H

#include <cstdint>
#include <iostream>
#include <string>

#include "iqzip.h"

class Iqzip_compressor : Iqzip{
private:
    void print_error(int status);
public:

    Iqzip_compressor ();

    Iqzip_compressor (uint8_t d_version, uint8_t d_type, uint8_t d_sec_hdr_flag,
			uint16_t d_apid, uint8_t d_sequence_flags,
			uint16_t d_sequence_count, uint16_t d_packet_data_length,
			uint16_t d_grouping_data_length, uint8_t d_compression_tech_id,
			uint8_t d_reference_sample_interval, uint8_t d_preprocessor_status,
			uint8_t predictor_type, uint8_t mapper_type, uint16_t d_block_size,
			uint8_t d_data_sense, uint8_t d_sample_resolution,
			uint16_t d_cds_per_packet, uint8_t d_restricted_codes,
			uint8_t d_endianness);

    ~Iqzip_compressor();

    int	iqzip_compress_init(const std::string fin, const std::string fout);

    int iqzip_compress();

    int iqzip_compress_fin();

    uint32_t getChunk() const;

	void setChunk(uint32_t);

	uint16_t getApid() const;

	void setApid(uint16_t);

	uint16_t getBlockSize() const;

	void setBlockSize(uint16_t);

	uint16_t getCdsPerPacket() const;

	void setCdsPerPacket(uint16_t);

	uint8_t getCompressionTechId() const;

	void setCompressionTechId(uint8_t);

	uint8_t getDataSense() const;

	void setDataSense(uint8_t);

	uint8_t getEndianness() const;

	void setEndianness(uint8_t);

	uint16_t getGroupingDataLength() const;

	void setGroupingDataLength(uint16_t);

	const iqzip_compression_header& getIqHeader() const;

	void setIqHeader(const iqzip_compression_header&);

	uint8_t getMapperType() const;

	void setMapperType(uint8_t);

	uint16_t getPacketDataLength() const;

	void setPacketDataLength(uint16_t);

	uint16_t getPacketSequenceCount() const;

	void setPacketSequenceCount(uint16_t);

	uint8_t getPredictorType() const;

	void setPredictorType(uint8_t);

	uint8_t getPreprocessorStatus() const;

	void setPreprocessorStatus(uint8_t);

	uint8_t getReferenceSampleInterval() const;

	void setReferenceSampleInterval(uint8_t);

	uint8_t getRestrictedCodes() const;

	void setRestrictedCodes(uint8_t);

	uint8_t getSampleResolution() const;

	void setSampleResolution(uint8_t);

	uint8_t getSecHdrFlag() const;

	void setSecHdrFlag(uint8_t);

	uint8_t getSequenceFlags() const;

	void setSequenceFlags(uint8_t);

	const aec_stream& getStrm() const;

	void setStrm(const aec_stream&);

	uint8_t getType() const;

	void setType(uint8_t type);

	uint8_t getVersion() const;

	void setVersion(uint8_t version);

	const std::ifstream& getInputStream() const;

	const std::ofstream& getOutputStream() const;

};
#endif /* IQZIP_COMPRESSOR_H */
