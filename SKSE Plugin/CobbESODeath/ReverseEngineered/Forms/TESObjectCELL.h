#pragma once

#include "ReverseEngineered/Forms/TESForm.h"

namespace RE {
   struct MapMarkerOperation;
   class TESObjectREFR; // Forward-declare instead of #including, so the compiler doesn't choke on a circular dependency
   class TESWorldSpace;
   class TESObjectCELL : public TESForm {
      public:
         operator ::TESObjectCELL*() const { return (::TESObjectCELL*) this; }
         enum { kTypeID = kFormType_Cell };
         //
         // Parents:
         TESFullName					fullName;	// 14
         //
         // Members:
         struct UnknownData {
            UInt32 unk00;
            UInt32 unk04;
         };

         struct TVDT { // sizeof == 0x60; constructor for interiors at 0x004AD710
            struct TVDT0 {
               struct TVDT1 {
                  UInt32* unk00; // array?
                  UInt32* unk04; // array?
                  UInt32  unk08; // size?
               };
               TVDT1* unk00;
               UInt32 unk04;
               UInt32 unk08; // skip TVDT1 if > 1
            };
            struct Unk28 { // sizeof == 0x20; constructor at 0x0045FCA0
               UInt32 unk00 = 0x00FFFFFF; // are these six values directional ambient lighting colors, defaulting to white?
               UInt32 unk04 = 0x00FFFFFF;
               UInt32 unk08 = 0x00FFFFFF;
               UInt32 unk0C = 0x00FFFFFF;
               UInt32 unk10 = 0x00FFFFFF;
               UInt32 unk14 = 0x00FFFFFF;
               UInt32 unk18 = 0;
               float  unk1C = 1.0;
            };
            //
            union {               // 00
               SInt32 cellX;         // exterior cells: cell-X-coordinate // TESObjectCELL has an exterior-specific getter: 0x004C0450
            };
            union {               // 04
               SInt32 cellY;         // exterior cells: cell-Y-coordinate // TESObjectCELL has an exterior-specific getter: 0x004C0470
            };
            union {               // 08
               void*  unk08;         // exterior cells: ???
               UInt32 fogColorNear;  // interior cells: near fog color; same color format as a BGSColorForm, but it isn't one
            };
            union {               // 0C
               TVDT0* unk0C;         // exterior cells: struct
               float fogPlaneNear;   // interior cells: near fog plane
            };
            union {               // 10
               float x;              // exterior cells: unit-X-coordinate // TESObjectCELL has an exterior-specific getter: 0x004C04D0
               float fogPlaneFar;    // interior cells: far fog plane
            };
            union {               // 14
               float y;              // exterior cells: unit-Y-coordinate // TESObjectCELL has an exterior-specific getter: 0x004C04F0
               UInt32 unk14;         // interior cells: unknown
            };
            union {               // 18
               UInt32 unk18;         // exterior cells: unknown
               float fogPower;       // interior cells: fog power
            };
            float   unk1C = 1.0;  // 1C
            UInt32  unk20;        // 20
            union {               // 24
               float unk24 = 1.0;    // TESObjectCELL has an exterior-specific getter: 0x004C0430 // getter also requires that the cell use sky lighting
            };
            Unk28   unk28;        // 28 // sizeof == 0x20
            union {               // 48
               UInt32 unk48;         // TESObjectCELL has an exterior-specific setter: 0x004C03D0 // setter also requires that the cell use sky lighting
            };
            float   unk4C = 1.0;  // 4C
            UInt32  unk50;
            UInt32  unk54;
            UInt32  unk58 = 0x79F;// 58
            UInt32  unk5C;
         };

         enum CellFlags : UInt16 {
            kCellFlag_IsInterior     = 0x0001,
            kCellFlag_Unknown0002    = 0x0002, // indicates relationship between land and the water level
            kCellFlag_Unknown0010    = 0x0010,
            kCellFlag_IsPublic       = 0x0020,
            kCellFlag_UseSkyLighting = 0x0100, // meaning inferred from documentation for Papyrus fog setters
         };

