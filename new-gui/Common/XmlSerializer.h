#ifndef XML_SERIALIZER_H
#define XML_SERIALIZER_H

#include <memory>


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


// This header must be included after the XSD generated header files,
//  because some compilers (like gcc) require xml_schema to be known,
//  even if it appears here only in a template, that is, before the
//  compiler needs it to generate code.


///////////////////////////////////////
//		Generic Load and Store
///////////////////////////////////////



template< class GE >
struct serializer_traits;


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
