#pragma once
#include <type_traits>

#include "ReverseEngineered/ExtraData.h"
#include "ReverseEngineered/Forms/TESForm.h"

#include "skse/GameObjects.h"
#include "skse/GameReferences.h"
#include "skse/NiTypes.h"
#include "skse/Utilities.h"

#define skyrim_re_clear_refr(v) if (v) v->handleRefObject.DecRefHandle(); v = nullptr;

class ExtraLock;
class ExtraMapMarker;
class MovementParameters;
class TESContainer;
namespace RE {
   class InventoryEntryData;
   class TESObjectCELL; // Forward-declare instead of #including, so the compiler doesn't choke on a circular dependency
   class TESWorldSpace;

   class IMovementInterface { // sizeof == 0x4
      public:
         virtual ~IMovementInterface();
         //
         // Confirmed not to have any virtual methods of its own. Odd.
         //
         //	void** _vtbl; // 00
   };
   class IMovementState : public IMovementInterface { // sizeof == 0x4
      public:
         virtual ~IMovementState();
         //
         // the below are all pure methods; meanings are derived from ActorState
         //
         virtual UInt32 Unk_01(); // 01 // returns the formID of whatever this IMovementState is a part of?
         virtual void   Unk_02(UInt32); // 02 // arg unknown
         virtual void   GetPosition(NiPoint3* out); // 03
         virtual void   GetRotation(NiPoint3* out); // 04
         virtual float  Unk_05(); // 05 // used by IsMoving; values >= 5.0F cause IsMoving to return 0
         virtual float  Unk_06(); // 06 // used by IsTurning; can be converted into an angle?
         virtual void   Unk_07(NiPoint3* out); // 07 // used by IsMoving; possibly a velocity getter?
         virtual bool   Unk_08(UInt32 unk); // 08 // messes with the string cache?
         virtual float  Unk_09(); // 09 // possibly related to height
         virtual float  Unk_0A(); // 0A // possibly related to height
         virtual float  Unk_0B(); // 0B // possibly related to height
         virtual float  GetComputedHeight(); // 0C
         virtual float  Unk_0D(); // 0D // returns computed height * 0.75
         virtual bool   Unk_0E(MovementParameters* p); // 0E // decrements the refcount on p, and then replaces it with a new MovementParameters*
         virtual void*  Unk_0F(); // 0F
         virtual UInt32 Unk_10(UInt32); // 10 // arg unknown // return value is int, but it can only be 0 or 1?
         virtual UInt32 Unk_11(); // 11 // return value unknown
         virtual bool   Unk_12(); // 12
         virtual bool   Unk_13(); // 13 // checks flags in the actor's race
   };

