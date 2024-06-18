#include "../include/SSTree.hpp"

/**
 * intersectsPoint
 * Verifica si un punto está dentro de la esfera delimitadora del nodo.
 * @param point Punto a verificar.
 * @return bool - Retorna true si el punto está dentro de la esfera, de lo
 * contrario false.
 */
auto SSNode::intersects_point(const Point &point) const -> bool {
  return Point::distance(m_centroid, point) <= m_radius;
}

/**
 * findClosestChild
 * Encuentra el hijo más cercano a un punto dado.
 * @param target El punto objetivo para encontrar el hijo más cercano.
 * @return SSNode*: Retorna un puntero al hijo más cercano.
 */
auto SSNode::find_closest_child(const Point &target)
    -> std::shared_ptr<SSNode> {
  return nullptr;
}

/**
 * updateBoundingEnvelope
 * Actualiza el centroide y el radio del nodo basándose en los nodos internos o
 * datos.
 */
void SSNode::update_bounding_envelope() {}

/**
 * directionOfMaxVariance
 * Calcula y retorna el índice de la dirección de máxima varianza.
 * @return size_t: Índice de la dirección de máxima varianza.
 */
auto SSNode::direction_of_max_variance() -> size_t { return 0; }

/**
 * split
 * Divide el nodo y retorna el nuevo nodo creado.
 * Implementación similar a R-tree.
 * @return SSNode*: Puntero al nuevo nodo creado por la división.
 */
auto SSNode::split() -> std::shared_ptr<SSNode> { return nullptr; }

void SSNode::add_child(std::shared_ptr<SSNode> child) {}
void SSNode::add_data(const std::shared_ptr<Data> &_data) {}

/**
 * findSplitIndex
 * Encuentra el índice de división en una coordenada específica.
 * @param coordinate_index Índice de la coordenada para encontrar el índice de
 * división.
 * @return size_t: Índice de la división.
 */
auto SSNode::find_split_index(size_t coordinate_index) -> size_t { return 0; }

/**
 * getEntriesCentroids
 * Devuelve los centroides de las entradas.
 * Estos centroides pueden ser puntos almacenados en las hojas o los centroides
 * de los nodos hijos en los nodos internos.
 * @return std::vector<Point>: Vector de centroides de las entradas.
 */
auto SSNode::get_entries_centroids() -> std::vector<Point> { return {}; }

/**
 * minVarianceSplit
 * Encuentra el índice de división óptimo para una lista de valores, de tal
 * manera que la suma de las varianzas de las dos particiones resultantes sea
 * mínima.
 * @param values Vector de valores para encontrar el índice de mínima varianza.
 * @return size_t: Índice de mínima varianza.
 */
auto SSNode::min_variance_split(const std::vector<float> &values) -> size_t {
  return 0;
}

/**
 * searchParentLeaf
 * Busca el nodo hoja adecuado para insertar un punto.
 * @param node Nodo desde el cual comenzar la búsqueda.
 * @param target Punto objetivo para la búsqueda.
 * @return SSNode*: Nodo hoja adecuado para la inserción.
 */
auto SSNode::search_parent_leaf(std::shared_ptr<SSNode> node,
                                const Point &target)
    -> std::shared_ptr<SSNode> {
  return nullptr;
}

/**
 * insert
 * Inserta un dato en el nodo, dividiéndolo si es necesario.
 * @param node Nodo donde se realizará la inserción.
 * @param data Dato a insertar.
 * @return SSNode*: Nuevo nodo raíz si se dividió, de lo contrario nullptr.
 */
auto SSNode::insert(std::shared_ptr<SSNode> node,
                    const std::shared_ptr<Data> &data)
    -> std::shared_ptr<SSNode> {
  return nullptr;
}

/**
 * search
 * Busca un dato específico en el árbol.
 * @param node Nodo desde el cual comenzar la búsqueda.
 * @param target Dato a buscar.
 * @return SSNode*: Nodo que contiene el dato (o nullptr si no se encuentra).
 */
auto SSNode::search(std::shared_ptr<SSNode> node,
                    const Point &target) -> std::shared_ptr<SSNode> {
  return nullptr;
}

/**
 * insert
 * Inserta un dato en el árbol.
 * @param data Dato a insertar.
 */
void SSTree::insert(const std::shared_ptr<Data> &data) {
  // if (m_root == nullptr) {
  //   m_root =
  //       std::make_shared<SSNode>(maxPointsPerNode, data->getEmbedding(),
  //       0.0F);
  //   m_root->addData(data);
  // } else {
  //   auto leaf = m_root->searchParentLeaf(root, data->getEmbedding());
  //   auto newParent = leaf->insert(leaf, data);
  //   if (newParent != nullptr) {
  //     if (newParent->getParent() == nullptr) {
  //       root = newParent;
  //     }
  //   }
  // }
}

/**
 * search
 * Busca un dato específico en el árbol.
 * @param data Dato a buscar.
 * @return SSNode*: Nodo que contiene el dato (o nullptr si no se encuentra).
 */
auto SSTree::search(const std::shared_ptr<Data> &data)
    -> std::shared_ptr<SSNode> {
  return nullptr;
}
