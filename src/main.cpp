#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_set>
#include <vector>

#include "Data.hpp"
#include "Point.hpp"
#include "SSTree.hpp"

constexpr size_t NUM_POINTS = 10000;
constexpr size_t MAX_POINTS_PER_NODE = 20;

/*
 * Helper functions
 */
inline auto
generate_random_data(size_t num_points) -> std::vector<std::shared_ptr<Data>> {
  std::vector<std::shared_ptr<Data>> data;
  for (size_t i = 0; i < num_points; ++i) {
    Point embedding = Point::random();
    std::string image_path = "image_" + std::to_string(i) + ".jpg";
    std::shared_ptr<Data> data_point =
        std::make_shared<Data>(embedding, image_path);
    data.push_back(data_point);
  }
  return data;
}

inline void
collect_data_dfs(const std::shared_ptr<SSNode> &node,
                 std::unordered_set<std::shared_ptr<Data>> &tree_data) {
  if (node->get_is_leaf()) {
    for (const auto &data : node->get_data()) {
      tree_data.insert(data);
    }
  } else {
    for (const auto &child : node->get_children()) {
      collect_data_dfs(child, tree_data);
    }
  }
}

/*
 * Testing functions
 */

// Test 1: Check if all data is present in the tree
inline auto
all_data_present(const SSTree &tree,
                 const std::vector<std::shared_ptr<Data>> &data) -> bool {
  std::unordered_set<std::shared_ptr<Data>> data_set(data.begin(), data.end());
  std::unordered_set<std::shared_ptr<Data>> tree_data;

  collect_data_dfs(tree.get_root(), tree_data);

  return (std::ranges::all_of(data_set,
                              [&tree_data](const auto &data_point) {
                                return tree_data.contains(data_point);
                              })) &&
         (std::ranges::all_of(tree_data,
                              [&data_set](const auto &data_point) {
                                return !data_set.contains(data_point);
                              }))

      ;
}

// Test 2: Check if all leaves are at the same level
inline auto leaves_at_same_level_dfs(const std::shared_ptr<SSNode> &node,
                                     const int &level,
                                     int &leaf_level) -> bool {
  if (node->get_is_leaf()) {
    if (leaf_level == -1) {
      leaf_level = level;
      return true;
    }
    return leaf_level == level;
  }

  return std::ranges::all_of(
      node->get_children(), [&level, &leaf_level](const auto &child) {
        return leaves_at_same_level_dfs(child, level + 1, leaf_level);
      });
}

inline auto leaves_at_same_level(const std::shared_ptr<SSNode> &root) -> bool {
  int leaf_level = -1;
  return leaves_at_same_level_dfs(root, 0, leaf_level);
}

// Test 3: Check if no node exceeds the maximum number of children
inline auto
no_node_exceeds_max_children_dfs(const std::shared_ptr<SSNode> &node,
                                 size_t max_points_per_node) -> bool {

  return node->get_children().size() <= max_points_per_node &&
         std::ranges::all_of(node->get_children(),
                             [&max_points_per_node](const auto &child) {
                               return no_node_exceeds_max_children_dfs(
                                   child, max_points_per_node);
                             }

         );
}

inline auto no_node_exceeds_max_children(const std::shared_ptr<SSNode> &root,
                                         size_t max_points_per_node) -> bool {
  return no_node_exceeds_max_children_dfs(root, max_points_per_node);
}

// Test 4: Check if all points are inside the bounding sphere of their
// respective nodes
inline auto
sphere_covers_all_points_dfs(const std::shared_ptr<SSNode> &node) -> bool {

  const Point &centroid = node->get_centroid();
  float radius = node->get_radius();

  return !node->get_is_leaf() ||
         std::ranges::all_of(
             node->get_data(), [&radius, &centroid](auto &data) {
               return Point::distance(centroid, data->get_embedding()) > radius;
             });
}

inline auto
dfs_sphere_covers_all_points(const std::shared_ptr<SSNode> &node) -> bool {
  if (node->get_is_leaf()) {
    return sphere_covers_all_points_dfs(node);
  }

  return std::ranges::all_of(node->get_children(), [](const auto &child) {
    return dfs_sphere_covers_all_points(child);
  });
}
inline auto
sphere_covers_all_points(const std::shared_ptr<SSNode> &root) -> bool {
  return dfs_sphere_covers_all_points(root);
}

// Test 5: Check if all children are inside the bounding sphere of their parent
// node
inline auto sphere_covers_all_children_spheres_dfs(
    const std::shared_ptr<SSNode> &node) -> bool {
  const Point &centroid = node->get_centroid();
  float radius = node->get_radius();

  return node->get_is_leaf() ||
         std::ranges::all_of(node->get_children(), [&radius,
                                                    &centroid](auto &child) {
           const Point &child_centroid = child->get_centroid();
           float child_radius = child->get_radius();
           return Point::distance(centroid, child_centroid) + child_radius <=
                  radius;
         });
}

inline auto dfs_sphere_covers_all_children_spheres(
    const std::shared_ptr<SSNode> &node) -> bool {

  return sphere_covers_all_children_spheres_dfs(node) &&
         std::ranges::all_of(node->get_children(), [](const auto &child) {
           return dfs_sphere_covers_all_children_spheres(child);
         });
}
inline auto sphere_covers_all_children_spheres(
    const std::shared_ptr<SSNode> &root) -> bool {
  return dfs_sphere_covers_all_children_spheres(root);
}

auto main() -> int {
  auto data = generate_random_data(NUM_POINTS);
  SSTree tree(MAX_POINTS_PER_NODE);
  for (const auto &data_point : data) {
    tree.insert(data_point);
  }

  // Realizar pruebas
  bool all_present = all_data_present(tree, data);
  bool same_level = leaves_at_same_level(tree.get_root());
  bool no_exceed =
      no_node_exceeds_max_children(tree.get_root(), MAX_POINTS_PER_NODE);
  bool sphere_points = sphere_covers_all_points(tree.get_root());
  bool sphere_children = sphere_covers_all_children_spheres(tree.get_root());

  std::cout << "All data is present: " << (all_present ? "Sí" : "No") << '\n';
  std::cout << "Nodos hojas en el mismo nivel: " << (same_level ? "Sí" : "No")
            << '\n';
  std::cout << "No se supera el límite de hijos por nodo: "
            << (no_exceed ? "Sí" : "No") << '\n';
  std::cout << "Hiper-esfera cubre todos los puntos de los nodos hoja: "
            << (sphere_points ? "Sí" : "No") << '\n';
  std::cout << "Hiper-esfera cubre todas las hiper-esferas internas de los "
               "nodos internos: "
            << (sphere_children ? "Sí" : "No") << '\n';
  std::cout << "Happy ending! :D" << '\n';

  return 0;
}
