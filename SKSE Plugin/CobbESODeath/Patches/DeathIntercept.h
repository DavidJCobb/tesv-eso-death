#pragma once
#include <vector>
#include "ReverseEngineered/Forms/Actor.h"

namespace Patches {
   namespace DeathIntercept {
      extern bool(*g_deathQueryHandler)(RE::Actor* dead);
      extern void(*g_deathInterceptHandler)(RE::Actor* dead); // fires when the game tries to kill the player; can fire multiple times per death as Beth's internal systems are sometimes redundantw ith each other
      extern void(*g_killmoveDoneHandler)(RE::Actor* dead);

      void Apply();
   };
};