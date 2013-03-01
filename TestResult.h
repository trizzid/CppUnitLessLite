//
// Copyright (c) 2004 Michael Feathers and James Grenning
// Released under the terms of the GNU General Public License version 2 or later.
//

///////////////////////////////////////////////////////////////////////////////
//
// TESTRESULT.H
// 
// A TestResult is a collection of the history of some test runs.  Right now
// it just collects failures.
// 
///////////////////////////////////////////////////////////////////////////////



#ifndef TESTRESULT_H
#define TESTRESULT_H

#include <stdio.h>

#include "SimpleString.h"
#include "Test.h"

class Failure;

class TestResult
{
public:
					TestResult (); 
  virtual void	testsStarted ();
  virtual void  countTest();
  virtual void  countRun();
  virtual void  countCheck();
  virtual void  countFilteredOut();
  virtual void  countIgnored();
  virtual void	addFailure (const Failure& failure);
  virtual void	testsEnded ();
  virtual void  testStarted(Test* test);
  virtual void  testEnded(Test* test);
          void  setOutput(FILE* output);
  virtual int   getFailureCount();

//private:
protected:
  int       testCount;
  int       runCount;
  int       checkCount;
  int		failureCount;
  int       filteredOutCount;
  int       ignoredCount;
  FILE*     output;
};

#endif
