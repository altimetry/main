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
#ifndef XML_SERIALIZER_H
#define XML_SERIALIZER_H

#include <memory>

#include "common/+Utils.h"


///////////////////////////////////////
//		Include Xerces
///////////////////////////////////////

#if defined(WIN32)
//avoid later inclusion of Microsoft XML stuff, which causes name collisions with xerces
#define DOMDocument MsDOMDocument
#include <msxml.h>
#include <urlmon.h>
#undef DOMDocument
#endif		//WIN32

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xsd/cxx/tree/exceptions.hxx>


// This header must be included after the XSD generated header files,
//  because some compilers (like gcc) require xml_schema to be known,
//  even if it appears here only in a template, that is, before the
//  compiler needs it to generate code.


namespace xml_schema
{
	typedef ::xsd::cxx::tree::exception< char > exception;
}


///////////////////////////////////////
//		Generic Load and Store
///////////////////////////////////////



template< class GE >
struct serializer_traits;


template< class GP >
inline bool store( const GP &ge, std::ostream &os )
{
	//other possible format: ft = auto_ptr<XMLFormatTarget> (new StdOutFormatTarget ());
	//
	typename serializer_traits< GP >::converter_t gp;
    xml_schema::namespace_infomap map;              //but not used so far
	serializer_traits< GP >::serialize( os, ge.IOcopy( gp ) );	//, map 
	return true;
}

template< class GP >
inline bool load( GP &ge, std::istream &is )
{
    std::unique_ptr< typename serializer_traits< GP >::converter_t > pgp =
            serializer_traits< GP >::deserialize( is, xml_schema::flags::dont_validate );
	ge = *pgp;
	return true;
}


template< class GP >
inline bool store( const GP &ge, const std::string &path )
{
	//other possible format: ft = auto_ptr<XMLFormatTarget> (new StdOutFormatTarget ());
	//
    std::unique_ptr< ::xercesc::XMLFormatTarget > ft =
            std::unique_ptr< ::xercesc::XMLFormatTarget > ( new ::xercesc::LocalFileFormatTarget( path.c_str() ) );
	typename serializer_traits< GP >::converter_t gp;
    xml_schema::namespace_infomap map;              //but not used so far
	serializer_traits< GP >::serialize( *ft, ge.IOcopy( gp ) );	//, map 
	return true;
}

template< class GP >
inline bool load( GP &ge, const std::string &path )
{
    std::unique_ptr< typename serializer_traits< GP >::converter_t > pgp =
            serializer_traits< GP >::deserialize( path, xml_schema::flags::dont_validate );
	ge = *pgp;
	return true;
}


template< typename STRING >
inline STRING& translate_exception( STRING &s, const xml_schema::exception &e )
{
    typename string_traits< STRING >::str_stream_type st;
	st << e;
	s = st.str();
	return s;
}



///////////////////////////////////////
//		XSD	Assignment Helpers
///////////////////////////////////////

template< typename TO, typename FROM >
static TO enum_cast( FROM e )
{
	typedef typename FROM::value value;
	return static_cast<TO>(static_cast<value>( e ));
}

template< typename TO, typename FROM >
static TO enum_reverse_cast( FROM e )
{
	typedef typename TO::value value;
	return static_cast<value>( e );
}

template< typename TO, typename FROM >
TO fromOpt( FROM &from, const TO &def = TO() )
{
	return from.present() ? from.get() : def;
}



#endif // XML_SERIALIZER_H
