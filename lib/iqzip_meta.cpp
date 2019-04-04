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

namespace iqzip
{

  namespace meta
  {

    iqzip_meta::iqzip_meta ()
    {
    }

    iqzip_meta::~iqzip_meta ()
    {
    }

    core::GlobalT&
    iqzip_meta::get_sigmf_global_segment() {
      return d_sigmf.global;
    }

    sigmf::SigMFVector<core::AnnotationT>&
    iqzip_meta::get_sigmf_annotation_segment() {
      return d_sigmf.annotations;
    }

  }
}
