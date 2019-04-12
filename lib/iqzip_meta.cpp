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

#include "iqzip/iqzip_meta.h"

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <fstream>

namespace iqzip
{

  namespace meta
  {

    iqzip_meta::iqzip_meta ()
    {
    }

    iqzip_meta::iqzip_meta (std::string datatype, std::string version)
    {
      initialize_global (datatype, version);
    }

    iqzip_meta::iqzip_meta (std::string datatype, double sample_rate,
                            std::string version, std::string sha512,
                            uint32_t offset, std::string description,
                            std::string author, std::string meta_doi,
                            std::string data_doi, std::string recorder,
                            std::string license, std::string hw)
    {
      initialize_global (datatype, sample_rate, version, sha512, offset,
                         description, author, meta_doi, data_doi, recorder,
                         license, hw);
    }

    iqzip_meta::~iqzip_meta ()
    {
    }

    void
    iqzip_meta::set_global_datatype (std::string datatype)
    {
      d_sigmf.global.access<core::GlobalT> ().datatype = datatype;
    }

    void
    iqzip_meta::set_global_sample_rate (double sample_rate)
    {
      d_sigmf.global.access<core::GlobalT> ().sample_rate = sample_rate;
    }

    void
    iqzip_meta::set_global_version (std::string version)
    {
      d_sigmf.global.access<core::GlobalT> ().version = version;
    }

    void
    iqzip_meta::set_global_sha512 (std::string sha512)
    {
      d_sigmf.global.access<core::GlobalT> ().sha512 = sha512;
    }

    void
    iqzip_meta::set_global_offset (uint32_t offset)
    {
      d_sigmf.global.access<core::GlobalT> ().offset = offset;
    }

    void
    iqzip_meta::set_global_description (std::string description)
    {
      d_sigmf.global.access<core::GlobalT> ().description = description;
    }

    void
    iqzip_meta::set_global_author (std::string author)
    {
      d_sigmf.global.access<core::GlobalT> ().author = author;
    }

    void
    iqzip_meta::set_global_meta_doi (std::string meta_doi)
    {
      d_sigmf.global.access<core::GlobalT> ().meta_doi = meta_doi;
    }

    void
    iqzip_meta::set_global_data_doi (std::string data_doi)
    {
      d_sigmf.global.access<core::GlobalT> ().data_doi = data_doi;
    }

    void
    iqzip_meta::set_global_recorder (std::string recorder)
    {
      d_sigmf.global.access<core::GlobalT> ().recorder = recorder;
    }

    void
    iqzip_meta::set_global_license (std::string license)
    {
      d_sigmf.global.access<core::GlobalT> ().license = license;
    }

    void
    iqzip_meta::set_global_hw (std::string hw)
    {
      d_sigmf.global.access<core::GlobalT> ().hw = hw;
    }

    std::string
    iqzip_meta::get_global_datatype ()
    {
      return d_sigmf.global.access<core::GlobalT> ().datatype;
    }

    double
    iqzip_meta::get_global_sample_rate ()
    {
      return d_sigmf.global.access<core::GlobalT> ().sample_rate;
    }

    std::string
    iqzip_meta::get_global_version ()
    {
      return d_sigmf.global.access<core::GlobalT> ().version;
    }

    std::string
    iqzip_meta::get_global_sha512 ()
    {
      return d_sigmf.global.access<core::GlobalT> ().sha512;
    }

    uint32_t
    iqzip_meta::get_global_offset ()
    {
      return d_sigmf.global.access<core::GlobalT> ().offset;
    }

    std::string
    iqzip_meta::get_global_description ()
    {
      return d_sigmf.global.access<core::GlobalT> ().description;
    }

    std::string
    iqzip_meta::get_global_author ()
    {
      return d_sigmf.global.access<core::GlobalT> ().author;
    }

    std::string
    iqzip_meta::get_global_meta_doi ()
    {
      return d_sigmf.global.access<core::GlobalT> ().meta_doi;
    }

    std::string
    iqzip_meta::get_global_data_doi ()
    {
      return d_sigmf.global.access<core::GlobalT> ().data_doi;
    }

    std::string
    iqzip_meta::get_global_recorder ()
    {
      return d_sigmf.global.access<core::GlobalT> ().recorder;
    }

    std::string
    iqzip_meta::get_global_license ()
    {
      return d_sigmf.global.access<core::GlobalT> ().license;
    }

    std::string
    iqzip_meta::get_global_hw ()
    {
      return d_sigmf.global.access<core::GlobalT> ().hw;
    }

    void
    iqzip_meta::append_capture_segment (uint64_t sample_start)
    {
      auto new_capture = sigmf::VariadicDataClass<core::CaptureT> ();
      new_capture.access<core::CaptureT> ().sample_start = sample_start;
      d_sigmf.captures.emplace_back (new_capture);
    }

    void
    iqzip_meta::append_capture_segment (uint64_t sample_start,
                                        uint64_t global_index, double frequency,
                                        std::string datetime)
    {
      auto new_capture = sigmf::VariadicDataClass<core::CaptureT> ();
      new_capture.access<core::CaptureT> ().sample_start = sample_start;
      new_capture.access<core::CaptureT> ().global_index = global_index;
      new_capture.access<core::CaptureT> ().frequency = frequency;
      new_capture.access<core::CaptureT> ().datetime = datetime;
      d_sigmf.captures.emplace_back (new_capture);
    }

    void
    iqzip_meta::set_capture_global_index (uint64_t sample_start,
                                          uint64_t global_index)
    {
      for (auto& i : d_sigmf.captures) {
        if (i.access<core::CaptureT> ().sample_start == sample_start) {
          i.access<core::CaptureT> ().global_index = global_index;
        }
      }
    }

