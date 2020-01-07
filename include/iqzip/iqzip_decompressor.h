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

#include <iqzip/iqzip.h>

namespace iqzip {

namespace compression {

class Iqzip_decompressor : Iqzip {
private:
    size_t d_iqzip_header_size;
    uint32_t STREAM_CHUNK = 32768;
    char *d_tmp_stream;
    size_t d_stream_avail_in;
    char *d_out;
    size_t d_total_out;

    /*!
     * Prints decompression related error messages.
     * @param status the value returned from decompression functions calls.
     */
    void print_error(int status);

public:

    /*!
     * Default constructor. All variables are initialized from reading the
     * header values with iqzip_decompress_init.
     */
    Iqzip_decompressor();

    /*!
     * Default destructor. Calls iqzip_compression_header destructor.
     */
    ~Iqzip_decompressor();

    /*!
     * Initializes necessary variables for decompression. Should always be called
     * before decompressing. It opens the fin and fout files. The CCSDS header is
     * read from the file specified by fin. The aec_stream parameter used by
     * libaec is also initialized.
     * @param fin Name of input file.
     * @param fout Name of output file.
     * @return 0 on success, != 0 otherwise.
     */
    int iqzip_decompress_init(const std::string fin, const std::string fout);

    /*!
     * Reads the input file given in iqzip_decompress_init, decompresses it, and
     * writes the results to fout given in iqzip_decompress_init.
     * @return 0 on succes, != 0 otherwise.
     */
    int iqzip_decompress();

    /*!
     * Reads the input file given in iqzip_decompress_init, decompresses it,
     * and writes the results to fout given in iqzip_decompress_init.
     * @param inbuf buffer to read samples from.
     * @param nbytes number of bytes to read from buffer.
     * @return 0 on success, !=0 otherwise.
     */
    int iqzip_stream_decompress(const char *inbuf, size_t nbytes);

    /*!
     * Finalizes the decompression and clears aec_stream. Should always be called
     * after iqzip_decompress otherwise output file may not be written correctly.
     * @return 0 on success, != 0 otherwise.
     */
    int iqzip_decompress_fin();

    /*!
     * Finalizes the decompression and clears aec_stream. Should always be called
     * after iqzip_stream_decompress otherwise output file may not be written correctly.
     * @return
     */
    int iqzip_stream_decompress_fin();

    /*!
     * Get the value of CHUNK.
     * @return the value of CHUNK.
     */
    uint32_t getChunk() const;

    /*!
     * Disabling setting chunk. All variables must be set from header of the
     * input file.
     */
    void setChunk(uint32_t);

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
     * Disabling setting application ID. All variables must be set from header of the
     * input file.
     */
    void setApid(uint16_t) = delete;

    /*!
     * Get block size
     * @return the value of block size (samples).
     */
    uint16_t getBlockSize() const;

    /*!
     * Disabling setting block size. All variables must be set from header of the
     * input file.
     */
    void setBlockSize(uint16_t) = delete;

    /*!
     * Get CDSes per packet.
     * @return the value of CDSes per packet.
     */
    uint16_t getCdsPerPacket() const;

    /*!
     * Disabling setting CDSes per packet. All variables must be set from header of the
     * input file.
     */
    void setCdsPerPacket(uint16_t) = delete;

    /*!
     * Get compression technique ID.
     * @return the value of compression technique ID.
     */
    uint8_t getCompressionTechId() const;

    /*!
     * Disabling setting compression technique ID. All variables must be set from header of the
     * input file.
     */
    void setCompressionTechId(uint8_t) = delete;

    /*!
     * Get data sense value.
     * @return the value of data sense.
     */
    uint8_t getDataSense() const;

    /*!
     * Disabling setting data sense. All variables must be set from header of the
     * input file.
     */
    void setDataSense(uint8_t) = delete;

    /*!
     * Get endianness value (0 LSB, 1 MSB).
     * @return the value of endianness.
     */
    uint8_t getEndianness() const;

    /*!
     * Disabling setting endianness. All variables must be set from header of the
     * input file.
     */
    void setEndianness(uint8_t) = delete;


    /*!
     * Get grouping data length value.
     * @return the value of grouping data length.
     */
    uint16_t getGroupingDataLength() const;

    /*!
     * Disabling setting grouping data length. All variables must be set from header of the
     * input file.
     */
    void setGroupingDataLength(uint16_t) = delete;