   class TESObjectREFR : public TESForm {
      public:
         operator ::TESObjectREFR*() const { return (::TESObjectREFR*) this; }
         enum { kTypeID = kFormType_Reference };
         //
         enum {
            kFlag_MarkedForDelete = 0x000020,
            kFlag_Disabled        = 0x000800,
            kFlag_Harvested       = 0x002000,
            kFlag_Unknown00004000 = 0x004000, // if this flag is set, Papyrus "Enable" aborts immediately, citing that the object is "temporary"
            kFlag_DisabledSKSE    = 0x010000, // not sure what this is (it's from SKSE), but I've confirmed that 0x800 is the REAL disabled flag
            kFlag_Destroyed       = 0x800000,
         };
         //
         virtual void	Unk_3B(void);
         virtual BGSLocation* GetEditorLocation(void); // 3C
         virtual bool	GetEditorCoordinates(NiPoint3* outPos, NiPoint3* outRot, void** outWorldOrCell, TESObjectCELL* veryRarelyUsedFallback); // 3D // This will fail if the reference has never been moved, because starting-position data isn't attached until it's moved for the first time.
         virtual void	Unk_3E(void);
         virtual void	Unk_3F(bool);
         virtual void	Unk_40();
         virtual void	Unk_41(void);
         virtual void	Unk_42(void);
         virtual bool	Unk_43(void); // Purpose unknown. Checks the reference's health and destructible state.
         virtual void	Unk_44(void);
         virtual void	Unk_45(void);
         virtual void	Unk_46(void);
         virtual void	Unk_47(void);
         virtual bool	HasKeyword(BGSKeyword*); // 48
         virtual void	Unk_49(void);
         virtual void*  Unk_4A(); // returns something
         virtual void	Unk_4B(UInt32);
         virtual void	Unk_4C(void);
         virtual void	Unk_4D(void);
         virtual void	Unk_4E(void);
         virtual void	Unk_4F();
         virtual void	Unk_50(void);
         virtual void	GetStartingPos(NiPoint3* pos);
         virtual void	Unk_52(void);
         virtual void	Unk_53(void);
         virtual void	Unk_54(NiPoint3*); // set the StartingPosition ExtraData?
         virtual void	Unk_55(void);
         virtual UInt32* Unk_56(UInt32* outHandle, TESForm* baseItem, bool, bool, BaseExtraList* item, bool, bool, bool); // modifies and returns outHandle
         virtual void	Unk_57(void);
         virtual void	Unk_58(void);
         virtual void	Unk_59(void);
         virtual void	Unk_5A(void);
         virtual void	GetMarkerPosition(NiPoint3* pos);
         virtual void*  Unk_5C(UInt32);
         virtual MagicTarget* GetSelfAsMagicTarget(); // 5D // Creates and attaches a NonActorMagicTarget for the reference if it is not an actor
         virtual UInt8  Unk_5E(); // probable bool
         virtual void	Unk_5F(void);
         virtual void	Unk_60(void);
         virtual BSFaceGenNiNode* GetFaceGenNiNode(void);
         virtual void	Unk_62(void);
         virtual BSFaceGenAnimationData* GetFaceGenAnimationData(void);
         virtual void	Unk_64(void);
         virtual void	Unk_65(void);
         virtual void	Unk_66(); // Related to setting up 3D, including primitives' collision. // ASM at 0x004E4FF0.
         virtual void	Unk_67();
         virtual void	Unk_68(void);
         virtual void	Unk_69(void);
         virtual void	Unk_6A(void);
         virtual void 	Unk_6B(void);
         virtual void	Unk_6C(UInt32, UInt32); // Unk_6C(0, 0) will instantly unload 3D.
         virtual void	SetNiNode(NiNode * root, UInt32 unk1); // NULL, 1?
         virtual void	Unk_6E(void);
         virtual NiNode* GetNiRootNode(bool firstPerson);
         virtual NiNode* GetNiNode(); // Root of the skeleton (Get3D)
         virtual bool   Unk_71();
         virtual void	Unk_72(void);
         virtual NiPoint3* GetMinBounds(NiPoint3* out);
         virtual NiPoint3* GetMaxBounds(NiPoint3* out);
         virtual void	Unk_75();
         virtual void	Unk_76(void);
         virtual void	Unk_77(void);
         virtual void	Unk_78(void);
         virtual void	Unk_79(void);
         virtual UInt8	Unk_7A(void);
         virtual UInt8	Unk_7B(void);
         virtual void	Unk_7C(void);
         virtual void	Unk_7D(void);
         virtual ActorWeightModel* GetWeightModel(UInt32 weightModel); // 0 Small 1 Large
         virtual void*	Unk_7F();
         virtual ActorWeightModel* Unk_80(); // seen in use by the code to fire arrows from a (cross)bow.
         virtual void	Unk_81(void);
         virtual void	Unk_82(void);
         virtual void	Unk_83(void);
         virtual void	Unk_84(TESBoundObject*);
         virtual void	Unk_85(void);
         virtual void	Unk_86(NiPoint3* out);
         virtual void	Unk_87(void);
         virtual void	Unk_88(void);
         virtual void	Unk_89(); // ASM at 0x004E1250. Prepares a reference to be moved, apparently. Calls Unk_6C(0, 0) in the process. // Does nothing to the player-actor.
         virtual void	Unk_8A(UInt32);
         virtual void*  Unk_8B();
         virtual void	Unk_8C(UInt32);
         virtual void	Unk_8D(); // most likely returns NiNode*; some code conditionally uses it instead of GetNiNode
         virtual void	Unk_8E(void);
         virtual bool	Unk_8F();
         virtual void	Unk_90(void);
         virtual void	Unk_91(void);
         virtual void	Unk_92(void);
         virtual void	Unk_93(void);
         virtual void	Unk_94(void);
         virtual void	Unk_95(void);
         virtual void	Unk_96(void);
         virtual TESObjectCELL* GetParentOrPersistentCell(); // 97 // used by 0x004D5EB0 (TESObjectREFR::GetParentWorldspace) when this->parentCell == NULL
         virtual void   Unk_98(TESObjectCELL*); // setter for parentCell?
         virtual bool	IsDead(UInt8 unk1); // 99
         virtual void	Unk_9A(void);
         virtual void*  Unk_9B(); // 9B // getter related to "AnimNoteReceiver" extra data
         //
         // Apparent new functions from here onward; these are also listed on Actor* and I may have more information there.
         //
         virtual bool	Unk_9C(void* arg1, UInt32 arg2, UInt32 arg3);  // 9C // new function?
         virtual bool	Unk_9D(UInt32 arg1, UInt32 arg2, UInt32 arg3); // 9D // new function? // no-op (returns false) for TESObjectREFR
         virtual void*  Unk_9E(); // 9E // new function? // no-op for TESObjectREFR // return type is TESAmmo*? pulls from the actor's process manager
         virtual void*	Unk_9F(); // 9F // new function? // getter related to "DecalGroup" extra data
         virtual void	Unk_A0(void*, void*, void*, NiPoint3*); // A0 // new function? // return value unknown; could be void // related to firing an arrow? see Actor
         virtual void	Unk_A1(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt32 arg4); // A1 // new function? // no-op for TESObjectREFR
         
