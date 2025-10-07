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
        vec3(const long double &v);
        vec3(const vec3& v);
        vec3(long double a_x, long double a_y, long double a_z);
        operator glm::vec3() const;
        vec3& operator*=(long double rhs);
        vec3 operator-() const;
        vec3& operator+=(const vec3 &rhs);
        vec3& operator-=(const vec3 &rhs);
        // set vector to zero
        void clear();
        // dot product
        long double dot(const vec3 &rhs) const;
        // cross product
        vec3 cross(const vec3 &rhs) const;
        // hadamard product
        vec3 hada(const vec3 &rhs) const;
        // square of norm of vector
        long double sqrNorm() const;
        // norm of vector
        long double norm() const;
        // square of (euclidean) distance to v
        long double sqrDist(const vec3 &v) const;
        // (euclidean) distance to v
        long double dist(const vec3 &v) const;
        vec3& normalize();
    };

    vec3 operator+(const vec3 &lhs, const vec3 &rhs);
    vec3 operator-(const vec3 &lhs, const vec3 &rhs);
    vec3 operator*(const vec3 &lhs, const long double &rhs);
    vec3 operator*(const long double &lhs, const vec3 &rhs);
    vec3 operator/(const vec3 &lhs, const long double &rhs);
}
