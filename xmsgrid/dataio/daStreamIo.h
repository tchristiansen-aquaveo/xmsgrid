#pragma once
//------------------------------------------------------------------------------
/// \file
/// \ingroup dataio
/// \copyright (C) Copyright Aquaveo 2018. Distributed under FreeBSD License
/// (See accompanying file LICENSE or https://aqaveo.com/bsd/license.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------
// 3. Standard Library Headers
#include <sstream>

// 4. External Library Headers

// 5. Shared Headers
#include <xmscore/misc/boost_defines.h>
#include <xmscore/stl/vector.h>

// 6. Non-shared Headers

//----- Forward declarations ---------------------------------------------------

//----- Namespace declaration --------------------------------------------------

namespace xms
{
//----- Constants / Enumerations -----------------------------------------------

//----- Forward declarations ---------------------------------------------------

//----- Structs / Classes ------------------------------------------------------

class DaStreamReader
{
public:
  explicit DaStreamReader(std::istream& a_inStream, bool a_binary = false);
  DaStreamReader(const DaStreamReader&) = delete;
  ~DaStreamReader();
  DaStreamReader& operator=(const DaStreamReader&) = delete;

  bool IsBinary() const;
  bool ReadNamedLine(const char* a_name);
  bool ReadLine(std::string& a_line);
  bool ReadIntLine(const char* a_name, int& a_val);
  bool ReadDoubleLine(const char* a_name, double& a_val);
  bool ReadStringLine(const char* a_name, std::string& a_val);
  bool ReadVecInt(const char* a_name, VecInt& a_vec);
  bool ReadVecDbl(const char* a_name, VecDbl& a_vec);
  bool ReadVecPt3d(const char* a_name, VecPt3d& a_vec);
  bool Read2StringLine(const char* a_name,
                       std::string& a_val1,
                       std::string& a_val2);
  bool Read3StringLine(const char* a_name,
                       std::string& a_val1,
                       std::string& a_val2,
                       std::string& a_val3);
  bool Read3DoubleLine(const char* a_name,
                       double& a_val1,
                       double& a_val2,
                       double& a_val3);

  static bool ReadIntFromLine(std::string& a_line, int& a_val);
  static bool ReadStringFromLine(std::string& a_line, std::string& a_val);
  static bool ReadDoubleFromLine(std::string& a_line, double& a_val);

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

class DaStreamWriter
{
public:
  explicit DaStreamWriter(std::ostream& a_outStream, bool a_binary = false);
  DaStreamWriter(const DaStreamWriter&) = delete;
  ~DaStreamWriter();
  DaStreamWriter& operator=(const DaStreamWriter&) = delete;
  
  bool IsBinary() const;
  void WriteNamedLine(const char* a_name);
  void WriteVecInt(const char* a_name, const VecInt& a_vec);
  void WriteVecDbl(const char* a_name, const VecDbl& a_vec);
  void WriteVecPt3d(const char* a_name, const VecPt3d& a_vec);

  void WriteIntLine(const char* a_name, int a_val);

  void WriteDoubleLine(const char* a_name, double a_val);
  void Write3DoubleLine(const char* a_name,
                        const double& a_val1,
                        const double& a_val2,
                        const double& a_val3);

  void WriteLine(const std::string& a_line);
  void WriteStringLine(const char* a_name, const std::string& a_val);
  void Write2StringLine(const char* a_name,
                          const std::string& a_val1,
                          const std::string& a_val2);
  void Write3StringLine(const char* a_name,
                        const std::string& a_val1,
                        const std::string& a_val2,
                        const std::string& a_val3);

private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

//----- Function prototypes ----------------------------------------------------

} // namespace xms