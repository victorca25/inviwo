/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2012-2014 Inviwo Foundation
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

#include "volumegl.h"
#include <inviwo/core/datastructures/volume/volume.h>
#include <modules/opengl/glwrap/shader.h>
#include <algorithm>

namespace inviwo {

VolumeGL::VolumeGL(uvec3 dimensions, const DataFormatBase* format, Texture3D* tex)
    : VolumeRepresentation(dimensions, format), volumeTexture_(tex) {
    initialize();
}

VolumeGL::VolumeGL(const VolumeGL& rhs) : VolumeRepresentation(rhs) {
    volumeTexture_ = rhs.volumeTexture_->clone();
}

VolumeGL& VolumeGL::operator=(const VolumeGL& rhs) {
    if (this != &rhs) {
        VolumeRepresentation::operator=(rhs);
        volumeTexture_ = rhs.volumeTexture_->clone();
    }

    return *this;
}

VolumeGL::~VolumeGL() { deinitialize(); }

VolumeGL* VolumeGL::clone() const { return new VolumeGL(*this); }

void VolumeGL::initialize() {
    if (!volumeTexture_) {
        GLFormats::GLFormat glFormat = getGLFormats()->getGLFormat(getDataFormatId());
        volumeTexture_ = new Texture3D(dimensions_, glFormat, GL_LINEAR);
    }
}

void VolumeGL::deinitialize() {
    if (volumeTexture_ && volumeTexture_->decreaseRefCount() <= 0) {
        delete volumeTexture_;
        volumeTexture_ = NULL;
    }
}

void VolumeGL::bindTexture(GLenum texUnit) const {
    glActiveTexture(texUnit);
    volumeTexture_->bind();
}

void VolumeGL::unbindTexture() const { volumeTexture_->unbind(); }

void VolumeGL::setDimension(uvec3 dimensions) {
    dimensions_ = dimensions;
    volumeTexture_->uploadAndResize(NULL, dimensions_);
}

Texture3D* VolumeGL::getTexture() { return volumeTexture_; }

const Texture3D* VolumeGL::getTexture() const { return volumeTexture_; }

void VolumeGL::setVolumeUniforms(const Volume* volume, Shader* shader,
                                 const std::string& samplerID) const {
    vec3 dimF = vec3(dimensions_);
    shader->setUniform(samplerID + ".dimensions_", dimF);
    shader->setUniform(samplerID + ".dimensionsRCP_", vec3(1.f) / dimF);
    shader->setUniform(samplerID + ".volumeToWorldTransform_", volume->getWorldTransform());
    // Note: The basically the same code is used in VolumeCLGL and VolumeCL as well.
    // Changes here should also be done there.
    // adjust data scaling to volume data range

    dvec2 dataRange = volume->dataMap_.dataRange;
    DataMapper defaultRange(volume->getDataFormat());

    double typescale = 1.0 - getGLFormats()->getGLFormat(getDataFormatId()).scaling;

    double scalingFactor = 1.0;
    double signedScalingFactor = 1.0;
    double offset = 0.0;
    double signedOffset = 0.0;

    double invRange = 1.0 / (dataRange.y - dataRange.x);
    double defaultToDataRange = (defaultRange.dataRange.y - defaultRange.dataRange.x) * invRange;
    double defaultToDataOffset = (dataRange.x - typescale*defaultRange.dataRange.x) /
        (typescale*defaultRange.dataRange.y - typescale*defaultRange.dataRange.x);

    switch (volume->getDataFormat()->getNumericType()) {
        case DataFormatEnums::FLOAT_TYPE:
            scalingFactor = invRange;
            offset = -dataRange.x;
            break;
        case DataFormatEnums::SIGNED_INTEGER_TYPE:
            if (volume->getDataFormat()->getId() == DataFormatEnums::INT32 ||
                volume->getDataFormat()->getId() == DataFormatEnums::Vec2INT32 ||
                volume->getDataFormat()->getId() == DataFormatEnums::Vec3INT32 ||
                volume->getDataFormat()->getId() == DataFormatEnums::Vec4INT32) {
                // 32 bit ints are not normalized at all by opengl...
                scalingFactor = 2.0 * invRange;
                offset = 1.0 - dataRange.x;

                signedScalingFactor = invRange;
                signedOffset = -dataRange.x;
            } else if (volume->getDataFormat()->getId() == DataFormatEnums::Vec3INT12 ||
                       volume->getDataFormat()->getId() == DataFormatEnums::Vec4INT12) {
                // These two are not SNORM...
                scalingFactor = defaultToDataRange;
                offset = defaultToDataOffset;

                signedScalingFactor = scalingFactor;
                signedOffset = offset;

            } else {
                // In this case scale [min max] -> [0, 1], gl will scale to [-1, 1]
                scalingFactor = 0.5 * defaultToDataRange;
                offset = 1.0 - 2 * defaultToDataOffset;

                signedScalingFactor = defaultToDataRange;
                signedOffset = -defaultToDataOffset;
            }
            break;
        case DataFormatEnums::UNSIGNED_INTEGER_TYPE:
            if (volume->getDataFormat()->getId() == DataFormatEnums::UINT32 ||
                volume->getDataFormat()->getId() == DataFormatEnums::Vec2UINT32 ||
                volume->getDataFormat()->getId() == DataFormatEnums::Vec3UINT32 ||
                volume->getDataFormat()->getId() == DataFormatEnums::Vec4UINT32) {
                // 32 bit uints are not normalized at all by opengl...
                scalingFactor = invRange;
                offset = -dataRange.x;
            } else {
                scalingFactor = defaultToDataRange;
                offset = -defaultToDataOffset;
            }
            signedScalingFactor = scalingFactor;
            signedOffset = offset;
            break;
        default:
            scalingFactor = 0.0;
            offset = 0.0;
    }

    // offset scaling because of reversed scaling in the shader, i.e. (1 - formatScaling_)
    shader->setUniform(samplerID + ".formatScaling_",
                       static_cast<float>(1.0 - scalingFactor * typescale));
    shader->setUniform(samplerID + ".formatOffset_", static_cast<float>(offset));

    shader->setUniform(samplerID + ".signedFormatScaling_",
                       static_cast<float>(1.0 - signedScalingFactor * typescale));
    shader->setUniform(samplerID + ".signedFormatOffset_", static_cast<float>(signedOffset));
}

}  // namespace
