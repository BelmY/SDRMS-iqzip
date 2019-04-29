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

#ifndef IQZIP_META_H_
#define IQZIP_META_H_

#include <stdint.h>
#include <string>

#include <sigmf_core_generated.h>
#include <sigmf_antenna_generated.h>
#include <sigmf.h>

namespace iqzip
{

  namespace meta
  {

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
    class iqzip_meta
    {

    public:

      iqzip_meta ();

      iqzip_meta (std::string datatype, std::string version);

      iqzip_meta (std::string datatype, double sample_rate, std::string version,
                  std::string sha512, uint32_t offset, std::string description,
                  std::string author, std::string meta_doi,
                  std::string data_doi, std::string recorder,
                  std::string license, std::string hw);

      virtual
      ~iqzip_meta ();

      void
      set_global_datatype (std::string datatype);

      void
      set_global_sample_rate (double sample_rate);

      void
      set_global_version (std::string version);

      void
      set_global_sha512 (std::string sha512);

      void
      set_global_offset (uint32_t offset);

      void
      set_global_description (std::string description);

      void
      set_global_author (std::string author);

      void
      set_global_meta_doi (std::string meta_doi);

      void
      set_global_data_doi (std::string data_doi);

      void
      set_global_recorder (std::string recorder);

      void
      set_global_license (std::string license);

      void
      set_global_hw (std::string hw);

      std::string
      get_global_datatype ();

      double
      get_global_sample_rate ();

      std::string
      get_global_version ();

      std::string
      get_global_sha512 ();

      uint32_t
      get_global_offset ();

      std::string
      get_global_description ();

      std::string
      get_global_author ();

      std::string
      get_global_meta_doi ();

      std::string
      get_global_data_doi ();

      std::string
      get_global_recorder ();

      std::string
      get_global_license ();

      std::string
      get_global_hw ();

      void
      append_capture_segment (uint64_t sample_start);

      void
      append_capture_segment (uint64_t sample_start, uint64_t global_index,
                              double frequency, std::string datetime);

      void
      set_capture_global_index (uint64_t sample_start, uint64_t global_index);

      void
      set_capture_frequency (uint64_t sample_start, double frequency);

      void
      set_capture_datetime (uint64_t sample_start, std::string datetime);

      uint64_t
      get_capture_global_index (uint64_t sample_start);

      double
      get_capture_frequency (uint64_t sample_start);

      std::string
      get_capture_datetime (uint64_t sample_start);

      void
      append_annotation_segment (uint64_t sample_start, uint64_t sample_count);

      void
      append_annotation_segment (uint64_t sample_start, uint64_t sample_count,
                                 std::string generator, std::string comment,
                                 double freq_lower_edge,
                                 double freq_upper_edge);

      void
      write_json (std::string filename);

      void
      parse_json (std::string filename);

      sigmf::VariadicDataClass<core::GlobalT>&
      get_sigmf_global_segment ();

      sigmf::SigMFVector<sigmf::VariadicDataClass<core::CaptureT>>&
      get_sigmf_captures_segment ();

      sigmf::SigMFVector<sigmf::VariadicDataClass<core::AnnotationT>>&
      get_sigmf_annotations_segment ();

      sigmf::SigMF<sigmf::VariadicDataClass<core::GlobalT>,
          sigmf::VariadicDataClass<core::CaptureT>,
          sigmf::VariadicDataClass<core::AnnotationT>>&
      get_sigmf ();

    private:

      sigmf::SigMF<sigmf::VariadicDataClass<core::GlobalT>,
          sigmf::VariadicDataClass<core::CaptureT>,
          sigmf::VariadicDataClass<core::AnnotationT>> d_sigmf;

      void
      initialize_global (std::string datatype, std::string version);

      void
      initialize_global (std::string datatype, double sample_rate,
                         std::string version, std::string sha512,
                         uint32_t offset, std::string description,
                         std::string author, std::string meta_doi,
                         std::string data_doi, std::string recorder,
                         std::string license, std::string hw);

    };

  } //namespace meta

} //namespace iqzip

#endif /* IQZIP_META_H_ */
