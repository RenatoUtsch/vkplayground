/*
 * vkplayground - Playing around with Vulkan
 *
 * Copyright 2016 Renato Utsch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "shaders.hpp"

/*
 * The following external resources are generated during compilation by the
 * build system, and contain the SPIR-V binaries of the shaders in this folder.
 * They will be automatically added to the compilation, so just assume that
 * they are defined somewhere else.
 */
extern const std::vector<uint8_t> Generated_shader_vert_spv;
extern const std::vector<uint8_t> Generated_shader_frag_spv;

const std::vector<uint8_t> &VertexShaderBinary() {
    return Generated_shader_vert_spv;
}

const std::vector<uint8_t> &FragmentShaderBinary() {
    return Generated_shader_frag_spv;
}
