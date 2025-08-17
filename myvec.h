#pragma once
#include <glm/fwd.hpp>

namespace myvec
{
    struct vec2 {
        long double x, y;
        vec2();
        vec2(const vec2& v);
        vec2(long double a_x, long double a_y);

        vec2& operator*=(long double rhs);
        vec2 operator-() const;
        vec2& operator+=(const vec2 &rhs);
        vec2& operator-=(const vec2 &rhs);
    };

    vec2 operator+(const vec2 &lhs, const vec2 &rhs);
    vec2 operator-(const vec2 &lhs, const vec2 &rhs);
    vec2 operator*(const vec2 &lhs, const long double &rhs);
    vec2 operator*(const long double &lhs, const vec2 &rhs);

    struct vec3 {
        long double x, y, z;
        vec3();
        vec3(const vec3& v);
        vec3(long double a_x, long double a_y, long double a_z);
        operator glm::vec3() const;
        vec3& operator*=(long double rhs);
        vec3 operator-() const;
        vec3& operator+=(const vec3 &rhs);
        vec3& operator-=(const vec3 &rhs);
        long double dot(const vec3 &rhs) const;
        vec3 cross(const vec3 &rhs) const;
        long double sqrNorm() const;
        long double norm() const;
        long double sqrDist(const vec3 &v) const;
        long double dist(const vec3 &v) const;
        vec3& normalize();
    };

    vec3 operator+(const vec3 &lhs, const vec3 &rhs);
    vec3 operator-(const vec3 &lhs, const vec3 &rhs);
    vec3 operator*(const vec3 &lhs, const long double &rhs);
    vec3 operator*(const long double &lhs, const vec3 &rhs);
}
