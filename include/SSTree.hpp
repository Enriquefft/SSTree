#ifndef INCLUDE_SSTREE_HPP_
#define INCLUDE_SSTREE_HPP_

#include <memory>
#include <vector>

#include "Data.hpp"
#include "Point.hpp"

class SSNode : public std::enable_shared_from_this<SSNode> {
private:
  size_t m_maxPointsPerNode;
  Point m_centroid;
  float m_radius;
  bool m_isLeaf;
  std::weak_ptr<SSNode> m_parent;
  std::vector<std::shared_ptr<SSNode>> m_children;
  std::vector<std::shared_ptr<Data>> m_data;

  // For searching
  auto find_closest_child(const Point &target) -> std::shared_ptr<SSNode>;

  // For insertion
  void update_bounding_envelope();
  auto direction_of_max_variance() -> size_t;
  auto split() -> std::shared_ptr<SSNode>;
  auto find_split_index(size_t coordinate_index) -> size_t;
  auto get_entries_centroids() -> std::vector<Point>;
  auto min_variance_split(const std::vector<float> &values) -> size_t;

public:
  SSNode(size_t _maxPointsPerNode, const Point &_centroid, float _radius,
         bool _isLeaf = true, const std::shared_ptr<SSNode> &_parent = nullptr)
      : m_maxPointsPerNode(_maxPointsPerNode), m_centroid(_centroid),
        m_radius(_radius), m_isLeaf(_isLeaf), m_parent(_parent) {}

  // Checks if a point is inside the bounding sphere
  auto intersects_point(const Point &point) const -> bool;

  // Getters
  auto get_centroid() const -> const Point & { return m_centroid; }
  auto get_radius() const -> float { return m_radius; }
  auto get_children() const -> const std::vector<std::shared_ptr<SSNode>> & {
    return m_children;
  }
  auto get_data() const -> const std::vector<std::shared_ptr<Data>> & {
    return m_data;
  }
  auto get_is_leaf() const -> bool { return m_isLeaf; }
  auto get_parent() const -> std::shared_ptr<SSNode> { return m_parent.lock(); }

  // Adders
  void add_child(std::shared_ptr<SSNode> child);
  void add_data(const std::shared_ptr<Data> &_data);

  // Insertion
  auto search_parent_leaf(std::shared_ptr<SSNode> node,
                          const Point &target) -> std::shared_ptr<SSNode>;
  auto insert(std::shared_ptr<SSNode> node,
              const std::shared_ptr<Data> &data) -> std::shared_ptr<SSNode>;
  auto search(std::shared_ptr<SSNode> node,
              const Point &target) -> std::shared_ptr<SSNode>;
};

class SSTree {
private:
  std::shared_ptr<SSNode> m_root;
  size_t m_maxPointsPerNode;

public:
  explicit SSTree(size_t _maxPointsPerNode)
      : m_root(nullptr), m_maxPointsPerNode(_maxPointsPerNode) {}

  [[nodiscard]] auto get_root() const -> std::shared_ptr<SSNode> {
    return m_root;
  }

  void insert(const std::shared_ptr<Data> &data);
  auto search(const std::shared_ptr<Data> &_data) -> std::shared_ptr<SSNode>;
};

#endif // INCLUDE_SSTREE_HPP_
