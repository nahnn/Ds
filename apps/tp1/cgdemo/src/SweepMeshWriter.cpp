// OVERVIEW: SweepMeshWriter.cpp
// ========
// implementation for SweepMeshProxy.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 30/10/2026

#include "../include/SweepMeshWriter.h"
#include <filesystem>
#include <algorithm>

namespace cg
{ // begin namespace cg

namespace
{ //begin namespace

void writeMeshData(FILE* file, const TriangleMesh& mesh)
{
    const auto& data = mesh.data();
    const bool normals = mesh.hasVertexNormals();

    fprintf(file, "# Wavefront OBJ: %d vertices, %d triangles\n",
        data.vertexCount, data.triangleCount);
    
    for (int i = 0; i < data.vertexCount; ++i)
    {
        const auto& v = data.vertices[i];

        fprintf(file, "v %.9g %.9g %.9g\n", v.x, v.y, v.z);
    }

    if (normals)
        for (int i = 0; i < data.vertexCount; ++i)
        {
        const auto& n = data.vertexNormals[i];

        fprintf(file, "vn %.9g %.9g %.9g\n", n.x, n.y, n.z);
        }
    
    for (int i = 0; i < data.triangleCount; ++i)
    {
        const int a = data.triangles[i].v[0] + 1;
        const int b = data.triangles[i].v[1] + 1;
        const int c = data.triangles[i].v[2] + 1;

        if (normals)
        fprintf(file, "f %d//%d %d//%d %d//%d\n", a, a, b, b, c, c);
        else
        fprintf(file, "f %d %d %d\n", a, b, c);
    }

    //no support for uv
}

}


/////////////////////////////////////////////////////////////////////
//
// SweepMeshWriter implementation (copy of MeshWriter, just to not change cg/)
// ==========

// accepts only letters, digits, '_' and '-' (no separators, dots or spaces)
bool SweepMeshWriter::isValidName(const std::string& name)
{
  return !name.empty() && name.size() <= 63
    && std::all_of(name.begin(), name.end(), [](unsigned char c)
    {
      return std::isalnum(c) || c == '_' || c == '-';
    });
}

bool
SweepMeshWriter::writeOBJ(const TriangleMesh& mesh, const char* filename)
{
    auto file = fopen(filename, "w");

    if (!file)
        return false;

    printf("Writing Wavefront OBJ file %s...\n", filename);
    writeMeshData(file, mesh);

    const bool ok = !ferror(file); 
    return (fclose(file) == 0) && ok;
}

} // end namespace cg