         UnknownData    unk1C;			// 1C
         UnknownData    unk24;			// 24
         CellFlags      unk2C;			// 2C // cell flags.
         UInt16         unk2E;			// 2E
         UInt8          unk30;			// 30 // loaded state, or whatever you want to call it. 0x07 means the cell is attached.
         UInt8          unk31;			// 31
         UInt8	        unk32;			// 32
         UInt8          pad33;			// 33
         BSExtraData*   extraData;	    // 34 - ExtraDataList
         NiNode*        unk38;		    // 38 // NiNode. Is its name set to "Shared Portal Geometry" at run-time?
         TVDT*          unk3C;		    // 3C // NOTE: This data's values have different meanings for interior and exterior cells.
         TESObjectLAND* unk40;		    // 40
         float          waterLevel;     // 44
         void*          unk48;		    // 48
         tArray<RE::TESObjectREFR*> objectList; // 4C
         UnkArray       unk58;			// 58
         UnkArray       unk64;			// 64
         UnkArray       unk70;			// 70
         SimpleLock     cellRefLock;    // 7C
         TESWorldSpace* parentWorld;	// 84
         void*          unk88;			// 88 // investigate subroutine 0x004C20A0 to learn more about this
         BGSLightingTemplate* lightingTemplate;	// 8C
         //
         MEMBER_FN_PREFIX(TESObjectCELL);
         //
         DEFINE_MEMBER_FN(CellRefLockEnter,  void, 0x004C0180); // Call this before modifying the cell's object list.
         DEFINE_MEMBER_FN(CellRefLockExit,   void, 0x004C0190); // Call this after  modifying the cell's object list.
         DEFINE_MEMBER_FN(AddRefToList,      void, 0x004D3EA0, RE::TESObjectREFR* reference, UInt32 unknown); // Second argument is zero. You need to call CellRefLockEnter before, and CellRefLockExit after.
         DEFINE_MEMBER_FN(RemoveRefFromList, void, 0x004CB7B0, RE::TESObjectREFR* reference); // Calls CellRefLockEnter and CellRefLockExit for you.
         //
         DEFINE_MEMBER_FN(GetOwner,            void*, 0x004C4DC0); // Returns TESNPC* or TESFaction*.
         DEFINE_MEMBER_FN(GetMapMarkerHandles, void,  0x004C8230, MapMarkerOperation* out);
         DEFINE_MEMBER_FN(GetRegionList,       TESRegionList*, 0x004C23E0, bool createIfMissing);
         //
         DEFINE_MEMBER_FN(GetExterior3C,   void*, 0x004C0390); // Returns unk3C for exterior cells only.
         DEFINE_MEMBER_FN(GetInterior3C,   void*, 0x004C03A0); // Returns unk3C for interior cells only.
         //
         DEFINE_MEMBER_FN(SetIsInterior,   void,  0x004C0240, bool flag); // Sets or modifies flag 0x0001 in unk2C. WARNING: interior and exterior cells store data differently, and this function DOES NOT CONVERT EXISTING DATA. It just changes the flag.
         DEFINE_MEMBER_FN(SetPublic,       void,  0x004C0260, bool flag); // Sets or modifies flag 0x0020 in unk2C.
         //
         DEFINE_MEMBER_FN(SetFogColor,     void,  0x004C03B0, UInt32 color); // Sets the fog color (as 0x00BBGGRR) for interior cells. (Exterior cells don't use that value.)
         DEFINE_MEMBER_FN(SetNearFogPlane, void,  0x004C03F0, float); // Sets the near fog plane for interior cells. (Exterior cells don't use that value.)
         DEFINE_MEMBER_FN(SetFarFogPlane,  void,  0x004C0410, float); // Sets the far  fog plane for interior cells. (Exterior cells don't use that value.)
         //
         DEFINE_MEMBER_FN(GetCellCoordinates, void,  0x004C0490, SInt32*, SInt32*); // Gets the cell-coordinates of an exterior cell. (Interior cells don't use these values.)
         DEFINE_MEMBER_FN(SetCellCoordinates, void,  0x004C0550, SInt32,  SInt32);  // Sets the cell-coordinates of an exterior cell. (Interior cells don't use these values.)
         DEFINE_MEMBER_FN(GetUnitCoordinates, void,  0x004C0510, float*,  float*);  // Gets the unit-coordinates of an exterior cell. (Interior cells don't use these values.)
         DEFINE_MEMBER_FN(GetWaterLevel,      float, 0x004C0710); // Returns water level, checking the parent world if needed; returns FLOAT_MAX if no water level. (Interior cells don't have a water level.)
         //
         DEFINE_MEMBER_FN(Subroutine004C05A0, UInt32, 0x004C05A0); // Interiors: returns (this->unk88 + 0x54). Exteriors: returns (this->parentWorld->unkBC).
         DEFINE_MEMBER_FN(Subroutine004C05C0, UInt32, 0x004C05C0); // Interiors: returns (this->unk88 + 0x64). Exteriors: returns (this->parentWorld->unkC0).
         DEFINE_MEMBER_FN(Subroutine004C0650, void,   0x004C0650, UInt32, UInt32); // Func(-1, 0) queues the cell to reset.
         DEFINE_MEMBER_FN(Subroutine004C1820, void, 0x004C1820);
         DEFINE_MEMBER_FN(Subroutine004C2470, void, 0x004C2470); // loops over all objects in cell; performs unknown operation on 3D; unloads 3D
         DEFINE_MEMBER_FN(Subroutine004C25D0, void, 0x004C25D0); // loops over all objects in cell; performs unknown operation on any that aren't: disabled; deleted; or doors that are marked as destroyed
         DEFINE_MEMBER_FN(Subroutine004C2660, void, 0x004C2660, bool); // loops over all objects in cell; performs one of two unknown operations on them based on bool
         DEFINE_MEMBER_FN(Subroutine004C26F0, void, 0x004C26F0, float, float, float, float, UInt32); // loops over all objects in cell; performs unknown operation them involving mathematical checks on their positions
         DEFINE_MEMBER_FN(Subroutine004C28F0, void, 0x004C28F0); // loops over all objects in cell; performs unknown operation them
         DEFINE_MEMBER_FN(Subroutine004C29A0, void, 0x004C29A0); // loops over all objects in cell; performs unknown operation them
         DEFINE_MEMBER_FN(Subroutine004C2B00, char*, 0x004C2B00); // returns the cell's name(?), or "Wilderness" if it's a nameless exterior cell
         DEFINE_MEMBER_FN(Subroutine004C31B0, void, 0x004C31B0, UInt32, UInt32, UInt32); // something to do with whether the cell's been seen; nearby subroutines (scroll up) also are related
         DEFINE_MEMBER_FN(Subroutine004C32C0, void, 0x004C32C0); // loops over all objects in cell; performs unknown operation them
   };
   static_assert(offsetof(TESObjectCELL, parentWorld) == 0x84, "Data layout incorrect for RE::TESObjectCELL: bad parentWorld."); // IntelliSense chokes on this; ignore the squiggly red lines.
};