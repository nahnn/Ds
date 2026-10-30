// OVERVIEW: Spiral.h
// ========
// Class definition for Spiral Swap.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 27/10/2026

#ifndef __Spiral_h
#define __Spiral_h

#include "include/PolylineSweeper.h"
#include "cg/include/geometry/TriangleMesh.h"

namespace cg
{ // begin namespace cg

template<IsReal R>
struct SpiralLimits
{
  // --- Spiral Swap Transform ---
  // static constexpr R MinWe
  // static constexpr R MaxWe

  // static constexpr R MinRe
  // static constexpr R MaxRe

  // static constexpr size_t MinNes
  // static constexpr size_t MaxNes

  // static constexpr R MinDeh
  // static constexpr R MinDeh

  // static constexpr R MinDew
  // static constexpr R MaxDew

};

template<IsReal R>
class Spiral : public PolylineSweeper
{
public:
  using type = Spiral<R>;
  using value_type = R;
  using vec3 = Vector3<R>;
  using mat3 = Matrix3x3<R>;
  using quat = Quaternion<R>;

  Spiral(Polyline<R>& poly, R we, R re, size_t nes, R deh, R dew):
    _we{we},
    _re{re},
    _nes{nes},
    _deh{deh},
    _dew{dew}
  {
    // Do nothing
  }

  ~Spiral() = default;

  TriangleMesh* build(const Polyline<R>& poly) override;

private:
  R _we{};
  R _re{};
  size_t _nes{};
  R _deh{};
  R _dew{};

}; // Spiral

template<IsReal R> using spiral = Spiral<R>;

} // end namespace cg

#endif // __Spiral_h