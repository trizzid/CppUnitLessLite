#include "./TestResult.h"

class XmlTestResult :
	public TestResult
{
public:
	XmlTestResult(void);
	virtual			~XmlTestResult(void);
	virtual void	testsStarted ();
	virtual void	testStarted (Test* test);
	virtual void	testEnded (Test* test);
	virtual void	addFailure (const Failure& failure);
	virtual void	testsEnded ();
};
