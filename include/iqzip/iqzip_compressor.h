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

/*!
 * \brief Compressor implementation
 *
 * Iqzip_compressor is the class responsible for compressing files using
 * libaec. It is a subclass of Iqzip and implements the necessary functions
 * for compression.
 */

#ifndef IQZIP_COMPRESSOR_H
#define IQZIP_COMPRESSOR_H

#include <cstdint>
#include <iostream>
#include <string>

#include <iqzip/iqzip.h>

namespace iqzip
{

namespace compression
{

class Iqzip_compressor : Iqzip{
private:
	uint32_t STREAM_CHUNK = 32768;
	char *d_tmp_stream;
	const uint32_t d_reference_samples_bytes;
	size_t d_stream_avail_in;
	char *d_out;
	size_t d_total_out;

	/*!
	 * Prints compression related error messages.
	 * @param status the value returned from compression functions calls.
	 */
    void print_error(int status);
public:
    /*!
     * Default constructor
     */
    Iqzip_compressor ();

    /*!
     * Constructor with arguments. All necessary variables for compression are
     *  initialized. A new iqzip_compression_header object is created with the
     *  specified parameters using the base class constructor
     * @param d_version CIP packet version.
     * @param d_type CIP type version.
     * @param d_sec_hdr_flag CIP secondary header flag.
     * @param d_apid CIP application ID.
     * @param d_sequence_flags CIP sequence flags.
     * @param d_sequence_count CIP sequence count.
     * @param d_packet_data_length CIP packet data length.
     * @param d_grouping_data_length CIP grouping data length.
     * @param d_compression_tech_id CIP compression technique ID.
     * @param d_reference_sample_interval CIP reference sample interval.
     * @param d_preprocessor_status Secondary header preprocessor status. 0 if preprocessor absent.
     * @param predictor_type Secondary header predictor type.
     * @param mapper_type Secondary header mapper type.
     * @param d_block_size Secondary header block size (samples).
     * @param d_data_sense Secondary header data sense.
     * @param d_sample_resolution Secondary header sample resolution (bits).
     * @param d_cds_per_packet Secondary header number of CDSes per packet.
     * @param d_restricted_codes Secondary header restricted codes.
     * @param d_endianness Endianness of samples.
     */
    Iqzip_compressor (uint8_t d_version, uint8_t d_type, uint8_t d_sec_hdr_flag,
			uint16_t d_apid, uint8_t d_sequence_flags,
			uint16_t d_sequence_count, uint16_t d_packet_data_length,
			uint16_t d_grouping_data_length, uint8_t d_compression_tech_id,
			uint8_t d_reference_sample_interval, uint8_t d_preprocessor_status,
			uint8_t predictor_type, uint8_t mapper_type, uint16_t d_block_size,
			uint8_t d_data_sense, uint8_t d_sample_resolution,
			uint16_t d_cds_per_packet, uint8_t d_restricted_codes,
			uint8_t d_endianness);

    /*!
     * Default destructor. Calls iqzip_compression_header destructor.
     */
    ~Iqzip_compressor();

    /*!
     * Initializes necessary variables for compression. Should always be called
     * before compressing. It opens the fin and fout files. The CCSDS header is
     * written to the file specified by fout. The aec_stream parameter used by
     * libaec is also initialized.
     * @param fin Name of input file.
     * @param fout Name of output file.
     * @return 0 on success, != 0 otherwise.
     */
    int	iqzip_compress_init(const std::string fin, const std::string fout);

    /*!
     * Initializes necessary variables for stream compression. Should always be called
     * before stream compressing. It opens the fout file and the CCSDS header is written.
     * The aec_stream parameter used by libaec is also initialized.
     * @param fout Name of output file.
     * @return 0 on succes, != 0 otherwise.
     */
    int iqzip_stream_compress_init(const std::string fout);

    /*!
     * Reads the input file given in iqzip_compress_init, compresses it, and
     * writes the results to fout given in iqzip_compress_init.
     * @return 0 on success, != 0 otherwise.
     */
    int iqzip_compress();

