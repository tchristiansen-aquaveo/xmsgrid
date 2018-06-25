//------------------------------------------------------------------------------
/// \file
/// \ingroup ugrid
/// \copyright (C) Copyright Aquaveo 2018.
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------

// 1. Precompiled header

// 2. My own header
#include <xmsgrid/ugrid/XmUGridUtils.h>

// 3. Standard library headers
#include <fstream>

// 4. External library headers

// 5. Shared code headers
#include <xmscore/misc/StringUtil.h>
#include <xmscore/misc/XmLog.h>
#include <xmscore/stl/vector.h>
#include <xmsgrid/ugrid/XmUGrid.h>

// 6. Non-shared code headers

//----- Forward declarations ---------------------------------------------------

//----- External globals -------------------------------------------------------

//----- Namespace declaration --------------------------------------------------
namespace xms
{
//----- Constants / Enumerations -----------------------------------------------

//----- Classes / Structs ------------------------------------------------------

//----- Internal functions -----------------------------------------------------

//----- Class / Function definitions -------------------------------------------

namespace
{

//------------------------------------------------------------------------------
/// \brief Save an XmUGrid ASCII text to output stream.
/// \param[in] a_ugrid: the UGrid to save
/// \param[in] a_outStream: the stream to write
//------------------------------------------------------------------------------
void iWriteUGridToAsciiFile(BSHP<XmUGrid> a_ugrid, std::ostream& a_outStream)
{
  a_outStream << "ASCII XmUGrid Version 1.0\n";

  // number of points
  const VecPt3d& points = a_ugrid->GetPoints();
  a_outStream << "NUM_POINTS " << points.size() << "\n";

  // points
  for (size_t i = 0; i < points.size(); ++i)
  {
    const Pt3d& p = points[i];
    a_outStream << "  POINT " << STRstd(p.x) << " " << STRstd(p.y) << " " << STRstd(p.z) << "\n";
  }

  // number of cell stream items
  const VecInt& cellStream = a_ugrid->GetCellStream();
  int cellStreamSize = (int)cellStream.size();
  a_outStream << "NUM_CELL_ITEMS " << cellStreamSize << "\n";

  // cells
  int currIdx = 0;
  while (currIdx < cellStreamSize)
  {
    int cellType = cellStream[currIdx++];
    int numItems = cellStream[currIdx++];
    a_outStream << "  CELL " << cellType;
    if (cellType == -1)
    {
      currIdx += numItems;
    }
    else if (cellType == XMU_POLYHEDRON)
    {
      int numFaces = numItems;
      a_outStream << " " << numFaces;
      for (int faceIdx = 0; faceIdx < numFaces; ++faceIdx)
      {
        numItems = cellStream[currIdx++];
        a_outStream << "\n    " << numItems;
        for (int itemIdx = 0; itemIdx < numItems; ++itemIdx)
        {
          a_outStream << " " << cellStream[currIdx++];
        }
      }
    }
    else
    {
      a_outStream << " " << numItems;
      for (int itemIdx = 0; itemIdx < numItems; ++itemIdx)
      {
        a_outStream << " " << cellStream[currIdx++];
      }
    }
    a_outStream << "\n";
  }
} // iWriteUGridToAsciiFile
//------------------------------------------------------------------------------
/// \brief Read an XmUGrid from ASCII text from an input stream.
/// \param[in] a_inStream: the stream to read
/// \return the UGrid
//------------------------------------------------------------------------------
BSHP<XmUGrid> iReadUGridFromAsciiFile(std::istream& a_inStream)
{
  if (a_inStream.eof())
    return nullptr;

  std::string line;
  std::getline(a_inStream, line);
  if (line != "ASCII XmUGrid Version 1.0")
  {
    XM_LOG(xmlog::error, "Unsupported file version or file type: " + line);
    return nullptr;
  }

  std::string cardName;
  VecPt3d points;
  VecInt cells;
  while (!a_inStream.eof())
  {
    cardName = "";
    a_inStream >> cardName;
    if (cardName == "NUM_POINTS")
    {
      int numPoints;
      a_inStream >> numPoints;
      points.reserve(numPoints);
    }
    else if (cardName == "POINT")
    {
      Pt3d pt;
      a_inStream >> pt.x >> pt.y >> pt.z;
      points.push_back(pt);
    }
    else if (cardName == "NUM_CELL_ITEMS")
    {
      int numCellItems;
      a_inStream >> numCellItems;
      cells.reserve(numCellItems);
    }
    else if (cardName == "CELL")
    {
      int cellType;
      a_inStream >> cellType;
      cells.push_back(cellType);
      if (cellType == XMU_POLYHEDRON)
      {
        int numFaces;
        a_inStream >> numFaces;
        cells.push_back(numFaces);
        for (int faceIdx = 0; faceIdx < numFaces; ++faceIdx)
        {
          int numPoints;
          a_inStream >> numPoints;
          cells.push_back(numPoints);
          for (int i = 0; i < numPoints; ++i)
          {
            int ptIdx;
            a_inStream >> ptIdx;
            cells.push_back(ptIdx);
          }
        }
      }
      else
      {
        int numPoints;
        a_inStream >> numPoints;
        cells.push_back(numPoints);
        for (int i = 0; i < numPoints; ++i)
        {
          int ptIdx;
          a_inStream >> ptIdx;
          cells.push_back(ptIdx);
        }
      }
    }
  }

  BSHP<XmUGrid> ugrid = XmUGrid::New(points, cells);
  return ugrid;
} // iReadUGridFromAsciiFile

} // namespace {

//------------------------------------------------------------------------------
/// \brief Read XmUGrid from an ASCII file.
//------------------------------------------------------------------------------
BSHP<XmUGrid> XmReadUGridFromAsciiFile(const std::string& a_filePath)
{
  std::ifstream inFile(a_filePath);
  return iReadUGridFromAsciiFile(inFile);
} // XmReadUGridFromAsciiFile
//------------------------------------------------------------------------------
/// \brief Write an XmUGrid to an ASCII file.
//------------------------------------------------------------------------------
void XmWriteUGridToAsciiFile(BSHP<XmUGrid> a_ugrid, const std::string& a_filePath)
{
  std::ofstream outFile(a_filePath);
  iWriteUGridToAsciiFile(a_ugrid, outFile);
} // XmWriteUGridToAsciiFile

} // namespace xms

