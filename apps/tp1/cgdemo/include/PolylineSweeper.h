// OVERVIEW: PolylineSweeper.h
// ========
// Class definition for polyline sweepers (base class).
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 31/10/2026

#ifndef __PolylineSweeper_h
#define __PolylineSweeper_h

#include "Polyline.h"
#include "geometry/TriangleMesh.h"

namespace cg
{ // begin namespace cg

/// @brief PolylineSweeperLimits: Represents the limits of a base polyline sweeper
template<IsReal R>
struct PolylineSweeperLimits
{
  // do nothing (reserved for limits common to all sweepers)

}; // PolylineSweeperLimits

/// @brief PolylineSweeperAttrs: Attributes common to all polyline sweepers
template<IsReal R>
struct PolylineSweeperAttrs
{
  bool capped{false};

}; // PolylineSweeperAttrs


/// @brief PolylineSweeper: Represents a base PolylineSweeper
template<IsReal R>
class PolylineSweeper
{
public:
  using type = PolylineSweeper<R>;
  using value_type = R;
  using polyline_type = Polyline<R>;
  using limits = PolylineSweeperLimits<R>;
  using attrs_type = PolylineSweeperAttrs<R>;

  virtual ~PolylineSweeper() = default;

  virtual const attrs_type& attributes() const = 0;

  //Sweep method
  [[nodiscard]] virtual TriangleMesh* sweep(const polyline_type& polyline) const = 0;

  bool isCapped() const
  {
    return attributes().capped;
  }

protected:
  static void clampSet([[maybe_unused]] attrs_type& attr)
  {
    // do nothing (no common limits yet)
  }

  /// @brief Caps are only valid for closed and non-extruded polylines
  bool hasCaps(const polyline_type& polyline) const
  {
    return isCapped() && polyline.isClosed() && !polyline.isExtruded();
  }

}; // PolylineSweeper

template<IsReal R>
inline void setTriangle(TriangleMesh::Triangle* t,  
  index_t i, 
  index_t j,
  index_t k)
{
  t->setVertices(i, j, k);
}

template<IsReal R>
inline void setVertexArray(Vector3<R>* to, const Vector3<R>* from, const size_t total)
{
  for(index_t i = 0; i < total; ++i) to[i] = from[i];
}

} // end namespace cg

#endif // __PolylineSweeper_h