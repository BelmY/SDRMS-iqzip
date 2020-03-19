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

#ifndef META_H_
#define META_H_

#include <memory>

namespace iqzip {

namespace metafile {

class meta;
typedef std::shared_ptr<meta> meta_sptr;

/*!
 *
 * \ingroup sigmf_meta
 *
 * The iqzip_meta class works as a wrapper for the libsigmf library, that provides an
 * API to parse or generate JSON metafiles that conform to the Signal Metafile
 * Format.
 *
 * This class incorporates an abstraction to the libsigmf API, and provides
 * the appropriate setters and getters to access the internal fields of
 * a SigMF object, alongside with methods to parse or write into SigMF JSON files.
 * In addition, thanks to the mindful implementation of libsigmf, multiple
 * SigMF namespaces can be supported.
 */
class meta {

public:

    virtual ~meta() {};

    virtual void set_global_datatype(std::string datatype) = 0;

    virtual void set_global_sample_rate(double sample_rate) = 0;

    virtual void set_global_version(std::string version) = 0;

    virtual void set_global_sha512(std::string sha512) = 0;

    virtual void set_global_offset(uint32_t offset) = 0;

    virtual void set_global_description(std::string description) = 0;

    virtual void set_global_author(std::string author) = 0;

    virtual void set_global_meta_doi(std::string meta_doi) = 0;

    virtual void set_global_data_doi(std::string data_doi) = 0;

    virtual void set_global_recorder(std::string recorder) = 0;

    virtual void set_global_license(std::string license) = 0;

    virtual void set_global_hw(std::string hw) = 0;

    virtual std::string get_global_datatype() = 0;

    virtual double get_global_sample_rate() = 0;

    virtual std::string get_global_version() = 0;

    virtual std::string get_global_sha512() = 0;

    virtual uint32_t get_global_offset() = 0;

    virtual std::string get_global_description() = 0;

    virtual std::string get_global_author() = 0;

    virtual std::string get_global_meta_doi() = 0;

    virtual std::string get_global_data_doi() = 0;

    virtual std::string get_global_recorder() = 0;

    virtual std::string get_global_license() = 0;

    virtual std::string get_global_hw() = 0;

    virtual void append_capture_segment(uint64_t sample_start) = 0;

    virtual void
    append_capture_segment(uint64_t sample_start, uint64_t global_index,
                           double frequency, std::string datetime) = 0;

    virtual void
    append_capture_segment(std::string capture_json) = 0;

    virtual void
    set_capture_global_index(uint64_t sample_start, uint64_t global_index) = 0;

    virtual void
    set_capture_frequency(uint64_t sample_start, double frequency) = 0;

    virtual void
    set_capture_datetime(uint64_t sample_start, std::string datetime) = 0;

    virtual uint64_t get_capture_global_index(uint64_t sample_start) = 0;

    virtual double get_capture_frequency(uint64_t sample_start) = 0;

    virtual std::string get_capture_datetime(uint64_t sample_start) = 0;

    virtual void
    append_annotation_segment(uint64_t sample_start, uint64_t sample_count) = 0;

    virtual void
    append_annotation_segment(uint64_t sample_start, uint64_t sample_count,
                              std::string generator, std::string comment,
                              double freq_lower_edge,
                              double freq_upper_edge) = 0;

    virtual void
    append_annotation_segment(std::string annotation_json) = 0;

    virtual void write_json(std::string filename) = 0;

    virtual void parse_json(std::string filename) = 0;

    virtual void
    create_archive(const std::string tar_filename, const std::string src_file) = 0;

};

meta_sptr create_iqzip_meta(std::string datatype, std::string version);
meta_sptr create_iqzip_meta(std::string datatype, double sample_rate,
                            std::string version,
                            std::string sha512, uint32_t offset, std::string description,
                            std::string author, std::string meta_impl_doi,
                            std::string data_doi, std::string recorder,
                            std::string license, std::string hw);

} //namespace metafile

} //namespace iqzip

#endif /* META_H_ */
