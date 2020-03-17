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

#include <iostream>
#include <iqzip/meta.h>

int main()
{

    metafile::meta_sptr sptr = metafile::create_iqzip_meta("u8", "0.0.1");
    std::cout << "My sigmf: " <<
              sptr->get_global_datatype() << std::endl;

    sptr->append_capture_segment(10);
    sptr->append_capture_segment(20);
    sptr->append_capture_segment(25);
    sptr->set_capture_global_index(20, 1);

    sptr->write_json("meta.sigmf-meta");
    sptr->parse_json("meta.sigmf-meta");
    sptr->create_archive("satnogs.sigmf", "meta.sigmf-meta");
    return 0;
}
