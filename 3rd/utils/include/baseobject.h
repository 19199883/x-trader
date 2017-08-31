

#ifndef _ENGINE_BASE_OBJECT_15_01_2010_
#define _ENGINE_BASE_OBJECT_15_01_2010_


class baseobject
{
  public:
    baseobject();
    virtual ~baseobject();
};

typedef baseobject* (*maker_t)();

#endif /* _ENGINE_BASE_OBJECT_15_01_2010_ */
