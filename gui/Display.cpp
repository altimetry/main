/*
* 
*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "new-gui/brat/stdafx.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "brathl.h"

#include "new-gui/Common/tools/Trace.h"
#include "Tools.h"
#include "new-gui/Common/tools/Exception.h"
#include "InternalFilesFactory.h"
#include "InternalFiles.h"

#include "BratProcess.h"

using namespace processes;
using namespace brathl;

#include "PlotData/MapProjection.h"

#include "BratGui.h"
#include "new-gui/brat/Workspaces/Workspace.h"
#include "new-gui/brat/Workspaces/Operation.h"

#include "Display.h"
#include "MapTypeDisp.h"

const char* CDisplayData::FMT_FLOAT_XY = "%-#.5g";
//const uint32_t CDisplayData::NB_DIMS = 3;

//----------------------------------------------------------------------------
// CDndDisplayData
//----------------------------------------------------------------------------
//----------------------------------------

CDndDisplayData::CDndDisplayData(CDndDisplayData& dndDisplayData)
          : m_data(false)
{
  Set(dndDisplayData);
}
//----------------------------------------------------------------------------
void CDndDisplayData::Set(CDndDisplayData& dndDisplayData)
{

  Set(dndDisplayData.m_data);

}
//----------------------------------------------------------------------------
void CDndDisplayData::Set(const CObArray* data)
{
  Set(*data);
}
//----------------------------------------------------------------------------
void CDndDisplayData::Set(const CObArray& data)
{

  m_data.RemoveAll();

  CObArray::const_iterator it;
  for (it = data.begin() ; it != data.end() ; it++)
  {
    m_data.Insert(*it);
  }
}
//----------------------------------------------------------------------------
// CDndDisplayDataObject
//----------------------------------------------------------------------------

CDndDisplayDataObject::CDndDisplayDataObject(CDndDisplayData* dndDisplayData)
{
  if (dndDisplayData != nullptr)
  {
    m_dndDisplayData = new CDndDisplayData(*dndDisplayData);
  }
  else
  {
    m_dndDisplayData = nullptr;
  }

  wxDataFormat dataDisplayFormat;
  dataDisplayFormat.SetId(displayDataFormatId);
  SetFormat(dataDisplayFormat);
}
//----------------------------------------

size_t CDndDisplayDataObject::GetDataSize() const
{
  size_t ret = 0;
  //size_t count = 0;

  if (m_dndDisplayData == nullptr)
  {
    return 0;
  }
  ret = sizeof(long);

  return ret;
}
//----------------------------------------

bool CDndDisplayDataObject::GetDataHere(void* buf) const
{
  if (m_dndDisplayData == nullptr)
  {
    return false;
  }
/*
  if (m_dndDisplayData->m_displayData == nullptr)
  {
    return false;
  }
  */
  if (m_dndDisplayData->m_data.size() <= 0)
  {
    return false;
  }

  long ptr = (long)(&(m_dndDisplayData->m_data));
  memset(buf, 0, sizeof(long));
  memcpy(buf, &ptr, sizeof(long));

  return true;
}
//----------------------------------------

bool CDndDisplayDataObject::SetData(size_t len, const void* buf)
{
    UNUSED(len);

  if (m_dndDisplayData == nullptr)
  {
    m_dndDisplayData = new CDndDisplayData();
  }

  long ptr = 0;
  memcpy(&ptr, buf, sizeof(long));
  /*
  CDisplayData* displayData = (CDisplayData*)(ptr);
  m_dndDisplayData->m_displayData = displayData;
  */
  CObArray* data = (CObArray*)(ptr);

  m_dndDisplayData->Set(*data);

  return true;
}


//-------------------------------------------------------------
//------------------- CDisplayData class --------------------
//-------------------------------------------------------------
CDisplayData::CDisplayData()
{
  Init();
}
//----------------------------------------
CDisplayData::CDisplayData(COperation* operation)
{
  Init();
  m_operation = operation;
}

//----------------------------------------
CDisplayData::CDisplayData(CDisplayData& d)
{
  Init();
  Copy(d);
}

//----------------------------------------
CDisplayData::~CDisplayData()
{

}
//----------------------------------------
void CDisplayData::Init()
{
  m_operation = nullptr;
  m_group = 1;
  m_withContour = false;
  m_withSolidColor = true;
  m_invertXYAxes = false;
  m_eastcomponent = false;
  m_northcomponent = false;

  m_type = -1;

  setDefaultValue(m_minValue);
  setDefaultValue(m_maxValue);

  m_dimFields.SetDelete(false);
  m_dimFields.Insert(GetX());
  m_dimFields.Insert(GetY());
  m_dimFields.Insert(GetZ());

}
//----------------------------------------
CDisplayData& CDisplayData::operator=(CDisplayData& d)
{
  Copy(d);
  return *this;
}
//----------------------------------------
void CDisplayData::CopyFieldUserProperties(CDisplayData& d)
{
  m_field.SetDescription(d.m_field.GetDescription());

  m_x.SetDescription(d.m_x.GetDescription());
  m_y.SetDescription(d.m_y.GetDescription());
  m_z.SetDescription(d.m_z.GetDescription());

  m_withContour = d.m_withContour;
  m_withSolidColor = d.m_withSolidColor;
  m_minValue = d.m_minValue;
  m_maxValue = d.m_maxValue;
  m_colorPalette = d.m_colorPalette;
  m_xAxis = d.m_xAxis;

  m_invertXYAxes = d.m_invertXYAxes;

  m_eastcomponent = d.m_eastcomponent;
  m_northcomponent = d.m_northcomponent;

}
//----------------------------------------
void CDisplayData::Copy(CDisplayData& d)
{
  m_field.SetName(d.m_field.GetName());
  m_field.SetUnit(d.m_field.GetUnit());
  m_field.SetDescription(d.m_field.GetDescription());

  m_type = d.m_type;

  m_x.SetName(d.m_x.GetName());
  m_x.SetUnit(d.m_x.GetUnit());
  m_x.SetDescription(d.m_x.GetDescription());

  m_y.SetName(d.m_y.GetName());
  m_y.SetUnit(d.m_y.GetUnit());
  m_y.SetDescription(d.m_y.GetDescription());

  m_z.SetName(d.m_z.GetName());
  m_z.SetUnit(d.m_z.GetUnit());
  m_z.SetDescription(d.m_z.GetDescription());

  m_operation = nullptr;
  if (d.m_operation != nullptr)
  {
    m_operation = CDisplay::FindOperation(d.m_operation->GetName());
  }

  m_group = d.m_group;
  m_withContour = d.m_withContour;
  m_withSolidColor = d.m_withSolidColor;
  m_minValue = d.m_minValue;
  m_maxValue = d.m_maxValue;
  m_colorPalette = d.m_colorPalette;
  m_xAxis = d.m_xAxis;

  m_invertXYAxes = d.m_invertXYAxes;

  m_eastcomponent = d.m_eastcomponent;
  m_northcomponent = d.m_northcomponent;
}
//----------------------------------------
wxString CDisplayData::GetDataKey()
{
  if (m_operation == nullptr)
  {
    return "";
  }
  return wxString::Format("%s_%s_%d",
                          m_operation->GetName().c_str(),
                          m_field.GetName().c_str(),
                          GetType());
}
//----------------------------------------
wxString CDisplayData::GetDataKey(int32_t type)
{
  if (m_operation == nullptr)
  {
    return "";
  }
  return wxString::Format("%s_%s_%d",
                          m_operation->GetName().c_str(),
                          m_field.GetName().c_str(),
                          type);
}

