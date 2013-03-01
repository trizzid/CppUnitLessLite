#ifndef _SEMA4_H_
#define _SEMA4_H_

#include <pthread.h>

#define MEMSEM Sema4::instance()

class Sema4
{
public:
	static Sema4* instance();
	virtual ~Sema4();
	bool take();
	bool give();
	enum {INFINITE_SEMAPHORE = -1};
	
private:
	Sema4(int initialState = 1, int maxCount = 1);
	static Sema4* theInstance;
	int m_count;
	int m_maxCount;
    pthread_mutex_t    m_mutex;
    pthread_cond_t     m_condition;
};

#endif //_SEMA4_H_
