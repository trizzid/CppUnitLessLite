#include "MemoryLeakWarning.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// For backtrace functions
#ifndef	NO_BACKTRACE
#include <execinfo.h>
#endif	/* NO_BACKTRACE */
#include <cxxabi.h>

#include "Sema4.h"

static int allocatedBlocks = 0;
static int initialBlocksUsed = 0;

static bool tolerate = false;
static bool enableTolerate = false;
static char message[50] = "";

// Changes for Opera development:
// Implement mechanism to hold data about each newed block
// and provide backtrace for leaked blocks

#define BACKTRACEDEPTH   20
#define BACKTRACESKIP     2

// Double-linked lists
typedef struct DLEntry
{
   struct DLEntry* previous;
   struct DLEntry* next;
   // Data for each newed block
   void* address;
   size_t size;
   void* ret_addr[BACKTRACEDEPTH];
} DLEntry;

// Static variables for the linked list
static bool checking = false;

static DLEntry* first = 0;
static DLEntry* last = 0;
static DLEntry* current = 0;

DLEntry*
First ( void )
{
   return current = first;
}
 
DLEntry*
Last ( void )
{
   return current = last;
}

DLEntry*
Next ( void )
{
   return current = current->next;
}

DLEntry*
Previous ( void )
{
   return current = current->previous;
}

DLEntry* 
Add (void)
{
   // adds the new entry to the end of the list
   DLEntry* insert = (DLEntry*)malloc(sizeof(DLEntry));
   insert->next = 0;
   insert->previous = last;
   if ( last != 0 )
      last->next = insert;
   last = insert;
   if ( first == 0 )
   {
      first = insert;
   }
   return insert;
}


void
Remove (DLEntry *old_entry)
{
   if ( old_entry->next != 0 )
      old_entry->next->previous = old_entry->previous;
   else
   {
      // Deleting last entry in the list
      last = old_entry->previous;
   }

   if ( old_entry->previous != 0 )
      old_entry->previous->next = old_entry->next;
   else
   {
      // Deleting first entry in the list
      first = old_entry->next;
   }

   free(old_entry);

}

// This block occupies the first part of the user's memory block
struct Blockhead
{
   DLEntry * dlentry;
} Blockhead;


void* my_malloc(size_t reqsize)
{
   struct Blockhead * blockh;
   struct DLEntry * dlentry;
   void* p; 
   size_t size = reqsize ? reqsize : 1; // in case reqsize is 0
   size += sizeof(Blockhead);

   // Request the block from Linux
   p = malloc(size);

   blockh = (struct Blockhead*)p;

   if(checking == true)
   {
      // Add a new entry to the list and fill up the data
      dlentry = Add();
      dlentry->address = blockh + 1;
      dlentry->size = reqsize;

      // Fill in the backtrace info
#ifndef	NO_BACKTRACE
      backtrace(dlentry->ret_addr, BACKTRACEDEPTH);
#endif	/* NO_BACKTRACE */
    
      // set the blockhead with the heapblock info
      blockh->dlentry = dlentry;
    }
    else
    {
      // not checking yet, so set dlendtry to 0
      blockh->dlentry = 0;
    }

   // skip the pointer over the header for the user's data
   return(blockh + 1);
      
}

void my_free(void* ptr)
{
   struct Blockhead* blockh;
   struct DLEntry* entry;

   if(ptr == 0)
      return;

   blockh = (struct Blockhead*)((char*)ptr - sizeof(Blockhead));

   if(blockh->dlentry)
   {
      // remove the heapblock from the linked list
      entry = blockh->dlentry;
      Remove(entry);
      blockh->dlentry = 0; // in case of double delete
   }

   // call free with the real ptr
   free((void*)blockh);

}

// End of Opera development changes.


void reportMemoryBallance()
{
  int balance = allocatedBlocks - initialBlocksUsed;
  if (balance == 0)
    printf("No leaks detected\n");
  else if (balance > 0)
    printf("Memory leak! %d blocks not deleted\n", balance); 
  else
    printf("More memory deleted than newed! %d extra deletes\n", -balance); 

}


void MemoryLeakWarning::Enable()
{
  initialBlocksUsed = allocatedBlocks;
  atexit(reportMemoryBallance);
}