//----------------------------------------
bool CDisplayData::IsYFXType()
{
  return (GetType() == CMapTypeDisp::typeDispYFX);
}
//----------------------------------------
bool CDisplayData::IsZYFXType()
{
  return (GetType() == CMapTypeDisp::typeDispZFXY);
}
//----------------------------------------
bool CDisplayData::IsZLatLonType()
{
  return (GetType() == CMapTypeDisp::typeDispZFLatLon);
}
//----------------------------------------
void CDisplayData::GetAvailableDisplayTypes(CUIntArray& displayTypes)
{
  displayTypes.RemoveAll();

  if (m_operation == nullptr)
  {
    return;
  }

  wxGetApp().GetDisplayType(m_operation, displayTypes);

}
//----------------------------------------
wxString CDisplayData::GetXAxisText(const std::string& name)
{

  std::string str = name;

  for (uint32_t index = 0 ; index < m_dimFields.size() ; index++)
  {
    CField* dim = dynamic_cast<CField*>(m_dimFields.at(index));

    if (dim == nullptr)
    {
      return "";
    }

    if (str_icmp(dim->GetName(), name))
    {
      str = dim->GetDescription().c_str();
    }

  }

  return str.c_str();

}
//----------------------------------------
wxString CDisplayData::GetXAxisText(uint32_t index)
{
  if ((index < 0) || (index >= m_dimFields.size()))
  {
    return "";
  }

  CField* dim = dynamic_cast<CField*>(m_dimFields.at(index));

  if (dim == nullptr)
  {
    return "";
  }

  return dim->GetDescription().c_str();

}
//----------------------------------------
void CDisplayData::SetXAxisText(uint32_t index, const wxString& value)
{
  if ((index < 0) || (index >= m_dimFields.size()))
  {
    return;
  }

  CField* dim = dynamic_cast<CField*>(m_dimFields.at(index));

  if (dim == nullptr)
  {
    return;
  }

  dim->SetDescription((const char *)value.c_str());
}
//----------------------------------------
bool CDisplayData::HasXComplement()
{
  CStringArray complement;
  GetXComplement(complement);

  return (complement.size() >= 1);

}
//----------------------------------------
void CDisplayData::GetXComplement(CStringArray& complement)
{
  CStringArray xName;
  CStringArray names;

  xName.Insert(GetX()->GetName());

  GetAvailableAxes(names);

  xName.Complement(names, complement);

}

