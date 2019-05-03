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

#include <iqzip/iqzip_compressor.h>
#include <cstring>

namespace iqzip
{

  namespace compression
  {

    Iqzip_compressor::Iqzip_compressor (uint8_t d_version, uint8_t d_type,
                                        uint8_t d_sec_hdr_flag, uint16_t d_apid,
                                        uint8_t d_sequence_flags,
                                        uint16_t d_sequence_count,
                                        uint16_t d_packet_data_length,
                                        uint16_t d_grouping_data_length,
                                        uint8_t d_compression_tech_id,
                                        uint8_t d_reference_sample_interval,
                                        uint8_t d_preprocessor_status,
                                        uint8_t predictor_type,
                                        uint8_t mapper_type,
                                        uint16_t d_block_size,
                                        uint8_t d_data_sense,
                                        uint8_t d_sample_resolution,
                                        uint16_t d_cds_per_packet,
                                        uint8_t d_restricted_codes,
                                        uint8_t d_endianness) :
            Iqzip (d_version, d_type, d_sec_hdr_flag, d_apid, d_sequence_flags,
                   d_sequence_count, d_packet_data_length,
                   d_grouping_data_length, d_compression_tech_id,
                   d_reference_sample_interval, d_preprocessor_status,
                   predictor_type, mapper_type, d_block_size, d_data_sense,
                   d_sample_resolution, d_cds_per_packet, d_restricted_codes,
                   d_endianness),
	    d_tmp_stream(new char[STREAM_CHUNK]),
	    d_reference_samples_bytes((d_reference_sample_interval + 1) *
				       d_sample_resolution / 8 * d_block_size),
	    d_stream_avail_in(0),
	    d_out(new char[CHUNK]),
	    d_total_out(0)

    {
    }
    
    Iqzip_compressor::~Iqzip_compressor ()
    {
      d_iq_header.~iqzip_compression_header ();
      delete[] d_tmp_stream;
    }

    int
    Iqzip_compressor::iqzip_compress_init (const std::string fin,
                                           const std::string fout)
    {
      /* Initialize libaec stream */
      init_aec_stream ();
      /* Initialize libaec stream for compression */
      int status = aec_encode_init (&d_strm);
      if (status != AEC_OK) {
        std::cout << "Error in initializing stream" << std::endl;
        print_error (status);
      }

      /* Write header to compressed file */
      d_iq_header.write_header_to_file (fout);

      /* Open input & output file */
      input_stream.open (fin, std::ios::in | std::ios::binary);
      if (!input_stream.is_open ()) {
        std::cout << "Error opening input file" << std::endl;
        return -1;
      }
      output_stream.open (fout,
                          std::ios::out | std::ios::app | std::ios::binary);
      if (!output_stream.is_open ()) {
        std::cout << "Error opening output file" << std::endl;
        return -1;
      }

      return status;
    }

    int
    Iqzip_compressor::iqzip_compress ()
    {
      int total_out = 0;
      int input_avail = 1;
      int output_avail = 1;
      int status;
      char *in = new char[CHUNK];
      char *out = new char[CHUNK];

      d_strm.next_out = reinterpret_cast<unsigned char*> (out);

      while (input_avail || output_avail) {
        if (d_strm.avail_in == 0 && input_avail) {
          input_stream.read (in, CHUNK);
          d_strm.avail_in = input_stream.gcount ();
          if (d_strm.avail_in != CHUNK) {
            input_avail = 0;
          }
          d_strm.next_in = reinterpret_cast<const unsigned char*> (in);
        }

        status = aec_encode (&d_strm, AEC_NO_FLUSH);
        if (status != AEC_OK) {
          std::cout << "Error in encoding" << std::endl;
          print_error (status);
          return status;
        }

        if (d_strm.total_out - total_out > 0) {
          output_stream.write (out, d_strm.total_out - total_out);
          total_out = d_strm.total_out;
          output_avail = 1;
          d_strm.next_out = reinterpret_cast<unsigned char*> (out);
          d_strm.avail_out = CHUNK;
        }
        else {
          output_avail = 0;
        }
      }

      status = aec_encode (&d_strm, AEC_FLUSH);
      if (status != AEC_OK) {
        std::cout << "ERROR: while flushing output" << std::endl;
        print_error (status);
        return -1;
      }
      if (d_strm.total_out - total_out > 0) {
        output_stream.write (out, d_strm.total_out - total_out);
      }

      return 0;
    }

