//
// Copyright (c) 2004 Michael Feathers and James Grenning
// Released under the terms of the GNU General Public License version 2 or later.
//


#include "Test.h"
#include "TestResult.h"
#include "TestRegistry.h"
#include "Failure.h"
#include "MemoryLeakWarning.h"


TestRegistry::TestRegistry()
: tests(0)
, verbose_(false)
, nameFilter_("")
, groupFilter_("")
, dotCount(0)
{
}

void TestRegistry::addTest (Test *test) 
{
	instance ().add (test);
}


void TestRegistry::runAllTests (TestResult& result) 
{
	instance ().run (result);
}

TestRegistry& TestRegistry::instance () 
{
	static TestRegistry registry;
	return registry;
}


void TestRegistry::add (Test *test) 
{
	if (tests == 0) {
		tests = test;
		return;
	}
	
	test->setNext (tests);
	tests = test;
}


void TestRegistry::run (TestResult& result) 
{
  dotCount = 0;
	result.testsStarted ();

	for (Test *test = tests; test != 0; test = test->getNext ())
  {
    result.countTest();
    if (testShouldRun(test, result) )
    {
      print(test);
      MemoryLeakWarning::CheckPointUsage();
      
	  testStarted(result, test);
	  
	  
	  try
	  {
		test->setUp();
		test->run(result);
		result.countRun();
	  }
	  catch(char * msg)
	  {
		result.addFailure( Failure( test, msg ));
	  }
	  catch(...)
	  {
		result.addFailure( Failure( test, "An exception was thrown" ) );
	  }

      test->tearDown();

	  testEnded(result, test);

      if (MemoryLeakWarning::UsageIsNotBalanced())
      {
          result.addFailure(Failure(test, MemoryLeakWarning::Message()));
      }
    }
  }
  
	result.testsEnded ();
}

void TestRegistry::verbose() 
{
    instance().verbose_ = 1;
}

void TestRegistry::nameFilter(SimpleString& f)
{
    instance().nameFilter_ = f;
}

void TestRegistry::groupFilter(SimpleString& f)
{
    instance().groupFilter_ = f;
}

bool TestRegistry::testShouldRun(Test* test, TestResult& result)
{

    if (test->shouldRun(groupFilter_, nameFilter_) )
      return true;
    else
    {
       result.countFilteredOut();
       return false;;
    } 

}

void TestRegistry::testStarted(TestResult& result, Test* test)
{
    result.testStarted(test);  
}

void TestRegistry::testEnded(TestResult& result, Test* test)
{
	result.testEnded(test);
}

void TestRegistry::print(Test* test)
{
  if (verbose_)
  {
    printf("%s\n", test->getFormattedName().asCharString());
  }
  else
  {
    printf(".");
    if (++dotCount % 50 == 0)
      printf("\n");
  }

}