//----------------------------------------
void CDisplayData::GetAvailableAxes(wxArrayString& names)
{
  CStringArray array;
  GetAvailableAxes(array);

  CBratGuiApp::CStringArrayToWxArray(array, names);

}
//----------------------------------------
void CDisplayData::GetAvailableAxes(CStringArray& names)
{

  uint32_t nbDims = m_dimFields.size();

  for (uint32_t i = 0 ; i < nbDims ; i++)
  {
    CField* dim = dynamic_cast<CField*>(m_dimFields.at(i));

    if (dim == nullptr)
    {
      continue;
    }

    wxString name = dim->GetName().c_str();

    if (name.IsEmpty())
    {
      continue;
    }

    names.Insert((const char *)name.c_str());

  }
}
//----------------------------------------
bool CDisplayData::LoadConfig(wxFileConfig* config, const wxString& path)
{
  //bool bOk = true;
  wxString valueString;

  if (config == nullptr)
  {
    return true;
  }
  /*
  wxString path = GetDataKey();
  if (pathSuff.IsEmpty() == false)
  {
    path += "_" + pathSuff;
  }
*/
  config->SetPath("/" + path);

  valueString = config->Read(ENTRY_TYPE, (const char *)CMapTypeDisp::GetInstance().IdToName(m_type).c_str());
  if (valueString.IsEmpty())
  {
    m_type = -1;
  }
  else
  {
    // Because Z=F(Lat,Lon) have been changed to Z=F(Lon,Lat)
    if (valueString.CmpNoCase("Z=F(Lat,Lon)") == 0)
    {
      m_type = CMapTypeDisp::typeDispZFLatLon;
    }
    else
    {
      m_type = CMapTypeDisp::GetInstance().NameToId(valueString);
    }
  }


  valueString = config->Read(ENTRY_FIELD);
  m_field.SetName((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_FIELDNAME);
  m_field.SetDescription((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_UNIT);
  m_field.SetUnit((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_X);
  m_x.SetName((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_XDESCR);
  m_x.SetDescription((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_XUNIT);
  m_x.SetUnit((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_Y);
  m_y.SetName((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_YDESCR);
  m_y.SetDescription((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_YUNIT);
  m_y.SetUnit((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_Z);
  m_z.SetName((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_ZDESCR);
  m_z.SetDescription((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_ZUNIT);
  m_z.SetUnit((const char *)valueString.c_str());

  valueString = config->Read(ENTRY_OPNAME);
  m_operation = CDisplay::FindOperation(valueString);

  config->Read(ENTRY_GROUP, &m_group, 1);
  config->Read(ENTRY_CONTOUR, &m_withContour, false);
  config->Read(ENTRY_SOLID_COLOR, &m_withSolidColor, true);

  config->Read(ENTRY_EAST_COMPONENT, &m_eastcomponent, false);
  config->Read(ENTRY_NORTH_COMPONENT, &m_northcomponent, false);

  config->Read(ENTRY_INVERT_XYAXES, &m_invertXYAxes, false);

  config->Read(ENTRY_MINVALUE, &m_minValue, CTools::m_defaultValueDOUBLE);
  config->Read(ENTRY_MAXVALUE, &m_maxValue, CTools::m_defaultValueDOUBLE);

  //config->Read(ENTRY_COLOR_PALETTE, &m_colorPalette, PALETTE_AEROSOL);
  config->Read(ENTRY_COLOR_PALETTE, &m_colorPalette);

  config->Read(ENTRY_X_AXIS, &m_xAxis);

  //config->Read(ENTRY_X_AXIS_TEXT, &m_xAxisText);

  // if color palette is a file (it has an extension)
  // save path in absolute form, based on workspace Display path

  wxFileName relative;
  relative.Assign(m_colorPalette);
  wxString paletteToSet = m_colorPalette;
  if (relative.HasExt())
  {
    CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
    if (wks != nullptr)
    {
      relative.Normalize(wxPATH_NORM_ALL, wks->GetPath());
      m_colorPalette = relative.GetFullPath();
    }
  }


  return true;
}
//----------------------------------------
bool CDisplayData::SaveConfig(wxFileConfig* config, const wxString& pathSuff)
{
  bool bOk = true;
  if (config == nullptr)
  {
    return true;
  }
  wxString path = GetDataKey();
  if (pathSuff.IsEmpty() == false)
  {
    path += "_" + pathSuff;
  }
  config->SetPath("/" + path);

  bOk &= config->Write(ENTRY_TYPE,
                     CMapTypeDisp::GetInstance().IdToName(m_type));


  bOk &= config->Write(ENTRY_FIELD, m_field.GetName().c_str());
  bOk &= config->Write(ENTRY_FIELDNAME, m_field.GetDescription().c_str());
  bOk &= config->Write(ENTRY_UNIT, m_field.GetUnit().c_str());

  bOk &= config->Write(ENTRY_X, m_x.GetName().c_str());
  bOk &= config->Write(ENTRY_XDESCR, m_x.GetDescription().c_str());
  bOk &= config->Write(ENTRY_XUNIT, m_x.GetUnit().c_str());

  bOk &= config->Write(ENTRY_Y, m_y.GetName().c_str());
  bOk &= config->Write(ENTRY_YDESCR, m_y.GetDescription().c_str());
  bOk &= config->Write(ENTRY_YUNIT, m_y.GetUnit().c_str());

  bOk &= config->Write(ENTRY_Z, m_z.GetName().c_str());
  bOk &= config->Write(ENTRY_ZDESCR, m_z.GetDescription().c_str());
  bOk &= config->Write(ENTRY_ZUNIT, m_z.GetUnit().c_str());


  if (m_operation != nullptr)
  {
    bOk &= config->Write(ENTRY_OPNAME, m_operation->GetName().c_str());
  }

  bOk &= config->Write(ENTRY_GROUP, m_group);
  bOk &= config->Write(ENTRY_CONTOUR, m_withContour);
  bOk &= config->Write(ENTRY_SOLID_COLOR, m_withSolidColor);
  bOk &= config->Write(ENTRY_EAST_COMPONENT, m_eastcomponent);
  bOk &= config->Write(ENTRY_NORTH_COMPONENT, m_northcomponent);

  bOk &= config->Write(ENTRY_INVERT_XYAXES, m_invertXYAxes);

  if (isDefaultValue(m_minValue) == false)
  {
    bOk &= config->Write(ENTRY_MINVALUE, CTools::Format("%.15g", m_minValue).c_str());
  }
  if (isDefaultValue(m_maxValue) == false)
  {
    bOk &= config->Write(ENTRY_MAXVALUE, CTools::Format("%.15g", m_maxValue).c_str());
  }
  if (m_colorPalette.IsEmpty() == false)
  {
    // if color palette is a file (it has an extension)
    // save path in relative form, based on workspace Display path
    wxFileName relative;
    relative.Assign(m_colorPalette);
    wxString paletteToWrite = m_colorPalette;
    if (relative.HasExt())
    {
      CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
      if (wks != nullptr)
      {
        relative.MakeRelativeTo(wks->GetPath());
        paletteToWrite = relative.GetFullPath();
      }
    }

    bOk &= config->Write(ENTRY_COLOR_PALETTE, paletteToWrite);
  }

  if (m_xAxis.IsEmpty() == false)
  {
    config->Write(ENTRY_X_AXIS, m_xAxis);
  }

  /*
  if (m_xAxisText.IsEmpty() == false)
  {
    config->Write(ENTRY_X_AXIS_TEXT, m_xAxisText);
  }

*/
  return true;
}

//-------------------------------------------------------------
//------------------- CMapDisplayData class --------------------
//-------------------------------------------------------------

CMapDisplayData::CMapDisplayData()
{
  Init();
}
//----------------------------------------
CMapDisplayData::CMapDisplayData(CMapDisplayData& m)
{
  Init();
  Copy(m);
}


//----------------------------------------
CMapDisplayData::~CMapDisplayData()
{
}
//----------------------------------------
CMapDisplayData& CMapDisplayData::operator=(CMapDisplayData& m)
{
  Copy(m);
  return *this;
}
//----------------------------------------
void CMapDisplayData::Init()
{
}

//----------------------------------------
void CMapDisplayData::Copy(CMapDisplayData& m)
{
  RemoveAll();

  CMapDisplayData::iterator it;

  for (it = m.begin() ; it != m.end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value == nullptr)
    {
      continue;
    }

    Insert((const char *)value->GetDataKey().c_str(), new CDisplayData(*value));
  }

}
//----------------------------------------
void CMapDisplayData::SetGroups(bool groupFields)
{
  if (groupFields)
  {
    GroupFields();
  }
  else
  {
    SplitFields();
  }

}
//----------------------------------------
bool CMapDisplayData::AreFieldsGrouped()
{
  CMapDisplayData::iterator it;

  bool isGroup = true;

  int groupNumber = 1;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      int otherNumber = value->GetGroup();

      if (it == begin())
      {
        groupNumber = otherNumber;
      }

      if (groupNumber != otherNumber)
      {
        isGroup = false;
        break;
      }
    }
  }

  return isGroup;
}
//----------------------------------------
void CMapDisplayData::GroupFields()
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      value->SetGroup(1);
    }
  }
}
//----------------------------------------
void CMapDisplayData::SplitFields()
{
  CMapDisplayData::iterator it;

  int32_t groupNumber = 0;
  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      groupNumber++;
      value->SetGroup(groupNumber);
    }
  }
}

//----------------------------------------
bool CMapDisplayData::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapDisplayData::ValidName(const char* name)
{
  CDisplayData* value = dynamic_cast<CDisplayData*>(Exists(name));
  return (value != nullptr);
}
//----------------------------------------
void CMapDisplayData::SetAllAxis(uint32_t index, const wxString& axisName, const wxString& axisLabel)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* displayData = dynamic_cast<CDisplayData*>(it->second);

    if (displayData == nullptr)
    {
      continue;
    }

    displayData->SetXAxis(axisName);
    displayData->SetXAxisText(index, axisLabel);
  }

}

//----------------------------------------
void CMapDisplayData::SetAllInvertXYAxes(bool value)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* displayData = dynamic_cast<CDisplayData*>(it->second);

    if (displayData == nullptr)
    {
      continue;
    }

    displayData->SetInvertXYAxes(value);
  }

}