    int
    Iqzip_compressor::iqzip_stream_compress_init(const std::string fout) {
      /* Initialize libaec stream */
      init_aec_stream();
      /* Initialize libaec stream for compression */
      int status = aec_encode_init(&d_strm);
      if (status != AEC_OK) {
      	std::cout << "Error in initializing stream" << std::endl;
      	print_error(status);
      }
      
      /* Write header to compressed file */
      d_iq_header.write_header_to_file(fout);
      
      /* Open output file */
      output_stream.open(fout, std::ios::out | std::ios::app | std::ios::binary);
      if (!output_stream.is_open()) {
	std::cout << "Error opening output file" << std::endl;
	return -1;
      }

      return status;
    }

    int
    Iqzip_compressor::iqzip_stream_compress(const char* inbuf, size_t nbytes)
    {
      int status;
      /* Save input buffer to internal buffer */
      if (d_stream_avail_in + nbytes < STREAM_CHUNK){
	std::memcpy(&d_tmp_stream[d_stream_avail_in], inbuf, nbytes);
	d_stream_avail_in += nbytes;
	d_strm.avail_in = d_stream_avail_in;
	return AEC_OK;
      }
      /* d_stream_avail_in + nbytes >= STREAM_CHUNK */
      long int remainder = d_stream_avail_in + nbytes - STREAM_CHUNK;
      while (remainder >= 0) {
	std::memcpy(&d_tmp_stream[d_stream_avail_in], inbuf,
			STREAM_CHUNK - d_stream_avail_in);
	d_stream_avail_in = STREAM_CHUNK;
	/* Ready aec_stream */
	d_strm.avail_in = d_stream_avail_in;
	d_strm.next_out = reinterpret_cast<unsigned char*>(d_out);
	d_strm.next_in = reinterpret_cast<unsigned char*>(d_tmp_stream);

	status = aec_encode(&d_strm, AEC_NO_FLUSH);
	if (status != AEC_OK) {
	  std::cout << "Error in encoding" << std::endl;
	  print_error(status);
	  return status;
	}
	if (d_strm.total_out - d_total_out > 0) {
	  /* Write encoded output to file */
	  output_stream.write(d_out, d_strm.total_out - d_total_out);
	  d_strm.avail_out = CHUNK;
	  d_total_out = d_strm.total_out;
	}
	inbuf += nbytes - remainder;
	nbytes -= nbytes - remainder;
	d_stream_avail_in = 0;
	d_strm.avail_in = d_stream_avail_in;
	remainder = d_stream_avail_in + nbytes - STREAM_CHUNK;
      }
	/* If we have leftover samples copy them to stream */
      if (nbytes) {
	std::memcpy(&d_tmp_stream[d_stream_avail_in], inbuf, nbytes);
	d_stream_avail_in = nbytes;
	return AEC_OK;
      }  

      return AEC_OK;
    }

    int
    Iqzip_compressor::iqzip_compress_fin() 
    {
      int status;

      status = aec_encode_end(&d_strm);
      if (status != AEC_OK) {
	std::cout << "Error finishing stream" << std::endl;
	print_error(status);
	return status;
      }
      d_strm.next_in = nullptr;
      d_strm.next_out = nullptr;
      d_strm.state = nullptr;

      input_stream.close();
      output_stream.close();

      return 0;
}

