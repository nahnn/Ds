// OVERVIEW: SweepMeshWriter.h
// ========
// Class definition for SweepMeshProxy.
//
// Authors: Augusto Dalal, João Pedro Sant' Ana de Carvalho, Nathan de Almeida Rezende
// Last revision: 30/10/2026


#ifndef __SweepMeshWriter_h
#define __SweepMeshWriter_h

#include "geometry/TriangleMesh.h"
#include <string>

namespace cg
{ // begin namespace cg

// SweepMesWriter (copy of MeshWriter, just to not change cg/)


/////////////////////////////////////////////////////////////////////
//
// SweepMeshWriter: writes sweep meshes to a file
// ===============
class SweepMeshWriter
{
public:
    static bool isValidName(const std::string& name);
    static bool writeOBJ(const TriangleMesh& mesh, const char* filename);

}; // SweepMeshWriter

} // end namespace cg

#endif // __SweepMeshWriter_h
