#ifndef INCLUDE_SSTREE_HPP_
#define INCLUDE_SSTREE_HPP_

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "Data.hpp"
#include "Point.hpp"

template <size_t MAX_POINTS_PER_NODE> class SSNode;

template <typename T, size_t MAX_POINTS_PER_NODE>
concept DataOrNode =
    std::is_same_v<T, Data> || std::is_same_v<T, SSNode<MAX_POINTS_PER_NODE>>;

template <size_t MAX_POINTS_PER_NODE>
class SSNode
    : public std::enable_shared_from_this<SSNode<MAX_POINTS_PER_NODE>> {
private:
  using MIN_POINTS_PER_NODE =
      std::integral_constant<size_t, MAX_POINTS_PER_NODE / 2>;

  Point m_centroid;
  float m_radius;
  bool m_isLeaf;
  std::weak_ptr<SSNode> m_parent;
  std::vector<std::shared_ptr<SSNode>> m_children;
  std::vector<std::shared_ptr<Data>> m_data;

  using split_t = std::optional<
      std::pair<std::shared_ptr<SSNode>, std::shared_ptr<SSNode>>>;

  // For searching
  auto find_closest_child(const Point &target) -> std::shared_ptr<SSNode>;

  // For insertion
  void update_bounding_envelope();
  auto direction_of_max_variance() -> size_t;
  auto split() -> split_t;
  auto find_split_index(size_t coordinate_index) -> size_t;
  auto get_entries_centroids() -> std::vector<Point>;
  [[nodiscard]] auto
  min_variance_split(const std::vector<float> &values) const -> size_t;

public:
  SSNode(const Point &_centroid, float _radius, bool _isLeaf = true,
         const std::shared_ptr<SSNode> &_parent = nullptr)
      : m_centroid(_centroid), m_radius(_radius), m_isLeaf(_isLeaf),
        m_parent(_parent) {}

  // Initialize with vector of children or data
  template <typename T>
    requires DataOrNode<T, MAX_POINTS_PER_NODE>
  SSNode(const std::vector<std::shared_ptr<T>> &points,
         const std::shared_ptr<SSNode<MAX_POINTS_PER_NODE>> &_parent)
      : m_radius(0.0F), m_isLeaf(std::is_same_v<T, Data>), m_parent(_parent) {

    if constexpr (std::is_same_v<T, Data>) {
      m_data = points;
    } else {
      m_children = points;
    }
    update_bounding_envelope();
  }

  // Checks if a point is inside the bounding sphere
  [[nodiscard]] auto intersects_point(const Point &point) const -> bool;

  // Getters
  [[nodiscard]] auto get_centroid() const -> const Point & {
    return m_centroid;
  }
  [[nodiscard]] auto get_radius() const -> float { return m_radius; }
  auto get_children() const -> const std::vector<std::shared_ptr<SSNode>> & {
    return m_children;
  }
  [[nodiscard]] auto
  get_data() const -> const std::vector<std::shared_ptr<Data>> & {
    return m_data;
  }
  [[nodiscard]] auto get_is_leaf() const -> bool { return m_isLeaf; }
  auto get_parent() const -> std::shared_ptr<SSNode> { return m_parent.lock(); }

  // Adders
  void add_child(const std::shared_ptr<SSNode> &child);
  void add_data(const std::shared_ptr<Data> &_data);

  // Insertion
  auto search_parent_leaf(const Point &target) -> std::shared_ptr<SSNode>;
  auto insert(const std::shared_ptr<Data> &data) -> split_t;
  auto search(const Point &target) -> std::shared_ptr<SSNode>;
};

template <size_t MAX_POINTS_PER_NODE> class SSTree {
private:
  using SSNode = SSNode<MAX_POINTS_PER_NODE>;

  std::shared_ptr<SSNode> m_root;

public:
  SSTree() : m_root(nullptr) {}

  [[nodiscard]] auto get_root() const -> std::shared_ptr<SSNode> {
    return m_root;
  }

  void insert(const std::shared_ptr<Data> &data);
  auto search(const std::shared_ptr<Data> &data) -> std::shared_ptr<SSNode>;
};

// Explicit instantiation
extern template class SSNode<20>;
extern template class SSTree<20>;

extern template class SSNode<7>;
extern template class SSTree<7>;

extern template class SSNode<11>;
extern template class SSTree<11>;

#endif // INCLUDE_SSTREE_HPP_
