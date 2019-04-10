#include <iqzip/iqzip_decompressor.h>

Iqzip_decompressor::Iqzip_decompressor () :
	Iqzip(),
	d_iqzip_header_size (0) {
}

Iqzip_decompressor::~Iqzip_decompressor() {}

int
Iqzip_decompressor::iqzip_decompress_init (const std::string fin,
											const std::string fout) {
	/* Read header and save options to class fields */
	d_iqzip_header_size = d_iq_header.parse_header_from_file(fin);
	d_version = d_iq_header.decode_version();
	d_type = d_iq_header.decode_type();
	d_sec_hdr_flag = d_iq_header.decode_secondary_header_flag();
	d_apid = d_iq_header.decode_appplication_process_identifier();
	d_sequence_flags = d_iq_header.decode_sequence_flags();
	d_packet_sequence_count = d_iq_header.decode_packet_sequence_count();
	d_packet_data_length = d_iq_header.decode_packet_data_length();
	d_grouping_data_length = d_iq_header.decode_grouping_data_length();
	d_compression_tech_id = d_iq_header.decode_compression_technique_id();
	d_reference_sample_interval = d_iq_header.decode_reference_sample_interval();
	d_preprocessor_status = d_iq_header.decode_preprocessor_status();
	d_predictor_type = d_iq_header.decode_preprocessor_predictor_type();
	d_mapper_type = d_iq_header.decode_preprocessor_mapper_type();
	d_block_size = d_iq_header.decode_block_size();
	d_data_sense = d_iq_header.decode_preprocessor_data_sense();
	d_sample_resolution = d_iq_header.decode_preprocessor_sample_resolution();
	d_cds_per_packet = 0;
	d_restricted_codes =
			d_iq_header.decode_extended_parameters_restricted_code_option();
	d_endianness = d_iq_header.decode_iqzip_header_endianess();

    /* Initialize libaec stream */
    init_aec_stream();

	/* Open input & output file */
	input_stream.open(fin, std::ios::in | std::ios::binary);
	if (!input_stream.is_open()) {
		std::cout << "Error opening input file" << std::endl;
		return -1;
	}
	output_stream.open(fout, std::ios::out | std::ios::binary);
	if (!output_stream.is_open()) {
		std::cout << "Error opening output file" << std::endl;
		return -1;
	}

	/* Skip header from file */
	input_stream.seekg(d_iqzip_header_size);

	/* Initialize libaec stream for decompression */
	int status = aec_decode_init(&d_strm);
	if (status != AEC_OK) {
		std::cout << "Error in initializing stream" << std::endl;
		print_error(status);
	}
	return status;
}

int
Iqzip_decompressor::iqzip_decompress() {
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

		status = aec_decode(&d_strm, AEC_NO_FLUSH);
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
	return 0;
}

int
Iqzip_decompressor::iqzip_decompress_fin() {
	int status;
	status = aec_decode_end(&d_strm);
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
Iqzip_decompressor::print_error(int status) {
    switch (status)
    {
    case AEC_CONF_ERROR:
        std::cout << "Decompressor: Configuration Error" << std::endl;
        break;
    case AEC_STREAM_ERROR:
        std::cout << "Decompressor: Streaming Error" << std::endl;
        break;
    case AEC_DATA_ERROR:
        std::cout << "Decompressor: Data Error" << std::endl;
        break;
    case AEC_MEM_ERROR:
        std::cout << "Decompressor: Memory allocation Error" << std::endl;
        break;
    default:
        std::cout << "Decompressor: Unknown Error" << std::endl;
    }
}
