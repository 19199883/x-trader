#ifndef _DLL_BASE_H_12_01_2010_
#define _DLL_BASE_H_12_01_2010_

#include "baseobject.h"

class dllbase : public baseobject
{
  public:
   dllbase();
   virtual ~dllbase();

 public:  
  virtual int start(int *pdata) = 0;
};

#endif /* _DLL_BASE_H_12_01_2010_ */
