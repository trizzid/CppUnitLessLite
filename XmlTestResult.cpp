#include "./XmlTestResult.h"
#include "./Failure.h"
#include <time.h>

XmlTestResult::XmlTestResult(void)
{
}

XmlTestResult::~XmlTestResult(void)
{
}

void XmlTestResult::testsStarted ()
{
	TestResult::testsStarted();
	fprintf(output,"<?xml version=\"1.0\"?>");
	fprintf(output,"\n<?xml-stylesheet type='text/xsl' href='unittests.xsl'?>");
	fprintf(output,"\n<testrun>");
}

void XmlTestResult::addFailure (const Failure& failure)
{
	TestResult::addFailure(failure);
	fprintf (output, "%s%s%s%s%s%s%s%s%ld%s%s",
		"\n\t<failure>",
		"\n\t\t<message>",
		failure.message.asCharString (),
		"</message>",
    	"\n\t\t<file>",
		failure.fileName.asCharString (),
		"</file>",
    	"\n\t\t<line>",
		failure.lineNumber,
		"</line>",
		"\n\t\t<detail>");

	failure.Print(output);
	
	fprintf (output, "%s%s",
		"</detail>",
    	"\n\t</failure>");
}

void XmlTestResult::testsEnded()
{
	TestResult::testsEnded();
	struct tm *newtime;
	time_t aclock;
	time( &aclock );
	newtime = localtime( &aclock );

	fprintf(output, "\n\t<testtime>%s</testtime>",asctime( newtime ));
	fprintf(output, "\n\t<testcount>%ld</testcount>",testCount);
	fprintf(output, "\n\t<checkcount>%ld</checkcount>",checkCount);
	fprintf(output, "\n\t<failures>%ld</failures>",failureCount);
	fprintf(output, "\n\t<ignoredcount>%ld</ignoredcount>",ignoredCount);
	fprintf(output, "\n\t<filteredcount>%ld</filteredcount>",filteredOutCount);
	fprintf(output,"\n</testrun>\n");
}

void XmlTestResult::testStarted(Test* test)
{
	TestResult::testStarted(test);

	fprintf(output, "\n\t<test><name>%s</name>",test->getFormattedName().asCharString());

	if( test->getFormattedName().asCharString()[0] == 'I' )
	{
		fprintf(output, "\n\t<ignore></ignore>");
	}
}

void XmlTestResult::testEnded(Test* test)
{
	TestResult::testEnded(test);
	fprintf(output, "\n\t</test>");
}
