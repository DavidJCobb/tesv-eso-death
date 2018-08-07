#pragma once
#include "ReverseEngineered/ExtraData.h"
#include "ReverseEngineered/Forms/Actor.h"
#include "Services/INISettings.h"

class SoulGemVisitor : public RE::ExtraContainerChanges::InventoryVisitor {
   public:
      virtual void Dispose(bool free);
      virtual BOOL Visit(RE::InventoryEntryData* entry);
      //
      SInt8 bestSoul       = CobbESODeath::INI::SoulGem::iMinimumSoulSize.iCurrent - 1;
      bool  bestIsReusable = false;
      RE::InventoryEntryData* bestGem = nullptr;
};

namespace SoulGemSystem {
   __declspec(noinline) bool HasGem(RE::Actor* subject);
   __declspec(noinline) bool UseGem(RE::Actor* subject);
};