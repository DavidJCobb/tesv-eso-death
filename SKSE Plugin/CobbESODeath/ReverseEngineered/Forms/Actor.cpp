#include "ReverseEngineered/Forms/Actor.h"
#include "skse/GameData.h"
#include "skse/GameExtraData.h"
#include "skse/GameRTTI.h"
#include "skse/NiNodes.h"

namespace RE {
   /*ActorKnowledge::ActorKnowledge(RE::Actor* a, RE::Actor* b) {
      if (!a || (a->handleRefObject.m_uiRefCount & 0x3FF) == 0)
         this->unk08 = *g_invalidRefHandle;
      else
         CreateRefHandleByREFR(&this->unk08, (::TESObjectREFR*)a);
      //
      if (!b || (b->handleRefObject.m_uiRefCount & 0x3FF) == 0)
         this->unk0C = *g_invalidRefHandle;
      else
         CreateRefHandleByREFR(&this->unk0C, (::TESObjectREFR*)b);
   };*/
   //
   bool Actor::IsAIEnabled() {
      return (this->flags1 & Actor::kFlags_AIEnabled) == Actor::kFlags_AIEnabled;
   };
   void Actor::StopCombatAlarm() {
      struct Unk012E32E8 {
         DEFINE_MEMBER_FN_LONG(Unk012E32E8, Subroutine00758910, void, 0x00758910, Actor*, UInt32);
      };
      auto unk = *(Unk012E32E8**)0x012E32E8;
      CALL_MEMBER_FN(unk, Subroutine00758910)(this, 0);
      this->Unk_E4();
      CALL_MEMBER_FN(this, SetFlag2_00000800)(false);
   };
   void Actor::UpdateHairColor() {
      TESNPC* npc = DYNAMIC_CAST(baseForm, TESForm, TESNPC);
      if (npc && npc->headData) {
         BGSColorForm * hairColor = npc->headData->hairColor; // Non-player actors won't have this
         if (hairColor) {
            NiColorA val;
            val.r = hairColor->color.red / 128.0;
            val.g = hairColor->color.green / 128.0;
            val.b = hairColor->color.blue / 128.0;
            NiColorA * color = &val;
            //
            NiNode * model = GetNiRootNode(0);
            if (model)
               UpdateModelHair(model, &color);
         }
      }
   }
   void Actor::UpdateSkinColor() {
      TESNPC* npc = DYNAMIC_CAST(baseForm, TESForm, TESNPC);
      if (npc) {
         NiColorA val;
         val.r = npc->color.red / 255.0;
         val.g = npc->color.green / 255.0;
         val.b = npc->color.blue / 255.0;
         NiColorA * color = &val;
         //
         NiNode * thirdPerson = GetNiRootNode(0);
         NiNode * firstPerson = GetNiRootNode(1);
         if (thirdPerson) {
            UpdateModelSkin(thirdPerson, &color); // Update for 3rd Person
         }
         if (firstPerson) {
            UpdateModelSkin(firstPerson, &color); // Update for 1st Person
         }
      }
   }
   bool Actor::VisitFactions(FactionVisitor& visitor) {
      TESNPC* npc = DYNAMIC_CAST(baseForm, TESForm, TESNPC);
      if (npc) {
         for (UInt32 i = 0; i < npc->actorData.factions.count; i++) {
            TESActorBaseData::FactionInfo info;
            npc->actorData.factions.GetNthItem(i, info);
            if (visitor.Accept(info.faction, info.rank))
               return true;
         }
         //
         ExtraFactionChanges* pFactionChanges = static_cast<ExtraFactionChanges*>(extraData.GetByType(kExtraData_FactionChanges));
         if (pFactionChanges) {
            for (UInt32 i = 0; i < pFactionChanges->factions.count; i++) {
               ExtraFactionChanges::FactionInfo info;
               pFactionChanges->factions.GetNthItem(i, info);
               if (visitor.Accept(info.faction, info.rank))
                  return true;
            }
         }
      }
      return false;
   }

   void Actor::SetHeadTracking(bool abEnabled) {
      if (abEnabled)
         this->actorState.flags08 |= 0x00000008;
      else
         this->actorState.flags08 &= 0xFFFFFFF7;
   };
};