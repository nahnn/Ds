// OVERVIEW: writer_test.cpp
// ========
// Unit tests for SweepMeshWriter.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 20/11/2026

#include "tests/check.h"
#include "include/sweep/SweepMeshWriter.h"
#include "include/sweep/RegularPolygon.h"
#include "include/sweep/TorsionSweeper.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

using namespace cg;

namespace
{ // begin namespace

void
testIsValidName()
{
  CHECK(SweepMeshWriter::isValidName("spiral_1-a"));
  CHECK(!SweepMeshWriter::isValidName(""));
  CHECK(!SweepMeshWriter::isValidName("a\"b"));
  CHECK(!SweepMeshWriter::isValidName("a/b"));
  CHECK(!SweepMeshWriter::isValidName("a\\b"));
  CHECK(!SweepMeshWriter::isValidName("a.b"));
  CHECK(!SweepMeshWriter::isValidName("a b"));
  CHECK(!SweepMeshWriter::isValidName(std::string(64, 'a')));
}

void
testWriteOBJ()
{
  auto polyline = RegularPolygon<float>{}.generate();
  std::unique_ptr<TriangleMesh> mesh{TorsionSweeper<float>{}.sweep(polyline)};
  auto path = std::filesystem::temp_directory_path() / "tp_writer_test.obj";

  CHECK(SweepMeshWriter::writeOBJ(*mesh, path.string().c_str()));

  // counts the "v" and "f" lines of the written file
  std::ifstream file{path};
  std::string line;
  int nv{}, nf{};

  while (std::getline(file, line))
    if (line.starts_with("v "))
      ++nv;
    else if (line.starts_with("f "))
      ++nf;
  file.close();
  std::filesystem::remove(path);
  CHECK(nv == mesh->vertexCount());
  CHECK(nf == mesh->triangleCount());
  CHECK(!SweepMeshWriter::writeOBJ(*mesh, "/nonexistent/dir/x.obj"));
}

} // end namespace

int
main()
{
  testIsValidName();
  testWriteOBJ();
  std::printf("writer_test: %d failure(s)\n", failures);
  return failures != 0;
}
