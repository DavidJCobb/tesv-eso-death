#pragma once

#include "ReverseEngineered/Forms/BaseForms/base.h"

namespace RE {
   class TESObjectACTI : public TESBoundAnimObject {
      public:
         enum { kTypeID = kFormType_Activator };

         static const UInt32 vtbl = 0x010841B4;
         //
         // Overridden virtual methods
         //
         // ===== TESObject =======================================================================
         virtual UInt32 Unk_40(void*, void*); // 40
         // ===== TESBoundObject ==================================================================
         virtual bool   Unk_4D(void* arg0, BSString* dst, void*); // 4D
         virtual bool   Unk_4E(void*, UInt8, UInt32, float); // 4E // always returns true

         //
         // Parents:
         TESFullName                fullName;      // 20
         TESModelTextureSwap        texSwap;       // 28
         BGSDestructibleObjectForm  destructible;  // 44
         BGSOpenCloseForm           openClose;     // 4C
         BGSKeywordForm             keyword;       // 50
         //
         // Members:
         UInt32 unk5C;     // 5C
         UInt32 unk60;     // 60
         UInt32 unk64;     // 64
         UInt16 unk68;     // 68
         UInt8  pad6A[2];  // 6A
         //
         MEMBER_FN_PREFIX(TESObjectACTI);
         DEFINE_MEMBER_FN(Constructor, TESObjectACTI*, 0x0049FEB0);
   };
};