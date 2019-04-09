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
#include "sigmf_core_generated.h"
#include "sigmf_antenna_generated.h"
#include "sigmf.h"

namespace iqzip
{

  namespace meta
  {

    class iqzip_meta
    {

    public:

      iqzip_meta ();

      virtual
      ~iqzip_meta ();

      core::GlobalT&
      get_sigmf_global_segment();

      sigmf::SigMFVector<core::CaptureT>&
      get_sigmf_capture_segment();

      sigmf::SigMFVector<core::AnnotationT>&
      get_sigmf_annotation_segment();

    private:

      sigmf::SigMF<core::GlobalT, core::CaptureT, core::AnnotationT> d_sigmf;

    };

  } //namespace meta

} //namespace iqzip

#endif /* IQZIP_META_H_ */