//----------------------------------------
CDisplayData* CMapDisplayData::GetDisplayData(CMapDisplayData::const_iterator& it)
{
  return dynamic_cast<CDisplayData*>(it->second);
}
//----------------------------------------
CDisplayData* CMapDisplayData::GetDisplayData(const std::string& name)
{
  return dynamic_cast<CDisplayData*>(Exists(name.c_str()));
}
//----------------------------------------
CDisplayData* CMapDisplayData::GetDisplayData(const wxString& name)
{
  return dynamic_cast<CDisplayData*>(Exists((const char *)name.c_str()));
}
//----------------------------------------
CDisplayData* CMapDisplayData::GetDisplayData(const char* name)
{
  return dynamic_cast<CDisplayData*>(Exists(name));
}
//----------------------------------------
bool CMapDisplayData::LoadConfig(wxFileConfig* config, const wxString& pathSuff)
{
  bool bOk = true;

  if (config == nullptr)
  {
    return false;
  }

  wxString path = GROUP_DISPLAY;
  if (pathSuff.IsEmpty() == false)
  {
    path += "_" + pathSuff;
  }
  config->SetPath("/" + path);

  //long maxEntries =
  config->GetNumberOfEntries();
  wxString entry;
  wxString valueString;
  wxString displayDataName;
  long i = 0;

  do
  {
    bOk = config->GetNextEntry(entry, i);
    if (bOk)
    {
      valueString = config->Read(entry);

      if (pathSuff.IsEmpty() == false)
      {
        displayDataName = valueString.Left(valueString.Length() - pathSuff.Length() - 1);
      }
      else
      {
        displayDataName = valueString;
      }

      CDisplayData* value = dynamic_cast<CDisplayData*>(Exists((const char *)displayDataName.c_str()));
      if (value != nullptr)
      {
        Erase((const char *)displayDataName.c_str());
      }
      Insert((const char *)displayDataName.c_str(), new CDisplayData());
    }
  }
  while (bOk);


  CMapDisplayData::iterator it;

  int32_t index = 0;

  CStringMap renameKeyMap;

  for (it = begin() ; it != end() ; it++)
  {
    index++;
    CDisplayData* displayData = dynamic_cast<CDisplayData*>(it->second);
    if (displayData == nullptr)
    {

      wxMessageBox("ERROR in  CMapDisplayData::LoadConfig\ndynamic_cast<CDisplayData*>(it->second) returns nullptr pointer"
                   "\nList seems to contain objects other than those of the class CDisplayData",
                   "Error",
                    wxOK | wxCENTRE | wxICON_ERROR);
      return false;
    }

    path = wxString::Format("%s_%s", it->first.c_str(), pathSuff.c_str());
    displayData->LoadConfig(config, path);

    // To maintain compatibility with Brat v1.x (display name doesn't contain 'display type' in v1.x)
    std::string displayDataKey = (const char *)displayData->GetDataKey().c_str();

    if (it->first != displayDataKey)
    {
      renameKeyMap.Insert(it->first, displayDataKey);
    }
  }


  CStringMap::const_iterator itStringMap;

  for (itStringMap = renameKeyMap.begin() ; itStringMap != renameKeyMap.end() ; itStringMap++)
  {
    this->RenameKey(itStringMap->first, itStringMap->second);
  }

  return true;

}

//----------------------------------------
bool CMapDisplayData::SaveConfig( wxFileConfig* config, const wxString& pathSuff )
{
	if ( config == nullptr )
		return false;

	int index = 0;
	for ( CMapDisplayData::iterator it = begin(); it != end(); it++ )
	{
		wxString path = GROUP_DISPLAY;
		if ( pathSuff.IsEmpty() == false )
		{
			path += "_" + pathSuff;
		}
		config->SetPath( "/" + path );

		CDisplayData* displayData = dynamic_cast<CDisplayData*>( it->second );
		if ( displayData != nullptr )
		{
			index++;
			wxString key = displayData->GetDataKey();
			if ( pathSuff.IsEmpty() == false )
			{
				key += "_" + pathSuff;
			}

			config->Write( ENTRY_DISPLAY_DATA + n2s<std::string>( index ), key);

			displayData->SaveConfig( config, pathSuff );
		}
	}
	return true;
}

//----------------------------------------
void CMapDisplayData::NamesToArrayString(wxArrayString& array)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      array.Add(value->GetDataKey());
    }
  }

}
//----------------------------------------
void CMapDisplayData::NamesToComboBox(wxComboBox& combo)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      combo.Append(value->GetDataKey());
    }
  }

}
//----------------------------------------
void CMapDisplayData::GetFiles(wxArrayString& array)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      array.Add(value->GetOperation()->GetOutputName().c_str());
    }
  }
}
//----------------------------------------
void CMapDisplayData::GetDistinctFiles(wxArrayString& array)
{
  CStringMap::iterator it;
  CStringMap mapTmp;

  GetDistinctFiles(mapTmp);

  for (it = mapTmp.begin() ; it != mapTmp.end() ; it++)
  {
    std::string value = it->second;
    if (value.empty() == false)
    {
      array.Add(value.c_str());
    }
  }

}
//----------------------------------------
void CMapDisplayData::GetDistinctFiles(CStringMap& array)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      array.Insert((const char *)value->GetOperation()->GetName().c_str(), (const char *)value->GetOperation()->GetOutputName().c_str(), false);
    }
  }

}

//----------------------------------------
void CMapDisplayData::GetFields(wxArrayString& array)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      array.Add(value->GetField()->GetName().c_str());
    }
  }
}

