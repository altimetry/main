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

#include "PythonEngine.hpp"				//include before any Qt

#include <QDirIterator>
#include "new-gui/Common/QtUtilsIO.h"
#include "new-gui/Common/QtStringUtils.h"

#include "BratEmbeddedPythonProcess.h"

using namespace brathl;
using namespace processes;

namespace processes
{
	//static 
	const PythonEngine *CBratEmbeddedPythonProcess::smPE = nullptr;
	//static 
	bool CBratEmbeddedPythonProcess::smPythonStatus = true;
	//static 
	std::string CBratEmbeddedPythonProcess::smPythonMessages;


	//static 
	bool CBratEmbeddedPythonProcess::LoadPythonEngine( const std::string &executable_dir )
	{
		static const std::string file_prefix = "BratAlgorithm-";
		static const std::string file_suffix = ".py";

		//types

		struct python_base_creator : public base_creator
		{
			//types

			using base_t = base_creator;

			//statics

			static CBratAlgorithmBase* pseud_python_factory()
			{
				assert__( false );
				throw CException( "Programming error: calling C factory for Python algorithm" );
			}

			//data

			const std::string mFilePath;
			const std::string mClassName;

			//construction / destruction

			python_base_creator( const std::string file_path, const std::string &class_name )
				: base_t( nullptr )
				, mFilePath( file_path )
				, mClassName( class_name )
			{
				mF = &pseud_python_factory;
			}

			virtual ~python_base_creator()
			{}

			//functor

			virtual CBratAlgorithmBase* operator()( void ) override
			{
				return new PyAlgo( mFilePath, mClassName );
			}
		};


		//function body

		try
		{
			const std::wstring wpython_dir = q2w( executable_dir.c_str() ) + L"/Python";
			wchar_t *argv_buffer = new wchar_t[ wpython_dir.length() + 1 ];
			memcpy( argv_buffer, wpython_dir.c_str(), ( wpython_dir.length() + 1 ) * sizeof( wchar_t ) );
			smPE = PythonEngine::CreateInstance( argv_buffer );


			// I. load C++ algorithms

			CBratAlgorithmBase::RegisterCAlgorithms();


			// II. load Python algorithms

			if ( smPE == nullptr )
			{
				smPythonMessages += "Python engine could not be created.\n";
				smPythonStatus = false;
			}

			const std::string python_algorithms_path = executable_dir + "/Python";
			if ( !IsDir( python_algorithms_path ) )
			{
				smPythonMessages += "Python directory " + python_algorithms_path + " not found.\n";
				smPythonStatus = false;
			}

			if ( !smPythonStatus )
			{
				smPythonMessages += "\nPython algorithms will not be available.\n";
				std::cout << smPythonMessages;
				return false;
			}


			smPythonMessages += ( "Searching python algorithms in " + python_algorithms_path + "\n" );
			std::cout << smPythonMessages;

			auto &registry = CBratAlgorithmBaseRegistry::GetInstance();
			QDirIterator it( python_algorithms_path.c_str(), QStringList() << "*.py", QDir::Files, QDirIterator::Subdirectories );
			while ( it.hasNext() )
			{
				std::string path = q2a( it.next() );
				std::string file_name = GetFilenameFromPath( path );
				if ( StartsWith( file_name, file_prefix ) && EndsWith( file_name, file_suffix ) )
				{
					std::string class_name = file_name.substr( file_prefix.length() );
					class_name = class_name.substr( 0, class_name.length() - file_suffix.length() );
					registry.Add( new python_base_creator( path, class_name ) );
				}
			}

			const std::string msg = "Finished registering python algorithms.\n";
			smPythonMessages += msg;
			std::cout << msg;

			return true;
		}
		catch ( const CException &e )
		{
			smPythonMessages += ( e.Message() + "\n" );
			std::cout << e.Message() << std::endl;
		}
		catch ( ... )
		{
			const std::string msg(
				"Unknown exception caught loading python algorithm.\nPlease make sure that the python file name matches the pattern "
				+ file_prefix + "<python-class-name>" + file_suffix );
			smPythonMessages += msg;
			std::cout << msg  << std::endl;
		}

		smPythonStatus = false;

		return smPythonStatus;
	}



	//virtual 
	bool CBratEmbeddedPythonProcess::Initialize( std::string& msg )
	{
		return LoadPythonEngine( mExecutableDir ) && base_t::Initialize( msg );
	}


}	//namespace processes
