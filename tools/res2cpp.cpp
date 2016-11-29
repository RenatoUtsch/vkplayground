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

/*
 * Command: res2cpp input [input2 ...] output
 * Converts the given input file to a .cpp file with the contents of the
 * resource inside a byte array. The name of the generated array is "Resource_"
 * appended to the file name, but with . replaced with _ (and not handling any
 * other special characters).
 */

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

const int BufferSize = 256;

size_t findPathEnd(std::string fileName) {
    return std::max(fileName.rfind('\\') + 1, fileName.rfind('/') + 1);
}

size_t multiFind(const std::string &str, const char *options) {
    size_t pos;
    for(const char *p = options; *p; ++p)
        if((pos = str.find(*p)) != std::string::npos)
            return pos;

    return std::string::npos;
}

std::string generateVarName(std::string fileName) {
    std::stringstream ss;
    ss << "Generated_" << fileName.substr(findPathEnd(fileName));

    std::string varName(ss.str());
    size_t pos = 0;
    while((pos = multiFind(varName, ".-")) != std::string::npos)
        varName[pos] = '_';

    return varName;
}

void writeResource(const char *fileName, std::ofstream &out) {
    std::string varName = generateVarName(fileName);
    std::basic_ifstream<unsigned char> in(fileName, std::ios::binary);
    if(!in.is_open())
        throw std::runtime_error("Failed to open input file");

    out << "extern const std::vector<uint8_t> " << varName << " = {\n";

    unsigned char buf[BufferSize];
    size_t nRead;
    size_t charCount = 0;
    while(in.good()) {
        in.read(buf, BufferSize);
        nRead = in.gcount();

        for(size_t i = 0; i < nRead; ++i) {
            out << "0x" << std::setfill('0') << std::setw(2)
                << std::hex << unsigned(buf[i]);

            if(in.eof() && i == nRead - 1)
                out << "\n";
            else if(++charCount == 10)
                out << ",\n", charCount = 0;
            else {
                out << ", ";
            }
        }
    }

    out << "};\n\n";
}

int main(int argc, char **argv) {
    std::ios_base::sync_with_stdio(false);

    if(argc < 3) {
        std::cerr << "usage: " << argv[0] << " input [input2 ...] output\n";
        return EXIT_FAILURE;
    }

    int outputIndex = argc - 1;
    std::ofstream out(argv[outputIndex]);
    if(!out.is_open())
        throw std::runtime_error("Failed to open output file");

    out << "// Generated file. Don't change.\n"
        << "#include <vector>\n"
        << "#include <cstdint>\n"
        << "\n";

    for(int i = 1; i < outputIndex; ++i)
        writeResource(argv[i], out);

    return EXIT_SUCCESS;
}
