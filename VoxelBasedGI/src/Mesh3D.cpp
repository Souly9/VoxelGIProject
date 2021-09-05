#include "stdafx.h"
#include "Mesh3D.h"


Mesh3D::Mesh3D(const Mesh3D& mesh) : Object(mesh), m_isMatDirty(mesh.m_isMatDirty),
                                     m_data(std::make_shared<GeometryData>(*mesh.m_data)),
                                     m_numPrimitives(mesh.m_numPrimitives)
{
}
