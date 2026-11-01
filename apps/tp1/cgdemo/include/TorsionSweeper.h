// OVERVIEW: TorsionSweeper.h
// ========
// Class definition for translational sweepers with torsion.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 31/10/2026

#ifndef __TorsionSweeper_h
#define __TorsionSweeper_h

#include "PolylineSweeper.h"

namespace cg
{ // begin namespace cg

template<IsReal R>
struct TorsionSweeperLimits: public PolylineSweeperLimits<R>
{
  // --- Length ---
  static constexpr R MIN_LV = 1; // min lv
  static constexpr R MAX_LV = 50; // max lv

  // --- Subdivisions ---
  static constexpr int MIN_NSV = 1; // min nsv
  static constexpr int MAX_NSV = 50; // max nsv

  // --- Scale (begin and end) ---
  static constexpr R MIN_SCALEV = .1; // min sb and se
  static constexpr R MAX_SCALEV = 5; // max sb and se

  // --- Torsion (1 = 360 degrees) ---
  static constexpr R MIN_RV = -2; // min rv
  static constexpr R MAX_RV = 2; // max rv

  // ow (-bx..bx) and oh (-by..by) checked  in sweep()
  // 

}; // TorsionSweeperLimits

template<IsReal R>
struct TorsionSweeperAttrs: public PolylineSweeperAttrs<R>
{
  R lv{10}; // length
  R ow{0}; // horizontal offset
  R oh{0}; // vertical offset
  int nsv{10}; // subdivisions
  R sb{1}; // initial scale
  R se{1}; // final scale
  R rv{0}; // torsion
  
}; // TorsionSweeperAttrs


template<IsReal R>
class TorsionSweeper: public PolylineSweeper<R>
{
public:
  using type = TorsionSweeper<R>;
  using value_type = R;
  using polyline_type = Polyline<R>;
  using limits = TorsionSweeperLimits<R>;
  using attrs_type = TorsionSweeperAttrs<R>;

  explicit TorsionSweeper(const attrs_type& attr = {})
  {
    set(attr);
  }

  const attrs_type& attributes() const override
  {
    return _attr;
  }

  /// sets the attributes
  void set(const attrs_type& attr)
  {
    auto a = attr;

    //clamp set base attributes
    this->clampSet(a);

    a.lv = std::clamp(a.lv, limits::MIN_LV, limits::MAX_LV);
    a.nsv = std::clamp(a.nsv, limits::MIN_NSV, limits::MAX_NSV);
    a.sb = std::clamp(a.sb, limits::MIN_SCALEV, limits::MAX_SCALEV);
    a.se = std::clamp(a.se, limits::MIN_SCALEV, limits::MAX_SCALEV);
    a.rv = std::clamp(a.rv, limits::MIN_RV, limits::MAX_RV);

    _attr = a;
  }

  [[nodiscard]] TriangleMesh* sweep(const polyline_type& polyline) const override;

private:
  attrs_type _attr;

}; // TorsionSweeper

} // end namespace cg

#endif // __TorsionSweeper_h