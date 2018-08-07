#include "Miscellaneous.h"

#include "skse/PapyrusNativeFunctions.h"
#include "skse/PapyrusObjects.h"
#include "skse/PapyrusVM.h"

#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Forms/TESObjectCELL.h"
#include "ReverseEngineered/Forms/TESObjectREFR.h"
#include "ReverseEngineered/Forms/TESWorldSpace.h"
#include "ReverseEngineered/Forms/HardcodedForms.h"
#include "ReverseEngineered/Player/PlayerCharacter.h"
#include "ReverseEngineered/Systems/GameData.h"
#include "ReverseEngineered/Types.h"
#include "Services/INISettings.h"
#include "Services/SoulGemSystem.h"

namespace CobbPapyrus {
   namespace Miscellaneous {
      namespace SoulGem {
         SInt32 CanChooseToUseSoulGem(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*) {
            if (!CobbESODeath::INI::SoulGem::bGemPreemptsTravel.bCurrent)
               return 0;
            return (SInt32) CobbESODeath::INI::SoulGem::bAlwaysLetPlayerChoose.bCurrent;
         };
         SInt32 GemPreemptsTravel(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*) {
            return (SInt32) CobbESODeath::INI::SoulGem::bGemPreemptsTravel.bCurrent;
         };
         SInt32 HasUsableSoulGem(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*, RE::Actor* actor) {
            ERROR_AND_RETURN_0_IF(actor == nullptr, "You must specify an actor.", registry, stackId);
            return (SInt32) SoulGemSystem::HasGem(actor);
         };
         SInt32 SoulGemFunctionalityEnabled(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*) {
            return (SInt32) CobbESODeath::INI::SoulGem::bEnabled.bCurrent;
         };
         SInt32 TryConsumeSoulGem(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*, RE::Actor* actor) {
            ERROR_AND_RETURN_0_IF(actor == nullptr, "You must specify an actor.", registry, stackId);
            return (SInt32) SoulGemSystem::UseGem(actor);
         };
      };
      namespace Travel {
         struct GetMapMarkerFunctor {
            NiPoint3     nearestTo;
            SInt32       minStatus;
            float        distance = FLT_MAX;
            RE::refr_ptr result;
            //
            static bool _callback(RE::TESWorldSpace* currentWorld, RE::TESObjectREFR* ref, RE::ExtraMapMarker::Data* extra, void* state) {
               auto pState = (GetMapMarkerFunctor*) state;
//_MESSAGE("[Papyrus:Miscellaneous::GetNearestMapMarkerTo!FUNCTOR] Marker [%s] type %04X flags %02X...", extra->name.name.data, extra->type, extra->flags);
               if (CobbESODeath::INI::ResurrectOffsite::bRequireSettlement.bCurrent == false || extra->IsSettlement()) {
                  if (pState->minStatus >= 1)
                     if (!CALL_MEMBER_FN(extra, GetIsVisible)())
                        return true; // continue
                  if (pState->minStatus >= 2)
                     if (!CALL_MEMBER_FN(extra, GetFastTravelEnabled)())
                        return true; // continue
                  float distance = pow(ref->pos.x - pState->nearestTo.x, 2) + pow(ref->pos.y - pState->nearestTo.y, 2) + pow(ref->pos.z - pState->nearestTo.z, 2);
                  distance = sqrt(distance);
                  if (distance < pState->distance) {
                     pState->distance = distance;
                     pState->result   = ref;
//_MESSAGE("[Papyrus:Miscellaneous::GetNearestMapMarkerTo!FUNCTOR]  - NEW BEST MATCH! Distance: %f", distance);
                  }
               }
               return true;
            };
         };
         //
         RE::TESObjectREFR* GetNearestMarkerTo(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*, RE::TESObjectREFR* subject, SInt32 minimumStatus) {
            ERROR_AND_RETURN_0_IF(subject == nullptr, "You must specify a reference.", registry, stackId);
            GetMapMarkerFunctor functor;
            functor.nearestTo = subject->pos;
            functor.minStatus = minimumStatus;
            auto worldspace = (RE::TESWorldSpace*) CALL_MEMBER_FN(subject, GetParentWorldspace)();
            if (!worldspace) {
               //
               // If the player is in an interior, then they don't have any data connecting them to a 
               // worldspace. However, g_TES maintains the last good worldspace (for the world map, 
               // among other things).
               //
               worldspace = (*RE::g_TES)->worldSpace;
               ERROR_AND_RETURN_0_IF(worldspace == nullptr, "The actor is in an interior, and we were unable to get the player's last good worldspace.", registry, stackId);
               RE::PlayerCharacter* player = (RE::PlayerCharacter*) DYNAMIC_CAST(subject, TESObjectREFR, PlayerCharacter);
               ERROR_AND_RETURN_0_IF(player == nullptr, "The actor you're querying is in an interior, and it's not the player. There's no way to get its last good outdoor coordinates.", registry, stackId);
               functor.nearestTo = player->unk3A0; // memory inspection indicates this is the player's position within the last good worldspace
               ERROR_AND_RETURN_0_IF(functor.nearestTo.x == FLT_MIN, "The actor is in an interior, and it doesn't have any last good outdoor coordinates.", registry, stackId);
            }
            worldspace->ForEachMapMarker(&GetMapMarkerFunctor::_callback, &functor);
            return functor.result.abandon();
         };
         SInt32 ResurrectOffsiteEnabled(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*) {
            return (SInt32) CobbESODeath::INI::ResurrectOffsite::bEnabled.bCurrent;
         };
      };
      SInt32 Exists(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*, TESForm* subject) {
         return (SInt32) (subject != nullptr);
      };
      SInt32 Exists(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*, ActiveEffect* subject) {
         return (SInt32)(subject != nullptr);
      };
      // ---------------------------------------------------------------------------------------------------------------------------
      bool Register(VMClassRegistry* registry) {
         registry->RegisterFunction(
            new NativeFunction0<StaticFunctionTag, SInt32>(
               "CanChooseToUseSoulGem",
               "ESODeath_Miscellaneous",
               SoulGem::CanChooseToUseSoulGem,
               registry
            )
         );
         registry->SetFunctionFlags("ESODeath_Miscellaneous", "CanChooseToUseSoulGem", VMClassRegistry::kFunctionFlag_NoWait);
         registry->RegisterFunction(
            new NativeFunction0<StaticFunctionTag, SInt32>(
               "GemPreemptsTravel",
               "ESODeath_Miscellaneous",
               SoulGem::GemPreemptsTravel,
               registry
            )
         );
         registry->SetFunctionFlags("ESODeath_Miscellaneous", "GemPreemptsTravel", VMClassRegistry::kFunctionFlag_NoWait);
         registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, SInt32, RE::Actor*>(
               "HasUsableSoulGem",
               "ESODeath_Miscellaneous",
               SoulGem::HasUsableSoulGem,
               registry
            )
         );
         registry->RegisterFunction(
            new NativeFunction0<StaticFunctionTag, SInt32>(
               "SoulGemFunctionalityEnabled",
               "ESODeath_Miscellaneous",
               SoulGem::SoulGemFunctionalityEnabled,
               registry
            )
         );
         registry->SetFunctionFlags("ESODeath_Miscellaneous", "SoulGemFunctionalityEnabled", VMClassRegistry::kFunctionFlag_NoWait);
         registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, SInt32, RE::Actor*>(
               "TryConsumeSoulGem",
               "ESODeath_Miscellaneous",
               SoulGem::TryConsumeSoulGem,
               registry
            )
         );
         //
         registry->RegisterFunction(
            new NativeFunction2<StaticFunctionTag, RE::TESObjectREFR*, RE::TESObjectREFR*, SInt32>(
               "GetNearestMapMarkerTo",
               "ESODeath_Miscellaneous",
               Travel::GetNearestMarkerTo,
               registry
            )
         );
         //
         registry->RegisterFunction(
            new NativeFunction0<StaticFunctionTag, SInt32>(
               "ResurrectOffsiteEnabled",
               "ESODeath_Miscellaneous",
               Travel::ResurrectOffsiteEnabled,
               registry
            )
         );
         registry->SetFunctionFlags("ESODeath_Miscellaneous", "ResurrectOffsiteEnabled", VMClassRegistry::kFunctionFlag_NoWait);
         //
         registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, SInt32, TESForm*>(
               "FormExists",
               "ESODeath_Miscellaneous",
               Exists,
               registry
            )
         );
         registry->SetFunctionFlags("ESODeath_Miscellaneous", "FormExists", VMClassRegistry::kFunctionFlag_NoWait);
         registry->RegisterFunction(
            new NativeFunction1<StaticFunctionTag, SInt32, ActiveEffect*>(
               "EffectExists",
               "ESODeath_Miscellaneous",
               Exists,
               registry
            )
         );
         registry->SetFunctionFlags("ESODeath_Miscellaneous", "EffectExists", VMClassRegistry::kFunctionFlag_NoWait);
         return true;
      };
   };
};