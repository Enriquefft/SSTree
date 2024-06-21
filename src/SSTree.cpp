#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <ranges>

#include "SSTree.hpp"

template <std::ranges::input_range R>

  requires std::indirectly_copyable_storable<std::ranges::iterator_t<R>,
                                             std::ranges::range_value_t<R> *>
constexpr auto variance(R &&range) -> double {
  // TODO(hybridz): Evaluate weather the use of std::forward is necessary

  auto size = static_cast<double>(std::ranges::size(range));

  if (size <= 1) {
    return 0; // Variance is undefined for empty ranges or single-element ranges
  }

  auto mean = std::accumulate(std::begin(range), std::end(range), 0.0) / size;
  auto sum_of_squares = std::inner_product(std::begin(range), std::end(range),
                                           std::begin(range), 0.0);
  auto variance_numerator = sum_of_squares / size - mean * mean;
  return variance_numerator * (size / (size - 1));
}

/**
 * intersectsPoint
 * Verifica si un punto está dentro de la esfera delimitadora del nodo.
 * @param point Punto a verificar.
 * @return bool - Retorna true si el punto está dentro de la esfera, de lo
 * contrario false.
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::intersects_point(const Point &point) const
    -> bool {
  return Point::distance(m_centroid, point) <= m_radius;
}

/**
 * findClosestChild
 * Encuentra el hijo más cercano a un punto dado.
 * @param target El punto objetivo para encontrar el hijo más cercano.
 * @return SSNode*: Retorna un puntero al hijo más cercano.
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::find_closest_child(const Point &target)
    -> std::shared_ptr<SSNode<MAX_POINTS_PER_NODE>> {
  return *std::ranges::min_element(
      m_children, [&target](const auto &child1, const auto &child2) {
        return Point::distance(child1->get_centroid(), target) <
               Point::distance(child2->get_centroid(), target);
      });
}

/**
 * updateBoundingEnvelope
 * Actualiza el centroide y el radio del nodo basándose en los nodos internos o
 * datos.
 *
 */
template <size_t MAX_POINTS_PER_NODE>
void SSNode<MAX_POINTS_PER_NODE>::update_bounding_envelope() {
  auto points = get_entries_centroids();
  m_centroid = std::accumulate(points.begin(), points.end(), Point(),
                               [](const auto &point1, const auto &point2) {
                                 return point1 + point2;
                               }) /
               static_cast<float>(points.size());

  m_radius = std::ranges::max(
      points | std::ranges::views::transform([&](const Point &point) {
        return Point::distance(point, m_centroid);
      }));
}

