#include "PIDefines.h"
#include <fstream>

using namespace std;

#ifndef MAX_PATH
#define MAX_PATH	256
#endif

/** Write stuff to a file on the desktop or in a fullpath
**/
class Logger {

	char fullPath[MAX_PATH];

	ofstream fileOut;

	void Open(void);
	
	void Close(void);

	/// Not allowed
	Logger();

 public:

	/// Create a logger file on the Desktop with .log on the end
	Logger( const char * inString );

	/// Do nothing
	~Logger();

	/// Write out a message to the log file, open, write, close
	void Write( const char * inMessage, const bool endOfLine = false );
	void Write( const int32 inValue, const bool endOfLine = false );
	void Write( const double inValue, const bool endOfLine = false );

	/// Delete the file
	void Delete(void);
};
