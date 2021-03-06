// This file is distributed under the MIT license.
// See the LICENSE file for details.

#include <cstddef>

#include <visionaray/math/array.h>
#include <visionaray/material.h>

namespace visionaray
{

//-------------------------------------------------------------------------------------------------
// Public interface
//

template <typename T, typename ...Ts>
template <template <typename> class M>
inline generic_material<T, Ts...>::generic_material(M<typename T::scalar_type> const& material)
    : generic_material<T, Ts...>::base_type(material)
{
}

template <typename T, typename ...Ts>
VSNRAY_FUNC
inline bool generic_material<T, Ts...>::is_emissive() const
{
    return apply_visitor( is_emissive_visitor(), *this );
}

template <typename T, typename ...Ts>
VSNRAY_FUNC
inline spectrum<typename T::scalar_type> generic_material<T, Ts...>::ambient() const
{
    return apply_visitor( ambient_visitor(), *this );
}

template <typename T, typename ...Ts>
template <typename SR>
VSNRAY_FUNC
inline spectrum<typename SR::scalar_type> generic_material<T, Ts...>::shade(SR const& sr) const
{
    return apply_visitor( shade_visitor<SR>(sr), *this );
}

template <typename T, typename ...Ts>
template <typename SR, typename U, typename Interaction, typename Generator>
VSNRAY_FUNC
inline spectrum<U> generic_material<T, Ts...>::sample(
        SR const&       sr,
        vector<3, U>&   refl_dir,
        U&              pdf,
        Interaction&    inter,
        Generator&      gen
        ) const
{
    return apply_visitor( sample_visitor<SR, U, Interaction, Generator>(sr, refl_dir, pdf, inter, gen), *this );
}


//-------------------------------------------------------------------------------------------------
// Private variant visitors
//

template <typename T, typename ...Ts>
struct generic_material<T, Ts...>::is_emissive_visitor
{
    using Base = generic_material<T, Ts...>;
    using return_type = bool;

    template <typename X>
    VSNRAY_FUNC
    bool operator()(X) const
    {
        return false;
    }

    VSNRAY_FUNC
    bool operator()(emissive<typename Base::scalar_type>) const
    {
        return true;
    }
};

template <typename T, typename ...Ts>
struct generic_material<T, Ts...>::ambient_visitor
{
    using Base = generic_material<T, Ts...>;
    using return_type = spectrum<typename Base::scalar_type>;

    template <typename X>
    VSNRAY_FUNC
    return_type operator()(X const& ref) const
    {
        return ref.ambient();
    }
};

template <typename T, typename ...Ts>
template <typename SR>
struct generic_material<T, Ts...>::shade_visitor
{
    using return_type = spectrum<typename SR::scalar_type>;

    VSNRAY_FUNC
    shade_visitor(SR const& sr) : sr_(sr) {}

    template <typename X>
    VSNRAY_FUNC
    return_type operator()(X const& ref) const
    {
        return ref.shade(sr_);
    }

    SR const& sr_;
};

template <typename T, typename ...Ts>
template <typename SR, typename U, typename Interaction, typename Generator>
struct generic_material<T, Ts...>::sample_visitor
{
    using return_type = spectrum<typename SR::scalar_type>;

    VSNRAY_FUNC
    sample_visitor(SR const& sr, vector<3, U>& refl_dir, U& pdf, Interaction& inter, Generator& gen)
        : sr_(sr)
        , refl_dir_(refl_dir)
        , pdf_(pdf)
        , inter_(inter)
        , gen_(gen)
    {
    }

    template <typename X>
    VSNRAY_FUNC
    return_type operator()(X const& ref) const
    {
        return ref.sample(sr_, refl_dir_, pdf_, inter_, gen_);
    }

    SR const&       sr_;
    vector<3, U>&   refl_dir_;
    U&              pdf_;
    Interaction&    inter_;
    Generator&      gen_;
};


namespace simd
{

//-------------------------------------------------------------------------------------------------
// SIMD type used internally. Contains N generic materials
//

template <size_t N, typename ...Ts>
class generic_material
{
public:

    using scalar_type      = float_from_simd_width_t<N>;
    using single_material  = visionaray::generic_material<Ts...>;

public:

    VSNRAY_FUNC
    generic_material(array<single_material, N> const& mats)
        : mats_(mats)
    {
    }

