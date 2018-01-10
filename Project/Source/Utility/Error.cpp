/* All content(c) 2017 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Error.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 30/09/2016
\brief
  Contains the implementation of the Error interface.
*/
/*****************************************************************************/

// PARAMETERS
//! The filename that Error exceptions will be written to from the 
// ErrorLog class.
#define ERROR_LOG_FILENAME     "assignment3.error"
#define ROOTERROR_LOG_FILENAME "assignment3.root.error"
// !PARAMETERS

#include <iostream>
#include <string>
#include <vector>

#include "Error.h"

// ERROR //////////////////////////////////////////////////////////////////////

/*****************************************************************************/
/*!
\brief
  This is the only constructor for an Error.

\param file
  The name of the file where an error was thrown.
\param function
  The name of the function where an error was thrown.
*/
/*****************************************************************************/
Error::Error(const char * file, const char * function)
: _file(file), _function(function) {}

/*****************************************************************************/
/*!
\brief
  Use this to add information to the Error's log. This can be called multiple
  times on a single Error. All of the log information is stored in a vector.
  See the operator<< to see how this is printed to console.

\param info
  The Error info to be added to the log.
*/
/*****************************************************************************/
void Error::Add(const char * info)
{
  _log.push_back(info);
}

/*****************************************************************************/
/*!
\brief
  Use this to add information to the Error's log. This can be called multiple
  times on a single Error. All of the log information is stored in a vector.
  See the operator<< to see how this is printed to console.

\param info
  The Error info to be added to the log.
*/
/*****************************************************************************/
void Error::Add(const std::string & info)
{
  _log.push_back(info);
}

/*****************************************************************************/
/*!
\brief
Operator << overload for the Error class. When an error is printed, the file
name, function name, and log will all be printed.

\param os
The outstream being printed to.
\param error
The Error to be printed.

\return The outstream being printed to.
*/
/*****************************************************************************/
std::ostream & operator<<(std::ostream & os, const Error & error)
{
  //print header
  os << "---- Error Start ----" << std::endl
    << "File - " << error._file << std::endl
    << "Function - " << error._function << std::endl
    << "> Log" << std::endl;
  //print log
  std::vector<std::string>::const_iterator info;
  for (info = error._log.begin(); info != error._log.end(); ++info)
    os << *(info) << std::endl;
  os << "----- Error End -----" << std::endl;
  return os;
}

/*****************************************************************************/
/*!
\brief 
  Appends an error to the end of a string.

\param string
  The string to be appended to.
\param error
  The error being appended to the string.
*/
/*****************************************************************************/
void operator<<(std::string * string, const Error & error)
{
  //print header
  string->append("---- Error Start ----\n");
  string->append("File - ");
  string->append(error._file);
  string->append(1, '\n');
  string->append("Function - ");
  string->append(error._function);
  string->append(1, '\n');
  //print log
  string->append("> Log\n");
  std::vector<std::string>::const_iterator info;
  for (info = error._log.begin(); info != error._log.end(); ++info) {
    string->append(*info);
    string->append(1, '\n');
  }
  //done
  string->append("----- Error End -----\n");
}

// ROOTERROR //////////////////////////////////////////////////////////////////

/*****************************************************************************/
/*!
\brief
  Creates a RootError. This should only be thrown in the error termninates the
  program.

\param file
  The file in which the error occured.
\param function
  The function in which the error occured.
*/
/*****************************************************************************/
RootError::RootError(const char * file, const char * function) :
  Error(file, function)
{}

// ERRORLOG ///////////////////////////////////////////////////////////////////

// STATIC INITIALIZATIONS
std::vector<std::string *> ErrorLog::_errorStrings;
bool ErrorLog::_errorWritten = false;
bool ErrorLog::_rootErrorWritten = false;
std::string ErrorLog::_errorFilename = ERROR_LOG_FILENAME;
std::string ErrorLog::_rootErrorFilename = ROOTERROR_LOG_FILENAME;
std::ofstream ErrorLog::_errorLog;

/*****************************************************************************/
/*!
\brief
  Rights clean to at the top of both log files to signify that there are no
  errors.
*/
/*****************************************************************************/
void ErrorLog::Clean()
{
  _errorLog.open(_errorFilename);
  if (_errorLog.is_open()) {
    _errorLog << "CLEAN" << std::endl;
    _errorLog.close();
  }
  _errorLog.open(_rootErrorFilename);
  if (_errorLog.is_open()) {
    _errorLog << "CLEAN" << std::endl;
    _errorLog.close();
  }
}

/*****************************************************************************/
/*!
\brief
  Writes an Error to the filename specified in ERROR_LOG_FILENAME at the top
  of this file.

\param error
  The error that is being written to the error log file.
*/
/*****************************************************************************/
void ErrorLog::Write(const Error & error)
{
  // append or overwrite
  if (_errorWritten) 
    _errorLog.open(_errorFilename, std::fstream::app);
  else {
    _errorLog.open(_errorFilename);
    _errorWritten = true;
  }
  // writing to file
  if (_errorLog.is_open()) {
    _errorLog << error;
    _errorLog.close();
  }
  // writing to error strings
  for(std::string * string : _errorStrings)
    string << error;
}

/*****************************************************************************/
/*!
\brief
  Writes an RootError to the filename specified in ROOTERROR_LOG_FILENAME at the 
  top of this file.

\param root_error
  The root error that is being written to the root error log file.
*/
/*****************************************************************************/
void ErrorLog::Write(const RootError & root_error)
{
  _errorLog.open(_rootErrorFilename);
  // writing root error
  if (_errorLog.is_open()) {
    // Checking for multiple root errors
    if (_rootErrorWritten) 
      _errorLog << "> MULTIPLE RootErrors";
    else 
      _rootErrorWritten = true;
    _errorLog << root_error;
    _errorLog.close();
  }
}

/*****************************************************************************/
/*!
\brief
  After giving a string to this function, it will be written
  to each time an error is written to the ErrorLog.

\param error_string
  The string being added to the ErrorLog writing process.
*/
/*****************************************************************************/
void ErrorLog::AddErrorString(std::string * error_string)
{
  _errorStrings.push_back(error_string);
}