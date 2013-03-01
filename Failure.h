//
// Copyright (c) 2004 Michael Feathers and James Grenning
// Released under the terms of the GNU General Public License version 2 or later.
//


///////////////////////////////////////////////////////////////////////////////
//
// FAILURE.H
//
// Failure is a class which holds information pertaining to a specific
// test failure. It can be overriden for more complex failure messages
//
///////////////////////////////////////////////////////////////////////////////


#ifndef FAILURE_H
#define FAILURE_H

#include "SimpleString.h"

class Test;

class Failure
{

public:
    Failure(Test*, long lineNumber, const SimpleString& theMessage);
    Failure(Test*, const SimpleString& theMessage);
    Failure(Test*, long lineNumber);
    Failure(const Failure&);
    virtual ~Failure( ) {};

    virtual void Print() const;
    virtual void Print(FILE* out) const;

	SimpleString message;
	SimpleString testName;
	SimpleString fileName;
	long lineNumber;

protected:
    virtual void PrintLeader(FILE* out) const;
    virtual void PrintSpecifics(FILE* out) const;
    virtual void PrintTrailer(FILE* out) const;

private:
    Failure& operator=(const Failure&);

};


#endif
