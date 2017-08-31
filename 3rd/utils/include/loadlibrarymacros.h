#ifndef _LAD_LIBRARY_MACROS_18_01_2010_
#define _LAD_LIBRARY_MACROS_18_01_2010_


#ifdef WINDOWS

#define START_MODULE_DECLARATION(name_space, class_name)\
class class_name;\
extern "C" {\
 __declspec(dllexport) class_name * name_space##_##class_name##_maker_t();\
};\
 \
namespace name_space \
{


#define END_MODULE_DECLARATION()\
};

#define START_MODULE(name_space, class_name)\
namespace name_space\
{\
  extern "C" {\
    __declspec(dllexport) class_name * name_space##_##class_name##_maker_t()\
    {\
      return new name_space::class_name();\
    }\
  }\


#define END_MODULE()\
};

#else  //NOT FOR MSVS

#define START_MODULE_DECLARATION(name_space, class_name)\
class class_name;\
extern "C" {\
class_name * name_space##_##class_name##_maker_t();\
};\
 \
namespace name_space \
{


#define END_MODULE_DECLARATION()\
};

#define START_MODULE(name_space, class_name)\
namespace name_space\
{\
  extern "C" {\
    class_name * name_space##_##class_name##_maker_t()\
    {\
      return new name_space::class_name();\
    }\
  }\


#define END_MODULE()\
};

#endif //WINDOWS


#define GET_MODULE_FUNCTION_MAKER_NAME(name_space, class_name) name_space##_##class_name##_maker_t

#endif //_LAD_LIBRARY_MACROS_18_01_2010_
