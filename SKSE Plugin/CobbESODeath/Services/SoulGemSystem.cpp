#include "SoulGemSystem.h"

#include "ReverseEngineered/Forms/Actor.h"
#include "ReverseEngineered/Systems/DefaultObjects.h"
#include "ReverseEngineered/Types.h"
#include "Services/INISettings.h"

template<class T> struct NotAnnoyingTList {
   //
   // The SKSE team did some iterator-LIKE implementation for tLists, 
   // but it's... really damn unwieldy, especially when I'm just trying 
   // to replicate vanilla processes as they exist in Bethesda's code.
   //
   struct Node {
      T*    data;
      Node* next;
      Node* prev;
   };
   Node items;
};
void SoulGemVisitor::Dispose(bool free) {
   this->~SoulGemVisitor();
   if (free)
      delete this;
};
BOOL SoulGemVisitor::Visit(RE::InventoryEntryData* entry) {
   auto  form = entry->type;
   UInt8 soulSize = entry->GetSoulSize();
   if (!soulSize) { // not a soul gem, or an empty soul gem
      if (this->bestSoul) // minimum requirement is filled, or we already found a filled gem
         return true;
      if (!form || form->formType != kFormType_SoulGem)
         return true;
   }
   if (entry->countDelta <= 0)
      return true;
   bool isReusable = false;
   {  // check if the gem is reusable
      auto reusableKywd = RE::BGSDefaultObjectManager::GetSingleton()->objects[0xD8]; // RUSG: ReUsable Soul Gem keyword
      if (reusableKywd && reusableKywd->formType == kFormType_Keyword) {
         isReusable = ((TESSoulGem*)form)->keyword.HasKeyword((BGSKeyword*) reusableKywd);
         if (isReusable && CobbESODeath::INI::SoulGem::bAllowReusableGems.current.b == false) {
            return true;
         }
      }
   }
   if (this->bestIsReusable && !isReusable) // prefer reusable gems over non-reusable gems
      return true;
   bool forceMatch = (!this->bestIsReusable && isReusable);
   if (soulSize > this->bestSoul || forceMatch) {
      this->bestSoul = soulSize;
      this->bestIsReusable = isReusable;
      this->bestGem = entry;
   }
   return true;
};

namespace SoulGemSystem {
   bool HasGem(RE::Actor* subject) {
      auto playerInventory = CALL_MEMBER_FN((RE::BaseExtraList*) &subject->extraData, GetExtraContainerChangesData)();
      //
      SoulGemVisitor visitor;
      CALL_MEMBER_FN(playerInventory, ExecuteVisitor)(&visitor);
      return !!visitor.bestSoul;
   }
   bool UseGem(RE::Actor* subject) {
      auto playerInventory = CALL_MEMBER_FN((RE::BaseExtraList*) &subject->extraData, GetExtraContainerChangesData)();
      //
      SoulGemVisitor visitor;
      CALL_MEMBER_FN(playerInventory, ExecuteVisitor)(&visitor);
      if (auto data = visitor.bestGem) {
         if (visitor.bestIsReusable) {
            auto extra = (RE::tList<RE::BaseExtraList>*) data->extendDataList;
            if (extra->items.data)
               CALL_MEMBER_FN(extra->items.data, SetExtraSoul)(0);
         } else {
            RE::BaseExtraList* list;
            {
               auto lists = (RE::tList<RE::BaseExtraList>*) data->extendDataList;
               list = lists ? lists->items.data : nullptr;
            }
            UInt32 discard; // ref handle for dropping items into the world (which we're not doing)
            subject->Unk_56(&discard, (RE::TESForm*) visitor.bestGem->type, 1, 0, list, 0, 0, 0);
         }
         return true;
      }
      return false;
   }
};