    int
    Iqzip_compressor::iqzip_stream_compress_fin() 
    {
      int status;

      d_strm.next_out = reinterpret_cast<unsigned char*>(d_out);
      d_strm.next_in = reinterpret_cast<unsigned char*>(d_tmp_stream);
      d_strm.avail_in = d_stream_avail_in;

      status = aec_encode(&d_strm, AEC_FLUSH);
      if (status != AEC_OK) {
	std::cout << "ERROR: while flushing output" << std::endl;
	print_error(status);
	return -1;
      }
      if (d_strm.total_out - d_total_out > 0) {
	output_stream.write(d_out, d_strm.total_out - d_total_out);
      }

      status = aec_encode_end(&d_strm);
      if (status != AEC_OK) {
	std::cout << "Error finishing stream" << std::endl;
	print_error(status);
	return status;
      }

      d_strm.next_in = nullptr;
      d_strm.next_out = nullptr;
      d_strm.state = nullptr;

      input_stream.close();
      output_stream.close();

      return 0;
    }

    void
    Iqzip_compressor::print_error (int status)
    {
      switch (status)
        {
        case AEC_CONF_ERROR:
          std::cout << "Compressor: Configuration Error" << std::endl;
          break;
        case AEC_STREAM_ERROR:
          std::cout << "Compressor: Streaming Error" << std::endl;
          break;
        case AEC_DATA_ERROR:
          std::cout << "Compressor: Data Error" << std::endl;
          break;
        case AEC_MEM_ERROR:
          std::cout << "Compressor: Memory allocation Error" << std::endl;
          break;
        default:
          std::cout << "Compressor: Unknown Error" << std::endl;
        }
    }

    uint32_t
    Iqzip_compressor::getChunk () const
    {
      return Iqzip::getChunk ();
    }

    void
    Iqzip_compressor::setChunk (uint32_t chunk = 10485760)
    {
      Iqzip::setChunk (chunk);
    }

    uint16_t
    Iqzip_compressor::getApid () const
    {
      return Iqzip::getApid ();
    }

    void
    Iqzip_compressor::setApid (uint16_t apid)
    {
      Iqzip::setApid (apid);
    }

    uint16_t
    Iqzip_compressor::getBlockSize () const
    {
      return Iqzip::getBlockSize ();
    }

    void
    Iqzip_compressor::setBlockSize (uint16_t blockSize)
    {
      Iqzip::setBlockSize (blockSize);
    }

    uint16_t
    Iqzip_compressor::getCdsPerPacket () const
    {
      return Iqzip::getCdsPerPacket ();
    }

    void
    Iqzip_compressor::setCdsPerPacket (uint16_t cdsPerPacket)
    {
      Iqzip::setCdsPerPacket (cdsPerPacket);
    }

    uint8_t
    Iqzip_compressor::getCompressionTechId () const
    {
      return Iqzip::getCompressionTechId ();
    }

    void
    Iqzip_compressor::setCompressionTechId (uint8_t compressionTechId)
    {
      Iqzip::setCompressionTechId (compressionTechId);
    }

    uint8_t
    Iqzip_compressor::getDataSense () const
    {
      return Iqzip::getDataSense ();
    }

    void
    Iqzip_compressor::setDataSense (uint8_t dataSense)
    {
      Iqzip::setDataSense (dataSense);
    }

    uint8_t
    Iqzip_compressor::getEndianness () const
    {
      return Iqzip::getEndianness ();
    }

    void
    Iqzip_compressor::setEndianness (uint8_t endianness)
    {
      Iqzip::setEndianness (endianness);
    }

    uint16_t
    Iqzip_compressor::getGroupingDataLength () const
    {
      return Iqzip::getGroupingDataLength ();
    }

    void
    Iqzip_compressor::setGroupingDataLength (uint16_t groupingDataLength)
    {
      Iqzip::setGroupingDataLength (groupingDataLength);
    }

    const iqzip::compression::iqzip_compression_header&
    Iqzip_compressor::getIqHeader () const
    {
      return Iqzip::getIqHeader ();
    }

    void
    Iqzip_compressor::setIqHeader (
        const iqzip::compression::iqzip_compression_header& iqHeader)
    {
      Iqzip::setIqHeader (iqHeader);
    }

