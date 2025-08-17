#include <cmath>

#include <glm/vec3.hpp>
#include "myvec.h"

namespace myvec {
    vec2::vec2() {}

    vec2::vec2(const vec2& v) {
        x = v.x;
        y = v.y;
    }

    vec2::vec2(long double a_x, long double a_y) {
        x = a_x;
        y = a_y;
    }

    vec2& vec2::operator*=(long double rhs) {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    vec2 vec2::operator-() const {
        return vec2(
            -this->x,
            -this->y
        );
    }

    vec2& vec2::operator+=(const vec2 &rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    vec2& vec2::operator-=(const vec2 &rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    vec2 operator+(const vec2 &lhs, const vec2 &rhs) {
        return vec2(
            lhs.x + rhs.x,
            lhs.y + rhs.y
        );
    }

    vec2 operator-(const vec2 &lhs, const vec2 &rhs) {
        return vec2(
            lhs.x - rhs.x,
            lhs.y - rhs.y
        );
    }

    vec2 operator*(const vec2 &lhs, const long double &rhs) {
        return vec2(
            lhs.x * rhs,
            lhs.y * rhs
        );
    }

    vec2 operator*(const long double &lhs, const vec2 &rhs) {
        return vec2(
            lhs * rhs.x,
            lhs * rhs.y
        );
    }



    vec3::vec3() {}

    vec3::vec3(const vec3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
    }

    vec3::vec3(long double a_x, long double a_y, long double a_z) {
        x = a_x;
        y = a_y;
        z = a_z;
    }

    vec3::operator glm::vec3() const{
        return glm::vec3(
            x, y, z
        );
    }

    vec3& vec3::operator*=(long double rhs) {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    vec3 vec3::operator-() const {
        return vec3(
            -this->x,
            -this->y,
            -this->z
        );
    }

    vec3& vec3::operator+=(const vec3 &rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    vec3& vec3::operator-=(const vec3 &rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    long double vec3::dot(const vec3 &rhs) const {
        return (
            x * rhs.x +
            y * rhs.y +
            z * rhs.z
        );
    }

    vec3 vec3::cross(const vec3 &rhs) const {
        return vec3(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }

    long double vec3::sqrNorm() const {
        return (
            x * x +
            y * y +
            z * z
        );
    }

    long double vec3::norm() const {
        return sqrt(
            x * x +
            y * y +
            z * z
        );
    }

    long double vec3::sqrDist(const vec3 &v) const {
        return (
            (x - v.x) * (x - v.x) +
            (y - v.y) * (y - v.y) +
            (z - v.z) * (z - v.z)
        );
    }

    long double vec3::dist(const vec3 &v) const {
        return sqrt(
            (x - v.x) * (x - v.x) +
            (y - v.y) * (y - v.y) +
            (z - v.z) * (z - v.z)
        );
    }

    vec3& vec3::normalize() {
        long double inv_norm = 1.0l / norm();
        x *= inv_norm;
        y *= inv_norm;
        z *= inv_norm;
        return *this;
    }

    vec3 operator+(const vec3 &lhs, const vec3 &rhs) {
        return vec3(
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z
        );
    }

    vec3 operator-(const vec3 &lhs, const vec3 &rhs) {
        return vec3(
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z
        );
    }

    vec3 operator*(const vec3 &lhs, const long double &rhs) {
        return vec3(
            lhs.x * rhs,
            lhs.y * rhs,
            lhs.z * rhs
        );
    }

    vec3 operator*(const long double &lhs, const vec3 &rhs) {
        return vec3(
            lhs * rhs.x,
            lhs * rhs.y,
            lhs * rhs.z
        );
    }
}

