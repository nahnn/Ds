// OVERVIEW: scene.h
// ========
// Scene helpers for the unit tests.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 20/11/2026

#ifndef __scene_h
#define __scene_h

#include "reader/SceneReader.h"
#include "graphics/GLGraphics3.h"
#include <cstring>
#include <filesystem>
#include <fstream>

namespace cg::test
{ // begin namespace cg::test

// registers the default meshes, as in MainWindow::beginInitialize
inline void
initAssets()
{
  Assets::initialize();
  Assets::meshes()["Sphere"] = GLGraphics3::sphere();
}

// reads a scene from a temporary file; returns the reader (scene and materials)
inline Reference<util::SceneReader>
read(const char* text)
{
  auto path = std::filesystem::temp_directory_path() / "tp_test.scn";

  std::ofstream{path} << text;

  Reference<util::SceneReader> reader = new util::SceneReader;

  reader->setInput(path.string().c_str());
  try
  {
    reader->execute();
  }
  catch (...)
  {
    std::filesystem::remove(path);
    throw;
  }
  std::filesystem::remove(path);
  return reader;
}

// component of type C of the top level object named name
template <typename C>
C*
find(graph::Scene& scene, const char* name)
{
  for (auto&& o : scene.root()->children())
    if (!std::strcmp(o.name(), name))
      for (auto&& c : o.components())
        if (auto p = dynamic_cast<C*>(&*c))
          return p;
  return nullptr;
}

} // end namespace cg::test

#endif // __scene_h
