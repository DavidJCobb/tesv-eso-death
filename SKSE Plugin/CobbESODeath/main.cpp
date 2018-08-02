#include "Shared.h" // So we can share them to files that need them.

#include "skse/PluginAPI.h"		// super
#include "skse/skse_version.h"	// What version of SKSE is running?
#include <shlobj.h> // CSIDL_MYCODUMENTS
#include <psapi.h>  // MODULEINFO, GetModuleInformation
#pragma comment(lib, "psapi.lib") // what a stupid nuisance

#include "Papyrus/Detection.h"
#include "Patches/DeathIntercept.h"
//#include "Patches/DefineHardcodedForms.h"
#include "Patches/Detection.h"
#include "ReverseEngineered/Objects/Actions.h"
#include "ReverseEngineered/Systems/DefaultObjects.h"
#include "ReverseEngineered/Systems/GameData.h"
#include "ReverseEngineered/Systems/012E32E8.h"

#include "skse/GameEvents.h"

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

static bool g_TESVVersionSupported = false;
static bool g_SKSEVersionSupported = false;
SKSEPapyrusInterface*   g_papyrus        = nullptr;
SKSEMessagingInterface* g_ISKSEMessaging = nullptr;

static SpellItem* g_deadAbility = nullptr;
void LocateDeadAbility() {
   //
   // This is an ugly workaround for an uglier problem. See, we need to be able 
   // to guarantee the existence of an ability spell for this mod, and ideally, 
   // we should be able to edit that spell in the Creation Kit. The straightfor-
   // ward way to do this would be to use an injected record with a known form 
   // ID.
   //
   // The problem, however, is that if you add an injected Spell to the player 
   // as an ability and they save the game, then the magic effects will be 
   // saved, but the spell itself will not. HasSpell will return false, and 
   // DispelSpell and DispelAllSpells won't get rid of the effects.
   //
   // So, the spell can't be injected, which means its form ID (or at least its 
   // load order prefix) can vary. Given that we don't want to break attempts to 
   // merge the ESP we use, we can't rely on a local form ID paired with a file-
   // name. We need some way to identify the spell regardless of its form ID.
   // 
   // Spells don't store their editor IDs in memory, by the way. Just a little... 
   // Just a little note there.
   // 
   // Sooooooo we're stuck with this hideous workaround:
   // 
   //  - Create a Perk that does nothing. Inject it to a known form ID (we've 
   //    decided to use 0x00C0BB02).
   // 
   //  - Have our spell use that Perk as its Casting Perk.
   // 
   //  - Loop over all loaded Spell forms and pluck out the one with that Perk 
   //    as its Casting Perk.
   //
   _MESSAGE("Attempting to locate a suitable ability spell for dead players...");
   auto perk = LookupFormByID(0x00C0BB02);
   if (!perk) {
      _MESSAGE("Failed to locate a suitable ability spell for dead players!\n - No form exists at ID 0x00C0BB02. There needs to be a Perk there.\n - The spell should use a Casting Perk whose form ID is 0x00C0BB02.");
      return;
   }
   _MESSAGE("Got the perk...");
   //
   auto   data = RE::TESDataHandler::GetSingleton();
   _MESSAGE("DataHandler: %08X", data);
   auto&  list = data->spellItems;
   _MESSAGE("List: %08X", &list);
   size_t count = list.count;
   _MESSAGE("Count: %u", count);
   for (size_t i = 0; i < count; i++) {
      auto item = list.arr.entries[i];
      if (!item)
         continue;
      if (item->data.spellPerk == perk) {
         g_deadAbility = item;
         _MESSAGE("Found a suitable ability spell for dead players.");
         return;
      }
   }
   _MESSAGE("Failed to locate a suitable ability spell for dead players!\n - The spell should use a Casting Perk whose form ID is 0x00C0BB02.\n - Yes, I know that's ugly; see source code or ESP contents for details.");
};