//----------------------------------------
void CMapDisplayData::GetDistinctFields(wxArrayString& array)
{
  CStringMap::iterator it;
  CStringMap mapTmp;

  GetDistinctFields(mapTmp);

  for (it = mapTmp.begin() ; it != mapTmp.end() ; it++)
  {
    std::string value = it->second;
    if (value.empty() == false)
    {
      array.Add(value.c_str());
    }
  }

}
//----------------------------------------
void CMapDisplayData::GetDistinctFields(CStringMap& array)
{
  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value != nullptr)
    {
      array.Insert(value->GetField()->GetName().c_str(), it->first, false);
    }
  }

}

//----------------------------------------
bool CMapDisplayData::CheckFields(wxString& errorMsg, CDisplay* display)
{
  if (this->size() <= 1)
  {
    return true;
  }

  if (display == nullptr)
  {
    return false;
  }

  CInternalFiles *file = nullptr;

  CObMap internalFilesMap;

  CMapDisplayData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    file = nullptr;

    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value == nullptr)
    {
      continue;
    }

    COperation* operation = value->GetOperation();

    if (operation == nullptr)
    {
      continue;
    }

    bool bOk = wxFileExists(operation->GetOutputName());
    if (bOk == false)
    {
      continue;
    }

    file = dynamic_cast<CInternalFiles*>(internalFilesMap.Exists((const char *)operation->GetOutputName().c_str()));
    if (file != nullptr)
    {
      continue;
    }

    try
    {
      file = BuildExistingInternalFileKind((const char *)operation->GetOutputName().c_str());
      file->Open(ReadOnly);

    }
    catch (CException& e)
    {
      e.what();
      if (file != nullptr)
      {
        delete file;
        file = nullptr;
      }

      continue;
    }

    internalFilesMap.Insert((const char *)operation->GetOutputName().c_str(), file, false);

  }


  CNetCDFVarDef* firstNetCDFVardef = nullptr;
  CInternalFiles *firstFile = nullptr;

  bool bOk = true;


  for (it = begin() ; it != end() ; it++)
  {
    file = nullptr;

    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);
    if (value == nullptr)
    {
      continue;
    }

    COperation* operation = value->GetOperation();

    if (operation == nullptr)
    {
      continue;
    }

    file = dynamic_cast<CInternalFiles*>(internalFilesMap.Exists((const char *)operation->GetOutputName().c_str()));

    if (file == nullptr)
    {
      continue;
    }


    CField *field = value->GetField();
    if (field == nullptr)
    {
      continue;
    }

    CNetCDFVarDef* netCDFVardef = file->GetNetCDFVarDef(field->GetName());
    if (netCDFVardef == nullptr)
    {
      continue;
    }

    if (firstNetCDFVardef == nullptr)
    {
      firstNetCDFVardef = netCDFVardef;
      firstFile = file;
      continue;
    }

    std::string msg;

    //if ( ! firstNetCDFVardef->HaveEqualDims(netCDFVardef, firstFile->GetFile(), file->GetFile()) )
    if ( ! firstNetCDFVardef->HaveEqualDimNames(netCDFVardef, &msg))
    {
      errorMsg.Append(msg.c_str());
      bOk = false;
      continue;
      //break;
    }

    if ( ! firstNetCDFVardef->HaveCompatibleDimUnits(netCDFVardef, firstFile->GetFile(), file->GetFile(), &msg) )
    {
      errorMsg.Append(msg.c_str());
      bOk = false;
      continue;
      //break;
    }

    if (display->IsYFXType())
    {
      if ( ! firstNetCDFVardef->HasCompatibleUnit(netCDFVardef) )
      {
        errorMsg.Append(wxString::Format("\tExpressions '%s' and '%s' have incompatible units (unit %s vs %s)\n",
                                  firstNetCDFVardef->GetName().c_str(),
                                  netCDFVardef->GetName().c_str(),
                                  firstNetCDFVardef->GetUnit()->GetText().c_str(),
                                  netCDFVardef->GetUnit()->GetText().c_str()
                                  ));

        errorMsg.Append(msg.c_str());
        bOk = false;
        continue;
        //break;
      }
    }


  }


  internalFilesMap.RemoveAll();

  return bOk;

}

//----------------------------------------

//-------------------------------------------------------------
//------------------- CDisplay class --------------------
//-------------------------------------------------------------

const std::string CDisplay::m_zoomDelimiter = " ";
//----------------------------------------

CDisplay::CDisplay(wxString name)
{
  Init();
  m_name = name;
}
//----------------------------------------
CDisplay::CDisplay(CDisplay& d)
{
  Init();
  Copy(d);
}
//----------------------------------------

CDisplay::~CDisplay()
{
}
//----------------------------------------
CDisplay& CDisplay::operator=(CDisplay& d)
{
  Copy(d);
  return *this;
}

//----------------------------------------
void CDisplay::Copy(CDisplay& d)
{
  m_name = d.m_name;

  m_data = d.m_data;

  m_withAnimation = d.m_withAnimation;

  m_maxXValue = d.m_maxXValue;
  m_minXValue = d.m_minXValue;

  m_maxYValue = d.m_maxYValue;
  m_minYValue = d.m_minYValue;

  m_projection = d.m_projection;

  m_title = d.m_title;

  m_type = d.m_type;

  m_zoom = d.m_zoom;

  InitOutput();

}

//----------------------------------------
void CDisplay::Init()
{
  //m_type = CMapTypeDisp::typeOpYFX;
  m_type = -1;
  m_verbose = 2;
  m_withAnimation = false;
//  m_projection = CMapProjection::GetInstance()->IdToName(VTK_PROJ2D_3D);
  m_projection = "3D";

  setDefaultValue(m_minXValue);
  setDefaultValue(m_maxXValue);

  setDefaultValue(m_minYValue);
  setDefaultValue(m_maxYValue);

}
//----------------------------------------
bool CDisplay::IsYFXType()
{
  return (GetType() == CMapTypeDisp::typeDispYFX);
}
//----------------------------------------
bool CDisplay::IsZYFXType()
{
  return (GetType() == CMapTypeDisp::typeDispZFXY);
}
//----------------------------------------
bool CDisplay::IsZLatLonType()
{
  return (GetType() == CMapTypeDisp::typeDispZFLatLon);
}

//----------------------------------------
CDisplayData* CDisplay::GetDisplayData(const wxString& name)
{
  return dynamic_cast<CDisplayData*>(m_data.Exists((const char *)name.c_str()));
}
//----------------------------------------
CDisplayData* CDisplay::GetDisplayData(CMapDisplayData::iterator it)
{
  return dynamic_cast<CDisplayData*>(it->second);
}

