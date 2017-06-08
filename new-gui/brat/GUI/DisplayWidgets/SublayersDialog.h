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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA	 02110-1301, USA.
*/
#ifndef GUI_DISPLAY_WIDGETS_SUB_LAYERS_DIALOG_H
#define GUI_DISPLAY_WIDGETS_SUB_LAYERS_DIALOG_H

#include <QDialog>





class CSubLayerIdentifier
{
	// statics 
	// 
	static const QString delim;

	// instance data
	// 
	QString mFullID;
	QStringList mIdElements;

	// construction / destruction
	
public:
	CSubLayerIdentifier( const QString &id )
		: mFullID( id )
	{
		mIdElements = mFullID.split( delim );
		while ( mIdElements.size() > 4 )
		{
			mIdElements[ 1 ] += delim + mIdElements[ 2 ];
			mIdElements.removeAt( 2 );
		}
	}

	// access
	// 
	const QStringList& IdElements() const { return mIdElements; }

	// Assumes "this" is in list
	// 
	// If there are more layers with the same name,
	// geometry type is appended separated by semicolon, example: <layer>:<geometryType>
	//
	QString UniqueQualifiedName( const std::vector<CSubLayerIdentifier> &list ) const
	{
		// If there are more sub layers of the same name (virtual for geometry types), add geometry type

		int count = 0;
		QString qualified_name = LayerName();

		for ( auto const &item : list )
		{
			if ( item.LayerName() == LayerName() )
			{
				count++;
			}
		}						assert__( count >= 1 );

		if ( count > 1 )
		{
			qualified_name += ":" + GeometryType();
		}
		else
		{
			qualified_name += ":any";
		}

		return qualified_name;
	}

	const QString& LayerID() const { return mIdElements[ 0 ]; }
	const QString& LayerName() const { return mIdElements[ 1 ]; }
	const QString& NumberOfFeatures() const { return mIdElements[ 2 ]; }
	const QString& GeometryType() const { return mIdElements[ 3 ]; }

	bool IsPlolygonType() const
	{
		auto const type = q2a( GeometryType() );
		return type.find( "Polygon" ) != std::string::npos;
	}
};


Q_DECLARE_METATYPE(const CSubLayerIdentifier*)






class CSublayersDialog : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	//types

	using base_t = QDialog;

protected:

	//instance data

	QTreeWidget *mLayersTable = nullptr;

	const std::vector<CSubLayerIdentifier> &mList;

	//construction / destruction

	void CreateWidgets();
public:
    CSublayersDialog( QWidget* parent, const std::vector<CSubLayerIdentifier> &list, Qt::WindowFlags fl = 0 );
    virtual ~CSublayersDialog();

	//access

    // Returns selected layer
	//
	const CSubLayerIdentifier* selection();

protected:
	virtual void  accept() override;

public slots:

    int exec()
#if QT_VERSION >= 0x050000
    override;
#else
    ;
#endif

};

#endif      //GUI_DISPLAY_WIDGETS_SUB_LAYERS_DIALOG_H
