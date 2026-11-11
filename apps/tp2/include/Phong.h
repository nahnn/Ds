// OVERVIEW: Phong.h
// ========
// Class definition for Phong Material. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 11/11/2026

#ifndef __Phong_h
#define __Phong_h

#include "UMaterial.h"

namespace cg
{ // begin namespace cg

class Phong : public UMaterial
{
public:
  Color shade(const vec3& P, const vec3& N, const Light& light) const override;
  const char* shaderCode() const override;

private:

};

} // end namespace cg

#endif // __Phong_h