//----------------------------------------
wxString CDisplay::FmtCmdParam(const std::string& name)
{
 return wxString::Format("%s=", name.c_str());
}

//----------------------------------------
bool CDisplay::SaveConfig(wxFileConfig* config)
{
  bool bOk = true;
  if (config == nullptr)
  {
    return true;
  }

  config->SetPath("/" + m_name);

  bOk &= config->Write(ENTRY_TYPE,
                       CMapTypeDisp::GetInstance().IdToName(m_type));


  bOk &= config->Write(ENTRY_TITLE, GetTitle());
  bOk &= config->Write(ENTRY_ANIMATION, GetWithAnimation());
  bOk &= config->Write(ENTRY_PROJECTION, GetProjection());

  if (isDefaultValue(m_minXValue) == false)
  {
    bOk &= config->Write(ENTRY_MINXVALUE, m_minXValue);
  }
  if (isDefaultValue(m_maxXValue) == false)
  {
    bOk &= config->Write(ENTRY_MAXXVALUE, m_maxXValue);
  }

  if (isDefaultValue(m_minYValue) == false)
  {
    bOk &= config->Write(ENTRY_MINYVALUE, m_minYValue);
  }
  if (isDefaultValue(m_maxYValue) == false)
  {
    bOk &= config->Write(ENTRY_MAXYVALUE, m_maxYValue);
  }

  wxString valueString = m_zoom.GetAsText(CDisplay::m_zoomDelimiter).c_str();
  bOk &= config->Write(ENTRY_ZOOM, valueString);

  // the entry ENTRY_OUTPUT  is not used any more
  //bOk &= config->Write(ENTRY_OUTPUT, GetOutputName());


  // Warning after formulas Load config conig path has changed
  m_data.SaveConfig(config, GetName() );

  return bOk;


}
//----------------------------------------
bool CDisplay::LoadConfig(wxFileConfig* config)
{
  //bool bOk = true;
  if (config == nullptr)
  {
    return true;
  }

  config->SetPath("/" + m_name);

  wxString valueString;

  valueString = config->Read(ENTRY_TYPE, (const char *)CMapTypeDisp::GetInstance().IdToName(m_type).c_str());
  if (valueString.IsEmpty())
  {
    m_type = -1;
  }
  else
  {
    m_type = CMapTypeDisp::GetInstance().NameToId(valueString);
  }

  m_title = config->Read(ENTRY_TITLE);
  config->Read(ENTRY_ANIMATION, &m_withAnimation, false);
  m_projection = config->Read(ENTRY_PROJECTION, "3D");

  config->Read(ENTRY_MINXVALUE, &m_minXValue, CTools::m_defaultValueDOUBLE);
  config->Read(ENTRY_MAXXVALUE, &m_maxXValue, CTools::m_defaultValueDOUBLE);

  config->Read(ENTRY_MINYVALUE, &m_minYValue, CTools::m_defaultValueDOUBLE);
  config->Read(ENTRY_MAXYVALUE, &m_maxYValue, CTools::m_defaultValueDOUBLE);

  valueString = config->Read(ENTRY_ZOOM);
  m_zoom.Set((const char *)valueString.c_str(), CDisplay::m_zoomDelimiter);

// the entry ENTRY_OUTPUT  is not used any more
//  valueString = config->Read(ENTRY_OUTPUT);
//  if (valueString.IsEmpty() == false)
//  {
//    SetOutput(valueString);
//  }

  InitOutput();

  m_data.LoadConfig(config, GetName());

  return true;

}

//----------------------------------------
COperation* CDisplay::FindOperation(const wxString& name)
{
  CWorkspaceOperation* wks = wxGetApp().GetCurrentWorkspaceOperation();
  if (wks == nullptr)
  {
    return nullptr;
  }

  return wks->GetOperation(name.ToStdString());
}
//----------------------------------------
bool CDisplay::UseOperation( const wxString& name )
{
	CMapDisplayData::iterator it;
	bool useOperation = false;

	for ( it = m_data.begin(); it != m_data.end(); it++ )
	{
		CDisplayData* data = dynamic_cast<CDisplayData*>( it->second );
		if ( data == nullptr )
		{
			wxMessageBox( "ERROR in  CDisplay::UseOperation\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplayData",
				"Error",
				wxOK | wxCENTRE | wxICON_ERROR );
			return false;
		}

		if ( str_icmp( data->GetOperation()->GetName(), name.ToStdString() ) )
		{
			useOperation = true;
			break;
		}
	}

	return useOperation;
}

//----------------------------------------
wxString CDisplay::GetFullCmd()
{
  wxString cmd;
#ifdef  USE_SYNC_OPTION
  cmd = wxString::Format("\"%s\" --sync \"%s\"",
                           COperation::GetExecDisplayName(),
                           m_cmdFile.GetFullPath().c_str());
#else
  cmd = wxString::Format("\"%s\" \"%s\"",
                           COperation::GetExecDisplayName(),
                           m_cmdFile.GetFullPath().c_str());
#endif
  return cmd;
}

//----------------------------------------
wxString CDisplay::GetTaskName()
{
  //wxString value = m_cmdFile.GetName();
  if (m_cmdFile.GetName().IsEmpty())
  {
    return "NoName";
  }
  /*
  if (value.Last() == '.')
  {
    value.RemoveLast();
  }
  return value;
  */
  return m_cmdFile.GetName();
}

//----------------------------------------
bool CDisplay::BuildCmdFile()
{
  m_cmdFile.Normalize();

  m_file.Create(m_cmdFile.GetFullPath(), true, wxS_DEFAULT | wxS_IXUSR);

  BuildCmdFileHeader();
  BuildCmdFileVerbose();
  BuildCmdFileGeneralProperties();
  BuildCmdFileFields();

  m_file.Close();
  return true;


}
//----------------------------------------
bool CDisplay::BuildCmdFileHeader()
{
//WriteComment("!/usr/bin/env " + wxGetApp().GetExecDisplayName()->GetName());
  WriteComment("Type:" + CMapTypeDisp::GetInstance().IdToName(m_type));
  return true;

}

