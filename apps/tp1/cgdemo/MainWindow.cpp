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
// OVERVIEW: MainWindow.cpp
// ========
// Source file for cg demo main window.
//
// Author: Paulo Pagliosa
// Last revision: 07/10/2026

#include "graphics/Application.h"
#include "graphics/AssetFolder.h"
#include "reader/SceneReader.h"
#include "SceneWriter.h"
#include "MainWindow.h"
#include "IconsFontAwesome7.h"


/////////////////////////////////////////////////////////////////////
//
// MainWindow implementation
// ==========
MeshMap MainWindow::_defaultMeshes;

inline void
MainWindow::buildDefaultMeshes()
{
  _defaultMeshes["Box"] = GLGraphics3::box();
  _defaultMeshes["Sphere"] = GLGraphics3::sphere();
  _defaultMeshes["Cylinder"] = GLGraphics3::cylinder();
  _defaultMeshes["Cone"] = GLGraphics3::cone();
  _defaultMeshes["Plane"] = GLGraphics3::quad();
  // TODO: add polyline com algo do GLGraphics3 ou classe derivada que criarmos.
}

void
MainWindow::initializeScene()
{
  auto a = (float)width() / (float)height();
  auto s = createCameraObject(a, "Main Camera");

  s->transform()->setLocalPosition({0, 0, 10});
  s = createLightObject(Light::Type::Directional, "Directional Light");
  s->transform()->setLocalEulerAngles({50, 30, 0});
  createDefaultPrimitiveObject("Box");
}

inline void
initializeFonts()
{
  auto fonts = ImGui::GetIO().Fonts;
  constexpr auto alpha = "fonts/Roboto-Regular.ttf";
  
  fonts->AddFontFromFileTTF(Application::assetFilePath(alpha).c_str(), 16);

  constexpr ImWchar iconRanges[]{ICON_MIN_FA, ICON_MAX_FA, 0};
  constexpr auto icons = "fonts/fa-solid-900.ttf";
  ImFontConfig fc;
  
  fc.MergeMode = fc.PixelSnapH = true;
  fc.GlyphMinAdvanceX = 16.0f;

  fonts->AddFontFromFileTTF(Application::assetFilePath(icons).c_str(),
    14,
    &fc,
    iconRanges);
  fonts->Build();
}

void
MainWindow::beginInitialize()
{
  initializeFonts();
  buildDefaultMeshes();
  Assets::initialize();
  Assets::meshes().insert(_defaultMeshes.begin(), _defaultMeshes.end());
  _sceneFolder = AssetFolder::New("scenes/", ".scn");

  //Polyline/Sweep inspection functions
  registerInspectFunction(inspectSpiralProxy);
  registerInspectFunction(inspectTorsionProxy);
}

void
MainWindow::createObjectMenu()
{
  if (ImGui::BeginMenu("3D Object"))
  {
    if (ImGui::MenuItem("Box"))
      createDefaultPrimitiveObject("Box");
    if (ImGui::MenuItem("Sphere"))
      createDefaultPrimitiveObject("Sphere");
    if (ImGui::MenuItem("Cylinder"))
      createDefaultPrimitiveObject("Cylinder");
    if (ImGui::MenuItem("Cone"))
      createDefaultPrimitiveObject("Cone");
    if (ImGui::MenuItem("Plane"))
      createDefaultPrimitiveObject("Plane");

    //Sweep spiral/torsion menu
    if (ImGui::BeginMenu("Sweep Object"))
    {
      if (ImGui::MenuItem("Spiral"))
        createSpiralObject();
      if (ImGui::MenuItem("Torsion"))
        createTorsionObject();

      ImGui::EndMenu();
    }
    ImGui::EndMenu();
  }
}

Component*
MainWindow::addComponentMenu(const SceneObject&)
{
  Component* component = nullptr;

  if (ImGui::BeginMenu("3D Object"))
  {
    if (ImGui::MenuItem("Box"))
      component = makeDefaultPrimitive("Box");
    if (ImGui::MenuItem("Sphere"))
      component = makeDefaultPrimitive("Sphere");
    if (ImGui::MenuItem("Cylinder"))
      component = makeDefaultPrimitive("Cylinder");
    if (ImGui::MenuItem("Cone"))
      component = makeDefaultPrimitive("Cone");
    if (ImGui::MenuItem("Plane"))
      component = makeDefaultPrimitive("Plane");

    //Sweep spiral/torsion menu
    if (ImGui::BeginMenu("Sweep Object"))
    {
      if (ImGui::MenuItem("Spiral"))
        component = makeSpiralProxy();
      if (ImGui::MenuItem("Torsion"))
        component = makeTorsionProxy();

      ImGui::EndMenu();
    }

    ImGui::EndMenu();
  }
  return component;
}

