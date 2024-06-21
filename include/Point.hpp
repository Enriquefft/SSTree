#ifndef INCLUDE_POINT_HPP_
#define INCLUDE_POINT_HPP_

#include <immintrin.h>

#include <array>

constexpr std::size_t DIM = 768;

class Point {
public:
  Point() = default;
  explicit Point(const std::array<float, DIM> &coordinates);

  auto operator+(const Point &other) const -> Point;
  auto operator+=(const Point &other) -> Point &;
  auto operator-(const Point &other) const -> Point;
  auto operator-=(const Point &other) -> Point &;
  auto operator*(float scalar) const -> Point;
  auto operator*=(float scalar) -> Point &;
  auto operator/(float scalar) const -> Point;
  auto operator/=(float scalar) -> Point &;
  auto operator==(const Point &other) const -> bool;

  [[nodiscard]] auto norm() const -> float;

  auto operator[](std::size_t index) const -> float;
  auto operator[](std::size_t index) -> float &;

  static auto random(float min = 0.0F, float max = 1.0F) -> Point;
  static auto distance(const Point &point1, const Point &point2) -> float;

private:
  std::array<float, DIM> m_coordinates = {0.0F};
};

#endif // INCLUDE_POINT_HPP_
