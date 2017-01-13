/*
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
#if !defined BRAT_SETTINGS_H
#define BRAT_SETTINGS_H

#include "common/QtUtilsIO.h"
#include "new-gui/Common/ApplicationSettings.h"
#include "ApplicationPaths.h"
#include "GUI/DisplayWidgets/MapWidget.h"


class CWorkspace;
class CBratApplication;






class CBratSettings : public CApplicationSettings
{
    //////////////////////////////////////
    //	types & friends
    //////////////////////////////////////

    using base_t = CApplicationSettings;

    friend class CBratApplication;


    //////////////////////////////////////
    //	static members
    //////////////////////////////////////

    static const CMapWidget::ELayerBaseType smDefaultLayerBaseType;
	static const bool smUseUnsupportedFields;
	static const size_t smMinimumFilesToWarnUser;

	static const double smHchildRatioNum;
	static const double smHchildRatioDnm;

	static const std::string smPlotsFontName;
	static const int smPlotsAxisFontSize;
	static const int smPlotsTitleFontSize;


protected:

    //////////////////////////////////////
    //	data
    //////////////////////////////////////

    CApplicationPaths &mBratPaths;

public:
    std::string m_lastDataPath;
    std::string m_lastPageReached;
    bool mAdvancedOperations = false;
    std::string m_lastWksPath;
    std::string m_lastColorTable;

	bool mLoadLastWorkspaceAtStartUp = true;

    CMapWidget::ELayerBaseType mMainLayerBaseType = CMapWidget::ELayerBaseType::eRasterLayer;
    CMapWidget::ELayerBaseType mViewsLayerBaseType = CMapWidget::ELayerBaseType::eVectorLayer;
	bool mVectorSimplifyMethod = true;

	bool mDesktopManagerSdi = true;
	bool mCheckOpenGL = true;
	bool mUseUnsupportedFields = smUseUnsupportedFields;

	bool mDisplayRadsInstallInformation = true;

protected:

	size_t mMinimumFilesToWarnUser = smMinimumFilesToWarnUser;

	double mHchildRatioNum = smHchildRatioNum;
	double mHchildRatioDnm = smHchildRatioDnm;

	std::string mPlotsFontName = smPlotsFontName;
	int mPlotsAxisFontSize = smPlotsAxisFontSize;
	int mPlotsTitleFontSize = smPlotsTitleFontSize;


    //////////////////////////////////////
    //	construction / destruction
    //////////////////////////////////////

protected:

    // Settings of type I - For exclusive use of application-wide settings
    //
    // For Application type initialization; static and non-static read/write functions can access (THE) instance type

    CBratSettings( CApplicationPaths &brat_paths, const std::string &org_name, const std::string &exec_name, const std::string &domain_name = "" ) :
        base_t( org_name, exec_name, domain_name )
      , mBratPaths( brat_paths )
    {}

public:
	// TODO REMOVE this constructor type (from this class only) when compatibility with old gui is not a concern
	//
	// Settings of type II - used by parameter files
	//
	// Module type instance; static read/write functions should not be used with this instance type

    CBratSettings( CApplicationPaths &brat_paths, const std::string &path ) :
		base_t( path )
      , mBratPaths( brat_paths )
	{}

    virtual ~CBratSettings()
    {}

    //////////////////////////////////////
    //	access
    //////////////////////////////////////


    const CApplicationPaths& BratPaths() const { return mBratPaths; }


	bool SetUserDataDirectory( bool unique, const std::string &path )
	{
		return mBratPaths.SetUserDataDirectory( unique, path );
	}


	bool SetWorkspacesDirectory( const std::string &path )
	{
		return mBratPaths.SetWorkspacesDirectory( path );
	}


    CMapWidget::ELayerBaseType MainLayerBaseType() const { return mMainLayerBaseType; }

    CMapWidget::ELayerBaseType ViewsLayerBaseType() const { return mViewsLayerBaseType; }

	bool VectorSimplifyMethod() const { return mVectorSimplifyMethod; }


    void SetURLRasterLayerPath( const std::string &path )
    {
        mBratPaths.SetURLRasterLayerPath( path );
    }


	size_t MinimumFilesToWarnUser() const { return mMinimumFilesToWarnUser; }


	//..plots 

	double HchildRatio() const { return mHchildRatioNum / mHchildRatioDnm; }

	const std::string& PlotsFontName() const { return mPlotsFontName; }

	int PlotsAxisFontSize() const { return mPlotsAxisFontSize; }

	int PlotsTitleFontSize() const { return mPlotsTitleFontSize; }


public:

    //////////////////////////////////////
    //	operations
    //////////////////////////////////////

    virtual bool LoadConfig() override;
    virtual bool SaveConfig() override;

private:

    bool LoadConfigSelectionCriteria();
    bool SaveConfigSelectionCriteria();

	bool LoadPaths();
	bool SavePaths();
	

public:
    ////////////////////////////////////////////////////
    //	TODO: DELETE
    //		- only for brat v3 support to v4 development
    ////////////////////////////////////////////////////

    template< class CONTAINER >
    bool LoadRecentFilesGeneric( CONTAINER &paths );

    bool LoadRecentFiles( std::vector<std::string> &paths );
    bool LoadRecentFiles( QStringList &paths );

    template< class CONTAINER >
    bool SaveRecentFilesGeneric( const CONTAINER &paths );

    bool SaveRecentFiles( const std::vector<std::string> &paths );
    bool SaveRecentFiles( const QStringList &paths );
};



#endif	//BRAT_SETTINGS_H
