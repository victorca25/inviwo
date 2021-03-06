/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017 Inviwo Foundation
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
 *********************************************************************************/

#ifndef IVW_EIGENPORTS_H
#define IVW_EIGENPORTS_H

#include <modules/eigenutils/eigenutilsmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <modules/eigenutils/eigenutils.h>

namespace inviwo {

using EigenMatrixOutport = DataOutport<Eigen::MatrixXf>;
using EigenMatrixInport = DataInport<Eigen::MatrixXf>;

template <>
struct port_traits<Eigen::MatrixXf> {
    static std::string class_identifier() { return "EigenMatrixXf"; }
    static uvec3 color_code() { return uvec3(141, 211, 199); }
    static std::string data_info(const Eigen::MatrixXf* data) {
        std::ostringstream oss;
        oss << "Eigen MatrixXf " << data->rows() << "-by-" << data->cols();
        return oss.str();
    }
};

}  // namespace

#endif  // IVW_PORTS_H
