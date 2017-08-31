#ifndef _MODULE_LOAD_LIBRARY_15_01_2010_
#define _MODULE_LOAD_LIBRARY_15_01_2010_

#include <string>

class CModuleLoadLibrary
{
  public:
   CModuleLoadLibrary();
   virtual ~CModuleLoadLibrary();

   virtual void *Open(std::string path) = 0;
   virtual int Close(void *handle) = 0;
   
   virtual void* Load(void *handle, std::string symbol) = 0;
   virtual std::string getErrorDescription() = 0;
   
  protected:
    std::string  librarypath;   
};

#endif /* _MODULE_LOAD_LIBRARY_15_01_2010_ */
