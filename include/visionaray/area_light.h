// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_AREA_LIGHT_H
#define VSNRAY_AREA_LIGHT_H 1

#include <cstddef>

#include "detail/macros.h"
#include "math/array.h"
#include "math/vector.h"

namespace visionaray
{

template <typename Geometry>
class area_light
{
public:

    area_light() = default;
    area_light(Geometry geometry);

    // Evaluate the light intensity at pos.
    template <typename T>
    VSNRAY_FUNC vector<3, T> intensity(vector<3, T> const& pos) const;

    template <typename T, typename Generator>
    VSNRAY_FUNC vector<3, T> sample(T& pdf, Generator& gen) const;

    // Get N sampled positions.
    template <typename T, size_t N, typename Generator>
    VSNRAY_FUNC void sample(
            array<T, N>& pdfs,
            array<vector<3, T>, N>& result,
            Generator& gen
            ) const;

    // Return center of bounding box
    // TODO: maybe return something more meaningful, e.g. center of gravity?
    VSNRAY_FUNC vector<3, float> position() const;

private:

    Geometry geometry_;

};

} // visionaray

#include "detail/area_light.inl"

#endif // VSNRAY_AREA_LIGHT_H
