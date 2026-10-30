// OVERVIEW: Polyline.h
// ========
// Class definition for utilities of polylines.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 24/10/2026

#ifndef __PolylineUtils_h
#define __PolylineUtils_h

namespace cg::PolylineUtils
{ // begin namespace cg::PolylineUtils

/// @brief PolylineLimits: Represents the limits of transformations of a polyline
template<IsReal R>
struct PolylineLimits
{
  // --- Polyline Points ---
  static constexpr size_t MinNp = 3; // min np
  static constexpr size_t MaxNp = 41; // max np

  // --- Polyline Segments ---
  static constexpr size_t MinNc = 2; // min np
  static constexpr size_t MaxNc = 40; // max np

  // --- Polyline Angle ---
  static constexpr R MinTc = 1; // min thetac
  static constexpr R MaxTc = 360; // max thetac

  // --- Scale Transform ---
  static constexpr R MinSp = .1; // min sp
  static constexpr R MaxSp = 5; // max sp

  // --- Rotation Transform ---
  static constexpr R MinTp = 0; // min thetap
  static constexpr R MaxTp = 360; // max thetap

  // --- Extrusion ---
  static constexpr R MinEp = .1; // min ep
  static constexpr R MaxEp = .9; // max ep

  // --- Spiral Swap Transform ---
  static constexpr R MinWe
  static constexpr R MaxWe

  static constexpr R MinRe
  static constexpr R MaxRe

  static constexpr size_t MinNes
  static constexpr size_t MaxNes

  static constexpr R MinDeh
  static constexpr R MinDeh

  static constexpr R MinDew
  static constexpr R MaxDew

  // --- Torsion Swap Transform ---

}; // PolylineLimits

} // end namespace cg

#endif // __PolylineUtils_h