         struct LoadedState {
            UInt32   unk00;	// 00
            float    unk04;	// 04
            UInt32   unk08;	// 08
            UInt32   unk0C;	// 0C
            float    unk10;	// 10 // related to cell water levels; see subroutine 004D9FE0 // can be FLT_MIN
            float    unk14;	// 14
            UInt32   unk18;	// 18 // bitmask
            void*    unk1C;	// 1C
            NiNode*  node;	// 20
            // ... probably more
            // if there's a 28, it would be a BGSAIWorldLocationPointRadius -- not a pointer
         };
         //
         // Parents:
         BSHandleRefObject handleRefObject;	// 14
         BSTEventSink<BSAnimationGraphEvent> animGraphEventSink;  // 1C
         IAnimationGraphManagerHolder        animGraphHolder;     // 20
         //
         // Members:
         TESForm* baseForm;         // 24
         NiPoint3 rot;              // 28
         NiPoint3 pos;              // 34
         TESObjectCELL* parentCell; // 40
         LoadedState*   loadedState;// 44
         ::BaseExtraList  extraData;// 48
         UInt32 referenceFlags;     // 50 // flags
         //
         bool   AttachScript(const char* scriptName);
         void   CreateDetectionEvent(Actor* owner, SInt32 soundLevel);
         UInt32 CreateRefHandle(void);
         std::vector<::TESObjectREFR*> GetActivateParents(::TESForm* baseFormFilter = NULL);
         UInt32 GetChangeFlags();
         SInt32 GetCurrentDestructionStage();
         TESObjectREFR* GetDestinationDoor(); // if the current reference is a load door
         bool   GetEditorCoordinateData(NiPoint3* pos, NiPoint3* rot, ::TESWorldSpace** worldspace, ::TESObjectCELL** cell);
         void   GetEditorCoordinateDataAlways(NiPoint3* pos, NiPoint3* rot, ::TESWorldSpace** worldspace, ::TESObjectCELL** cell); // Return editor coordinates if any, or current coordinates otherwise
         TESKey* GetKey();
         SInt32 GetLockLevel();
         float  GetMass();
         SInt8  GetMotionType();
         UInt32 GetOpenState();
         bool   HasActivateChild(TESObjectREFR*);
         bool   HasCollision();
         bool   IsActivationBlocked();
         bool   IsDestinationTeleportMarkerInAttachedCell();
         bool   IsTeleportMarkerInAttachedCell(::TESObjectREFR* destination = NULL); // pass destination to micro-optimize
         bool   IsValidLoadDoor(bool quick = false);
         void   MoveTo(UInt32* pTargetHandle, void* parentCell, void* worldSpace, NiPoint3* postion, NiPoint3* rotation); // MoveRefrToPosition wrapper with some additions
         bool   MoveToMyEditorLocation(bool native = false);
         void   Reload3D();
         void   SetMotionType(UInt32 motionType, bool, bool markChanged = true); // asynch; careful about this
         //
         MEMBER_FN_PREFIX(TESObjectREFR);
         DEFINE_MEMBER_FN(ClearDestruction,        void,             0x00449630);
         DEFINE_MEMBER_FN(GetBaseContainerData,    TESContainer*,    0x004D4A30); // returns &(this->baseForm.container) for NPCs and container references
         DEFINE_MEMBER_FN(GetBaseScale,            float,            0x004D5230)
         DEFINE_MEMBER_FN(GetDistance,             float,            0x004D7ED0, TESObjectREFR* other, bool evenIfDisabled, bool oftenFalse);
         DEFINE_MEMBER_FN(GetDistanceSquared,      float,            0x004D7ED0, TESObjectREFR* other, bool evenIfDisabled, bool oftenFalse);
         DEFINE_MEMBER_FN(GetHealth,               float,            0x004E9F90);
         DEFINE_MEMBER_FN(GetHealthPercent,        float,            0x004EA050);
         DEFINE_MEMBER_FN(GetLinkedRef,            TESObjectREFR*,   0x004EA4B0, BGSKeyword*);
         DEFINE_MEMBER_FN(GetWeight,               float,            0x004EA180);
         DEFINE_MEMBER_FN(GetReferenceName,        const char*,      0x004DE820);
         DEFINE_MEMBER_FN(GetRotationAsMatrix,     NiMatrix33*,      0x004D5300, NiMatrix33* out);
         DEFINE_MEMBER_FN(GetTriggerObjectCount,   UInt32,           0x004DFB90);
         DEFINE_MEMBER_FN(GetWorldspace,           TESWorldSpace*,   0x004D5EB0);
         DEFINE_MEMBER_FN(IsLocked,                bool,             0x004EB5B0); // if the reference is a teleport door, this checks the door on the other side, too
         DEFINE_MEMBER_FN(IsOffLimits,             bool,             0x004DA760);
         DEFINE_MEMBER_FN(MoveToMyEditorLocation,  bool,             0x004E6270, TESObjectCELL*, UInt32); // both arguments are zeroes?
         DEFINE_MEMBER_FN(SetDestroyed,            void,             0x00450E30, bool);
         DEFINE_MEMBER_FN(SetDisabled,             void,             0x00450FB0, bool); // sets enable state and marks changed
         DEFINE_MEMBER_FN(SetPitch,                void,             0x004DC9D0, float); // X // Does not honor actor-specific limitations like the race "immobile" flag. Does call Unk_54 for actors, though. Sets change-flag 2 (MOVE).
         DEFINE_MEMBER_FN(SetRoll,                 void,             0x004DCA60, float); // Y // Does not honor actor-specific limitations like the race "immobile" flag. Does call Unk_54 for actors, though. Sets change-flag 2 (MOVE).
         DEFINE_MEMBER_FN(SetYaw,                  void,             0x004DCAF0, float); // Z // Does not honor actor-specific limitations like the race "immobile" flag. Does call Unk_54 for actors, though. Sets change-flag 2 (MOVE).
         //
         // 4D9CF0 returns true if:
         //  - The reference is temporary (form ID starts with 0xFF)
         //  - The reference's base form is not SCOL, TREE, FLOR, FURN, or STAT
         //  - The reference's base form is STAT, and the base form's Unk_43() returns true
         //     - I don't fully understand Unk_43(), but I know it checks the reference's 
         //       health and its destructible state.
         // 
         // Used by SetPos, and by Papyrus MoveTo and MoveToMyEditorLocation: they all refuse to move 
         // references if this is false.
         //
         DEFINE_MEMBER_FN(Subroutine004D9CF0,   bool,             0x004D9CF0);
         DEFINE_MEMBER_FN(Subroutine00450F60,   void,             0x00450F60, UInt32);
         DEFINE_MEMBER_FN(Subroutine004D6140,   void,             0x004D6140, char*);      // <-- Sets non-persistent activate prompt? I've seen it used to set "Open" or "Close".
         DEFINE_MEMBER_FN(SetFlag00000002State, void,             0x00450ED0, bool);       // <-- Setter for flag 0x02, wrapped in TLS stuff.
         DEFINE_MEMBER_FN(SetPosition,          void,             0x004DCBA0, NiPoint3*);  // <-- Does something with worldspace data, too. DOES NOT UPDATE THE REFERENCE'S LOADED 3D.
         DEFINE_MEMBER_FN(SetRotation,          void,             0x004DC8F0, NiPoint3*);  // <-- Does something with loadedState, but I'm not sure if it updates the loaded 3D. Has special handling for characters.
         DEFINE_MEMBER_FN(StartsDead,           bool,             0x004D4E60);             // <-- returns true if the reference is an actor that starts dead
         DEFINE_MEMBER_FN(Subroutine004E07D0,   void,             0x004E07D0);             // <-- Called as part of the Reset3DState console command; seems to do the bulk of the work there.
         DEFINE_MEMBER_FN(Subroutine004E0E30,   void,             0x004E0E30);             // <-- Apparently enables a reference. If called after Unk_89, reloads 3D.
         DEFINE_MEMBER_FN(GetExtraLock,         ExtraLock*,       0x004EB5B0);             // <-- Gets the lock data on this reference. If this reference is a teleport door and has no lock, gets the lock on the other end (if any).
         DEFINE_MEMBER_FN(GetExtraMapMarker,    ExtraMapMarker*,  0x004EA3F0);
         DEFINE_MEMBER_FN(GetParentWorldspace,  TESWorldSpace*,   0x004D5EB0);             // <-- Return type confirmed. Purpose assumed based on usage in subroutine 0x0090CC60.
         DEFINE_MEMBER_FN(GetOrCreateRefHandle, UInt32*,          0x006BD6C0, UInt32* out);// <-- Modifies *out; returns out. Calls CreateRefHandleByREFR if the reference's refcount is non-zero.
         DEFINE_MEMBER_FN(Subroutine004DFE80,   void,             0x004DFE80, bool);       // <-- Unknown. Not related to un-/re-loading 3D, though. Usually takes false as an argument.
         //
         DEFINE_MEMBER_FN(GetExtraEnableStateChildren,      RE::ExtraEnableStateChildren::Entry*, 0x004EA870);
         DEFINE_MEMBER_FN(GetExtraEnableStateParentFlag1,   bool, 0x004EA860);
   };
   //
   //extern DEFINE_SUBROUTINE(BGSDestructibleObjectForm*, GetBGSDestructibleObjectFormForRef, 0x00448090, TESObjectREFR*);
   //extern DEFINE_SUBROUTINE(void, Update3DBasedOnHarvestedFlag, 0x00455BD0, TESObjectREFR*, NiNode* root); // finds and updates the NiSwitchNode in the model
   
