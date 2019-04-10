#include "iqzip/iqzip_compressor.h"

Iqzip_compressor::Iqzip_compressor(uint8_t d_version, uint8_t d_type,
		uint8_t d_sec_hdr_flag, uint16_t d_apid, uint8_t d_sequence_flags,
		uint16_t d_sequence_count, uint16_t d_packet_data_length,
		uint16_t d_grouping_data_length, uint8_t d_compression_tech_id,
		uint8_t d_reference_sample_interval, uint8_t d_preprocessor_status,
		uint8_t predictor_type, uint8_t mapper_type, uint16_t d_block_size,
		uint8_t d_data_sense, uint8_t d_sample_resolution,
		uint16_t d_cds_per_packet, uint8_t d_restricted_codes,
		uint8_t d_endianness) :
		Iqzip(d_version, d_type, d_sec_hdr_flag, d_apid, d_sequence_flags,
				d_sequence_count, d_packet_data_length, d_grouping_data_length,
				d_compression_tech_id, d_reference_sample_interval,
				d_preprocessor_status, predictor_type, mapper_type,
				d_block_size, d_data_sense, d_sample_resolution,
				d_cds_per_packet, d_restricted_codes, d_endianness) {
}

Iqzip_compressor::~Iqzip_compressor() {
	d_iq_header.~iqzip_compression_header();
}

int
Iqzip_compressor::iqzip_compress_init(const std::string fin,
									  const std::string fout) {
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

	/* Open input & output file */
	input_stream.open(fin, std::ios::in | std::ios::binary);
	if (!input_stream.is_open()) {
		std::cout << "Error opening input file" << std::endl;
		return -1;
	}
	output_stream.open(fout, std::ios::out | std::ios::app | std::ios::binary);
	if (!output_stream.is_open()) {
		std::cout << "Error opening output file" << std::endl;
		return -1;
	}

	return status;
}

int
Iqzip_compressor::iqzip_compress() {
	int total_out = 0;
	int input_avail = 1;
	int output_avail = 1;
	int status;
	char *in = new char[CHUNK];
	char *out = new char[CHUNK];

	d_strm.next_out = reinterpret_cast<unsigned char*>(out);

	while (input_avail || output_avail) {
		if (d_strm.avail_in == 0 && input_avail) {
			input_stream.read(in, CHUNK);
			d_strm.avail_in = input_stream.gcount();
			if (d_strm.avail_in != CHUNK) {
				input_avail = 0;
			}
			d_strm.next_in = reinterpret_cast<const unsigned char*>(in);
		}

		status = aec_encode(&d_strm, AEC_NO_FLUSH);
		if (status != AEC_OK) {
			std::cout << "Error in encoding" << std::endl;
			print_error(status);
			return status;
		}

		if (d_strm.total_out - total_out > 0) {
			output_stream.write(out, d_strm.total_out - total_out);
			total_out = d_strm.total_out;
			output_avail = 1;
			d_strm.next_out = reinterpret_cast<unsigned char*>(out);
			d_strm.avail_out = CHUNK;
		}
		else {
			output_avail = 0;
		}
	}

	status = aec_encode(&d_strm, AEC_FLUSH);
	if (status != AEC_OK) {
		std::cout << "ERROR: while flushing output" << std::endl;
		print_error(status);
		return -1;
	}
	if (d_strm.total_out - total_out > 0) {
		output_stream.write(out, d_strm.total_out - total_out);
	}

	return 0;
}

int
Iqzip_compressor::iqzip_compress_fin() {
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

void
Iqzip_compressor::print_error(int status) {
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
