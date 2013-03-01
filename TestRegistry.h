//
// Copyright (c) 2004 Michael Feathers and James Grenning
// Released under the terms of the GNU General Public License version 2 or later.
//

///////////////////////////////////////////////////////////////////////////////
//
// TESTREGISTRY.H
// 
// TestRegistry is a singleton collection of all the tests to run in a system.  
// 
///////////////////////////////////////////////////////////////////////////////

#ifndef TESTREGISTRY_H
#define TESTREGISTRY_H

#include "SimpleString.h"

class Test;
class TestResult;


class TestRegistry
{
public:
  TestRegistry();
  static void addTest(Test *test);
  static void runAllTests(TestResult& result);
  static void verbose();
  static void nameFilter(SimpleString&);
  static void groupFilter(SimpleString&);

private:

  static TestRegistry& instance();
  void add(Test* test);
  void run(TestResult& result);
  bool testShouldRun(Test* test, TestResult& result);
  void print(Test* test);
  void testStarted(TestResult& result, Test* test);
  void testEnded(TestResult& result, Test* test);
	
  Test *tests;
  bool verbose_;
  SimpleString nameFilter_;
  SimpleString groupFilter_;
  int dotCount;

};

#endif
