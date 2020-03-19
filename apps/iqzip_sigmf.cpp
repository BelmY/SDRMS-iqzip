/*
 *    Copyright 2019 DeepSig Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
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

#include <iostream>
#include <iqzip/meta.h>

int main()
{

    iqzip::metafile::meta_sptr meta = iqzip::metafile::create_iqzip_meta("u8",
                                      "0.0.1");

    meta->append_capture_segment(10);
    meta->append_capture_segment(20);
    meta->append_capture_segment(25);
    meta->set_capture_global_index(20, 1);

    std::string ano_json = R"({
      "core:description": "Pretty easy",
      "core:generator": "iqzip",
      "core:sample_count": 600000
  	})";

    meta->append_annotation_segment(ano_json);

    meta->write_json("meta.sigmf-meta");

    meta->create_archive("satnogs.sigmf", "meta.sigmf-meta");
    return 0;
}
