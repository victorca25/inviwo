/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2013-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Main file author: Erik Sund�n
 *
 *********************************************************************************/

#include <modules/freeimage/freeimagewriter.h>
#include <inviwo/core/io/imageio.h>
#include <inviwo/core/util/urlparser.h>
#include <inviwo/core/datastructures/image/layer.h>

namespace inviwo {

FreeImageWriter::FreeImageWriter() 
    : DataWriterType<Layer>() {
    addExtension(FileExtension("png", "Portable Network Graphics"));
    addExtension(FileExtension("jpg", "Joint Photographic Experts Group"));
    addExtension(FileExtension("jpeg", "Joint Photographic Experts Group"));
    addExtension(FileExtension("tiff", "Tagged Image File Format"));
    addExtension(FileExtension("bmp", "Windows bitmap"));
    addExtension(FileExtension("exr", "OpenEXR"));
    addExtension(FileExtension("hdr", "High dynamic range"));
}

FreeImageWriter::FreeImageWriter(const FreeImageWriter& rhs) : DataWriterType<Layer>(rhs) {}

FreeImageWriter& FreeImageWriter::operator=(const FreeImageWriter& that) {
    if (this != &that)
        DataWriterType<Layer>::operator=(that);

    return *this;
}

FreeImageWriter* FreeImageWriter::clone() const {
    return new FreeImageWriter(*this);
}

void FreeImageWriter::writeData(const Layer* data, const std::string filePath) const {
    ImageIO::saveLayer(filePath.c_str(), data);
}

} // namespace