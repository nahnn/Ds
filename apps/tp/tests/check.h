// OVERVIEW: check.h
// ========
// Minimal check macros for the unit tests (assert is disabled by NDEBUG).
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 20/11/2026

#ifndef __check_h
#define __check_h

#include <cmath>
#include <cstdio>

inline int failures = 0;

#define CHECK(cond) \
  ((cond) ? void() : \
  (std::printf("%s:%d: FAIL: %s\n", __FILE__, __LINE__, #cond), ++failures, void()))

#define CHECK_NEAR(a, b, eps) CHECK(std::fabs((a) - (b)) <= (eps))

#endif // __check_h
