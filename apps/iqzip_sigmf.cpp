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

int main() {

    iqzip::meta::iqzip_meta meta = iqzip::meta::iqzip_meta("u8", "0.0.1");
    std::cout << "My sigmf: " << meta.get_sigmf_global_segment().access<core::GlobalT>().datatype << std::endl;
    /*
     * Create a record with core namespace using the VariadicDataClass as an example. This is kind of
     * like a "pure" usage of the variadic data class lightly wrapped together with our SigMF object.
     * The suggested usage for SigMF purposes is that you do not directly use sigmf::VariadticDataClass<...>
     * but instead use the more descriptive sigmf::Annotation, sigmf::Capture, sigmf::Global classes.
     */
//    sigmf::SigMF<sigmf::VariadicDataClass<core::GlobalT>,
//            sigmf::VariadicDataClass<core::CaptureT>,
//            sigmf::VariadicDataClass<core::AnnotationT> > example_record;
//
//    example_record.global.access<core::GlobalT>().author = "Nathan";
//    example_record.global.access<core::GlobalT>().description = "Example of creating a new record";
//    example_record.global.access<core::GlobalT>().sample_rate = 1.0;

    // Add a capture segment
//    auto new_capture = sigmf::VariadicDataClass<core::CaptureT>();
//    new_capture.access<core::CaptureT>().frequency = 870e6;
//    new_capture.access<core::CaptureT>().global_index = 5000;
//    new_capture.access<core::CaptureT>().sample_start = 10;
//    new_capture.access<core::CaptureT>().datetime = "now";
//    example_record.captures.emplace_back(new_capture);
    meta.append_capture_segment(10);
    meta.append_capture_segment(20);
    meta.append_capture_segment(25);
    meta.set_capture_global_index(20,1);

    // Add some annotations
//    auto anno0 = sigmf::VariadicDataClass<core::AnnotationT>();
//    anno0.access<core::AnnotationT>().description = "Annotation 1";
//    anno0.access<core::AnnotationT>().generator = "libsigmf";
//    anno0.access<core::AnnotationT>().description = "Woah!";
//    auto &ref = anno0.access<core::AnnotationT>();
//    // You can get it once at keep a reference if you want
//    ref.comment = "References make it a bit easier to access and modify the data parts";
//    ref.sample_count = 500000;
//    example_record.annotations.emplace_back(anno0);

//    auto anno1 = sigmf::VariadicDataClass<core::AnnotationT>();
//    anno1.access<core::AnnotationT>().sample_count = 600000;
//    anno1.access<core::AnnotationT>().description = "Annotation 2";
//    anno1.access<core::AnnotationT>().generator = "libsigmf";
//    anno1.access<core::AnnotationT>().description = "Pretty easy";
//    example_record.annotations.emplace_back(anno1);

//    // You can also use reference_wrappers to make it slightly less verbose...
//    auto anno2 = sigmf::VariadicDataClass<core::AnnotationT>();
//    auto core_anno_ref = std::ref(anno1.access<core::AnnotationT>());
//    core_anno_ref.get().sample_count = 100000;
//    core_anno_ref.get().description = "ref-wrapped";

//    auto anno3 = sigmf::VariadicDataClass<core::AnnotationT>();
//    core_anno_ref = std::ref(anno3.access<core::AnnotationT>());
//    // Notice we didn't push anno2 or anno3 on to the annotations vector, so they aren't part of the record

    // Make it a json (from modernjson) object and verify its output
    std::stringstream json_output;
    json_output << json(meta.get_sigmf()).dump(2) << std::flush;
    std::cout << json(meta.get_sigmf()).dump(2) << std::endl;
    meta.write_json("/home/ctriant/meta.sigmf-meta");
    meta.parse_json("/home/ctriant/meta.sigmf-meta");
    return 0; // assert passed, we're good :+1:
}