//----------------------------------------
bool CDisplay::BuildCmdFileGeneralProperties()
{

  WriteEmptyLine();
  WriteComment("----- GENERAL PROPERTIES -----");
  WriteEmptyLine();

  if (m_title.IsEmpty() == false)
  {
    WriteLine(FmtCmdParam(kwDISPLAY_TITLE) + m_title);
  }

  WriteEmptyLine();
  WriteComment("Display Type:" + CMapTypeDisp::GetInstance().Enum());
  WriteEmptyLine();

  WriteLine(FmtCmdParam(kwDISPLAY_PLOT_TYPE) + GetTypeAsString());

  WriteEmptyLine();

  switch (m_type)
  {
    case CMapTypeDisp::typeDispYFX :
      BuildCmdFileGeneralPropertiesXY();
      break;
    case CMapTypeDisp::typeDispZFXY :
      BuildCmdFileGeneralPropertiesZXY();
      break;
    case CMapTypeDisp::typeDispZFLatLon :
      BuildCmdFileGeneralPropertiesZLatLon();
      break;
    default :
      break;
  }


  return true;
}
//----------------------------------------
bool CDisplay::BuildCmdFileGeneralPropertiesXY()
{
  wxString valueString;

  valueString = (isDefaultValue(GetMinXValue())) ? "DV" : GetMinXValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_XMINVALUE) + valueString);

  valueString = (isDefaultValue(GetMaxXValue())) ? "DV" : GetMaxXValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_XMAXVALUE) + valueString);

  valueString = (isDefaultValue(GetMinYValue())) ? "DV" : GetMinYValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_YMINVALUE) + valueString);

  valueString = (isDefaultValue(GetMaxYValue())) ? "DV" : GetMaxYValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_YMAXVALUE) + valueString);

  WriteEmptyLine();

  wxString axisName;
  wxString axisLabel;

  WriteComment("----- X AXIS -----");

  CObMap::iterator it;
  for (it = m_data.begin() ; it != m_data.end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);

    if (value == nullptr)
    {
      continue;
    }


    if (value->GetXAxis().IsEmpty())
    {
      axisName = value->GetX()->GetName().c_str();
      axisLabel = value->GetX()->GetDescription().c_str();
    }
    else
    {
      axisName = value->GetXAxis();
      axisLabel = value->GetXAxisText((const char *)value->GetXAxis().c_str());
    }

    WriteEmptyLine();
    WriteLine(FmtCmdParam(kwDISPLAY_XAXIS) + axisName);
    WriteLine(FmtCmdParam(kwDISPLAY_XLABEL) + axisLabel);

    if (AreFieldsGrouped())
    {
      break;
    }
  }



  return true;
}
//----------------------------------------
bool CDisplay::BuildCmdFileGeneralPropertiesZXY()
{
  wxString valueString;

  WriteLine(FmtCmdParam(kwDISPLAY_ANIMATION) + GetWithAnimationAsText());

  valueString = (isDefaultValue(GetMinXValue())) ? "DV" : GetMinXValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_XMINVALUE) + valueString);

  valueString = (isDefaultValue(GetMaxXValue())) ? "DV" : GetMaxXValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_XMAXVALUE) + valueString);

  valueString = (isDefaultValue(GetMinYValue())) ? "DV" : GetMinYValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_YMINVALUE) + valueString);

  valueString = (isDefaultValue(GetMaxYValue())) ? "DV" : GetMaxYValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_YMAXVALUE) + valueString);

  WriteEmptyLine();

  WriteComment("----- X/Y AXES -----");

  wxString xAxisName;
  wxString yAxisName;

  CObMap::iterator it;
  for (it = m_data.begin() ; it != m_data.end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);

    if (value == nullptr)
    {
      continue;
    }

    if (value->IsInvertXYAxes())
    {
      xAxisName = value->GetY()->GetName().c_str();
      yAxisName = value->GetX()->GetName().c_str();
    }
    else
    {
      xAxisName = value->GetX()->GetName().c_str();
      yAxisName = value->GetY()->GetName().c_str();
    }

    WriteEmptyLine();
    WriteLine(FmtCmdParam(kwDISPLAY_XAXIS) + xAxisName);
    WriteLine(FmtCmdParam(kwDISPLAY_YAXIS) + yAxisName);

    if (AreFieldsGrouped())
    {
      break;
    }
  }


  return true;
}
//----------------------------------------
bool CDisplay::BuildCmdFileGeneralPropertiesZLatLon()
{
  wxString valueString;

  WriteLine(FmtCmdParam(kwDISPLAY_ANIMATION) + GetWithAnimationAsText());

  WriteLine(FmtCmdParam(kwDISPLAY_PROJECTION) + m_projection);

  CStringArray array;

  m_zoom.GetAsString(array);

  if (array.size() == 4)
  {
    WriteLine(FmtCmdParam(kwDISPLAY_ZOOM_LAT1) + array.at(0).c_str());
    WriteLine(FmtCmdParam(kwDISPLAY_ZOOM_LON1) + array.at(1).c_str());
    WriteLine(FmtCmdParam(kwDISPLAY_ZOOM_LAT2) + array.at(2).c_str());
    WriteLine(FmtCmdParam(kwDISPLAY_ZOOM_LON2) + array.at(3).c_str());
  }

  return true;
}
//----------------------------------------
bool CDisplay::BuildCmdFileFields()
{
  bool bOk = true;

  if (m_type == CMapTypeDisp::typeDispYFX)
  {
     bOk = BuildCmdFileFieldsYFX();
  }
  else
  {
    bOk = BuildCmdFileFieldsZFXY();
  }

  return bOk;
}

//----------------------------------------
bool CDisplay::BuildCmdFileFieldsZFXY()
{


  CMapDisplayData::iterator it;

  for (it = m_data.begin() ; it != m_data.end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);

    if (value == nullptr)
    {
      continue;
    }


    WriteEmptyLine();
    WriteComment(wxString::Format("----- %s FIELD -----", value->GetField()->GetName().c_str()));
    WriteEmptyLine();
    WriteLine(wxString::Format("%s=%s", kwFIELD.c_str(), value->GetField()->GetName().c_str()));
    WriteLine(wxString::Format("%s=%s", kwFILE.c_str(), value->GetOperation()->GetOutputName().c_str()));
    BuildCmdFileFieldProperties((it->first).c_str());
  }

  return true;

