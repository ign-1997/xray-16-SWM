#pragma once
#include <math.h>
#include "xrCore/math_constants.h"
#include "xrCommon/inlining_macros.h"
#include "xrCore/_bitwise.h" // iFloor

inline float _abs(float x) noexcept { return fabsf(x); }
inline float _sqrt(float x) noexcept { return sqrtf(x); }
inline float _sin(float x) noexcept { return sinf(x); }
inline float _cos(float x) noexcept { return cosf(x); }
inline double _abs(double x) noexcept { return fabs(x); }
inline double _sqrt(double x) noexcept { return sqrt(x); }
inline double _sin(double x) noexcept { return sin(x); }
inline double _cos(double x) noexcept { return cos(x); }

// comparisions
inline bool fsimilar(float a, float b, float cmp = EPS) { return _abs(a-b)<cmp; }
inline bool dsimilar(double a, double b, double cmp = EPS) { return _abs(a-b)<cmp; }

inline bool fis_zero(float val, float cmp = EPS_S) noexcept { return _abs(val) < cmp; }
inline bool dis_zero(double val, double cmp = EPS_S) noexcept { return _abs(val) < cmp; }

// degree to radians and vice-versa
constexpr float  deg2rad(float  val) noexcept { return val*M_PI / 180; }
constexpr double deg2rad(double val) noexcept { return val*M_PI / 180; }
constexpr float  rad2deg(float  val) noexcept { return val*180 / M_PI; }
constexpr double rad2deg(double val) noexcept { return val*180 / M_PI;}

// clamping/snapping
template <class T>
constexpr void clamp(T& val, const T& _low, const T& _high)
{
    if (val<_low)
        val = _low;
    else if (val>_high)
        val = _high;
}

// XXX: Check usages and provide overloads for native types where arguments are NOT references.
template <class T>
constexpr T clampr(const T& val, const T& _low, const T& _high)
{
    if (val < _low)
        return _low;
    if (val > _high)
        return _high;
    return val;
}

inline float snapto(float value, float snap)
{
    if (snap <= 0.f)
        return value;
    return float(iFloor((value + (snap*0.5f)) / snap)) * snap;
}

// linear interpolation
template <class T>
inline constexpr T _lerp(const T& _val_a, const T& _val_b, const float& _factor)
{
    return (_val_a * (1.0 - _factor)) + (_val_b * _factor);
}

template <class T>
inline constexpr T _lerpc(const T& _val_a, const T& _val_b, const float& _factor)
{
    float factor_c = clampr(_factor, 0.0f, 1.0f);
    return (_val_a * (1.0 - factor_c)) + (_val_b * factor_c);
}

// inertion
//--> Inertion by friction [0.0f - Immediately, 0.99f - Very slow]
template <class T>
inline constexpr void _inertion(T& _val_cur, const T& _val_trgt, const float& _friction)
{
    float friction_i = 1.f - _friction;
    _val_cur = _val_cur * _friction + _val_trgt * friction_i;
}

template <class T>
inline constexpr T _inertionr(const T& _val_cur, const T& _val_trgt, const float& _friction)
{
    float friction_i = 1.f - _friction;
    return _val_cur * _friction + _val_trgt * friction_i;
}
//--> Inertion by the approximate time [0.0f - Immediately, 2.0f - In ~ 1.8-2.3 seconds]
template <class T>
inline constexpr T _inertionT(T& _val_cur, const T& _val_trgt, const float& _dt /* = Device.fTimeDelta */, const float& _time_in_sec)
{
    _val_cur = _inertionr(
        _val_cur,
        _val_trgt,
        1.0f - clampr(_dt * 3.0f / (_time_in_sec + 0.0001f), 0.0f, 1.0f)
    );
}

template <class T>
inline constexpr T _inertionTr(const T& _val_cur, const T& _val_trgt, const float& _dt /* = Device.fTimeDelta */, const float& _time_in_sec)
{
    return _inertionr(
        _val_cur,
        _val_trgt,
        1.0f - clampr(_dt * 3.0f / (_time_in_sec + 0.0001f), 0.0f, 1.0f)
    );
}