namespace DeathHandling {
   void _HandleBounties(RE::Actor* subject) {
      auto  subjectID  = subject->formID;
      auto  singleton  = RE::Unknown012E32E8::GetInstance();
      auto& handleList = singleton->unk028;
      for (UInt32 i = 0; i < handleList.count; i++) {
         UInt32     handle = singleton->unk028.arr.entries[i]; // copy the handle so that LookupREFRByHandle doesn't clear the original
         RE::Actor* other  = nullptr;
         LookupREFRByHandle(&handle, (TESObjectREFR**) &other);
         if (!other)
            continue;
         if (other->IsGuard() && CALL_MEMBER_FN(other, IsHostileToActor)(subject)) {
            auto crimeFaction = CALL_MEMBER_FN(other, GetCrimeFaction)();
            if (crimeFaction) {
               if (subject->GetBounty(crimeFaction)) {
                  const char* name = crimeFaction->fullName.name.data;
                  subject->PayBounty(crimeFaction, false, true);
               }
//const char* name = crimeFaction->fullName.name.data;
//_MESSAGE("    - Actor is a guard working for crimeFaction %08X (%s).", crimeFaction, name ? name : "");
            } else {
//_MESSAGE("    - Actor is a guard but has no crime faction.");
            }
         }
         other->handleRefObject.DecRefHandle(); // LookupREFRByHandle incremented the refcount
         other = nullptr; // if ref is non-null, LookupREFRByHandle will decrement its refcount (which we can't rely on because it wouldn't run after the loop)
      }
//_MESSAGE("Done checking actors hostile to subject.");
   }
   void _HandleDeathCount(RE::Actor* subject) {
      auto base = DYNAMIC_CAST(subject->baseForm, TESForm, TESActorBase);
      if (!base)
         return;
      if (base->actorData.flags & 0x20) {
         auto extra = (RE::ExtraLeveledCreature*) CALL_MEMBER_FN((RE::BaseExtraList*) &subject->extraData, GetExtraData)(0x2D);
         if (extra && extra->unk08)
            base = extra->unk08;
      }
      CALL_MEMBER_FN((RE::TES*) *g_TES, ModActorBaseDeathCount)(base, 1);
   }
   //
   void OnKilled(RE::Actor* subject) {
      _HandleBounties(subject);
      //
      bool unused;
      subject->StopCombatAlarm(); // take player out of combat, if possible
      DetectionInterceptor::GetInstance().SetActorUnseen(subject, true); // TODO: magic effect should dispel this OnEffectFinish using a script; we should write code to check for the spell when a save is loaded and handle this
      //
      CALL_MEMBER_FN(RE::Unknown012E32E8::GetInstance(), ResetAllDetection)(&unused);
      {  // Reset subject health to full
         //
         // Don't use ActorValueOwner::SetCurrent; it's misnamed. That function actually sets 
         // the current AND base values.
         //
         //CALL_MEMBER_FN(subject, Subroutine006DFEC0)(0x18, 9999999.0F);
         CALL_MEMBER_FN(subject, ResetHealthAndLimbs)();
      }
      //
      /*if (subject->IsSwimming())*/ {
         auto ai = subject->processManager;
         if (ai)
            CALL_MEMBER_FN(ai, PushActorAway)(subject, 0.0F, 0.0F, 1.0F, subject->IsSwimming() ? 0.5F : 0.0F);
      }
      _HandleDeathCount(subject);
      if (g_deadAbility)
         CALL_MEMBER_FN(subject, AddSpell)(g_deadAbility);
   };
   void OnKillmoved(RE::Actor* subject) {
      //
      // Several killmoves will glitch the player's animations and physics after 
      // they complete, likely just due to their being created on the assumption 
      // that they'll actually kill an actor. The effect is as if the game is 
      // trying to treat the player as both an animating model AND a ragdolling 
      // model at the same time, with these two methods of movement coming into 
      // conflict in varying ways.
      //
      // Observed behaviors include:
      //
      //  - The player seems to animate normally, aside from their neck, which 
      //    looks broken and stuck at an angle. However, if you stop moving, 
      //    point the character behind the player, and then walk forward, then 
      //    the player's arms will ragdoll through the torso to get to their 
      //    proper animated positions.
      //
      //  - The player will begin to float into the air like a balloon, with 
      //    their animations only seeming to carry them through the air. The 
      //    player is able to "move" properly, but their model isn't tethered 
      //    to their "real" position anymore.
      //
      // Fortunately, forcing the player into a ragdoll state seems to fix this, 
      // by making them ragdoll "all the way" and allowing them to stand back up 
      // normally.
      //
      /*// Commented out because we always ragdoll a dying player now
      auto ai = subject->processManager;
      if (ai)
         CALL_MEMBER_FN(ai, PushActorAway)(subject, 0.0F, 0.0F, -1.0F, 0.0F);
      //*/
      return;
   };
};

