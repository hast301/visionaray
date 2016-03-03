// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#ifndef VSNRAY_COVER_TEX_COORDS_KERNEL_H
#define VSNRAY_COVER_TEX_COORDS_KERNEL_H 1

#include <visionaray/result_record.h>
#include <visionaray/traverse.h>

namespace visionaray { namespace cover {

//-------------------------------------------------------------------------------------------------
// Tex coords debug kernel
//

template <typename Params>
struct tex_coords_kernel
{
    VSNRAY_FUNC explicit tex_coords_kernel(Params const& p)
        : params(p)
    {
    }

    template <typename R>
    VSNRAY_FUNC result_record<typename R::scalar_type> operator()(R ray) const
    {
        using S = typename R::scalar_type;
        using C = vector<4, S>;


        result_record<S> result;
        auto hit_rec        = closest_hit(ray, params.prims.begin, params.prims.end);
        auto tc             = get_tex_coord(params.tex_coords, hit_rec);
        result.hit          = hit_rec.hit;
        result.color        = select( hit_rec.hit, C(tc, S(1.0), S(1.0)), C(0.0) );
        result.isect_pos    = ray.ori + ray.dir * hit_rec.t;
        return result;
    }

    Params params;
};

}} // namespace visionaray::cover

#endif // VSNRAY_COVER_TEX_COORDS_KERNEL_H