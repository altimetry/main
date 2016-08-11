BratAlgorithmBase.py
Must be imported by every user written Python algorithm to be loaded in brat.

BratAlgorithmExample1.py is used by the command line executable RunPythonAlgorithm,
a prototype for embedding Python.

BratAlgorithm-PyAlgoExample.py
and
BratAlgorithm-PyAlgoExampleAnother.py

are examples of user written Python algorithms that can be loaded by brat. To test
them, they must be copied first, with BratAlgorithmBase.py, to the sub-directory 
"python" of the "Default Data Directory", which you can specify in the brat 
application settigs dialog. 
At start-up, brat will always search this directory for python algorithms to load.
