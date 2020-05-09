#include "Detection.h"

#include "skse/PapyrusNativeFunctions.h"
#include "skse/PapyrusObjects.h"
#include "skse/PapyrusVM.h"

#include "ReverseEngineered/Forms/Actor.h"
#include "Patches/DeathIntercept.h"
#include "Patches/Detection.h"

namespace CobbPapyrus {
   namespace Detection {
      void SetActorUnseen(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*, RE::Actor* actor, bool unseen) {
         ERROR_AND_RETURN_IF(actor == nullptr, "You cannot modify the \"unseen\" status for a None actor.", registry, stackId);
         DetectionInterceptor::GetInstance().SetActorUnseen(actor, unseen);
      }
      void SetActorUnseeing(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*, RE::Actor* actor, bool unseen) {
         ERROR_AND_RETURN_IF(actor == nullptr, "You cannot modify the \"unseeing\" status for a None actor.", registry, stackId);
         DetectionInterceptor::GetInstance().SetActorUnseeing(actor, unseen);
      }

      bool Register(VMClassRegistry* registry) {
         registry->RegisterFunction(
            new NativeFunction2<StaticFunctionTag, void, RE::Actor*, bool>(
               "SetActorUnseen",
               "ESODeath_Detection",
               SetActorUnseen,
               registry
            )
         );
         registry->RegisterFunction(
            new NativeFunction2<StaticFunctionTag, void, RE::Actor*, bool>(
               "SetActorUnseeing",
               "ESODeath_Detection",
               SetActorUnseeing,
               registry
            )
         );
         return true;
      };
   };
};