void MemoryLeakWarning::Report()
{
  if (initialBlocksUsed != allocatedBlocks)
  {
    printf("Memory new/delete imbalance after running tests\n");
  }
}

static int blockUsageCheckPoint = 0;

void MemoryLeakWarning::CheckPointUsage()
{
  blockUsageCheckPoint = allocatedBlocks;
  checking = true;
}

void MemoryLeakWarning::TolerateLeakInThisTest()
{
  if (enableTolerate)
    tolerate = true;
}

void MemoryLeakWarning::EnableTolerateSpecificLeaks()
{
  enableTolerate = true;
}

#define MIN(a,b) (a <= b ? a : b)
bool MemoryLeakWarning::UsageIsNotBalanced()
{
	if (tolerate)
	  return false;

    // Changes for Opera Development:
    // Check the list and dump leaked block details,
    // clearing the list as we go
    DLEntry* dlentry = First();
    int total_bytes = 0;

    while(dlentry != 0)
    {
        total_bytes += dlentry->size;
#ifndef	NO_BACKTRACE
        printf("\nBlock size: %d, Address: %p, Back Trace:\n", dlentry->size, dlentry->address);
        char** trace_text = backtrace_symbols(dlentry->ret_addr, BACKTRACEDEPTH); 
#endif	/* NO_BACKTRACE */
        for(int i = 0; i < BACKTRACEDEPTH; i++)
        {
            if(i >= BACKTRACESKIP)
            {
				char* libraryNameEnd;
				char* functionNamePtr;
				char* programCounterPtr;
#ifndef	NO_BACKTRACE
				if(trace_text[i] != NULL)
				{
					// Find the start of the mangled function name
					libraryNameEnd = functionNamePtr = strstr(trace_text[i], "(");
					programCounterPtr = strstr(trace_text[i], ")");
					programCounterPtr++;
					if((functionNamePtr == NULL) || (functionNamePtr[0] == '\0'))
						continue;
					functionNamePtr++;
					// Find the end of the mangled function name
					int index1 = strcspn(functionNamePtr, "+");
					int index2 = strcspn(functionNamePtr, ")");
					// Take a copy of the name
					char* namebuffer = (char*)malloc(MIN(index1, index2)+1);
					strncpy(namebuffer, functionNamePtr, MIN(index1, index2));
					namebuffer[MIN(index1, index2)] = '\0';
					int status;
					// Demangle the name
					char* realname = abi::__cxa_demangle(namebuffer, 0, 0, &status);

					if(status != 0)
					{
						// Could'nt demangle a name
						printf ("%d. %s\n", i, trace_text[i]);
					}
					else
					{
						// We could demangle the name
						*libraryNameEnd = '\0';
						printf ("%d. %s(%s)%s\n", i, trace_text[i], realname, programCounterPtr);
					}
					free(namebuffer);
					free(realname);
				}
#endif	/* NO_BACKTRACE */
            }
        }
        printf("\n");
#ifndef	NO_BACKTRACE
        free(trace_text);
#endif	/* NO_BACKTRACE */
        Remove(dlentry);
        dlentry = First();

    }

    if (blockUsageCheckPoint != allocatedBlocks)
        sprintf(message, "this test leaks %d bytes in %d blocks",
            total_bytes, allocatedBlocks - blockUsageCheckPoint);

    checking = false;

	return blockUsageCheckPoint != allocatedBlocks;
}

const char* MemoryLeakWarning::Message()
{
    return message;
}

// These functions should be called from the user's code:

void* operator new(size_t size) 
  {
   void* ptr = 0;
   MEMSEM->take();
   allocatedBlocks++; 
   ptr = my_malloc(size);
   MEMSEM->give();
   return ptr;
  }

void operator delete(void* mem) 
{ 
  if(mem == NULL)
    return;
  MEMSEM->take();
  allocatedBlocks--; 
  my_free(mem);
  MEMSEM->give();
}

void* operator new[](size_t size) 
{
  void* ptr = 0;
  MEMSEM->take();
  allocatedBlocks++;
  ptr = my_malloc(size);
  MEMSEM->give();
  return ptr;
}

void operator delete[](void* mem) 
{
  if(mem == NULL)
    return;
  MEMSEM->take();
  allocatedBlocks--;
  my_free(mem);
  MEMSEM->give();
}

