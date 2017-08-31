
#ifndef _MODULE_LOAD_LIBRARY_LINUX_15_01_2010_
#define _MODULE_LOAD_LIBRARY_LINUX_15_01_2010_

#include "moduleloadlibrary.h"

class CModuleLoadLibraryLinux : public CModuleLoadLibrary
{
  public:
    CModuleLoadLibraryLinux();
    virtual ~CModuleLoadLibraryLinux();

   virtual void* Open(std::string path);
   virtual int Close(void *handle);
   
   virtual void* Load(void *handle, std::string symbol);

   std::string getErrorDescription();

};

#endif /* _MODULE_LOAD_LIBRARY_LINUX_15_01_2010_ */