/*
  wxString valueString;


  CStringMap strMap;
  CStringMap::iterator it;
  m_data.GetDistinctFields(strMap);

  for (it = strMap.begin() ; it != strMap.end() ; it++)
  {
    WriteEmptyLine();
    WriteComment(wxString::Format("----- %s FIELD -----", (it->first).c_str()));
    WriteEmptyLine();
    WriteLine(wxString::Format("FIELD=%s", (it->first).c_str()));
    BuildCmdFileFieldProperties((it->second).c_str());
  }
  return true;
*/

}
//----------------------------------------
bool CDisplay::BuildCmdFileFieldsYFX()
{

  CMapDisplayData::iterator it;

  for (it = m_data.begin() ; it != m_data.end() ; it++)
  {
    CDisplayData* value = dynamic_cast<CDisplayData*>(it->second);

    if (value == nullptr)
    {
      continue;
    }


    WriteEmptyLine();
    WriteComment(wxString::Format("----- %s FIELD -----", value->GetField()->GetName().c_str()));
    WriteEmptyLine();
    WriteLine(wxString::Format("%s=%s", kwFIELD.c_str(), value->GetField()->GetName().c_str()));
    WriteLine(wxString::Format("%s=%s", kwFILE.c_str(), value->GetOperation()->GetOutputName().c_str()));
    BuildCmdFileFieldProperties((it->first).c_str());
  }

  return true;

}
//----------------------------------------
bool CDisplay::BuildCmdFileFieldProperties(const wxString& dataKey)
{
  CDisplayData* data = m_data.GetDisplayData(dataKey);
  if (data == nullptr)
  {

    wxMessageBox("ERROR in  CDisplay::BuildCmdFileFieldProperties\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
                 "\nList seems to contain objects other than those of the class CDisplayData",
                 "Error",
                  wxOK | wxCENTRE | wxICON_ERROR);
    return false;
  }
  WriteEmptyLine();
  WriteComment(wxString::Format("----- %s FIELDS PROPERTIES -----", data->GetField()->GetName().c_str()));
  WriteEmptyLine();

  wxString displayName = data->GetField()->GetDescription().c_str();
  if (displayName.IsEmpty() == false)
  {
    WriteLine(FmtCmdParam(kwDISPLAY_NAME) + displayName);
  }
  else
  {
    WriteLine(FmtCmdParam(kwDISPLAY_NAME) + data->GetField()->GetName().c_str());
  }


  WriteLine(FmtCmdParam(kwFIELD_GROUP) + data->GetGroupAsText());

  switch (m_type)
  {
    case CMapTypeDisp::typeDispYFX :
      BuildCmdFileFieldPropertiesXY(data);
      break;
    case CMapTypeDisp::typeDispZFXY :
    case CMapTypeDisp::typeDispZFLatLon :
      BuildCmdFileFieldPropertiesZXY(data);
      break;
    default :
      break;
  }

  return true;
}

//----------------------------------------
bool CDisplay::BuildCmdFileFieldPropertiesXY(CDisplayData* value)
{
  if (value == nullptr)
  {
    return false;
  }
  return true;
}

//----------------------------------------
bool CDisplay::BuildCmdFileFieldPropertiesZXY(CDisplayData* value)
{
  wxString valueString;

  if (value == nullptr)
  {
    return false;
  }

  valueString = (isDefaultValue(value->GetMinValue())) ? "DV" : value->GetMinValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_MINVALUE) + valueString);

  valueString = (isDefaultValue(value->GetMaxValue())) ? "DV" : value->GetMaxValueAsText();
  WriteLine(FmtCmdParam(kwDISPLAY_MAXVALUE) + valueString);

  WriteLine(FmtCmdParam(kwDISPLAY_CONTOUR) + value->GetContourAsText());
  WriteLine(FmtCmdParam(kwDISPLAY_SOLID_COLOR) + value->GetSolidColorAsText());

  valueString = (value->GetColorPalette().IsEmpty()) ? "DV" : value->GetColorPalette();
  WriteLine(FmtCmdParam(kwDISPLAY_COLORTABLE) + valueString);

  WriteLine(FmtCmdParam(kwDISPLAY_EAST_COMPONENT) + (value->IsEastComponent() ? "Y" : "N"));

  WriteLine(FmtCmdParam(kwDISPLAY_NORTH_COMPONENT) + (value->IsNorthComponent() ? "Y" : "N"));

  return true;

}

//----------------------------------------
bool CDisplay::BuildCmdFileVerbose()
{

  WriteEmptyLine();
  WriteComment("----- LOG -----");
  WriteEmptyLine();
  WriteLine(wxString::Format("%s=%d", kwVERBOSE.c_str(), m_verbose));

  return true;

}

//----------------------------------------
bool CDisplay::WriteComment(const wxString& text)
{
  WriteLine("#" + text);
  return true;
}
//----------------------------------------
bool CDisplay::WriteLine(const wxString& text)
{
  if (m_file.IsOpened() == false)
  {
    return false;
  }
  m_file.Write(text + "\n");
  return true;
}
//----------------------------------------
bool CDisplay::WriteEmptyLine()
{
  if (m_file.IsOpened() == false)
  {
    return false;
  }
  m_file.Write("\n");
  return true;
}
//----------------------------------------
void CDisplay::InitOutput()
{
	CWorkspaceDisplay* wks = wxGetApp().GetCurrentWorkspaceDisplay();
	if ( wks == nullptr )
		return;

	wxString output = wks->GetPath()	//wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR	!!! femm: Check this !!!
		+ "/Display"
		+ GetName() + "." + COMMANDFILE_EXTENSION;
	SetOutput( output );
}

//----------------------------------------
bool CDisplay::ExistData(const wxString& key)
{
  CDisplayData* dataTmp = dynamic_cast<CDisplayData*>(m_data.Exists((const char *)key.c_str()));
  return (dataTmp != nullptr);
}
//----------------------------------------
bool CDisplay::InsertData(const wxString& key, CDisplayData* data)
{
  if (ExistData(key))
  {
    return false;
  }
  m_data.Insert((const char *)key.c_str(), data, false);

  m_type = data->GetType();

  return true;
}
//----------------------------------------
bool CDisplay::RemoveData(const wxString& key)
{
  bool bOk = m_data.Erase((const char *)key.c_str());

  if (m_data.size() <= 0)
  {
    m_type = -1;
  }

  return bOk;
}
//----------------------------------------

bool CDisplay::AreFieldsGrouped()
{
  return m_data.AreFieldsGrouped();

}
//----------------------------------------

void CDisplay::SetGroups(bool groupFields)
{
  m_data.SetGroups(groupFields);

}
//----------------------------------------
void CDisplay::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }


  fOut << "==> Dump a CDisplay Object at "<< this << std::endl;

  fOut << "==> END Dump a CDisplay Object at "<< this << std::endl;

  fOut << std::endl;

}