/**
 * directionOfMaxVariance
 * Calcula y retorna el índice de la dirección de máxima varianza.
 * @return size_t: Índice de la dirección de máxima varianza.
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::direction_of_max_variance() -> size_t {

  auto centroids = get_entries_centroids();

  auto kth_elements_view = [&centroids](size_t dim) {
    return centroids |
           std::views::transform([dim](const auto &vec) { return vec[dim]; });
  };

  auto max_variance_dim = std::ranges::max_element(
      std::views::iota(0UL, DIM),
      [&kth_elements_view](const auto &dim1, const auto &dim2) {
        auto variance1 = variance(kth_elements_view(dim1));
        auto variance2 = variance(kth_elements_view(dim2));

        return variance1 < variance2;
      });
  return *max_variance_dim;
}

/**
 * split
 * Divide el nodo y retorna el nuevo nodo creado.
 * Implementación similar a R-tree.
 * @return SSNode*: Puntero al nuevo nodo creado por la división.
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::split() -> split_t {

  auto coordinate_index = direction_of_max_variance();

  auto split_index = find_split_index(coordinate_index);

  if (m_isLeaf) {
    auto new_node1 = std::make_shared<SSNode<MAX_POINTS_PER_NODE>>(
        std::vector(m_data.begin(),
                    m_data.begin() + static_cast<int64_t>(split_index)),
        this->shared_from_this());
    auto new_node2 = std::make_shared<SSNode<MAX_POINTS_PER_NODE>>(
        std::vector(m_data.begin() + static_cast<int64_t>(split_index),
                    m_data.end()),
        this->shared_from_this());
    return std::make_pair(new_node1, new_node2);
  }

  auto new_node1 = std::make_shared<SSNode<MAX_POINTS_PER_NODE>>(
      std::vector(m_children.begin(),
                  m_children.begin() + static_cast<int64_t>(split_index)),
      this->shared_from_this());
  auto new_node2 = std::make_shared<SSNode<MAX_POINTS_PER_NODE>>(
      std::vector(m_children.begin() + static_cast<int64_t>(split_index),
                  m_children.end()),
      this->shared_from_this());

  return std::make_pair(new_node1, new_node2);
}

template <size_t MAX_POINTS_PER_NODE>
void SSNode<MAX_POINTS_PER_NODE>::add_child(
    const std::shared_ptr<SSNode> &child) {
  m_children.push_back(child);
  update_bounding_envelope();
}

template <size_t MAX_POINTS_PER_NODE>
void SSNode<MAX_POINTS_PER_NODE>::add_data(const std::shared_ptr<Data> &_data) {

  m_data.push_back(_data);
  update_bounding_envelope();
}
/**
 * findSplitIndex
 * Encuentra el índice de división en una coordenada específica.
 * @param coordinate_index Índice de la coordenada para encontrar el índice de
 * división.
 * @return size_t: Índice de la división.
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::find_split_index(size_t coordinate_index)
    -> size_t {

  std::vector<float> values;
  std::ranges::transform(get_entries_centroids(), std::back_inserter(values),
                         [&coordinate_index](const auto &point) {
                           return point[coordinate_index];
                         });
  return min_variance_split(values);
}

/**
 * getEntriesCentroids
 * Devuelve los centroides de las entradas.
 * Estos centroides pueden ser puntos almacenados en las hojas o los centroides
 * de los nodos hijos en los nodos internos.
 * @return std::vector<Point>: Vector de centroides de las entradas.
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::get_entries_centroids()
    -> std::vector<Point> {

  std::vector<Point> centroids;

  if (m_isLeaf) {
    std::ranges::transform(
        m_data, std::back_inserter(centroids),
        [](const auto &data) { return data->get_embedding(); });
  } else {
    std::ranges::transform(
        m_children, std::back_inserter(centroids),
        [](const auto &child) { return child->get_centroid(); });
  }

  return centroids;
}

/**
 * minVarianceSplit
 * Encuentra el índice de división óptimo para una lista de valores, de tal
 * manera que la suma de las varianzas de las dos particiones resultantes sea
 * mínima.
 * @param values Vector de valores para encontrar el índice de mínima varianza.
 * @return size_t: Índice de mínima varianza.
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::min_variance_split(
    const std::vector<float> &values) const -> size_t {

  auto sorted_values = values;
  std::ranges::sort(sorted_values);

  double min_variance = std::numeric_limits<double>::max();

  auto split_index = MIN_POINTS_PER_NODE::value;

  for (auto i = split_index; i < values.size() - MIN_POINTS_PER_NODE::value;
       ++i) {

    auto variance1 = variance(
        std::ranges::subrange(sorted_values.begin(),
                              sorted_values.begin() + static_cast<int64_t>(i)));
    auto variance2 = variance(std::ranges::subrange(
        sorted_values.begin() + static_cast<int64_t>(i), sorted_values.end()));

    if (variance1 + variance2 < min_variance) {
      min_variance = variance1 + variance2;
      split_index = i;
    }
  }
  return split_index;
}

/**
 * searchParentLeaf
 * Busca el nodo hoja adecuado para insertar un punto.
 * @param target Punto objetivo para la búsqueda.
 * @return SSNode*: Nodo hoja adecuado para la inserción.
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::search_parent_leaf(const Point &target)
    -> std::shared_ptr<SSNode> {
  if (m_isLeaf) {
    return this->shared_from_this();
  }
  auto child = find_closest_child(target);
  return child->search_parent_leaf(target);
}

/**
 * insert
 * Inserta un dato en el nodo, dividiéndolo si es necesario.
 * @param data Dato a insertar.
 * @return SSNode*: Nuevo nodo raíz si se dividió, de lo contrario nullptr.
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::insert(const std::shared_ptr<Data> &data)
    -> split_t {

  if (m_isLeaf) {

    if (std::ranges::find(m_data, data) != m_data.end()) {
      return std::nullopt;
    }

    m_data.push_back(data);
    update_bounding_envelope();
    if (m_data.size() <= MAX_POINTS_PER_NODE) {
      return std::nullopt;
    }
    return split();
  }

  auto closest_child = find_closest_child(data->get_embedding());
  auto new_nodes = closest_child->insert(data);

  if (new_nodes == std::nullopt) {
    update_bounding_envelope();
    return std::nullopt;
  }
  // A split was made

  // remove closest_child from children
  std::ranges::remove(m_children, closest_child);

  // add the new nodes to the children
  m_children.push_back(new_nodes->first);
  m_children.push_back(new_nodes->second);

  if (m_children.size() <= MAX_POINTS_PER_NODE) {
    return std::nullopt;
  }
  return split();
}

/**
 * search
 * Busca un dato específico en el árbol.
 * @param target Dato a buscar.
 * @return SSNode*: Nodo que contiene el dato (o nullptr si no se encuentra).
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSNode<MAX_POINTS_PER_NODE>::search(const Point &target)
    -> std::shared_ptr<SSNode> {
  if (m_isLeaf) {
    for (const auto &point : m_data) {
      if (point->get_embedding() == target) {
        return this->shared_from_this();
      }
    }
    return nullptr;
  }
  for (const auto &child : m_children) {
    if (child->intersects_point(target)) {
      return child->search(target);
    }
  }
  return nullptr;
}
/**
 * insert
 * Inserta un dato en el árbol.
 * @param data Dato a insertar.
 */
template <size_t MAX_POINTS_PER_NODE>
void SSTree<MAX_POINTS_PER_NODE>::insert(const std::shared_ptr<Data> &data) {

  std::cout << "Inserting data: " << data->get_path() << '\n';

  if (m_root == nullptr) {
    m_root = std::make_shared<SSNode>(data->get_embedding(), 0.0F);
  }
  m_root->insert(data);
}

/**
 * search
 * Busca un dato específico en el árbol.
 * @param data Dato a buscar.
 * @return SSNode*: Nodo que contiene el dato (o nullptr si no se encuentra).
 */
template <size_t MAX_POINTS_PER_NODE>
auto SSTree<MAX_POINTS_PER_NODE>::search(const std::shared_ptr<Data> &data)
    -> std::shared_ptr<SSNode> {
  return m_root->search(data->get_embedding());
}

// Explicit instantiation
template class SSTree<20>;
template class SSNode<20>;

template class SSTree<7>;
template class SSNode<7>;

template class SSTree<11>;
template class SSNode<11>;
