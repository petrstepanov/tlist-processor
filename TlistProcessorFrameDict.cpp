// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME TlistProcessorFrameDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "AppSettings.h"
#include "Constants.h"
#include "FittingFunctions.h"
#include "Geometry.h"
#include "GraphicsHelper.h"
#include "HistProcessor.h"
#include "StringUtils.h"
#include "TlistProcessorFrame.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *TlistProcessorFrame_Dictionary();
   static void TlistProcessorFrame_TClassManip(TClass*);
   static void delete_TlistProcessorFrame(void *p);
   static void deleteArray_TlistProcessorFrame(void *p);
   static void destruct_TlistProcessorFrame(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TlistProcessorFrame*)
   {
      ::TlistProcessorFrame *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TlistProcessorFrame));
      static ::ROOT::TGenericClassInfo 
         instance("TlistProcessorFrame", "TlistProcessorFrame.h", 39,
                  typeid(::TlistProcessorFrame), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TlistProcessorFrame_Dictionary, isa_proxy, 0,
                  sizeof(::TlistProcessorFrame) );
      instance.SetDelete(&delete_TlistProcessorFrame);
      instance.SetDeleteArray(&deleteArray_TlistProcessorFrame);
      instance.SetDestructor(&destruct_TlistProcessorFrame);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TlistProcessorFrame*)
   {
      return GenerateInitInstanceLocal((::TlistProcessorFrame*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::TlistProcessorFrame*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TlistProcessorFrame_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TlistProcessorFrame*)0x0)->GetClass();
      TlistProcessorFrame_TClassManip(theClass);
   return theClass;
   }

   static void TlistProcessorFrame_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_TlistProcessorFrame(void *p) {
      delete ((::TlistProcessorFrame*)p);
   }
   static void deleteArray_TlistProcessorFrame(void *p) {
      delete [] ((::TlistProcessorFrame*)p);
   }
   static void destruct_TlistProcessorFrame(void *p) {
      typedef ::TlistProcessorFrame current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TlistProcessorFrame

namespace {
  void TriggerDictionaryInitialization_TlistProcessorFrameDict_Impl() {
    static const char* headers[] = {
"AppSettings.h",
"Constants.h",
"FittingFunctions.h",
"Geometry.h",
"GraphicsHelper.h",
"HistProcessor.h",
"StringUtils.h",
"TlistProcessorFrame.h",
0
    };
    static const char* includePaths[] = {
"/Applications/root_v6.06.02/include",
"/Applications/root_v6.06.02/include",
"/Users/petrstepanov/Development/NetBeansProjects/tlist-processor/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "TlistProcessorFrameDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$TlistProcessorFrame.h")))  TlistProcessorFrame;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "TlistProcessorFrameDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "AppSettings.h"
#include "Constants.h"
#include "FittingFunctions.h"
#include "Geometry.h"
#include "GraphicsHelper.h"
#include "HistProcessor.h"
#include "StringUtils.h"
#include "TlistProcessorFrame.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"TlistProcessorFrame", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("TlistProcessorFrameDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_TlistProcessorFrameDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_TlistProcessorFrameDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_TlistProcessorFrameDict() {
  TriggerDictionaryInitialization_TlistProcessorFrameDict_Impl();
}
