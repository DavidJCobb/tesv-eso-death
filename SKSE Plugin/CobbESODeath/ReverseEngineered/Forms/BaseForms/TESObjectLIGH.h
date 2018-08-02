#pragma once

#include "ReverseEngineered/Forms/BaseForms/base.h"

namespace RE {
   class TESObjectLIGH : public TESBoundAnimObject {
      public:
         enum { kTypeID = kFormType_Light };
         enum LightType : UInt32 {
            kLightType_Omni       = 0x0000,
            kLightType_OmniShadow = 0x1000,
            kLightType_OmniHemi   = 0x0800,
            kLightType_SpotShadow = 0x0400,
            kLightFlag_Carryable  = 0x0002,
         };
         //
         // Parents:
         TESFullName                fullName;      // 20
         TESModelTextureSwap        texSwap;       // 28
         TESIcon                    icon;          // 44
         BGSMessageIcon             messageIcon;   // 4C
         TESWeightForm              weight;        // 58
         TESValueForm               value;         // 60
         BGSDestructibleObjectForm  destructible;  // 68
         BGSEquipType               equipType;     // 70
         //
         // Members:
         struct Data78 { // sizeof() == 0x28 // light data, probably
            struct Color {
               UInt8 pad03; // 03
               UInt8 blue;  // 02
               UInt8 green; // 01
               UInt8 red;   // 00
            };
            UInt32	unk00;	// 00
            UInt32	radius;	// 04
            Color   color;  // 08
            LightType type;	// 0C // This is a bitmask. AND with 0x1C00 and then compare to the enum above to see the light type.
            float 	unk10;	// falloff exponent or near clip
            float	baseFOV;// 14
            float	unk18;	// falloff exponent or near clip
            UInt32	unk1C;	// 1C
            UInt32	unk20;	// 20
            UInt32	unk24;	// 24
         };
         struct DataA8 {
            UInt32	unk0;	// 0
            UInt32	unk4;	// 4
            UInt32	unk8;	// 8
         };
         //
         Data78   unk78;   // 78
         float    fade;    // A0 // are we sure this isn't part of the previous member? minor quibble, though
         UInt32   unkA4;   // A4
         DataA8   unkA8;   // A8
   };
   static_assert(sizeof(TESObjectLIGH) == 0xB4, "RE::TESObjectLIGH is not the right size.");
}