    /*!
     * Reads the buffer inbuf and when block size samples are read, compresses
     * and writes them to fout file given in iqzip_stream_compress_init.
     * The bytes passed to iqzip_stream compress must be a multiple of sample size.
     * @param inbuf buffer to read samples from.
     * @param nbytes number of bytes to read from buffer.
     * @return 0 on success, !=0 otherwise.
     */
    int iqzip_stream_compress(const char* inbuf, size_t nbytes);

    /*!
     * Finalizes the compression and clears aec_stream. Should always be called
     * after iqzip_compress otherwise output file may not be written correctly.
     * @return 0 on success, != 0 otherwise.
     */
    int iqzip_compress_fin();

    /*!
     * Finalizes the compression and clears aec_stream. Should always be called
     * after iqzip_stream_compress otherwise output file may not be written correctly.
     * @return 0 on success, != 0 otherwise.
     */
    int iqzip_stream_compress_fin();
    /*!
     * Get the value of CHUNK.
     * @return the value of CHUNK.
     */
    uint32_t getChunk() const;

    /*!
     * Set value to CHUNK. Should not be used between compress function calls.
     * @param chunk unsigned value to be passed.
     */
	void setChunk(uint32_t chunk);

	/*!
	 * Get the value of CHUNK used in streams.
	 * @return the value of STREAM_CHUNK.
	 */
	uint32_t getStreamChunk() const;

	/*!
	 * Set value to STREAM_CHUNK. Should not be used between compress funtion calls.
	 * @param stream_chunk unsigned value to be passed.
	 */
	void setStreamChunk(uint32_t stream_chunk);

	/*!
	 * Get application ID.
	 * @return the value of application ID.
	 */
	uint16_t getApid() const;

	/*!
	 * Set application ID.
	 * @param apid the value of application ID.
	 */
	void setApid(uint16_t apid);

	/*!
	 * Get block size
	 * @return the value of block size (samples).
	 */
	uint16_t getBlockSize() const;

	/*!
	 * Set block size.
	 * @param block_size the value of block size (samples).
	 */
	void setBlockSize(uint16_t block_size);

	/*!
	 * Get CDSes per packet.
	 * @return the value of CDSes per packet.
	 */
	uint16_t getCdsPerPacket() const;

	/*!
	 * Set CDSes per packet value.
	 * @param cds_per_packet the value of CDSes per packet.
	 */
	void setCdsPerPacket(uint16_t cds_per_packet);

	/*!
	 * Get compression technique ID.
	 * @return the value of compression technique ID.
	 */
	uint8_t getCompressionTechId() const;

	/*!
	 * Set compression technique ID.
	 * @param teq_id the value of compression technique ID.
	 */
	void setCompressionTechId(uint8_t teq_id);

	/*!
	 * Get data sense value.
	 * @return the value of data sense.
	 */
	uint8_t getDataSense() const;

	/*!
	 * Set data sense value.
	 * @param data_sense the value of data sense.
	 */
	void setDataSense(uint8_t data_sense);

	/*!
	 * Get endianness value (0 LSB, 1 MSB).
	 * @return the value of endianness.
	 */
	uint8_t getEndianness() const;

	/*!
	 * Set endianness value (0 LSB, 1 MSB);
	 * @param endianness the value of endianness.
	 */
	void setEndianness(uint8_t endianness);

	/*!
	 * Get grouping data length value.
	 * @return the value of grouping data length.
	 */
	uint16_t getGroupingDataLength() const;

	/*!
	 * Set grouping data length value.
	 * @param grouping_data_length the value of grouping data length.
	 */
	void setGroupingDataLength(uint16_t grouping_data_length);

	/*!
	 * Get the pointer of CCSDS header object.
	 * @return the pointer of a CCSDS header object.
	 */
	const iqzip_compression_header& getIqHeader() const;

	/*!
	 * Set the pointer to CCSDS header pointer.
	 * @param header the pointer of a CCSDS header object
	 */
	void setIqHeader(const iqzip_compression_header& header);

	/*!
	 * Get the mapper type value.
	 * @return the value of mapper type.
	 */
	uint8_t getMapperType() const;

