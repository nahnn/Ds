//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2022, 2026 Paulo Pagliosa.                        |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: MainWindow.h
// ========
// Class definition for cg demo main window.
//
// Author: Paulo Pagliosa
// Last revision: 07/10/2026

#ifndef __MainWindow_h
#define __MainWindow_h

#include "graph/SceneWindow.h"
#include "graphics/AssetFolder.h"
#include "graphics/GLImage.h"
#include "RayTracer.h"

#include <functional>
#include <map>

#include "include/sweep/RegularPolygon.h"
#include "include/sweep/CircularArc.h"
#include "include/sweep/SweepMeshProxy.h"
#include "include/pbr/USceneEditor.h"

//TODO: check whether it needs to be removed. Residual from the original repo
// namespace cg::graph
// { // begin namespace cg::graph

// class HelixProxy;
// class SweepMeshProxy;

// } // end namespace cg::graph

using namespace cg;
using namespace cg::graph;


/////////////////////////////////////////////////////////////////////
//
// MainWindow: cg demo main window class
// ==========
class MainWindow final: public SceneWindow
{
public:
  MainWindow(int width, int height):
    SceneWindow{"Ds Demo Version 1.51", width, height}
  {
    //PBR Primitive Material
    registerInspectFunction(inspectPrimitivePBR);

    //Polyline Generators
    registerPolyGenInspector<RegularPolygon<float>>(PolyGenType::RegularPolygon, inspectRegularPolygon);
    registerPolyGenInspector<CircularArc<float>>(PolyGenType::CircularArc, inspectCircularArc);

    //Sweep
    registerInspectFunction(inspectSpiralProxy);
    registerInspectFunction(inspectTorsionProxy);
  }

private:
  AssetFolderRef _sceneFolder;
  Reference<RayTracer> _rayTracer;
  Reference<GLImage> _image;
  int _maxRecursionLevel{6};
  float _minWeight{RayTracer::minMinWeight};

  static MeshMap _defaultMeshes;

  auto makeDefaultPrimitive(const char* const meshName)
  {
    return makePrimitive(*_defaultMeshes[meshName], meshName);
  }

  auto createDefaultPrimitiveObject(const char* const meshName)
  {
    return createPrimitiveObject(*_defaultMeshes[meshName], meshName);
  }

  void beginInitialize() override;
  void initializeScene() override;
  void renderScene() override;
  void gui() override;
  bool onResize(int width, int height) override;
  Component* addComponentMenu(const SceneObject&) override;
  void createObjectMenu() override;

  void mainMenu();
  void fileMenu();
  void viewMenu();
  void createMenu();
  void showOptions();

  void readScene(const std::string& filename);
  void updateRayTracerScene();
  void openSceneCommand();
  void saveScene();

  static void buildDefaultMeshes();

  //===== Adapted Functions for Sweep and PBR implementation =====

  using PolyGenProxy = PolylineGeneratorProxy<float>;
  using PolyGenType = PolyGenProxy::GenType;

  //Inspectors for polylines
  using PolyGenInspector = std::function<void(PolyGenProxy&, SweepMeshProxy<float>&)>;

  //map for different polylines
  inline static std::map<PolyGenType, PolyGenInspector> _polyGenInspectors;

  template <typename G, typename F>
  static void registerPolyGenInspector(PolyGenType type, F inspect)
  {
    _polyGenInspectors[type] =
      [inspect](PolyGenProxy& g, SweepMeshProxy<float>& p)
      {
        inspect(static_cast<G&>(g.activeGen()).attributes(), p);
      };
  }

  static void inspectPolylineBase(PolylineAttrs<float>& attrs, SweepMeshProxy<float>& proxy);
  static void inspectRegularPolygon(RegularPolygonAttrs<float>& attrs, SweepMeshProxy<float>& proxy);
  static void inspectCircularArc(CircularArcAttrs<float>& attrs, SweepMeshProxy<float>& proxy);
  static void inspectGeneratrix(SceneWindow& window, SweepMeshProxy<float>& proxy);

  //to inspect PBR works
  SceneEditor* makeEditor(SceneBase& scene) override { return new USceneEditor{scene}; }
  TriangleMeshProxy* makePrimitive(const TriangleMesh& mesh, const std::string& meshName);
  SceneObject* createPrimitiveObject(const TriangleMesh& mesh, const std::string& meshName);
  Material* createMaterial();

  //to inspect PBR works
  static void inspectPrimitivePBR(SceneWindow&, TriangleMeshProxy&);
  static void inspectMaterialPBR(SceneWindow&, Primitive&);
  static void inspectMaterialPBR(Material&);

  // Main Inspectors
  static void inspectSweeperBase(PolylineSweeperAttrs<float>& attrs, SweepMeshProxy<float>& proxy);
  static void inspectSpiralProxy(SceneWindow& window, SpiralProxy<float>& proxy);
  static void inspectTorsionProxy(SceneWindow& window, TorsionProxy<float>& proxy);

  // Mesh OBJ Inspectors
  static void inspectSweepObjWrite(SweepMeshProxy<float>& proxy);

  SceneObject* createSpiralObject();
  SceneObject* createTorsionObject();

  Component* makeSpiralProxy();
  Component* makeTorsionProxy();

}; // MainWindow

#endif // __MainWindow_h
