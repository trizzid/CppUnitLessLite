//
// Copyright (c) 2004 Michael Feathers and James Grenning
// Released under the terms of the GNU General Public License version 2 or later.
//



#include "Failure.h"
#include "Test.h"

#include <cstdio>  // for sprintf()
#include <cstring> // for strlen()

Failure::Failure(Test* test, long lineNumber, const SimpleString& theMessage) 
: testName (test->getFormattedName()) 
, fileName (test->getFile()) 
, lineNumber (lineNumber)
, message (theMessage)
{
}


Failure::Failure(Test* test, const SimpleString& theMessage) 
: testName (test->getFormattedName()) 
, fileName (test->getFile()) 
, lineNumber (test->getLineNumber())
, message (theMessage)
{
}

Failure::Failure(Test* test, long lineNum) 
: testName (test->getFormattedName()) 
, fileName (test->getFile()) 
, lineNumber (lineNum)
, message("no message")
{
}


void Failure::Print()const
{
    PrintLeader(stderr);
    PrintSpecifics(stderr);
    PrintTrailer(stderr);
}

void Failure::Print(FILE* out)const
{
    PrintLeader(out);
    PrintSpecifics(out);
    PrintTrailer(out);
}

void Failure::PrintLeader(FILE* out)const
{
    fprintf (out, "Failure in %s\n    %s(%d)\n",
        testName.asCharString(),
        fileName.asCharString(),
        lineNumber);
}

void Failure::PrintSpecifics(FILE* out)const
{
    fprintf (out, "    %s",
		message.asCharString());
}

void Failure::PrintTrailer(FILE* out)const
{
    fprintf (out, "\n");
}


