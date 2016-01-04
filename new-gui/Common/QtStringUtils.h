#ifndef BRAT_QT_STRING_UTILS_H
#define BRAT_QT_STRING_UTILS_H

#if defined (QT_STRING_UTILS_H)
#error Wrong QtStringUtils.h included
#endif


#include "+Utils.h"

#include <QtCore/QString>


///////////////////////////////////////////////////////////////////////////
//                          QString Conversions
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////
/// q2X conversions
///////////////////////////////////////////////

inline std::wstring q2w( const QString qs )
{
#ifdef _MSC_VER
    return std::wstring((const wchar_t *)qs.utf16());
#else
    return qs.toStdWString();
#endif
}

inline std::string q2a( const QString qs )
{
    return qs.toStdString();
}

template< typename STRING >
STRING q2t( const QString &str )
{
	return str;
}

template<>
inline std::string q2t( const QString &str )
{
    return q2a( str );
}
template<>
inline std::wstring q2t( const QString &str )
{
    return q2w( str );
}


///////////////////////////////////////////////
/// X2q conversions
///////////////////////////////////////////////

inline QString t2q( const std::wstring ws )
{
    //return QString::fromWCharArray( ws.c_str(), (int)ws.length() );
#ifdef _MSC_VER
    return QString::fromUtf16((const ushort *)ws.c_str());
#else
    return QString::fromStdWString( ws );
#endif
}

inline QString t2q( const std::string s )
{
    return QString::fromStdString( s );
}
//
// for use in generics
//
inline const QString& t2q( const QString &s )
{
	return s;
}





#endif		//BRAT_QT_STRING_UTILS_H
