//
//
// C++ Interface for module: Allocators
//
// Description: 
//
//
// Author: exa
//
//

#ifndef Allocators_Interface
#define Allocators_Interface

#include "General.hxx"
#include "Graph_Utility.hxx"


//  namespace Graph_Lib {

//    using namespace Utility;

//    namespace Allocators {

    // Allocator classes are simple minded, they do not care
    // about free memory much. The whole point is in efficiency.

    static const int big_workspace_size = (1 << 26);
    static const int small_workspace_size = (1 << 22);

    class Allocation_Error : public Exception {
    public:
      Allocation_Error(string msg)
	: Exception("Allocation Error: " + msg) {}
    };

    // prior to using the class one must initialize class, only once
    // initialization must occur after entry to main()
    // adapted from malloc_alloc of SGI STL implementation
    template <int Workspace_Size, int instance> class Generic_Allocator {
    public:
      
      static void * allocate(size_t n) {
	if (available < n)
	  throw Allocation_Error("Graph allocator chunk depleted");
	  //return malloc_alloc::allocate(n) // gotta track this then.. :(;
	else {
	  void *result = free;
	  free += n;
	  available -= n;
	  return result;
	}
      }

      static void deallocate(void *p, size_t /* n */) {
	// code: mark free list
      }

      static void * reallocate(void *p, size_t old_sz, size_t new_sz) {
	//code: check free list
	void *result = allocate(new_sz);
	memcpy(result, p, old_sz);
	return result;
      }

      static void (* set_malloc_handler(void (*f)()))() {
	void (* old)() = oom_handler;
	oom_handler = f;
	return(old);
      }

      static void init_class();

    private:
      static char *chunk;	// the chunk to allocate
      static char *free;	// beginning of free memory
      static unsigned int available;	// amount of available memory
      static bool class_initialized; // successful initialization?
      
      struct Block{
	char* address;
	char* size;
      };
      list<Block> free_list;

      static void *oom_malloc(size_t);
      static void *oom_realloc(void *, size_t);
      static void (* oom_handler)();
    };

    template <int Workspace_Size, int i>
      char* Generic_Allocator<Workspace_Size,i>::chunk  = 0;

    template <int Workspace_Size, int i>
      void Generic_Allocator<Workspace_Size,i>::init_class() {
      if (class_initialized)
	throw Allocation_Error("Allocator must be initialized only once.");
      void * result = malloc (Workspace_Size);
      if (result==0) {
	throw Allocation_Error("Graph Allocator initialization failed.");
      }
      chunk = free = (char *)result;
      available = Workspace_Size;
      class_initialized = true;
      L("Generic_Allocator<0x%x,0x%x> initialized\n", Workspace_Size, i);
    }
    
    template <int Workspace_Size, int i>
      char* Generic_Allocator<Workspace_Size,i>::free = 0;

    template <int Workspace_Size, int i>
      unsigned int Generic_Allocator<Workspace_Size,i>::available = 0;

    template <int Workspace_Size, int i>
      bool Generic_Allocator<Workspace_Size,i>::class_initialized = false;

    typedef Generic_Allocator<big_workspace_size,0> Hullavi;
    typedef Generic_Allocator<small_workspace_size,0> Gullavi;
  
//    }

//  } // namespace


#endif
