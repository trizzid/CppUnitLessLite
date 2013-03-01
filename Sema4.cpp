#include "Sema4.h"
#include <errno.h>
#include <stdlib.h>
#include <new>

Sema4* Sema4::theInstance = NULL;

Sema4::Sema4(int initialState, int maxCount)
		: m_count(initialState), m_maxCount(maxCount)
{
	pthread_mutex_init(&m_mutex, NULL);
	pthread_cond_init(&m_condition, NULL);
}

Sema4::~Sema4()
{
    pthread_cond_destroy(&m_condition);
    pthread_mutex_destroy(&m_mutex);
}

Sema4* Sema4::instance()
{
	if(theInstance == NULL)
	{
		void* ptr = malloc(sizeof(Sema4));
		theInstance = new (ptr) Sema4;
	}
	return theInstance;
}

bool Sema4::take()
{	
	pthread_mutex_lock(&m_mutex);

	m_count--;
	
	if(m_count < 0)
	{
		pthread_cond_wait(&m_condition, &m_mutex);
	}
	
	pthread_mutex_unlock(&m_mutex);
	
	return true;
}

bool Sema4::give()
{
	pthread_mutex_lock(&m_mutex);
	
	if((m_maxCount == INFINITE_SEMAPHORE) || (m_count < m_maxCount))
	{
		m_count++;
	}
	
	if(m_count <= 0)
		pthread_cond_signal(&m_condition);
	
	pthread_mutex_unlock(&m_mutex);
	
	return true;
}

