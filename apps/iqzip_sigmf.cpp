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
 */

#include <iqzip/iqzip_meta.h>
#include <iostream>

int main()
{

    iqzip::meta::iqzip_meta meta = iqzip::meta::iqzip_meta("u8", "0.0.1");
    std::cout << "My sigmf: " <<
              meta.get_sigmf_global_segment().access<core::GlobalT>().datatype << std::endl;

    meta.append_capture_segment(10);
    meta.append_capture_segment(20);
    meta.append_capture_segment(25);
    meta.set_capture_global_index(20, 1);

    // Make it a json (from modernjson) object and verify its output
    std::stringstream json_output;
    json_output << json(meta.get_sigmf()).dump(2) << std::flush;
    std::cout << json(meta.get_sigmf()).dump(2) << std::endl;
    meta.write_json("meta.sigmf-meta");
    meta.parse_json("meta.sigmf-meta");
    meta.create_tar("satnogs.sigmf", "meta.sigmf-meta");
    return 0;
}