	/*!
	 * Set the mapper type value.
	 * @param mapper the value of mapper type.
	 */
	void setMapperType(uint8_t mapper);

	/*!
	 * Get packet data length.
	 * @return the value of packet data length.
	 */
	uint16_t getPacketDataLength() const;

	/*!
	 * Set packet data length.
	 * @param packet_data_length the value of packet data length.
	 */
	void setPacketDataLength(uint16_t packet_data_length);

	/*!
	 * Get packet sequence count value.
	 * @return the value of packet sequence count.
	 */
	uint16_t getPacketSequenceCount() const;

	/*!
	 * Set packet sequence count.
	 * @param packet_sequence_count the value of packetsequence count
	 */
	void setPacketSequenceCount(uint16_t packet_sequence_count);

	/*!
	 * Get predictor type.
	 * @return the value of predictor type.
	 */
	uint8_t getPredictorType() const;

	/*!
	 * Set predictor type value.
	 * @param predictor_type the value of predictor type.
	 */
	void setPredictorType(uint8_t predictor_type);

	/*!
	 * Get preprocessor status value.
	 * @return the value of the preprocessor status.
	 */
	uint8_t getPreprocessorStatus() const;

	/*!
	 * Set preprocessor status value.
	 * @param preprocessor_status the value of preprocessor status.
	 */
	void setPreprocessorStatus(uint8_t preprocessor_status);

	/*!
	 * Get the reference sample interval value.
	 * @return the value of reference sample interval.
	 */
	uint8_t getReferenceSampleInterval() const;

	/*!
	 * Set the reference sample interval value.
	 * @param rsi the value of reference sample interval.
	 */
	void setReferenceSampleInterval(uint8_t rsi);

	/*!
	 * Get restricted codes value.
	 * @return the value of restricted codes.
	 */
	uint8_t getRestrictedCodes() const;

	/*!
	 * Set restricted codes value.
	 * @param restricted_codes the value of restricted codes.
	 */
	void setRestrictedCodes(uint8_t restricted_codes);

	/*!
	 * Get sample resolution value.
	 * @return the value of sample resolution.
	 */
	uint8_t getSampleResolution() const;

	/*!
	 * Set sample resolution value.
	 * @param sample_resolution the value of sample resolution.
	 */
	void setSampleResolution(uint8_t sample_resolution);

	/*!
	 * Get secondary header flag value.
	 * @return the value of secondary header flag.
	 */
	uint8_t getSecHdrFlag() const;

	/*!
	 * Set secondary header flag value.
	 * @param sec_header_flag the value of secondary header flag.
	 */
	void setSecHdrFlag(uint8_t sec_header_flag);

	/*!
	 * Get sequence flags value.
	 * @return the value of secondary sequence flags.
	 */
	uint8_t getSequenceFlags() const;

	/*!
	 * Set sequence flags value.
	 * @param sequence_flags the value of sequence flags.
	 */
	void setSequenceFlags(uint8_t sequence_flags);

	/*!
	 * Get the pointer of aec stream object.
	 * @return the pointer of aec stream object.
	 */
	const aec_stream& getStrm() const;

	/*!
	 * Set the pointer of aec stream object
	 * @param stream pointer to an aec stream object.
	 */
	void setStrm(const aec_stream& stream);

	/*!
	 * Get CIP type value.
	 * @return the value of type.
	 */
	uint8_t getType() const;

	/*!
	 * Set CIP type value.
	 * @param type the value of type.
	 */
	void setType(uint8_t type);

	/*!
	 * Get CIP version value.
	 * @return the value of version.
	 */
	uint8_t getVersion() const;

	/*!
	 * Set CIP version value.
	 * @param version the value of version.
	 */
	void setVersion(uint8_t version);

	/*!
	 * Get the pointer of input stream.
	 * @return the pointer of input stream.
	 */
	const std::ifstream& getInputStream() const;

	/*!
	 * Get the pointer of output stream.
	 * @return the pointer of output stream.
	 */
	const std::ofstream& getOutputStream() const;

};

}

}
#endif /* IQZIP_COMPRESSOR_H */
