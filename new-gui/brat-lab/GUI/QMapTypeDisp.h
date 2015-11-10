
#if !defined(Q_MAP_TYPE_DISP_H)
#define Q_MAP_TYPE_DISP_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "QMapTypeDisp.h"
#endif


QT_BEGIN_NAMESPACE
class QString;
class QComboBox;
QT_END_NAMESPACE


#include "../../libbrathl/List.h"
using namespace brathl;


//-------------------------------------------------------------
//------------------- QMapTypeDisp class --------------------
//-------------------------------------------------------------

class QMapTypeDisp : public CUIntMap
{
public:
	QMapTypeDisp();

	virtual ~QMapTypeDisp();

	static QMapTypeDisp&  GetInstance();


	bool ValidName( const char* name );
	bool ValidName( const std::string& name );

	QString IdToName( uint32_t id );
	uint32_t NameToId( const QString& name );

	void NamesToArrayString( QStringList& array );
	void NamesToComboBox( QComboBox& combo );

	QString Enum();


protected:

public:

	enum typeDisp
	{
		typeDispYFX,
		typeDispZFXY,
		typeDispZFLatLon
	};

private:
};



#endif	//Q_MAP_TYPE_DISP_H