#ifdef CXX_TEST
//------------------------------------------------------------------------------
// Unit Tests
//------------------------------------------------------------------------------
using namespace xms;
#include <xmsgrid/ugrid/XmUGridUtils.t.h>
#include <xmsgrid/ugrid/XmUGrid.t.h>

////////////////////////////////////////////////////////////////////////////////
/// \class XmUGridTests
/// \brief Tests XmUGrids.
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
/// \brief Test writing an ASCII file for an empty UGrid.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testWriteEmptyUGrid()
{
  BSHP<XmUGrid> ugrid = XmUGrid::New();
  std::ostringstream output;
  iWriteUGridToAsciiFile(ugrid, output);

  std::string outputBase = "ASCII XmUGrid Version 1.0\n"
    "NUM_POINTS 0\n"
    "NUM_CELL_ITEMS 0\n";
  TS_ASSERT_EQUALS(outputBase, output.str());
} // XmUGridReaderTests::testWriteEmptyUGrid
//------------------------------------------------------------------------------
/// \brief Test writing an ASCII file for a single triangle UGrid.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testWriteBasicUGrid()
{
  BSHP<XmUGrid> ugrid = TEST_XmUGrid1Left90Tri();
  std::ostringstream output;
  iWriteUGridToAsciiFile(ugrid, output);

  std::string outputBase =
    "ASCII XmUGrid Version 1.0\n"
    "NUM_POINTS 3\n"
    "  POINT 0.0 0.0 0.0\n"
    "  POINT 20.0 0.0 0.0\n"
    "  POINT 0.0 20.0 0.0\n"
    //"NUM_CELLS 1\n"
    "NUM_CELL_ITEMS 5\n"
    "  CELL 5 3 0 1 2\n";
  TS_ASSERT_EQUALS(outputBase, output.str());
} // XmUGridUtilsTests::testWriteBasicUGrid
//------------------------------------------------------------------------------
/// \brief Test writing an ASCII file for a single polyhedron UGrid.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testWritePolyhedronUGrid()
{
  BSHP<XmUGrid> ugrid = TEST_XmUGridHexagonalPolyhedron();
  std::ostringstream output;
  iWriteUGridToAsciiFile(ugrid, output);

  std::string outputBase =
    "ASCII XmUGrid Version 1.0\n"
    "NUM_POINTS 8\n"
    "  POINT 0.0 0.0 10.0\n"
    "  POINT 10.0 0.0 10.0\n"
    "  POINT 10.0 10.0 10.0\n"
    "  POINT 0.0 10.0 10.0\n"
    "  POINT 0.0 0.0 0.0\n"
    "  POINT 10.0 0.0 0.0\n"
    "  POINT 10.0 10.0 0.0\n"
    "  POINT 0.0 10.0 0.0\n"
    //"NUM_CELLS 1\n"
    "NUM_CELL_ITEMS 32\n"
    "  CELL 42 6\n"
    "    4 0 1 2 3\n"
    "    4 4 5 7 2\n"
    "    4 5 6 2 1\n"
    "    4 6 7 3 2\n"
    "    4 7 4 0 3\n"
    "    4 4 7 6 5\n";
  TS_ASSERT_EQUALS(outputBase, output.str());
} // XmUGridUtilsTests::testWritePolyhedronUGrid
//------------------------------------------------------------------------------
/// \brief Test writing an ASCII file for UGrid with supported 1D and 2D linear
///        cell types.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testWriteLinear2dCells()
{
  BSHP<XmUGrid> ugrid = TEST_XmUGrid2dLinear();
  std::ostringstream output;
  iWriteUGridToAsciiFile(ugrid, output);

  std::string outputBase =
    "ASCII XmUGrid Version 1.0\n"
    "NUM_POINTS 14\n"
    "  POINT 0.0 0.0 0.0\n"
    "  POINT 10.0 0.0 0.0\n"
    "  POINT 20.0 0.0 0.0\n"
    "  POINT 30.0 0.0 0.0\n"
    "  POINT 40.0 0.0 0.0\n"
    "  POINT 0.0 10.0 0.0\n"
    "  POINT 10.0 10.0 0.0\n"
    "  POINT 20.0 10.0 0.0\n"
    "  POINT 40.0 10.0 0.0\n"
    "  POINT 0.0 20.0 0.0\n"
    "  POINT 10.0 20.0 0.0\n"
    "  POINT 20.0 20.0 0.0\n"
    "  POINT 30.0 20.0 0.0\n"
    "  POINT 40.0 20.0 0.0\n"
    "NUM_CELL_ITEMS 34\n"
    "  CELL 9 4 0 1 6 5\n"
    "  CELL 8 4 1 2 6 7\n"
    "  CELL 5 3 2 3 7\n"
    "  CELL 7 6 3 4 8 13 12 7\n"
    "  CELL 4 3 7 11 10\n"
    "  CELL 3 2 5 9\n";
  TS_ASSERT_EQUALS(outputBase, output.str());
} // XmUGridUtilsTests::testWriteLinear2dCells
//------------------------------------------------------------------------------
/// \brief Test writing an ASCII file for UGrid with supported 1D and 2D linear
///        cell types.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testWriteLinear3dCells()
{
  BSHP<XmUGrid> ugrid = TEST_XmUGrid3dLinear();
  std::ostringstream output;
  iWriteUGridToAsciiFile(ugrid, output);

  std::string outputBase =
    "ASCII XmUGrid Version 1.0\n"
    "NUM_POINTS 30\n"
    "  POINT 0.0 0.0 0.0\n"
    "  POINT 10.0 0.0 0.0\n"
    "  POINT 20.0 0.0 0.0\n"
    "  POINT 30.0 0.0 0.0\n"
    "  POINT 40.0 0.0 0.0\n"
    "  POINT 0.0 10.0 0.0\n"
    "  POINT 10.0 10.0 0.0\n"
    "  POINT 20.0 10.0 0.0\n"
    "  POINT 30.0 10.0 0.0\n"
    "  POINT 40.0 10.0 0.0\n"
    "  POINT 0.0 10.0 0.0\n"
    "  POINT 10.0 20.0 0.0\n"
    "  POINT 20.0 20.0 0.0\n"
    "  POINT 30.0 20.0 0.0\n"
    "  POINT 40.0 20.0 0.0\n"
    "  POINT 0.0 0.0 10.0\n"
    "  POINT 10.0 0.0 10.0\n"
    "  POINT 20.0 0.0 10.0\n"
    "  POINT 30.0 0.0 10.0\n"
    "  POINT 40.0 0.0 10.0\n"
    "  POINT 0.0 10.0 10.0\n"
    "  POINT 10.0 10.0 10.0\n"
    "  POINT 20.0 10.0 10.0\n"
    "  POINT 30.0 10.0 10.0\n"
    "  POINT 40.0 10.0 10.0\n"
    "  POINT 0.0 10.0 10.0\n"
    "  POINT 10.0 20.0 10.0\n"
    "  POINT 20.0 20.0 10.0\n"
    "  POINT 30.0 20.0 10.0\n"
    "  POINT 40.0 20.0 10.0\n"
    "NUM_CELL_ITEMS 73\n"
    "  CELL 10 4 0 1 5 15\n"
    "  CELL 11 8 1 2 6 7 16 17 21 22\n"
    "  CELL 12 8 2 3 8 7 17 18 22 23\n"
    "  CELL 42 6\n"
    "    4 8 9 14 13\n"
    "    4 8 9 24 23\n"
    "    4 8 13 28 23\n"
    "    4 13 14 29 28\n"
    "    4 9 14 29 24\n"
    "    4 23 24 29 28\n"
    "  CELL 13 6 3 4 18 8 9 23\n"
    "  CELL 14 5 5 6 11 10 20\n";
  TS_ASSERT_EQUALS(outputBase, output.str());
} // XmUGridUtilsTests::testWriteLinear3dCells
//------------------------------------------------------------------------------
/// \brief Test reading an ASCII file for an empty UGrid.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testReadEmptyUGridAsciiFile()
{
  std::string inputText = "ASCII XmUGrid Version 1.0\n"
    "NUM_POINTS 0\n"
    "NUM_CELL_ITEMS 0\n";
  std::istringstream input;
  input.str(inputText);
  BSHP<XmUGrid> ugrid = iReadUGridFromAsciiFile(input);
  if (!ugrid)
    TS_FAIL("Failed to read UGrid.");

  TS_ASSERT(ugrid->GetPoints().empty());
  TS_ASSERT(ugrid->GetCellStream().empty());
} // XmUGridReaderTests::testReadEmptyUGridAsciiFile
//------------------------------------------------------------------------------
/// \brief Test reading an ASCII file for a single triangle UGrid.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testReadBasicUGrid()
{
  std::string inputText =
    "ASCII XmUGrid Version 1.0\n"
    "NUM_POINTS 3\n"
    "  POINT 0.0 0.0 0.0\n"
    "  POINT 20.0 0.0 0.0\n"
    "  POINT 0.0 20.0 0.0\n"
    //"NUM_CELLS 1\n"
    "NUM_CELL_ITEMS 5\n"
    "  CELL 5 3 0 1 2\n";
  std::istringstream input;
  input.str(inputText);
  BSHP<XmUGrid> ugrid = iReadUGridFromAsciiFile(input);
  if (!ugrid)
    TS_FAIL("Failed to read UGrid.");

  BSHP<XmUGrid> ugridBase = TEST_XmUGrid1Left90Tri();
  TS_ASSERT_EQUALS(ugridBase->GetPoints(), ugrid->GetPoints());
  TS_ASSERT_EQUALS(ugridBase->GetCellStream(), ugrid->GetCellStream());
} // XmUGridUtilsTests::testReadBasicUGrid
//------------------------------------------------------------------------------
/// \brief Test reading an ASCII file for a single triangle UGrid.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testReadPolyhedronUGrid()
{
  std::string inputText =
    "ASCII XmUGrid Version 1.0\n"
    "NUM_POINTS 8\n"
    "  POINT 0.0 0.0 10.0\n"
    "  POINT 10.0 0.0 10.0\n"
    "  POINT 10.0 10.0 10.0\n"
    "  POINT 0.0 10.0 10.0\n"
    "  POINT 0.0 0.0 0.0\n"
    "  POINT 10.0 0.0 0.0\n"
    "  POINT 10.0 10.0 0.0\n"
    "  POINT 0.0 10.0 0.0\n"
    //"NUM_CELLS 1\n"
    "NUM_CELL_ITEMS 32\n"
    "  CELL 42 6\n"
    "    4 0 1 2 3\n"
    "    4 4 5 7 2\n"
    "    4 5 6 2 1\n"
    "    4 6 7 3 2\n"
    "    4 7 4 0 3\n"
    "    4 4 7 6 5\n";
  std::istringstream input;
  input.str(inputText);
  BSHP<XmUGrid> ugrid = iReadUGridFromAsciiFile(input);
  if (!ugrid)
    TS_FAIL("Failed to read UGrid.");

  BSHP<XmUGrid> ugridBase = TEST_XmUGridHexagonalPolyhedron();
  TS_ASSERT_EQUALS(ugridBase->GetPoints(), ugrid->GetPoints());
  TS_ASSERT_EQUALS(ugridBase->GetCellStream(), ugrid->GetCellStream());
} // XmUGridUtilsTests::testReadPolyhedronUGrid
//------------------------------------------------------------------------------
/// \brief Test reading an ASCII file for a single triangle UGrid.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testLinear2dWriteThenRead()
{
  BSHP<XmUGrid> ugridBase = TEST_XmUGrid2dLinear();

  // write
  std::ostringstream output;
  iWriteUGridToAsciiFile(ugridBase, output);

  // read
  std::istringstream input;
  input.str(output.str());
  BSHP<XmUGrid> ugridOut = iReadUGridFromAsciiFile(input);

  TS_ASSERT_EQUALS(ugridBase->GetPoints(), ugridOut->GetPoints());
  TS_ASSERT_EQUALS(ugridBase->GetCellStream(), ugridOut->GetCellStream());
} // XmUGridUtilsTests::testLinear2dWriteThenRead
//------------------------------------------------------------------------------
/// \brief Test reading an ASCII file for a single triangle UGrid.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testLinear3dWriteThenRead()
{
  BSHP<XmUGrid> ugridBase = TEST_XmUGrid3dLinear();

  // write
  std::ostringstream output;
  iWriteUGridToAsciiFile(ugridBase, output);

  // read
  std::istringstream input;
  input.str(output.str());
  BSHP<XmUGrid> ugridOut = iReadUGridFromAsciiFile(input);

  TS_ASSERT_EQUALS(ugridBase->GetPoints(), ugridOut->GetPoints());
  TS_ASSERT_EQUALS(ugridBase->GetCellStream(), ugridOut->GetCellStream());
} // XmUGridUtilsTests::testLinear3dWriteThenRead
//------------------------------------------------------------------------------
/// \brief Test reading from file.
//------------------------------------------------------------------------------
void XmUGridUtilsTests::testWriteThenReadUGridFile()
{
  BSHP<XmUGrid> ugridBase = TEST_XmUGrid3dLinear();

  // write
  std::string outFileName(TestFilesPath() + "3d_grid_linear.xmugrid");
  std::ofstream output(outFileName);
  iWriteUGridToAsciiFile(ugridBase, output);
  output.close();

  // read
  std::ifstream input(outFileName);
  BSHP<XmUGrid> ugridOut = iReadUGridFromAsciiFile(input);
  input.close();
  if (!ugridOut)
  {
    TS_FAIL("Unable to read ugrid.");
    return;
  }

  TS_ASSERT_EQUALS(ugridBase->GetPoints(), ugridOut->GetPoints());
  TS_ASSERT_EQUALS(ugridBase->GetCellStream(), ugridOut->GetCellStream());
} // XmUGridUtilsTests::testWriteThenReadUGridFile

#endif