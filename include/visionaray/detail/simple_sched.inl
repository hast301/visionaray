// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <type_traits>
#include <utility>

#include <visionaray/matrix_camera.h>
#include <visionaray/pinhole_camera.h>
#include <visionaray/random_sampler.h>

#include "sched_common.h"

namespace visionaray
{

namespace detail
{
namespace simple
{

//-------------------------------------------------------------------------------------------------
// Private functions
//

// Iterate over all pixels in a loop
template <typename R, typename K, typename SP, typename ...Args>
void sample_pixels_impl(R /* */, K kernel, SP sched_params, unsigned frame_num, Args&&... args)
{
    // TODO: support any sampler
    random_sampler<typename R::scalar_type> samp(detail::tic());

    for (size_t y = 0; y < sched_params.rt.height(); ++y)
    {
        for (size_t x = 0; x < sched_params.rt.width(); ++x)
        {
            auto r = detail::make_primary_rays(
                    R{},
                    typename SP::pixel_sampler_type{},
                    samp,
                    x,
                    y,
                    std::forward<Args>(args)...
                    );

            sample_pixel(
                    kernel,
                    typename SP::pixel_sampler_type{},
                    r,
                    samp,
                    frame_num,
                    sched_params.rt.ref(),
                    x,
                    y,
                    std::forward<Args>(args)...
                    );
        }
    }
}


// Implementation using matrices
template <typename R, typename K, typename SP>
void frame_impl(
        R /* */,
        K kernel,
        SP sparams,
        unsigned frame_num,
        typename std::enable_if<std::is_same<typename SP::camera_type, matrix_camera>::value>::type* = nullptr
        )
{
    using matrix_type = matrix<4, 4, typename R::scalar_type>;

    // Iterate over all pixels
    sample_pixels_impl(
            R{},
            kernel,
            sparams,
            frame_num,
            sparams.rt.width(),
            sparams.rt.height(),
            matrix_type(sparams.cam.get_view_matrix()),
            matrix_type(sparams.cam.get_view_matrix_inv()),
            matrix_type(sparams.cam.get_proj_matrix()),
            matrix_type(sparams.cam.get_proj_matrix_inv())
            );
}


// Implementation using a pinhole camera
template <typename R, typename K, typename SP>
void frame_impl(
        R /* */,
        K kernel,
        SP sched_params,
        unsigned frame_num,
        typename std::enable_if<std::is_same<typename SP::camera_type, pinhole_camera>::value>::type* = nullptr
        )
{
    typedef typename R::scalar_type scalar_type;

    //  front, side, and up vectors form an orthonormal basis
    auto f = normalize( sched_params.cam.eye() - sched_params.cam.center() );
    auto s = normalize( cross(sched_params.cam.up(), f) );
    auto u =            cross(f, s);

    auto eye   = vector<3, scalar_type>(sched_params.cam.eye());
    auto cam_u = vector<3, scalar_type>(s) * scalar_type( tan(sched_params.cam.fovy() / 2.0f) * sched_params.cam.aspect() );
    auto cam_v = vector<3, scalar_type>(u) * scalar_type( tan(sched_params.cam.fovy() / 2.0f) );
    auto cam_w = vector<3, scalar_type>(-f);

    // Iterate over all pixels
    sample_pixels_impl(
            R{},
            kernel,
            sched_params,
            frame_num,
            sched_params.rt.width(),
            sched_params.rt.height(),
            eye,
            cam_u,
            cam_v,
            cam_w
            );
}

} // simple
} // detail


//-------------------------------------------------------------------------------------------------
// Simple sched
//

template <typename R>
template <typename K, typename SP>
void simple_sched<R>::frame(K kernel, SP sched_params, unsigned frame_num)
{
    sched_params.rt.begin_frame();

    detail::simple::frame_impl(
            R{},
            kernel,
            sched_params,
            frame_num
            );
}

} // visionaray
