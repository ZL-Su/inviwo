/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2016 Inviwo Foundation
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

#ifndef IVW_VOLUMESAMPLER_H
#define IVW_VOLUMESAMPLER_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/util/indexmapper.h>

#include <inviwo/core/util/interpolation.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>

#include <inviwo/core/util/spatialsampler.h>

namespace inviwo {

/**
 * \class VolumeDoubleSampler
 */
template <unsigned int DataDims>
class VolumeDoubleSampler : public SpatialSampler<3, DataDims, double> {
public:
    VolumeDoubleSampler(std::shared_ptr<const Volume> vol, Space space = Space::Data);
    virtual ~VolumeDoubleSampler() = default;

    virtual Vector<DataDims, double> sampleDataSpace(const dvec3 &pos) const override;

protected:
    Vector<DataDims, double> getVoxel(const size3_t &pos) const;

    virtual bool withinBoundsDataSpace(const dvec3 &pos) const override;

    std::shared_ptr<const Volume> volume_;
    const VolumeRAM *ram_;
    size3_t dims_;
};

template <>
IVW_CORE_API Vector<1, double> VolumeDoubleSampler<1>::getVoxel(const size3_t &pos) const;

template <>
IVW_CORE_API Vector<2, double> VolumeDoubleSampler<2>::getVoxel(const size3_t &pos) const;

template <>
IVW_CORE_API Vector<3, double> VolumeDoubleSampler<3>::getVoxel(const size3_t &pos) const;

template <>
IVW_CORE_API Vector<4, double> VolumeDoubleSampler<4>::getVoxel(const size3_t &pos) const;

using VolumeSampler = VolumeDoubleSampler<4>;

template <unsigned int DataDims>
bool VolumeDoubleSampler<DataDims>::withinBoundsDataSpace(const dvec3 &pos) const {
    if (glm::any(glm::lessThan(pos, dvec3(0.0)))) {
        return false;
    }
    if (glm::any(glm::greaterThan(pos, dvec3(1.0)))) {
        return false;
    }
    return true;
}

template <unsigned int DataDims>
Vector<DataDims, double> VolumeDoubleSampler<DataDims>::sampleDataSpace(const dvec3 &pos) const {
    if (!withinBoundsDataSpace(pos)) {
        return Vector<DataDims, double>(0.0);
    }
    const dvec3 samplePos = pos * dvec3(dims_ - size3_t(1));
    const size3_t indexPos = size3_t(samplePos);
    const dvec3 interpolants = samplePos - dvec3(indexPos);

    Vector<DataDims, double> samples[8];
    samples[0] = getVoxel(indexPos);
    samples[1] = getVoxel(indexPos + size3_t(1, 0, 0));
    samples[2] = getVoxel(indexPos + size3_t(0, 1, 0));
    samples[3] = getVoxel(indexPos + size3_t(1, 1, 0));

    samples[4] = getVoxel(indexPos + size3_t(0, 0, 1));
    samples[5] = getVoxel(indexPos + size3_t(1, 0, 1));
    samples[6] = getVoxel(indexPos + size3_t(0, 1, 1));
    samples[7] = getVoxel(indexPos + size3_t(1, 1, 1));

    return Interpolation<Vector<DataDims, double>>::trilinear(
        samples, interpolants, &Interpolation<Vector<DataDims, double>>::linear);
}

template <unsigned int DataDims>
VolumeDoubleSampler<DataDims>::VolumeDoubleSampler(std::shared_ptr<const Volume> vol, Space space)
    : SpatialSampler<3, DataDims, double>(vol, space)
    , volume_(vol)
    , ram_(vol->getRepresentation<VolumeRAM>())
    , dims_(vol->getDimensions()) {}

}  // namespace

#endif  // IVW_VOLUMESAMPLER_H