void
MainWindow::openSceneCommand()
{
  if (ImGui::BeginListBox("##SceneList",
    ImVec2{180, 8 * ImGui::GetTextLineHeightWithSpacing()}))
  {
    for (auto& file : _sceneFolder->files())
      if (ImGui::Selectable(file->filename().c_str()))
      {
        auto filename = (_sceneFolder->path() / file->name()).string();

        readScene(filename);
        ImGui::CloseCurrentPopup();
      }
    ImGui::EndListBox();
  }
}

inline void
MainWindow::updateRayTracerScene()
{
  if (_rayTracer)
    _rayTracer->setScene(*scene());
  _image = nullptr;
}

void
MainWindow::readScene(const std::string& filename) try
{
  util::SceneReader reader;

  reader.setInput(filename);
  reader.execute();
  if (reader.scene())
  {
    SceneWindow::setScene(*reader.scene());

    auto& materials = Assets::materials();

    for (const auto& [name, m] : reader.materials)
      materials[name] = m;
    updateRayTracerScene();
  }
}
catch (const std::exception& e)
{
  puts(e.what());
}

void
MainWindow::saveScene()
{
  if (auto name = scene()->name(); *name)
  {
    auto filename = std::string{name} + ".scn";
    auto path = (_sceneFolder->path() / filename).string();

    util::SceneWriter{path.c_str()}.write(*scene());
    _sceneFolder->addFile(filename.c_str());
  }
}

inline void
MainWindow::fileMenu()
{
  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("New Scene"))
    {
      newScene();
      updateRayTracerScene();
    }
    if (ImGui::BeginMenu("Open"))
    {
      openSceneCommand();
      ImGui::EndMenu();
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Save"))
      saveScene();
    ImGui::Separator();
    if (ImGui::MenuItem("Exit", "Alt+F4"))
    {
      shutdown();
    }
    ImGui::EndMenu();
  }
}

inline void
MainWindow::viewMenu()
{
  if (ImGui::BeginMenu("View"))
  {
    ImGui::MenuItem("Hierarchy Window", nullptr, &_showHierarchy);
    ImGui::MenuItem("Inspector Window", nullptr, &_showInspector);
    ImGui::MenuItem("Camera Preview", nullptr, &_showPreview);
    ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
    ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
    ImGui::EndMenu();
  }
}

inline void
MainWindow::createMenu()
{
  ImGui::BeginDisabled(!editHierarchy());
  if (ImGui::BeginMenu("Create"))
  {
    if (ImGui::MenuItem("Material"))
      createMaterial();
    ImGui::Separator();
    createObjectMenu();
    ImGui::EndMenu();
  }
  ImGui::EndDisabled();
}

inline void
MainWindow::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  ImGui::showStyleSelector("Color Theme##Selector");
  ImGui::colorEdit3("Selected Wireframe", _selectedWireframeColor[0]);
  ImGui::colorEdit3("Selected Children Wireframe", _selectedWireframeColor[1]);
  ImGui::PopItemWidth();
}

