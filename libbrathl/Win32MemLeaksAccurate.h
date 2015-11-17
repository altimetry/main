// When debugging changes all calls to "new" to be calls to "DEBUG_NEW" allowing for memory leaks to
// give you the file name and line number where it occurred.
// Needs to be included after all #include commands
#ifdef WIN32
  #ifdef _DEBUG
    //#pragma message("Win32MemLeaksAccurate activated")
    #include <crtdbg.h>
    #define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
    #define new DEBUG_NEW
  #else
	  #define DEBUG_NEW new
  #endif
#endif