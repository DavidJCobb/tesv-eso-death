#pragma once

#include "ReverseEngineered/Forms/TESForm.h"

namespace RE {
   class TESObjectCELL; // Forward-declare instead of #including, so the compiler doesn't choke on a circular dependency
   class TESWorldSpace : public TESForm {
      public:
         operator ::TESWorldSpace*() const { return (::TESWorldSpace*) this; }
         enum { kTypeID = kFormType_WorldSpace };
         //
         // Parents:
         TESFullName fullName; // 14
         TESTexture  texture;  // 1C
         //
         // Members:
         struct Data038 { // sizeof() == 4
            UInt16 unk0;
            UInt16 unk2;
         };
         struct Data03C { // sizeof() == 0x20
            UInt8 unk00[0x20];
         };
         struct OFFSET_DATA { }; // ?
         //
         struct Data070 { // sizeof == 0x20 // SKSE team thinks this is the same struct as TESQuest::Data05C, but I can't confirm that.
            UInt32 unk00;
            UInt32 unk04;
            UInt32 unk08;
            UInt32 unk0C;
            UInt32 unk10;
            tArray<RE::TESObjectREFR*> objectList;
         };
         //
         struct Data12C { // sizeof() == 0x40
            struct Entry { // SKSE: "This struct keeps showing up everywhere"
               UInt8    pad00[8]; // 00
               UInt32   unk08;    // 08
               void*    unk0C;    // 0C // Has a VTBL and at least 0xA virtual methods. Known non-virtual method: 0x004ABB30
               UInt32   unk10;    // 10
               void*    unk14;    // 14
               UInt8    pad18[4]; // 18
               UInt32   unk1C;    // 1C
            };
            Entry unk00;
            Entry unk20;
         };
         //
         typedef NiTPointerMap<int, TESObjectCELL*> CellMap;
         //
         CellMap*    cellMap;       // 024
         UInt32      unk028;        // 028
         UInt32      unk02C;        // 02C
         UInt32      unk030;        // 030
         UInt8       worldSpaceFlags; // 034
         UInt8       pad035;        // 035
         UInt16      unk036;        // 036
         Data038     unk038;        // 038
         Data03C     unk03C;        // 03C
         UnkArray    unk05C;        // 05C
         UInt32      unk068;        // 068
         UInt32      unk06C;        // 06C
         Data070     unk070;        // 070
         void*       unk090;        // 090 - refcounted ptr
         UInt32      unk094;        // 094
         UInt32      unk098;        // 098
         UInt32      unk09C;        // 09C
         UInt32      unk0A0;        // 0A0
         UInt32      unk0A4;        // 0A4
         UInt32      unk0A8;        // 0A8
         UInt32      unk0AC;        // 0AC
         UInt32      unk0B0;        // 0B0
         UInt32      unk0B4;        // 0B4
         UInt32      unk0B8;        // 0B8 // getter at 00962150
         UInt8       unk0BC[0x10];  // 0BC
         float       unk0CC;        // 0CC // actually, this might be a parent world
         UInt32      unk0D0;        // 0D0
         UInt32      unk0D4;        // 0D4
         UInt32      unk0D8;        // 0D8
         UInt32      unk0DC;        // 0DC
         UInt32      unk0E0;        // 0E0
         UInt32      unk0E4;        // 0E4
         UInt32      unk0E8;        // 0E8
         UInt32      unk0EC;        // 0EC
         NiTMap<TESFile*, TESWorldSpace::OFFSET_DATA*> unk0F0; // 0F0
         BSString    unk100;        // 100
         void*       unk108;        // 108
         UInt32      unk10C;        // 10C
         float       unk110;        // 110
         UInt32      unk114;        // 114
         UInt32      unk118;        // 118
         TESTexture  unk11C;        // 11C
         TESTexture  unk124;        // 124
         Data12C     unk12C;        // 12C
         UInt32      unk16C;        // 16C
         void*       unk170;        // 170
         //
         MEMBER_FN_PREFIX(TESWorldSpace);
         DEFINE_MEMBER_FN(GetUnk2CFlagFlag00000010,   bool,               0x004C0C50); 
         DEFINE_MEMBER_FN(GetCellAtMergedCoordinates, RE::TESObjectCELL*, 0x004375D0, UInt32); // Argument is: 0xXXXXYYYY.
         DEFINE_MEMBER_FN(GetCellThatContainsPoint,   RE::TESObjectCELL*, 0x004F3410, NiPoint3*); // Assumed, but it's a pretty good assumption to make
         DEFINE_MEMBER_FN(GetWaterLevel,              float,              0x004F0930); // There is also a similar subroutine at 0x004F0900, but I don't know what it returns.
         //
         DEFINE_MEMBER_FN(Subroutine004F5330, void*, 0x004F5330, UInt32, UInt32); // Arguments are cell coordinates.
   };
   static_assert(sizeof(TESWorldSpace) == 0x174, "RE::TESWorldSpace is not the right size.");
};