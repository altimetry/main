
*** About the files in this directory ***


- BratAlgorithmBase.py

Must be imported by every user written Python algorithm to be loaded in brat.



- BratAlgorithm-PyAlgoExample.py and BratAlgorithm-PyAlgoExampleAnother.py

Examples of user written Python algorithms that can be loaded by brat. At 
start-up, BRAT will always search the installation sub-directory "bin/Python", 
and any of its sub-directories, for Python algorithms to load.


***

BratAlgorithmExample1.py is used only by RunPythonAlgorithm, a prototype that
illustrates a technique for embedding Python. It is not loaded by BRAT.
