#pragma once

#include <immintrin.h>

//===================================================================================
/*                                      Class                                      */
union _vector_ll
{
    _vector_ll();

    _vector_ll(long long var);
    _vector_ll(long long var1, long long var2, long long var3, long long var4);
    _vector_ll(__m256i other);
    _vector_ll(const _vector_ll &other);
    _vector_ll(_vector_ll &&other);

    long long &operator[](size_t i);
    _vector_ll &operator=(const _vector_ll &other);
    _vector_ll &operator=(_vector_ll &&other);

    operator __m256i() const;

    ~_vector_ll();

private:
    __m256i vector_epi64;
    long long vector_ptr[4];
};

//===================================================================================
/*                                     Methods                                     */
inline _vector_ll::_vector_ll()
{
}
//-----------------------------------------------
inline _vector_ll::_vector_ll(long long var)
{
    vector_epi64 = _mm256_set1_epi64x(var);
}
inline _vector_ll::_vector_ll(long long var1, long long var2, long long var3, long long var4)
{
    vector_epi64 = _mm256_set_epi64x(var1, var2, var3, var4);
}
inline _vector_ll::_vector_ll(__m256i other)
{
    vector_epi64 = other;
}
inline _vector_ll::_vector_ll(const _vector_ll &other)
{
    vector_epi64 = other.vector_epi64;
}
inline _vector_ll::_vector_ll(_vector_ll &&other)
{
    vector_epi64 = other.vector_epi64;
}
//-----------------------------------------------
inline long long &_vector_ll::operator[](size_t i)
{
    return vector_ptr[3 - i];
}
inline _vector_ll &_vector_ll::operator=(const _vector_ll &other)
{
    vector_epi64 = other.vector_epi64;
    return *this;
}
inline _vector_ll &_vector_ll::operator=(_vector_ll &&other)
{
    vector_epi64 = other.vector_epi64;
    return *this;
}
//-----------------------------------------------
_vector_ll::operator __m256i() const
{
    return vector_epi64;
}
//-----------------------------------------------
inline _vector_ll::~_vector_ll()
{
}
//===================================================================================
/*                                    Functions                                    */

inline _vector_ll operator+(const _vector_ll &first, const _vector_ll &second)
{
    return _mm256_add_epi64(first, second); // 2 casts and constructor
}
inline _vector_ll operator-(const _vector_ll &first, const _vector_ll &second)
{
    return _mm256_sub_epi64(first, second); // 2 casts and constructor
}
//===================================================================================








//===================================================================================
/*                                      Class                                      */
union _vector_d
{
    _vector_d();

    _vector_d(double var);
    _vector_d(double var1, double var2, double var3, double var4);
    _vector_d(__m256d other);
    _vector_d(const _vector_d &other);
    _vector_d(_vector_d &&other);

    double &operator[](size_t i);
    _vector_d &operator=(const _vector_d &other);
    _vector_d &operator=(_vector_d &&other);

    operator __m256d() const;
    operator _vector_ll() const;

    ~_vector_d();

private:
    __m256d vector_pd;
    double vector_ptr[4];
};

//===================================================================================
/*                                     Methods                                     */
inline _vector_d::_vector_d()
{
}
//-----------------------------------------------
inline _vector_d::_vector_d(double var)
{
    vector_pd = _mm256_set1_pd(var);
}
inline _vector_d::_vector_d(double var1, double var2, double var3, double var4)
{
    vector_pd = _mm256_set_pd(var1, var2, var3, var4);
}
inline _vector_d::_vector_d(__m256d other)
{
    vector_pd = other;
}
inline _vector_d::_vector_d(const _vector_d &other)
{
    vector_pd = other.vector_pd;
}
inline _vector_d::_vector_d(_vector_d &&other)
{
    vector_pd = other.vector_pd;
}
//-----------------------------------------------
inline double &_vector_d::operator[](size_t i)
{
    return vector_ptr[3 - i];
}
inline _vector_d &_vector_d::operator=(const _vector_d &other)
{
    vector_pd = other.vector_pd;
    return *this;
}
inline _vector_d &_vector_d::operator=(_vector_d &&other)
{
    vector_pd = other.vector_pd;
    return *this;
}
//-----------------------------------------------
_vector_d::operator __m256d() const
{
    return vector_pd;
}
_vector_d::operator _vector_ll() const
{
    return _mm256_castpd_si256(vector_pd);
}
//-----------------------------------------------
inline _vector_d::~_vector_d()
{
}
//===================================================================================
/*                                    Functions                                    */

inline _vector_d operator+(const _vector_d &first, const _vector_d &second)
{
    return _mm256_add_pd(first, second); // 2 casts and constructor
}
inline _vector_d operator-(const _vector_d &first, const _vector_d &second)
{
    return _mm256_sub_pd(first, second); // 2 casts and constructor
}
inline _vector_d operator*(const _vector_d &first, const _vector_d &second)
{
    return _mm256_mul_pd(first, second); // 2 casts and constructor
}
inline _vector_d operator/(const _vector_d &first, const _vector_d &second)
{
    return _mm256_div_pd(first, second); // 2 casts and constructor
}
//-----------------------------------------------
template <int imm8>
inline bool cmp(const _vector_d &first, const _vector_d &second)
{
    int mask = _mm256_movemask_pd(_mm256_cmp_pd(first, second, imm8)); // 2 casts
    return (mask == 16 - 1) ? true : false;                            // 0...01111
}
inline _vector_d sqrt(const _vector_d &other)
{
    return _mm256_sqrt_pd(other); //cast and constructor
}
template <int imm8>
inline _vector_d get_cmp(const _vector_d &first, const _vector_d &second)
{
    return _mm256_cmp_pd(first, second, imm8);
}
inline int get_mask(const _vector_d &cmp)
{
    return _mm256_movemask_pd(cmp);
}
//===================================================================================