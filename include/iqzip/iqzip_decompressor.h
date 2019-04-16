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

#ifndef IQZIP_DECOMPRESSOR_H
#define IQZIP_DECOMPRESSOR_H

#include <cstdint>
#include <iostream>
#include <string>

#include "iqzip.h"

class Iqzip_decompressor : Iqzip {
private:
	size_t d_iqzip_header_size;

    void print_error(int status);

public:

    Iqzip_decompressor ();

    ~Iqzip_decompressor();

    int	iqzip_decompress_init(const std::string fin, const std::string fout);

    int iqzip_decompress();

    int iqzip_decompress_fin();

    uint32_t getChunk() const;

	void setChunk(uint32_t) = delete;

	uint16_t getApid() const;

	void setApid(uint16_t) = delete;

	uint16_t getBlockSize() const;

	void setBlockSize(uint16_t) = delete;

	uint16_t getCdsPerPacket() const;

	void setCdsPerPacket(uint16_t) = delete;

	uint8_t getCompressionTechId() const;

	void setCompressionTechId(uint8_t) = delete;

	uint8_t getDataSense() const;

	void setDataSense(uint8_t) = delete;

	uint8_t getEndianness() const;

	void setEndianness(uint8_t) = delete;

	uint16_t getGroupingDataLength() const;

	void setGroupingDataLength(uint16_t) = delete;

	const iqzip_compression_header& getIqHeader() const;

	void setIqHeader(const iqzip_compression_header&) = delete;

	uint8_t getMapperType() const;

	void setMapperType(uint8_t) = delete;

	uint16_t getPacketDataLength() const;

	void setPacketDataLength(uint16_t) = delete;

	uint16_t getPacketSequenceCount() const;

	void setPacketSequenceCount(uint16_t) = delete;

	uint8_t getPredictorType() const;

	void setPredictorType(uint8_t) = delete;

	uint8_t getPreprocessorStatus() const;

	void setPreprocessorStatus(uint8_t) = delete;

	uint8_t getReferenceSampleInterval() const;

	void setReferenceSampleInterval(uint8_t) = delete;

	uint8_t getRestrictedCodes() const;

	void setRestrictedCodes(uint8_t) = delete;

	uint8_t getSampleResolution() const;

	void setSampleResolution(uint8_t) = delete;

	uint8_t getSecHdrFlag() const;

	void setSecHdrFlag(uint8_t) = delete;

	uint8_t getSequenceFlags() const;

	void setSequenceFlags(uint8_t) = delete;

	const aec_stream& getStrm() const;

	void setStrm(const aec_stream&) = delete;

	uint8_t getType() const;

	void setType(uint8_t type) = delete;

	uint8_t getVersion() const;

	void setVersion(uint8_t version) = delete;

	const std::ifstream& getInputStream() const;

	const std::ofstream& getOutputStream() const;
};
#endif /* IQZIP_DECOMPRESSOR_H */
