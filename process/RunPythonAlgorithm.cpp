/*
*  This file is part of BRAT.
*
*    BRAT is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    BRAT is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with BRAT.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "RunPythonAlgorithm.hpp"

/* Example of embedding Python script in C application  */



//////////////////////////////////////////////////////////////
//  Python interpreter singleton
//////////////////////////////////////////////////////////////
//const PythonEngine *PyAlgo::sm_pe = &PythonEngine::Instance();




//////////////////////////////////////////////////////////////
//							Main
//////////////////////////////////////////////////////////////


void createAlgorithmParamVector( PyAlgo &algo, CVectorBratAlgorithmParam &args, int argc, char *argv[] )
{
	static const unsigned first_arg_index = 3;
	
    // Checking the number of input parameters provided at command line
    if (argc - first_arg_index != algo.GetNumInputParam())
    {
        throw e_wrong_number_input_param;
    }

	algo.CreateAlgorithmParamVector( args, &argv[first_arg_index], argc - first_arg_index );
}



void usage()
{
    std::cout << "Usage: RunPythonAlgorithm <python-algorithm-file> <algorithm-class-name> [list-of-algorithm-arguments]" << std::endl;
}



int main(int argc, char *argv[])
{
    if ( argc < 3 ) {
        std::cout << "Error calling RunPythonAlgorithm: wrong number of parameters." << std::endl;
        usage();
		return -1;
	}

    EMessageCode error_code = e_OK;
    try {

        // I. create algorithm object

        PyAlgo algo( argv[1], argv[2] );

        // II. output algorithm properties

        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Algorithm Name:        " << algo.GetName()        << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        std::cout << "Algorithm Description: " << algo.GetDescription() << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        int size = algo.GetNumInputParam();
        std::cout << "Nb. Input Parameters:  " << size                  << std::endl;

        for ( int i = 0; i < size; ++i )
        {
            std::cout << "  " << i+1 << ":" << std::endl;;
            std::cout << "    Description: " <<           algo.GetInputParamDesc(i)     << std::endl;
            std::cout << "    Format:      " << typeName( algo.GetInputParamFormat(i) ) << std::endl;
            std::cout << "    Unit:        " <<           algo.GetInputParamUnit(i)     << std::endl;
        }
        std::cout << "------------------------------------------------" << std::endl;

        std::cout << "Algorithm Output Unit: " << algo.GetOutputUnit()  << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        // III. use algorithm to compute something

        CVectorBratAlgorithmParam args;
		createAlgorithmParamVector( algo, args, argc, argv );

        std::cout << "Running Algorithm... "                        << std::endl;
        double result = algo.Run( args );
        std::cout << "The result is: " << result << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
    }
    catch( EMessageCode msg )
    {
        error_code = msg;
    }
    catch( ... )
    {
        error_code = e_unspecified_error;
    }

    if ( error_code != e_OK )
    {
        std::cout << py_error_message( error_code ) << std::endl;
        return -1;
    }

    return 0;
}
