#pragma once
//------------------------------------------------------------------------------
/// \file
/// \ingroup ugrid
/// \copyright (C) Copyright Aquaveo 2018.
//------------------------------------------------------------------------------

#ifdef CXX_TEST

// 3. Standard Library Headers

// 4. External Library Headers
#include <cxxtest/TestSuite.h>

// 5. Shared Headers
#include <xmscore/misc/base_macros.h>
#include <xmscore/misc/boost_defines.h>

// 6. Non-shared Headers

namespace xms
{
class XmUGrid;
}

////////////////////////////////////////////////////////////////////////////////
class XmUGridTests : public CxxTest::TestSuite
{
public:
  void testUGridStreams();
  void testGetCellType();
  void testGetCellDimension();
  void testGetNumberOfCellEdges();
}; // XmUGridTests

BSHP<xms::XmUGrid> TEST_XmUGrid1Left90Tri();
BSHP<xms::XmUGrid> TEST_XmUGrid2dLinear();
BSHP<xms::XmUGrid> TEST_XmUGrid3dLinear();
BSHP<xms::XmUGrid> TEST_XmUGridHexagonalPolyhedron();
std::string TestFilesPath();

#endif