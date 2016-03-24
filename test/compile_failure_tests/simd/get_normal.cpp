// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <visionaray/get_normal.h>

using namespace visionaray;


int main()
{

#if defined GET_NORMAL_FLOAT4

    vector<3, float>* normals;
    hit_record<basic_ray<simd::float4>, primitive<unsigned>> hr;
    vector<3, simd::float4> n = get_normal(normals, hr, primitive<unsigned>{}, per_face_binding{});

#elif defined GET_NORMAL_FLOAT8

#if VSNRAY_SIMD_ISA >= VSNRAY_SIMD_ISA_AVX
    vector<3, float>* normals;
    hit_record<basic_ray<simd::float8>, primitive<unsigned>> hr;
    vector<3, simd::float8> n = get_normal(normals, hr, primitive<unsigned>{}, per_face_binding{});
#endif

#elif defined GET_NORMAL_INT4

    vector<3, float>* normals;
    hit_record<basic_ray<simd::int4>, primitive<unsigned>> hr;
    auto n = get_normal(normals, hr, primitive<unsigned>{}, per_face_binding{});

#elif defined GET_NORMAL_MASK4

    vector<3, float>* normals;
    hit_record<basic_ray<simd::mask4>, primitive<unsigned>> hr;
    auto n = get_normal(normals, hr, primitive<unsigned>{}, per_face_binding{});

#elif defined GET_NORMAL_TRI_FLOAT4

    vector<3, float>* normals;
    hit_record<basic_ray<simd::float4>, primitive<unsigned>> hr;
    vector<3, simd::float4> n = get_normal(normals, hr, basic_triangle<3, float, unsigned>{}, per_vertex_binding{});

#elif defined GET_NORMAL_TRI_FLOAT8

#if VSNRAY_SIMD_ISA >= VSNRAY_SIMD_ISA_AVX
    vector<3, float>* normals;
    hit_record<basic_ray<simd::float8>, primitive<unsigned>> hr;
    vector<3, simd::float8> n = get_normal(normals, hr, basic_triangle<3, float, unsigned>{}, per_vertex_binding{});
#endif

#elif defined GET_NORMAL_TRI_INT4

    vector<3, float>* normals;
    hit_record<basic_ray<simd::int4>, primitive<unsigned>> hr;
    auto n = get_normal(normals, hr, basic_triangle<3, float, unsigned>{}, per_vertex_binding{});

#elif defined GET_NORMAL_TRI_MASK4

    vector<3, float>* normals;
    hit_record<basic_ray<simd::mask4>, primitive<unsigned>> hr;
    auto n = get_normal(normals, hr, basic_triangle<3, float, unsigned>{}, per_vertex_binding{});

#endif

    return 0;
}