void Callback_Messaging_SKSE(SKSEMessagingInterface::Message* message) {
   if (message->type == SKSEMessagingInterface::kMessage_DataLoaded) {
      LocateDeadAbility();
   } else if (message->type == SKSEMessagingInterface::kMessage_PostLoad) {
      //
      // Apply some patches here. Hopefully, this will allow us to avoid the game's 
      // attempts at checking whether persistent actors should be alive.
      //
      Patches::DeathIntercept::Apply();
      Patches::Detection::Apply();
      DetectionInterceptor::GetInstance(); // forcibly create it in advance of gameplay beginning
   } else if (message->type == SKSEMessagingInterface::kMessage_NewGame) {
      DetectionInterceptor::GetInstance().Reset();
   } else if (message->type == SKSEMessagingInterface::kMessage_PreLoadGame) {
      DetectionInterceptor::GetInstance().Reset();
   } else if (message->type == SKSEMessagingInterface::kMessage_PostLoadGame) {
      if (g_deadAbility) {
         auto player = *(RE::Actor**)(0x01310588);
         if (player && CALL_MEMBER_FN(player, HasSpell)(g_deadAbility))
            DetectionInterceptor::GetInstance().SetActorUnseen(player, true);
      }
   }
};
extern "C" {
   //
   // SKSEPlugin_Query: Called by SKSE to learn about this plug-in and check that it's safe to load.
   //
   bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) {
      gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim\\SKSE\\CobbESODeath.log");
      gLog.SetPrintLevel(IDebugLog::kLevel_Error);
      gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);
      //
      _MESSAGE("CobbESODeath");
      //
      // Populate info structure.
      //
      // Plug-in versions:
      //   1 = 1.0
      //   2 = 1.2
      //   3 = 1.3
      //   4 = 1.4
      //   5 = 1.4.1 // NUMBERING SCHEME DIVERGES HERE
      //
      // 0xAABBCCDD = AA.BB.CC.DD with values converted to decimal // major.minor.update.internal-build-or-zero
      //
      info->infoVersion = PluginInfo::kInfoVersion;
      info->name = "CobbESODeath";
      info->version = 0x01000000;
      {  // Log version number
         auto  v     = info->version;
         UInt8 main  = v >> 0x18;
         UInt8 major = v >> 0x10;
         UInt8 minor = v >> 0x08;
         _MESSAGE("Current version is %d.%d.%d.", main, major, minor);
      }
      {  // Get run-time information
         HMODULE    baseAddr = GetModuleHandle("CobbESODeath"); // DLL filename
         MODULEINFO info;
         if (baseAddr && GetModuleInformation(GetCurrentProcess(), baseAddr, &info, sizeof(info)))
            _MESSAGE("We're loaded to the span of memory at %08X - %08X.", info.lpBaseOfDll, (UInt32)info.lpBaseOfDll + info.SizeOfImage);
      }
      //
      // Store plugin handle so we can identify ourselves later.
      //
      g_pluginHandle = skse->GetPluginHandle();
      //
      g_TESVVersionSupported = (skse->runtimeVersion == RUNTIME_VERSION_1_9_32_0);
      g_SKSEVersionSupported = (skse->skseVersion >= 0x01070300); // 1.7.3.0
      //
      if (skse->isEditor) {
         _MESSAGE("Loaded in the Creation Kit. Marking as incompatible.");
		   return false;
      } else if (!g_TESVVersionSupported) {
         _MESSAGE("Unsupported Skyrim version: %08X.", skse->runtimeVersion);
         return false;
      }
      {  // Get the messaging interface and query its version.
         g_ISKSEMessaging = (SKSEMessagingInterface *)skse->QueryInterface(kInterface_Messaging);
         if (!g_ISKSEMessaging) {
            _MESSAGE("Couldn't get messaging interface.");
            return false;
         } else if (g_ISKSEMessaging->interfaceVersion < SKSEMessagingInterface::kInterfaceVersion) {
            _MESSAGE("Messaging interface too old (%d; we expected %d).", g_ISKSEMessaging->interfaceVersion, SKSEMessagingInterface::kInterfaceVersion);
            return false;
         }
      }
      //
      // This plug-in supports the current Skyrim and SKSE versions:
      //
      return true;
   }
   bool SKSEPlugin_Load(const SKSEInterface* skse)	{
      _MESSAGE("Loaded.");
      //
      //Patches::DefineHardcodedForms::Apply();
      //
      Patches::DeathIntercept::g_deathInterceptHandler = &DeathHandling::OnKilled;
      Patches::DeathIntercept::g_killmoveDoneHandler   = &DeathHandling::OnKillmoved;
      //
      g_ISKSEMessaging->RegisterListener(g_pluginHandle, "SKSE", Callback_Messaging_SKSE);
      //
      g_papyrus = (SKSEPapyrusInterface*) skse->QueryInterface(kInterface_Papyrus);
      g_papyrus->Register(CobbPapyrus::Detection::Register);
      //
      return true;
   }
};