    VSNRAY_FUNC
    single_material& get(int i)
    {
        return mats_[i];
    }

    VSNRAY_FUNC
    single_material const& get(int i) const
    {
        return mats_[i];
    }

    VSNRAY_FUNC
    mask_type_t<scalar_type> is_emissive() const
    {
        using mask_t = mask_type_t<scalar_type>;
        using mask_array = aligned_array_t<mask_t>;

        mask_array arr;

        for (size_t i = 0; i < N; ++i)
        {
            arr[i] = mats_[i].is_emissive();
        }

        return mask_t(arr);
    }

    VSNRAY_FUNC
    spectrum<scalar_type> ambient() const
    {
        array<spectrum<float>, N> amb;

        for (size_t i = 0; i < N; ++i)
        {
            amb[i] = mats_[i].ambient();
        }

        return pack(amb);
    }


    template <typename SR>
    VSNRAY_FUNC
    spectrum<scalar_type> shade(SR const& sr) const
    {
        auto srs = unpack(sr);

        array<spectrum<float>, N> shaded;

        for (size_t i = 0; i < N; ++i)
        {
            shaded[i] = mats_[i].shade(srs[i]);
        }

        return pack(shaded);
    }

    template <typename SR, typename Generator>
    VSNRAY_FUNC
    spectrum<scalar_type> sample(
            SR const&                sr,
            vector<3, scalar_type>&  refl_dir,
            scalar_type&             pdf,
            int_type_t<scalar_type>& inter,
            Generator&               gen
            ) const
    {
        using float_array = aligned_array_t<scalar_type>;
        using int_array = aligned_array_t<int_type_t<scalar_type>>;

        auto srs = unpack(sr);

        array<vector<3, float>, N> rds;
        float_array                pdfs;
        int_array                  inters;
        array<spectrum<float>, N>  sampled;

        for (size_t i = 0; i < N; ++i)
        {
            sampled[i] = mats_[i].sample(srs[i], rds[i], pdfs[i], inters[i], gen.get_generator(i));
        }

        refl_dir = pack(rds);
        pdf = scalar_type(pdfs);
        inter = int_type_t<scalar_type>(inters);
        return pack(sampled);
    }

private:

    array<single_material, N> mats_;

};


//-------------------------------------------------------------------------------------------------
// Pack and unpack
//

template <typename ...Ts>
VSNRAY_FUNC
inline generic_material<4, Ts...> pack(array<visionaray::generic_material<Ts...>, 4> const& mats)
{
    return generic_material<4, Ts...>(mats);
}

template <typename ...Ts>
VSNRAY_FUNC
inline array<visionaray::generic_material<Ts...>, 4> unpack(generic_material<4, Ts...> const& m4)
{
    return array<visionaray::generic_material<Ts...>, 4>{{ m4.get(0), m4.get(1), m4.get(2), m4.get(3) }};
}

template <typename ...Ts>
inline generic_material<8, Ts...> pack(array<visionaray::generic_material<Ts...>, 8> const& mats)
{
    return generic_material<8, Ts...>(mats);
}

template <typename ...Ts>
inline array<visionaray::generic_material<Ts...>, 8> unpack(generic_material<8, Ts...> const& m8)
{
    return array<visionaray::generic_material<Ts...>, 8>{{
            m8.get(0), m8.get(1), m8.get(2), m8.get(3),
            m8.get(4), m8.get(5), m8.get(6), m8.get(7)
            }};
}

template <typename ...Ts>
inline generic_material<16, Ts...> pack(array<visionaray::generic_material<Ts...>, 16> const& mats)
{
    return generic_material<16, Ts...>(mats);
}

template <typename ...Ts>
inline array<visionaray::generic_material<Ts...>, 16> unpack(generic_material<16, Ts...> const& m16)
{
    return array<visionaray::generic_material<Ts...>, 8>{{
            m16.get( 0), m16.get( 1), m16.get( 2), m16.get( 3),
            m16.get( 4), m16.get( 5), m16.get( 6), m16.get( 7),
            m16.get( 8), m16.get( 9), m16.get(10), m16.get(11),
            m16.get(12), m16.get(13), m16.get(14), m16.get(15)
            }};
}

} // simd
} // visionaray
