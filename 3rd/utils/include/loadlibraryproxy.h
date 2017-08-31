
#ifndef _LOAD_LIBRARY_PROXY_15_01_2010_
#define _LOAD_LIBRARY_PROXY_15_01_2010_

#include "moduleloadlibrary.h"
#include <string>
#include <list>
#include <map>

class baseobject;

typedef struct _tag_ObjectInfo
{
  std::string   name;
  void         *handle;
}ObjectInfo;

class CLoadLibraryProxy
{
protected:
   static CLoadLibraryProxy* pLoadLibraryProxy;
   static unsigned long CLoadLibraryProxyCounter;

  public:
    CLoadLibraryProxy();
    virtual ~CLoadLibraryProxy();
    
	  static CLoadLibraryProxy* CreateLoadLibraryProxy();

    virtual unsigned long DeleteLoadLibraryProxy();
    static bool IsCreatedLoadLibraryProxy();


    void setModuleLoadLibrary( CModuleLoadLibrary * moduleloader);
    CModuleLoadLibrary *getModuleLoadLibrary();


    void setBasePathLibrary(std:: string basepathlibrary);
    std::string getBasePathLibrary();

    
    void *findObject(std::string sobjectname, std::string methodname);
    void deleteObject(std::string sobjectname);

    std::string getexepath();
    std::string getexedir();

    const char* get_directory( char* path );

    const char getPathSeparator();
    const char getNameSpaceSeparator();

protected:    

  protected:
   CModuleLoadLibrary          *pmoduleloader;
   std::string                  basepath;
   std::map<std::string, ObjectInfo>        dl_list;



};

#endif /* _LOAD_LIBRARY_PROXY_15_01_2010_ */