    /*!
     * Get the pointer of CCSDS header object.
     * @return the pointer of a CCSDS header object.
     */
    const iqzip_compression_header &getIqHeader() const;

    /*!
     * Disabling setting CCSDS header object. All variables must be set from header of the
     * input file.
     */
    void setIqHeader(const iqzip_compression_header &) = delete;

    /*!
     * Set the mapper type value.
     * @param the value of mapper type.
     */
    uint8_t getMapperType() const;

    /*!
     * Disabling setting mapper type. All variables must be set from header of the
     * input file.
     */
    void setMapperType(uint8_t) = delete;

    /*!
     * Get packet data length.
     * @return the value of packet data length.
     */
    uint16_t getPacketDataLength() const;

    /*!
     * Disabling setting packet data length. All variables must be set from header of the
     * input file.
     */
    void setPacketDataLength(uint16_t) = delete;

    /*!
     * Get packet sequence count value.
     * @return the value of packet sequence count.
     */
    uint16_t getPacketSequenceCount() const;

    /*!
     * Disabling setting packet sequence count. All variables must be set from header of the
     * input file.
     */
    void setPacketSequenceCount(uint16_t) = delete;

    /*!
     * Get predictor type.
     * @return the value of predictor type.
     */
    uint8_t getPredictorType() const;

    /*!
     * Disabling setting predictor type. All variables must be set from header of the
     * input file.
     */
    void setPredictorType(uint8_t) = delete;

    /*!
     * Get preprocessor status value.
     * @return the value of the preprocessor status.
     */
    uint8_t getPreprocessorStatus() const;

    /*!
     * Disabling setting preprocessor status. All variables must be set from header of the
     * input file.
     */
    void setPreprocessorStatus(uint8_t) = delete;

    /*!
     * Get the reference sample interval value.
     * @return the value of reference sample interval.
     */
    uint8_t getReferenceSampleInterval() const;

    /*!
     * Disabling setting reference sample interval. All variables must be set from header of the
     * input file.
     */
    void setReferenceSampleInterval(uint8_t) = delete;

    /*!
     * Get restricted codes value.
     * @return the value of restricted codes.
     */
    uint8_t getRestrictedCodes() const;

    /*!
     * Disabling setting restricted codes. All variables must be set from header of the
     * input file.
     */
    void setRestrictedCodes(uint8_t) = delete;

    /*!
     * Get sample resolution value.
     * @return the value of sample resolution.
     */
    uint8_t getSampleResolution() const;

    /*!
     * Disabling setting sample resolution. All variables must be set from header of the
     * input file.
     */
    void setSampleResolution(uint8_t) = delete;

    /*!
     * Get secondary header flag value.
     * @return the value of secondary header flag.
     */
    uint8_t getSecHdrFlag() const;

    /*!
     * Disabling setting secondary header flag. All variables must be set from header of the
     * input file.
     */
    void setSecHdrFlag(uint8_t) = delete;

    /*!
     * Get sequence flags value.
     * @return the value of secondary sequence flags.
     */
    uint8_t getSequenceFlags() const;

    /*!
     * Disabling setting sequence flags. All variables must be set from header of the
     * input file.
     */
    void setSequenceFlags(uint8_t) = delete;

    /*!
     * Get the pointer of aec stream object.
     * @return the pointer of aec stream object.
     */
    const aec_stream &getStrm() const;

    /*!
     * Disabling setting aec stream object pointer. All variables must be set from header of the
     * input file.
     */
    void setStrm(const aec_stream &) = delete;

    /*!
     * Get CIP type value.
     * @return the value of type.
     */
    uint8_t getType() const;

    /*!
     * Disabling setting type. All variables must be set from header of the
     * input file.
     */
    void setType(uint8_t type) = delete;

    /*!
     * Get CIP version value.
     * @return the value of version.
     */
    uint8_t getVersion() const;

    /*!
     * Disabling setting version. All variables must be set from header of the
     * input file.
     */
    void setVersion(uint8_t version) = delete;

    /*!
     * Get the pointer of input stream.
     * @return the pointer of input stream.
     */
    const std::ifstream &getInputStream() const;

    /*!
     * Get the pointer of output stream.
     * @return the pointer of output stream.
     */
    const std::ofstream &getOutputStream() const;

    /*!
     * Get the size of the CCSDS header of the file passed to iqzip_decompress_init.
     * @return the size of CCSDS header
     */
    size_t getHeaderSize() const;
};

}

}

#endif /* IQZIP_DECOMPRESSOR_H */