inline void
MainWindow::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    fileMenu();
    createMenu();
    viewMenu();
    if (ImGui::BeginMenu("Ray Tracing"))
    {
      ImGui::DragInt("Max Recursion Level",
        &_maxRecursionLevel,
        1.0f,
        0,
        RayTracer::maxMaxRecursionLevel);
      ImGui::DragFloat("Min Weight",
        &_minWeight,
        0.01f,
        RayTracer::minMinWeight,
        1.0f);
      ImGui::EndMenu();
    }
    if (_viewMode == ViewMode::Editor)
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
      if (ImGui::MenuItem(ICON_FA_PLAY))
        _viewMode = ViewMode::Renderer;
      ImGui::PopStyleColor();
      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Run ray tracer");
    }
    else
    {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
      if (ImGui::MenuItem(ICON_FA_BACKWARD))
      {
        _viewMode = ViewMode::Editor;
        _image = nullptr;
      }
      ImGui::PopStyleColor();
      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Back to editor");
    }
    if (ImGui::BeginMenu("Tools"))
    {
      if (ImGui::BeginMenu("Options"))
      {
        showOptions();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void
MainWindow::gui()
{
  mainMenu();
  if (_viewMode == ViewMode::Renderer)
    return;

  constexpr auto evw = (float)360;
  constexpr auto evh = (float)310;
  constexpr auto lft = (float)0;
  constexpr auto top = (float)23;
  auto w = (float)width();
  auto h = (float)height();
  auto evy = h - evh;

  // Hierarchy Window
  ImGui::SetNextWindowPos({lft, top});
  ImGui::SetNextWindowSize({evw, evy - top});
  hierarchyWindow();
  // Editor View
  ImGui::SetNextWindowPos({lft, evy});
  ImGui::SetNextWindowSize({evw, evh});
  editorView();

  constexpr auto iww = (float)420;
  auto rgt = w - iww;
  auto awh = h / 3;
  auto awy = h - awh;

  // Inspector Window
  ImGui::SetNextWindowPos({rgt, top});
  ImGui::SetNextWindowSize({iww, awy - top});
  inspectorWindow();
  // Assets Window
  ImGui::SetNextWindowPos({rgt, awy});
  ImGui::SetNextWindowSize({iww, awh});
  assetWindow();
}

void
MainWindow::renderScene()
{
  if (_viewMode != ViewMode::Renderer)
    return;

  auto camera = CameraProxy::current();

  if (!camera)
    camera = editor()->camera();
  if (!_image)
  {
    _image = new GLImage{width(), height()};
    if (!_rayTracer)
      _rayTracer = new RayTracer{*scene(), *camera};
    else
      _rayTracer->setCamera(*camera);
    _rayTracer->setMaxRecursionLevel(_maxRecursionLevel);
    _rayTracer->setMinWeight(_minWeight);
    _rayTracer->renderImage(*_image);
  }
  _image->draw(0, 0);
}

bool
MainWindow::onResize(int width, int height)
{

  _viewMode = ViewMode::Editor;
  _image = nullptr;
  return true;
}

// Spiral/ Torsion methods
// emulations of createPrimitiveObject()

SceneObject* MainWindow::createSpiralObject()
{
  //create new object associated with the scene
  auto object = SceneObject::New(*_scene);
  object->setName("Spiral Sweep %d", ++_primitiveId);

  //builds the mesh by sweep method
  auto proxy = SpiralProxy<float>::New("Spiral Mesh");
  proxy->build(); 

  //add component and return
  object->addComponent(proxy);
  return object;
}

SceneObject* MainWindow::createTorsionObject()
{
  auto object = SceneObject::New(*_scene);
  object->setName("Torsion Sweep %d", ++_primitiveId);

  auto proxy = TorsionProxy<float>::New("Torsion Mesh");
  proxy->build(); 

  object->addComponent(proxy);
  return object;
}

Component* MainWindow::makeSpiralProxy()
{
  auto proxy = SpiralProxy<float>::New("Spiral Mesh");
  proxy->build();
  return proxy;
}

Component* MainWindow::makeTorsionProxy()
{
  auto proxy = TorsionProxy<float>::New("Torsion Mesh");
  proxy->build();
  return proxy;
}

// Inspect Polyline/Sweep methods

//PolylineBase
void MainWindow::inspectPolylineBase(PolylineAttrs<float>& attrs, SweepMeshProxy<float>& proxy)
{
  using Limits = cg::PolylineLimits<float>;

  ImGui::Text("Transformações Base");

  if (ImGui::SliderFloat("Scale", &attrs.sp, Limits::MIN_SCALEP, Limits::MAX_SCALEP))
    proxy.setPolylineDirty();
    
  if (ImGui::SliderFloat("Rotation", &attrs.tp, Limits::MIN_THETAP, Limits::MAX_THETAP))
    proxy.setPolylineDirty();

  ImGui::Checkbox("Habilitar Extrusão", &attrs.isExtruded);
  ImGui::BeginDisabled(!attrs.isExtruded);

  if (ImGui::SliderFloat("Extrusion", &attrs.ep, Limits::MIN_EP, Limits::MAX_EP))
    proxy.setPolylineDirty();
    
  ImGui::EndDisabled();
}

//RegularPolygon
void MainWindow::inspectRegularPolygon(RegularPolygonAttrs<float>& attrs, SweepMeshProxy<float>& proxy)
{
  using Limits = cg::RegularPolygonLimits<float>;

  inspectPolylineBase(attrs, proxy);
  
  ImGui::Separator();
  ImGui::Text("Parâmetros do Polígono");

  if (ImGui::SliderInt("Points", &attrs.np, Limits::MIN_NP, Limits::MAX_NP))
    proxy.setPolylineDirty();
}

// CircularArc
void MainWindow::inspectCircularArc(CircularArcAttrs<float>& attrs, SweepMeshProxy<float>& proxy)
{
  using Limits = cg::CircularArcLimits<float>;

  inspectPolylineBase(attrs, proxy);
  
  ImGui::Separator();
  ImGui::Text("Parâmetros do Arco");

  if (ImGui::SliderFloat("Angle", &attrs.tc, Limits::MIN_THETAC_EXCLUSIVE + 0.1f, Limits::MAX_THETAC))
    proxy.setPolylineDirty();

  if (ImGui::SliderInt("Segments", &attrs.nc, Limits::MIN_NC, Limits::MAX_NC))
    proxy.setPolylineDirty();

  if (ImGui::Checkbox("Open Arc", &attrs.isOpen))
    proxy.setPolylineDirty();
}

// Generic Polyline Generatrix
void MainWindow::inspectGeneratrix(cg::graph::SweepMeshProxy<float>& proxy)
{
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "=== Geratriz (2D) ===");
  
  if (ImGui::Combo("Generator", (int*)&proxy.activeGen, "Regular Polygon\0Circular Arc\0"))
    proxy.setPolylineDirty();

  ImGui::Separator();

  //Sets the actual polyline gen type
  if (proxy.activeGen == SweepMeshProxy<float>::PolylineGeneratorType::RegularPolygon)
    inspectRegularPolygon((RegularPolygonAttrs<float>&)proxy.attrs, proxy);
  else
    inspectCircularArc((CircularArcAttrs<float>&)proxy.attrs, proxy);
}

//Sweep Inspects

void MainWindow::inspectSweeperBase(PolylineSweeperAttrs<float>& attrs, SweepMeshProxy<float>& proxy)
{
  if (ImGui::Checkbox("Capped", &attrs.capped))
    proxy.setSweepDirty();
}

void MainWindow::inspectSpiralProxy(SceneWindow& window, SpiralProxy<float>& proxy)
{
  using Limits = cg::SpiralSweeperLimits<float>;

  inspectGeneratrix(proxy);

  ImGui::Separator();
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Spiral");

  inspectSweeperBase(proxy.attr, proxy);
  ImGui::Separator();

  if (ImGui::SliderFloat("Initial Width (we)", &proxy.attr.we, Limits::MIN_WE_ABS, Limits::MAX_WE_ABS))
    proxy.setSweepDirty();

  if (ImGui::SliderFloat("Revolutions (re)", &proxy.attr.re, Limits::MIN_RE, Limits::MAX_RE))
    proxy.setSweepDirty();

  if (ImGui::SliderInt("Subdivs / Rev (nes)", &proxy.attr.nes, Limits::MIN_NES, Limits::MAX_NES))
    proxy.setSweepDirty();

  if (ImGui::SliderFloat("Height Inc. / Rev (dh)", &proxy.attr.dh, Limits::MIN_DELTAH, Limits::MAX_DELTAH))
    proxy.setSweepDirty();

  if (ImGui::SliderFloat("Width Inc. / Rev (dw)", &proxy.attr.dw, Limits::MIN_DELTAW, Limits::MAX_DELTAW))
    proxy.setSweepDirty();

  ImGui::Separator();

  proxy.build();
}

void MainWindow::inspectTorsionProxy(SceneWindow& window, TorsionProxy<float>& proxy)
{
  using Limits = cg::TorsionSweeperLimits<float>;

  inspectGeneratrix(proxy);

  ImGui::Separator();
  ImGui::TextColored({0.0f, 1.0f, 0.0f, 1.0f}, "Torsion");

  inspectSweeperBase(proxy.attr, proxy);
  ImGui::Separator();

  if (ImGui::SliderFloat("Length (lv)", &proxy.attr.lv, Limits::MIN_LV, Limits::MAX_LV))
    proxy.setSweepDirty();

  //TODO: verificar se -50 a 50 é bom mesmo
  if (ImGui::SliderFloat("Offset Width (ow)", &proxy.attr.ow, -50.0f, 50.0f))
    proxy.setSweepDirty();
    
  if (ImGui::SliderFloat("Offset Height (oh)", &proxy.attr.oh, -50.0f, 50.0f))
    proxy.setSweepDirty();

  if (ImGui::SliderInt("Subdivisions (nsv)", &proxy.attr.nsv, Limits::MIN_NSV, Limits::MAX_NSV))
    proxy.setSweepDirty();

  if (ImGui::SliderFloat("Begin Scale (sb)", &proxy.attr.sb, Limits::MIN_SCALEV, Limits::MAX_SCALEV))
    proxy.setSweepDirty();

  if (ImGui::SliderFloat("End Scale (se)", &proxy.attr.se, Limits::MIN_SCALEV, Limits::MAX_SCALEV))
    proxy.setSweepDirty();

  if (ImGui::SliderFloat("Torsion (rv)", &proxy.attr.rv, Limits::MIN_RV, Limits::MAX_RV))
    proxy.setSweepDirty();

  ImGui::Separator();

  proxy.build();
}

