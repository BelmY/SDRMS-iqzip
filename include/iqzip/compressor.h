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

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <memory>

namespace compression {

class compressor;
typedef std::shared_ptr<compressor> compressor_sptr;

class compressor {

public:

    virtual ~compressor() {};

    /*!
     * Initializes necessary variables for compression. Should always be called
     * before compressing. It opens the fin and fout files. The CCSDS header is
     * written to the file specified by fout. The aec_stream parameter used by
     * libaec is also initialized.
     * @param fin Name of input file.
     * @param fout Name of output file.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int compress_init(const std::string fin, const std::string fout) = 0;

    /*!
     * Initializes necessary variables for stream compression. Should always be called
     * before stream compressing. It opens the fout file and the CCSDS header is written.
     * The aec_stream parameter used by libaec is also initialized.
     * @param fout Name of output file.
     * @return 0 on succes, != 0 otherwise.
     */
    virtual int stream_compress_init(const std::string fout) = 0;

    /*!
     * Reads the input file given in iqzip_compress_init, compresses it, and
     * writes the results to fout given in iqzip_compress_init.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int compress() = 0;

    /*!
     * Reads the buffer inbuf and when block size samples are read, compresses
     * and writes them to fout file given in iqzip_stream_compress_init.
     * The bytes passed to iqzip_stream compress must be a multiple of sample size.
     * @param inbuf buffer to read samples from.
     * @param nbytes number of bytes to read from buffer.
     * @return 0 on success, !=0 otherwise.
     */
    virtual int stream_compress(const char *inbuf, size_t nbytes) = 0;

    /*!
     * Finalizes the compression and clears aec_stream. Should always be called
     * after iqzip_compress otherwise output file may not be written correctly.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int compress_fin() = 0;

    /*!
     * Finalizes the compression and clears aec_stream. Should always be called
     * after iqzip_stream_compress otherwise output file may not be written correctly.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int stream_compress_fin() = 0;

};

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
}

#endif /* COMPRESSOR_H */
