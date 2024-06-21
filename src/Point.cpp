#include "Point.hpp"
#include <algorithm>
#include <random>
#include <stdexcept>

// float comparison error margin
constexpr float EPSILON = 1e-5F;
constexpr size_t MM256_VEC_SIZE = 8;

Point::Point(const std::array<float, DIM> &_coordinates)
    : m_coordinates(_coordinates) {}

auto Point::operator+(const Point &other) const -> Point {

  Point result;
  std::ranges::transform(m_coordinates, other.m_coordinates,
                         result.m_coordinates.begin(), std::plus<>());
  return result;
}

auto Point::operator+=(const Point &other) -> Point & {
  std::ranges::transform(m_coordinates, other.m_coordinates,
                         m_coordinates.begin(), std::plus<>());
  return *this;
}

auto Point::operator-(const Point &other) const -> Point {
  Point result;

  std::ranges::transform(m_coordinates, other.m_coordinates,
                         result.m_coordinates.begin(), std::minus<>());

  return result;
}
auto Point::operator-=(const Point &other) -> Point & {

  std::ranges::transform(m_coordinates, other.m_coordinates,
                         m_coordinates.begin(), std::minus<>());

  return *this;
}

auto Point::operator*(float scalar) const -> Point {
  Point result;

  std::ranges::transform(m_coordinates, result.m_coordinates.begin(),
                         [&scalar](float coord) { return coord * scalar; });

  return result;
}

auto Point::operator*=(float scalar) -> Point & {
  std::ranges::transform(m_coordinates, m_coordinates.begin(),
                         [&scalar](float coord) { return coord * scalar; });

  return *this;
}

auto Point::operator/(float scalar) const -> Point {
  if (scalar - 0 <= EPSILON) {
    throw std::invalid_argument("Division by zero");
  }
  Point result;

  std::ranges::transform(m_coordinates, result.m_coordinates.begin(),
                         [&scalar](float coord) { return coord / scalar; });

  return result;
}

auto Point::operator/=(float scalar) -> Point & {
  if (scalar - 0 <= EPSILON) {
    throw std::invalid_argument("Division by zero");
  }

  std::ranges::transform(m_coordinates, m_coordinates.begin(),
                         [&scalar](float coord) { return coord / scalar; });

  return *this;
}

auto Point::operator==(const Point &other) const -> bool {
  return std::ranges::equal(m_coordinates, other.m_coordinates,
                            [](const float &coord1, const float &coord2) {
                              return std::abs(coord1 - coord2) <= EPSILON;
                            });
}

auto Point::norm() const -> float {
  __m256 vsum = _mm256_setzero_ps();

  for (size_t i = 0; i + MM256_VEC_SIZE - 1 < DIM; i += MM256_VEC_SIZE) {
    __m256 vcoords = _mm256_loadu_ps(&m_coordinates.at(i));
    vsum = _mm256_add_ps(vsum, _mm256_mul_ps(vcoords, vcoords));
  }

  std::array<float, MM256_VEC_SIZE> buffer{};
  _mm256_storeu_ps(buffer.data(), vsum);

  float sum = 0.0F;
  sum = std::accumulate(buffer.begin(), buffer.end(), 0.0F);

  std::ranges::for_each(m_coordinates,
                        [&sum](float coord) { sum += coord * coord; });

  return std::sqrt(sum);
}

auto Point::operator[](std::size_t index) const -> float {
  if (index >= DIM) {
    throw std::out_of_range("Index out of range");
  }
  return m_coordinates.at(index);
}

auto Point::operator[](std::size_t index) -> float & {
  if (index >= DIM) {
    throw std::out_of_range("Index out of range");
  }
  return m_coordinates.at(index);
}

auto Point::random(float min, float max) -> Point {
  std::random_device random_device;
  std::mt19937 gen(random_device());
  std::uniform_real_distribution<float> dis(min, max);

  std::array<float, DIM> coordinates{};
  std::ranges::generate(coordinates, [&]() { return dis(gen); });

  return Point(coordinates);
}

auto Point::distance(const Point &point1, const Point &point2) -> float {
  float sum = 0.0F;
  __m256 vsum = _mm256_setzero_ps();

  for (size_t i = 0; i + MM256_VEC_SIZE - 1 < DIM; i += MM256_VEC_SIZE) {
    __m256 vcoords1 = _mm256_loadu_ps(&point1.m_coordinates.at(i));
    __m256 vcoords2 = _mm256_loadu_ps(&point2.m_coordinates.at(i));
    __m256 vdiff = _mm256_sub_ps(vcoords1, vcoords2);
    vsum = _mm256_add_ps(vsum, _mm256_mul_ps(vdiff, vdiff));
  }

  // Resultados parciales
  std::array<float, MM256_VEC_SIZE> buffer{};

  _mm256_storeu_ps(buffer.data(), vsum);

  sum = std::accumulate(buffer.begin(), buffer.end(), 0.0F);

  // Reducción
  for (size_t i = 0; i < DIM; ++i) {
    float diff = point1.m_coordinates.at(i) - point2.m_coordinates.at(i);
    sum += diff * diff;
  }
  return std::sqrt(sum);
}