    void
    iqzip_meta::set_capture_frequency (uint64_t sample_start, double frequency)
    {
      for (auto& i : d_sigmf.captures) {
        if (i.access<core::CaptureT> ().sample_start == sample_start) {
          i.access<core::CaptureT> ().frequency = frequency;
        }
      }
    }

    void
    iqzip_meta::set_capture_datetime (uint64_t sample_start,
                                      std::string datetime)
    {
      for (auto& i : d_sigmf.captures) {
        if (i.access<core::CaptureT> ().sample_start == sample_start) {
          i.access<core::CaptureT> ().datetime = datetime;
        }
      }
    }

    uint64_t
    iqzip_meta::get_capture_global_index (uint64_t sample_start)
    {
      for (auto& i : d_sigmf.captures) {
        if (i.access<core::CaptureT> ().sample_start == sample_start) {
          return i.access<core::CaptureT> ().global_index;
        }
      }
      return 0;
    }

    double
    iqzip_meta::get_capture_frequency (uint64_t sample_start)
    {
      for (auto& i : d_sigmf.captures) {
        if (i.access<core::CaptureT> ().sample_start == sample_start) {
          return i.access<core::CaptureT> ().frequency;
        }
      }
      return 0;
    }

    std::string
    iqzip_meta::get_capture_datetime (uint64_t sample_start)
    {
      for (auto& i : d_sigmf.captures) {
        if (i.access<core::CaptureT> ().sample_start == sample_start) {
          return i.access<core::CaptureT> ().datetime;
        }
      }
      return 0;
    }

    void
    iqzip_meta::append_annotation_segment (uint64_t sample_start,
                                           uint64_t sample_count)
    {
      auto new_annotation = sigmf::VariadicDataClass<core::AnnotationT> ();
      new_annotation.access<core::AnnotationT> ().start_sample = sample_start;
      new_annotation.access<core::AnnotationT> ().sample_count = sample_count;
      d_sigmf.annotations.emplace_back (new_annotation);
    }

    void
    iqzip_meta::append_annotation_segment (uint64_t sample_start,
                                           uint64_t sample_count,
                                           std::string generator,
                                           std::string comment,
                                           double freq_lower_edge,
                                           double freq_upper_edge)
    {
      auto new_annotation = sigmf::VariadicDataClass<core::AnnotationT> ();
      new_annotation.access<core::AnnotationT> ().start_sample = sample_start;
      new_annotation.access<core::AnnotationT> ().sample_count = sample_count;
      new_annotation.access<core::AnnotationT> ().generator = generator;
      new_annotation.access<core::AnnotationT> ().comment = comment;
      new_annotation.access<core::AnnotationT> ().freq_lower_edge =
          freq_lower_edge;
      new_annotation.access<core::AnnotationT> ().freq_upper_edge =
          freq_upper_edge;
      d_sigmf.annotations.emplace_back (new_annotation);
    }

    void
    iqzip_meta::write_json (std::string filename)
    {
      std::ofstream f(filename);
      std::stringstream json_output;
      json_output << json(d_sigmf).dump(2) << std::flush;
      f << json_output.rdbuf();
      f.close();
    }

    void
    iqzip_meta::parse_json(std::string filename) {
      std::ifstream f(filename);
      std::stringstream json_output;
      json_output << f.rdbuf();
      auto as_json = json::parse(json_output);
      d_sigmf = as_json;
    }

    sigmf::VariadicDataClass<core::GlobalT>&
    iqzip_meta::get_sigmf_global_segment ()
    {
      return d_sigmf.global;
    }

    sigmf::SigMFVector<sigmf::VariadicDataClass<core::CaptureT>>&
    iqzip_meta::get_sigmf_captures_segment ()
    {
      return d_sigmf.captures;
    }

    sigmf::SigMFVector<sigmf::VariadicDataClass<core::AnnotationT>>&
    iqzip_meta::get_sigmf_annotations_segment ()
    {
      return d_sigmf.annotations;
    }

    sigmf::SigMF<sigmf::VariadicDataClass<core::GlobalT>,
        sigmf::VariadicDataClass<core::CaptureT>,
        sigmf::VariadicDataClass<core::AnnotationT>>&
    iqzip_meta::get_sigmf ()
    {
      return d_sigmf;
    }

    void
    iqzip_meta::initialize_global (std::string datatype, std::string version)
    {
      d_sigmf.global.access<core::GlobalT> ().datatype = datatype;
      d_sigmf.global.access<core::GlobalT> ().version = version;
    }

    void
    iqzip_meta::initialize_global (std::string datatype, double sample_rate,
                                   std::string version, std::string sha512,
                                   uint32_t offset, std::string description,
                                   std::string author, std::string meta_doi,
                                   std::string data_doi, std::string recorder,
                                   std::string license, std::string hw)
    {
      d_sigmf.global.access<core::GlobalT> ().datatype = datatype;
      d_sigmf.global.access<core::GlobalT> ().sample_rate = sample_rate;
      d_sigmf.global.access<core::GlobalT> ().version = version;
      d_sigmf.global.access<core::GlobalT> ().sha512 = sha512;
      d_sigmf.global.access<core::GlobalT> ().offset = offset;
      d_sigmf.global.access<core::GlobalT> ().description = description;
      d_sigmf.global.access<core::GlobalT> ().author = author;
      d_sigmf.global.access<core::GlobalT> ().meta_doi = meta_doi;
      d_sigmf.global.access<core::GlobalT> ().data_doi = data_doi;
      d_sigmf.global.access<core::GlobalT> ().recorder = recorder;
      d_sigmf.global.access<core::GlobalT> ().license = license;
      d_sigmf.global.access<core::GlobalT> ().hw = hw;
    }

  }
}
