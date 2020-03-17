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

/*!
 * \brief Compressor implementation
 *
 * Iqzip_compressor is the class responsible for compressing files using
 * libaec. It is a subclass of Iqzip and implements the necessary functions
 * for compression.
 */

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <memory>

namespace iqzip {

namespace compression {

class compressor;
typedef std::shared_ptr<compressor> compressor_sptr;

class compressor {

public:

    virtual ~compressor() {};

    /*!
     * Initializes necessary variables for compression. Should always be called
     * before compressing. It opens the fin and fout files. The CCSDS header is
     * written to the file specified by fout.
     * @param fin Name of input file.
     * @param fout Name of output file.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int compress_init(const std::string fin, const std::string fout) = 0;

    /*!
     * Initializes necessary variables for stream compression. Should always be called
     * before stream compressing. It opens the fout file and the CCSDS header is written.
     * @param fout Name of output file.
     * @return 0 on succes, != 0 otherwise.
     */
    virtual int stream_compress_init(const std::string fout) = 0;

    /*!
     * Reads the input file given in compress_init, compresses it, and
     * writes the results to fout given in compress_init.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int compress() = 0;

    /*!
     * Reads the buffer inbuf and when block size samples are read, compresses
     * and writes them to fout file given in stream_compress_init.
     * The bytes passed to stream compress must be a multiple of sample size.
     * @param inbuf buffer to read samples from.
     * @param nbytes number of bytes to read from buffer.
     * @return 0 on success, !=0 otherwise.
     */
    virtual int stream_compress(const char *inbuf, size_t nbytes) = 0;

    /*!
     * Finalizes the compression and clears internal variables. Should always be called
     * after compress otherwise output file may not be written correctly.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int compress_fin() = 0;

    /*!
     * Finalizes the compression and clears internal variables. Should always be called
     * after stream_compress otherwise output file may not be written correctly.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int stream_compress_fin() = 0;

};

/*!
 * Instantiates a compressor class object given all the required parameters for
 * the CCSDS CIP header and the compression algorithm.
 * @param version CIP packet version.
 * @param type CIP type version.
 * @param sec_hdr_flag CIP secondary header flag.
 * @param apid CIP application ID.
 * @param sequence_flags CIP sequence flags.
 * @param sequence_count CIP sequence count.
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
 * @return a std::shared_ptr<compressor>
 */
compressor_sptr create_compressor(uint8_t version, uint8_t type,
                                  uint8_t sec_hdr_flag,
                                  uint16_t apid, uint8_t sequence_flags,
                                  uint16_t sequence_count, uint16_t packet_data_length,
                                  uint16_t grouping_data_length, uint8_t compression_tech_id,
                                  uint8_t reference_sample_interval, uint8_t preprocessor_status,
                                  uint8_t predictor_type, uint8_t mapper_type, uint16_t block_size,
                                  uint8_t data_sense, uint8_t sample_resolution,
                                  uint16_t cds_per_packet, uint8_t restricted_codes,
                                  uint8_t endianness);
} // namspace compression
} // namespace iqzip

#endif /* COMPRESSOR_H */
