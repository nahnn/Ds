// OVERVIEW: Material.h
// ========
// Class definition for USceneEditor. U stands for universal. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 09/11/2026

#ifndef __USceneEditor_h
#define __USceneEditor_h

#include "SceneEditor.h"
#include "PBR.h"

namespace cg
{ // begin namespace cg
    
class USceneEditor : public SceneEditor
{
public:
  USceneEditor(const SceneBase& scene) // compila vs + gs + pbr::shader code? 
  {

  }

protected:
  void beginRender() override;
  void renderLights() override;
  void renderActors() override;

private:
  GLSL::Program _pbr{"PBR"};

  void initUniformLocations();
  bool drawPBR(const Primitive&, const PBR&);
  void restoreBaseProgram() { GLSL::Program::setCurrent(&program()); }

}; // USceneEditor

} // end namespace cg

#endif // __USceneEditor_h