   class refr_ptr {
      protected:
         RE::TESObjectREFR* ref = nullptr;
         inline void _inc() {
            if (ref)
               ref->handleRefObject.IncRef();
         };
         inline void _dec() {
            if (ref)
               ref->handleRefObject.DecRefHandle();
         };
         //
         struct flag_already_implemented {};
      public:
         refr_ptr() {};
         refr_ptr(RE::TESObjectREFR* a) : ref(a) { _inc(); }
         refr_ptr(const refr_ptr& a) : ref(a.ref) { _inc(); }
         refr_ptr(const UInt32& refHandle) { // does NOT exchange the handle; uses a copy
            this->set_from_handle(refHandle);
         }
         //
         inline static refr_ptr make_from_already_incremented(TESObjectREFR* a) {
            refr_ptr out;
            out.set_from_already_incremented(a);
            return out;
         };
         //
         RE::TESObjectREFR* operator->(); // implies comparison operators with pointers, because it allows for an implicit cast
         operator bool() const;
         operator RE::TESObjectREFR*() const;
         refr_ptr operator=(RE::TESObjectREFR* rhs);
         refr_ptr operator=(const refr_ptr& rhs);
         //
         bool operator!();
         //
         ~refr_ptr();
         //
         TESObjectREFR* abandon(); // stops refcounting the pointer, and returns it
         TESObjectREFR* copy_bare();
         inline TESObjectREFR* get() const { return this->ref; };
         inline ::TESObjectREFR* get_base() const { return (::TESObjectREFR*) this->ref; };
         inline void set_from_already_incremented(TESObjectREFR* a) { this->ref = a; };
         void set_from_handle(const UInt32 handle); // does NOT exchange the handle; uses a copy
         void set_from_handle(UInt32* handle); // exchanges the handle
         inline void swap(refr_ptr& other) noexcept {
            auto a = this->ref;
            auto b = other.ref;
            other.ref = a;
            this->ref = b;
         };
         inline long int use_count() const {
            if (!ref)
               return 0;
            return ref->handleRefObject.GetRefCount();
         };
   };
};