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

using precision = float; // ?

/// @brief PolylineLimits: Represents the limits of transformations of a polyline
struct PolylineLimits
{
  // --- Polyline Points ---
  static constexpr size_t MinNp = 3; // min np
  static constexpr size_t MaxNp = 40; // max np

  // --- Polyline Segments ---
  static constexpr size_t MinNc = 2; // min np
  static constexpr size_t MaxNc = 40; // max np

  // --- Polyline Angle ---
  static constexpr float MinTc = 1; // min thetac
  static constexpr float MaxTc = 360; // max thetac

  // --- Scale Transform ---
  static constexpr float MinSp = .1; // min sp
  static constexpr float MaxSp = 5; // max sp

  // --- Rotation Transform ---
  static constexpr float MinTp = 0; // min thetap
  static constexpr float MaxTp = 360; // max thetap

  // --- Extrusion ---
  static constexpr float MinEp = .1; // min ep
  static constexpr float MaxEp = .9; // max ep

  // --- Spiral Swap Transform ---

  // --- Torsion Swap Transform ---

}; // PolylineLimits

} // end namespace cg

#endif // __PolylineUtils_h