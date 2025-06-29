#pragma once

#include <chrono>
#include <cstdint>

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using iptr = std::intptr_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using uptr = std::uintptr_t;
using usize = std::size_t;

using f32 = float;
using f64 = double;

struct Point {
    i32 x = 0;
    i32 y = 0;

    Point& operator+=(Point other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};

inline bool operator==(Point lhs, Point rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
inline Point operator+(Point lhs, Point rhs) { return {lhs.x + rhs.x, lhs.y + rhs.y}; }
inline Point operator-(Point lhs, Point rhs) { return {lhs.x - rhs.x, lhs.y - rhs.y}; }

inline constexpr Point Up = {0, -1};
inline constexpr Point Down = {0, 1};
inline constexpr Point Left = {-1, 0};
inline constexpr Point Right = {1, 0};

struct Color {
    u8 r = 0;
    u8 g = 0;
    u8 b = 0;
    u8 a = 0;
};

inline constexpr Color White = {255, 255, 255, 255};
inline constexpr Color Black = {0, 0, 0, 255};
inline constexpr Color Red = {255, 0, 0, 255};
inline constexpr Color Green = {0, 255, 0, 255};
inline constexpr Color Blue = {0, 0, 255, 255};

class Clock {
public:
    void update() {
        const auto now = std::chrono::high_resolution_clock::now();
        m_delta = now - m_previous;
        m_previous = now;
    }

    [[nodiscard]] constexpr f64 delta_sec() const { return static_cast<f64>(m_delta.count()) / 1'000'000'000.0; }

private:
    std::chrono::high_resolution_clock::time_point m_previous = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds m_delta{0};
};
