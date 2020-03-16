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

#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <memory>

namespace compression {

class decompressor;
typedef std::shared_ptr<decompressor> decompressor_sptr;

class decompressor {
private:
    size_t d_iqzip_header_size;
    uint32_t STREAM_CHUNK = 32768;
    char *d_tmp_stream;
    size_t d_stream_avail_in;
    char *d_out;
    size_t d_total_out;

public:

    /*!
     * Default destructor. Calls iqzip_compression_header destructor.
     */
    virtual ~decompressor() {};

    /*!
     * Initializes necessary variables for decompression. Should always be called
     * before decompressing. It opens the fin and fout files. The CCSDS header is
     * read from the file specified by fin. The aec_stream parameter used by
     * libaec is also initialized.
     * @param fin Name of input file.
     * @param fout Name of output file.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int decompress_init(const std::string fin, const std::string fout) = 0;

    /*!
     * Reads the input file given in iqzip_decompress_init, decompresses it, and
     * writes the results to fout given in iqzip_decompress_init.
     * @return 0 on succes, != 0 otherwise.
     */
    virtual int decompress() = 0;

    /*!
     * Reads the input file given in iqzip_decompress_init, decompresses it,
     * and writes the results to fout given in iqzip_decompress_init.
     * @param inbuf buffer to read samples from.
     * @param nbytes number of bytes to read from buffer.
     * @return 0 on success, !=0 otherwise.
     */
    virtual int stream_decompress(const char *inbuf, size_t nbytes) = 0;

    /*!
     * Finalizes the decompression and clears aec_stream. Should always be called
     * after iqzip_decompress otherwise output file may not be written correctly.
     * @return 0 on success, != 0 otherwise.
     */
    virtual int decompress_fin() = 0;

    /*!
     * Finalizes the decompression and clears aec_stream. Should always be called
     * after iqzip_stream_decompress otherwise output file may not be written correctly.
     * @return
     */
    virtual int stream_decompress_fin() = 0;
};

decompressor_sptr create_decompressor();

}

#endif /* IQZIP_DECOMPRESSOR_H */
