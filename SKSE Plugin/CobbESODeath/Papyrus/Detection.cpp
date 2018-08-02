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
         if (!actor)
            return;
         DetectionInterceptor::GetInstance().SetActorUnseen(actor, unseen);
      }
      void SetActorUnseeing(VMClassRegistry* registry, UInt32 stackId, StaticFunctionTag*, RE::Actor* actor, bool unseen) {
         if (!actor)
            return;
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