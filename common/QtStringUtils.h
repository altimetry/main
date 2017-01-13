#ifndef BRAT_QT_STRING_UTILS_H
#define BRAT_QT_STRING_UTILS_H

#if defined (QT_STRING_UTILS_H)
#error Wrong QtStringUtils.h included
#endif


#include "common/+Utils.h"

#include <QtCore/QString>
#include <QDebug>


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
inline STRING q2t( const QString &qs )
{
	return qs;
}

template<>
inline std::string q2t< std::string >( const QString &qs )
{
    return q2a( qs );
}
template<>
inline std::wstring q2t< std::wstring >( const QString &qs )
{
    return q2w( qs );
}


///////////////////////////////////////////////
/// X2q conversions
///////////////////////////////////////////////

template< typename STRING = QString >
inline STRING t2q( const std::string &str )
{
	return str;
}

template< typename STRING = QString >
inline STRING t2q( const std::wstring &str )
{
	return str;
}

template< typename STRING = QString >
inline STRING t2q( const QString &qs )
{
	return qs;
}

template<>
inline QString t2q< QString >( const std::wstring &ws )
{
    //return QString::fromWCharArray( ws.c_str(), (int)ws.length() );
#ifdef _MSC_VER
    return QString::fromUtf16((const ushort *)ws.c_str());
#else
    return QString::fromStdWString( ws );
#endif
}

template<>
inline QString t2q< QString >( const std::string &s )
{
    return QString::fromStdString( s );
}

inline QString t2q( const char *s )
{
	return s;
}




///////////////////////////////////////////////
/// X2X conversions
///////////////////////////////////////////////

template< typename STRING >
inline STRING x2x( const QString &s )
{
	return q2t<STRING>( s );
}

template< typename STRING >
inline STRING x2x( const std::string &s )
{
	return t2q<STRING>( s );
}

template< typename STRING >
inline STRING x2x( const std::wstring &s )
{
	return t2q<STRING>( s );
}






//////////////////////////////////////////////////////////////////
//				Convert numbers to q-strings
//////////////////////////////////////////////////////////////////

template< typename T >
QString n2q( T n )
{
    return QString::number( n );
}





#endif		//BRAT_QT_STRING_UTILS_H
