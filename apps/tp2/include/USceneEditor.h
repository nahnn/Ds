// OVERVIEW: Material.h
// ========
// Class definition for USceneEditor. U stands for universal. 
//
// Author: Augusto Cesar Dalal, João Pedro San't Ana de Carvalho, Nathan de Almeida Rezende 
// Last revision: 09/11/2026

#ifndef __USceneEditor_h
#define __USceneEditor_h

#include "SceneEditor.h"

namespace cg
{ // begin namespace cg
    
class USceneEditor : public SceneEditor
{
public:
  void render() override;

private:

}; // USceneEditor

} // end namespace cg

#endif // __USceneEditor_h