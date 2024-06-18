#ifndef INCLUDE_DATA_HPP_
#define INCLUDE_DATA_HPP_

#include <string>
#include <utility>

#include "Point.hpp"

class Data {
private:
  std::string m_image_path;
  Point m_embedding;

public:
  Data(const Point &_embedding, std::string image_path)
      : m_image_path(std::move(image_path)), m_embedding(_embedding) {}

  // Getters
  [[nodiscard]] auto get_embedding() const -> const Point & {
    return m_embedding;
  }
  [[nodiscard]] auto get_path() const -> const std::string & {
    return m_image_path;
  }

  auto operator<=>(const Data &other) const {
    return m_image_path <=> other.m_image_path;
  }
};

#endif // INCLUDE_DATA_HPP_
