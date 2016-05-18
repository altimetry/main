#!/usr/bin/python -tt
#
#  This file is part of BRAT.
#
#    BRAT is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    BRAT is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with BRAT.  If not, see <http://www.gnu.org/licenses/>.
#

'''
  This python module contains the code used for defining a new algorithm, and
  it shall be taken as an example.
  (Python v.3.2)
'''

from BratAlgorithmBase import PyBratAlgoBase


class PyAlgoExample(PyBratAlgoBase):
    
    def __init__(self):
        self.m_1 = float()
        self.m_2 = int()
        self.m_3 = str()
        self.m_4 = int()
        self.m_5 = bytes()

    def Run(self, PyAlgoParams):
        self.SetParamValues(PyAlgoParams)
        self.Dump()
        
        return float(self.m_2) + float(self.m_4)
        
    
    def GetName(self):
        return "PythonExample"
    
    def GetDescription(self):
        return ("Example 1 of algorithm " +
                "- return (parameter 2 + parameter 4)")

    def GetNumInputParam(self):
        return 5
    
    def GetInputParamName(self, indexParam):
        Param_dict = {0       : "1st",
                      1       : "2nd",
                      2       : "3rd",
                      3       : "4th",
                      4       : "5th",
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamDesc(self, indexParam):
        Param_dict = {0       : "first PyAlgoExample parameter description",
                      1       : "second PyAlgoExample parameter description",
                      2       : "third PyAlgoExample parameter description",
                      3       : "fourth PyAlgoExample parameter description",
                      4       : "fifth PyAlgoExample parameter description",
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamFormat(self, indexParam):
        Param_dict = {0       : PyBratAlgoBase.Py_FLOAT,
                      1       : PyBratAlgoBase.Py_INT,
                      2       : PyBratAlgoBase.Py_STRING,
                      3       : PyBratAlgoBase.Py_LONG,
                      4       : PyBratAlgoBase.Py_BYTE,
                      }
        value = Param_dict.get(indexParam)
        return value
    
    def GetInputParamUnit(self, indexParam):
        Param_dict = {0       : "degrees_north",
                      1       : "m",
                      2       : "",
                      3       : "m", 
                      4       : ""   
                      }
        value = Param_dict.get(indexParam)
        return value

    def GetOutputUnit(self):
        return "m"

    #----------------------------------#
    #   Used by PyAlgoExample.Run()   #
    #----------------------------------#
    def SetParamValues(self, PyAlgoParams):
        self.CheckInputParams(PyAlgoParams)
        
        self.m_1 = float(PyAlgoParams[0])
        self.m_2 =   int(PyAlgoParams[1])
        self.m_3 =   str(PyAlgoParams[2])
        self.m_4 =   int(PyAlgoParams[3])
        self.m_5 = bytes([PyAlgoParams[4]])
    
    def Dump(self):
        fOut = ""
        fOut += ("\n==> Dump a PyAlgoExample Object at " + str(hex(id(self))) + 
                 "\n")
  
        fOut += ("m_1: " + str(self.m_1) + "\n")
        fOut += ("m_2: " + str(self.m_2) + "\n")
        fOut += ("m_3: " + str(self.m_3) + "\n")
        fOut += ("m_4: " + str(self.m_4) + "\n")
        fOut += ("m_5: " + str(self.m_5) + "\n")
        
        fOut += ("==> END Dump a PyAlgoExample Object at " + str(hex(id(self)))
                 + "\n")
        print (fOut)

 

def main():   
    """ xxx xxx"""
    
    print ('Subclass:', issubclass(PyAlgoExample,   PyBratAlgoBase) )
    print ('Instance:', isinstance(PyAlgoExample(), PyBratAlgoBase) )
    
    algo = PyAlgoExample()
    print (" ")
    print ("Name:        " + str( algo.GetName() ))
    print ("Description: " + str( algo.GetDescription() ))
    print ("Nb of Input: " + str( algo.GetNumInputParam() ))
    
    print ("RESULT:      " + str( algo.Run([10, 20, 'abcd', 40, 42]) )
                           + " " + algo.GetOutputUnit()            )
    
    print ('\nSubclasses of PyAlgoBase: ')
    for sc in PyBratAlgoBase.__subclasses__():
        print (sc.__name__)
      


# This is the standard boilerplate that calls the main() function.
if __name__ == '__main__':
    main()