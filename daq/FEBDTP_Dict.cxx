// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME FEBDTP_Dict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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
#include "FEBDTP.hxx"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_FEBDTP(void *p = 0);
   static void *newArray_FEBDTP(Long_t size, void *p);
   static void delete_FEBDTP(void *p);
   static void deleteArray_FEBDTP(void *p);
   static void destruct_FEBDTP(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::FEBDTP*)
   {
      ::FEBDTP *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::FEBDTP >(0);
      static ::ROOT::TGenericClassInfo 
         instance("FEBDTP", ::FEBDTP::Class_Version(), "FEBDTP.hxx", 83,
                  typeid(::FEBDTP), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::FEBDTP::Dictionary, isa_proxy, 4,
                  sizeof(::FEBDTP) );
      instance.SetNew(&new_FEBDTP);
      instance.SetNewArray(&newArray_FEBDTP);
      instance.SetDelete(&delete_FEBDTP);
      instance.SetDeleteArray(&deleteArray_FEBDTP);
      instance.SetDestructor(&destruct_FEBDTP);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::FEBDTP*)
   {
      return GenerateInitInstanceLocal((::FEBDTP*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::FEBDTP*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr FEBDTP::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *FEBDTP::Class_Name()
{
   return "FEBDTP";
}

//______________________________________________________________________________
const char *FEBDTP::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::FEBDTP*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int FEBDTP::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::FEBDTP*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *FEBDTP::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::FEBDTP*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *FEBDTP::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::FEBDTP*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void FEBDTP::Streamer(TBuffer &R__b)
{
   // Stream an object of class FEBDTP.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(FEBDTP::Class(),this);
   } else {
      R__b.WriteClassBuffer(FEBDTP::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_FEBDTP(void *p) {
      return  p ? new(p) ::FEBDTP : new ::FEBDTP;
   }
   static void *newArray_FEBDTP(Long_t nElements, void *p) {
      return p ? new(p) ::FEBDTP[nElements] : new ::FEBDTP[nElements];
   }
   // Wrapper around operator delete
   static void delete_FEBDTP(void *p) {
      delete ((::FEBDTP*)p);
   }
   static void deleteArray_FEBDTP(void *p) {
      delete [] ((::FEBDTP*)p);
   }
   static void destruct_FEBDTP(void *p) {
      typedef ::FEBDTP current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::FEBDTP

namespace {
  void TriggerDictionaryInitialization_FEBDTP_Dict_Impl() {
    static const char* headers[] = {
"FEBDTP.hxx",
0
    };
    static const char* includePaths[] = {
"/home/yuri/Desktop/root_v6.14.02/root/include",
"/home/yuri/Desktop/FEBDAQMULT_1.1/DAQ/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "FEBDTP_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(FEBDTP)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(FEBDTP)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(FEBDTP)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(FEBDTP)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$FEBDTP.hxx")))  FEBDTP;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "FEBDTP_Dict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "FEBDTP.hxx"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"FEBDTP", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("FEBDTP_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_FEBDTP_Dict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_FEBDTP_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_FEBDTP_Dict() {
  TriggerDictionaryInitialization_FEBDTP_Dict_Impl();
}
