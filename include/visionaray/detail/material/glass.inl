// This file is distributed under the MIT license.
// See the LICENSE file for details.

namespace visionaray
{

//-------------------------------------------------------------------------------------------------
// Public interface
//

template <typename T>
VSNRAY_FUNC
inline spectrum<T> glass<T>::ambient() const
{
    return spectrum<T>(0.0); // TODO: no support for  ambient
}

template <typename T>
template <typename SR>
VSNRAY_FUNC
inline spectrum<typename SR::scalar_type> glass<T>::shade(SR const& sr) const
{
    return specular_bsdf_.f(sr.normal, sr.view_dir, sr.light_dir);
}

template <typename T>
template <typename SR, typename U, typename Interaction, typename Generator>
VSNRAY_FUNC
inline spectrum<U> glass<T>::sample(
        SR const&       sr,
        vector<3, U>&   refl_dir,
        U&              pdf,
        Interaction&    inter,
        Generator&      gen
        ) const
{
    return specular_bsdf_.sample_f(sr.normal, sr.view_dir, refl_dir, pdf, inter, gen);
}

template <typename T>
VSNRAY_FUNC
inline spectrum<T>& glass<T>::ct()
{
    return specular_bsdf_.ct;
}

template <typename T>
VSNRAY_FUNC
inline spectrum<T> const& glass<T>::ct() const
{
    return specular_bsdf_.ct;
}

template <typename T>
VSNRAY_FUNC
inline T& glass<T>::kt()
{
    return specular_bsdf_.kt;
}

template <typename T>
VSNRAY_FUNC
inline T const& glass<T>::kt() const
{
    return specular_bsdf_.kt;
}

template <typename T>
VSNRAY_FUNC
inline spectrum<T>& glass<T>::cr()
{
    return specular_bsdf_.cr;
}

template <typename T>
VSNRAY_FUNC
inline spectrum<T> const& glass<T>::cr() const
{
    return specular_bsdf_.cr;
}

template <typename T>
VSNRAY_FUNC
inline T& glass<T>::kr()
{
    return specular_bsdf_.kr;
}

template <typename T>
VSNRAY_FUNC
inline T const& glass<T>::kr() const
{
    return specular_bsdf_.kr;
}

template <typename T>
VSNRAY_FUNC
inline spectrum<T>& glass<T>::ior()
{
    return specular_bsdf_.ior;
}

template <typename T>
VSNRAY_FUNC
inline spectrum<T> const& glass<T>::ior() const
{
    return specular_bsdf_.ior;
}

} // visionaray