    uint8_t
    Iqzip_compressor::getMapperType () const
    {
      return Iqzip::getMapperType ();
    }

    void
    Iqzip_compressor::setMapperType (uint8_t mapperType)
    {
      Iqzip::setMapperType (mapperType);
    }

    uint16_t
    Iqzip_compressor::getPacketDataLength () const
    {
      return Iqzip::getPacketDataLength ();
    }

    void
    Iqzip_compressor::setPacketDataLength (uint16_t packetDataLength)
    {
      Iqzip::setPacketDataLength (packetDataLength);
    }

    uint16_t
    Iqzip_compressor::getPacketSequenceCount () const
    {
      return Iqzip::getPacketSequenceCount ();
    }

    void
    Iqzip_compressor::setPacketSequenceCount (uint16_t packetSequenceCount)
    {
      Iqzip::setPacketSequenceCount (packetSequenceCount);
    }

    uint8_t
    Iqzip_compressor::getPredictorType () const
    {
      return Iqzip::getPredictorType ();
    }

    void
    Iqzip_compressor::setPredictorType (uint8_t predictorType)
    {
      Iqzip::setPredictorType (predictorType);
    }

    uint8_t
    Iqzip_compressor::getPreprocessorStatus () const
    {
      return Iqzip::getPreprocessorStatus ();
    }

    void
    Iqzip_compressor::setPreprocessorStatus (uint8_t preprocessorStatus)
    {
      Iqzip::setPreprocessorStatus (preprocessorStatus);
    }

    uint8_t
    Iqzip_compressor::getReferenceSampleInterval () const
    {
      return Iqzip::getReferenceSampleInterval ();
    }

    void
    Iqzip_compressor::setReferenceSampleInterval (
        uint8_t referenceSampleInterval)
    {
      Iqzip::setReferenceSampleInterval (referenceSampleInterval);
    }

    uint8_t
    Iqzip_compressor::getRestrictedCodes () const
    {
      return Iqzip::getRestrictedCodes ();
    }

    void
    Iqzip_compressor::setRestrictedCodes (uint8_t restrictedCodes)
    {
      Iqzip::setRestrictedCodes (restrictedCodes);
    }

    uint8_t
    Iqzip_compressor::getSampleResolution () const
    {
      return Iqzip::getSampleResolution ();
    }

    void
    Iqzip_compressor::setSampleResolution (uint8_t sampleResolution)
    {
      Iqzip::setSampleResolution (sampleResolution);
    }

    uint8_t
    Iqzip_compressor::getSecHdrFlag () const
    {
      return Iqzip::getSecHdrFlag ();
    }

    void
    Iqzip_compressor::setSecHdrFlag (uint8_t secHdrFlag)
    {
      Iqzip::setSecHdrFlag (secHdrFlag);
    }

    uint8_t
    Iqzip_compressor::getSequenceFlags () const
    {
      return Iqzip::getSequenceFlags ();
    }

    void
    Iqzip_compressor::setSequenceFlags (uint8_t sequenceFlags)
    {
      Iqzip::setSequenceFlags (sequenceFlags);
    }

    const aec_stream&
    Iqzip_compressor::getStrm () const
    {
      return Iqzip::getStrm ();
    }

    void
    Iqzip_compressor::setStrm (const aec_stream& strm)
    {
      Iqzip::setStrm (strm);
    }

    uint8_t
    Iqzip_compressor::getType () const
    {
      return Iqzip::getType ();
    }

    void
    Iqzip_compressor::setType (uint8_t type)
    {
      Iqzip::setType (type);
    }

    uint8_t
    Iqzip_compressor::getVersion () const
    {
      return Iqzip::getVersion ();
    }

    void
    Iqzip_compressor::setVersion (uint8_t version)
    {
      Iqzip::setVersion (version);
    }

    const std::ifstream&
    Iqzip_compressor::getInputStream () const
    {
      return Iqzip::getInputStream ();
    }

    const std::ofstream&
    Iqzip_compressor::getOutputStream () const
    {
      return Iqzip::getOutputStream ();
    }

  }
}
