Scriptname ESODeathEffectScriptSpawnReqs extends ActiveMagicEffect Hidden 

Message Property CobbESODeathMenuChooseSpawnMethod Auto
Message Property CobbESODeathMessageForcingTravel Auto

Spell Property pkSpell Auto

Event OnEffectStart(Actor akTarget, Actor akCaster)
   ObjectReference kTravelTo = ESODeath_Miscellaneous.GetNearestMapMarkerTo(akTarget, 2)
   If !kTravelTo
      ;
      ; Handle the possibility that the player somehow hasn't discovered any locations in 
      ; this (family of) worldspace(s).
      ;
      ; Note that this won't handle the case of a worldspace (e.g. a modded one) with no 
      ; settlements or cities in it.
      ;
      kTravelTo = ESODeath_Miscellaneous.GetNearestMapMarkerTo(akTarget, 1) ; prefer known-on-map
      If !kTravelTo
         kTravelTo = ESODeath_Miscellaneous.GetNearestMapMarkerTo(akTarget, 0) ; allow any
      EndIf
   EndIf
   Bool bTravel = True
   Bool bSuppressTravelMessage = False
   If ESODeath_Miscellaneous.SoulGemFunctionalityEnabled() &&  ESODeath_Miscellaneous.HasUsableSoulGem(akTarget)
      If ESODeath_Miscellaneous.GemPreemptsTravel()
         If kTravelTo && CobbESODeathMenuChooseSpawnMethod && ESODeath_Miscellaneous.CanChooseToUseSoulGem()
            bTravel = CobbESODeathMenuChooseSpawnMethod.Show() as Bool
            bSuppressTravelMessage = True
         Else
            bTravel = False
         EndIf
         If !bTravel
            ESODeath_Miscellaneous.TryConsumeSoulGem(akTarget)
         EndIf
      Else
         ESODeath_Miscellaneous.TryConsumeSoulGem(akTarget)
         bSuppressTravelMessage = True
      EndIf
   EndIf
   If bTravel && ESODeath_Miscellaneous.ResurrectOffsiteEnabled()
      Debug.Trace("[ESODeath] Attempting to resurrect off-site at: " + kTravelTo)
      If kTravelTo
         If !bSuppressTravelMessage
            CobbESODeathMessageForcingTravel.Show()
         EndIf
         akTarget.ForceRemoveRagdollFromWorld() ; ragdolling breaks FastTravel and MoveTo
         Game.FastTravel(kTravelTo)
         akTarget.MoveTo(kTravelTo)
         If akTarget.GetAnimationVariableInt("iGetUpType") == 2 ; the player was ragdolled while swimming
            ;
            ; There's a nasty edge-case in the animations used for standing up from a ragdoll 
            ; state. If the player is ragdolled while swimming, their animation for regaining con-
            ; trol puts them back in a swimming state. The game engine doesn't check whether 
            ; they're still on land, however. As such, a precise sequence of ragdolling, termin-
            ; ating the ragdoll state early, and moving the player can cause the player to end 
            ; up swimming on land. That sequence, incidentally, is literally what this mod does.
            ;
            Debug.Trace("[ESODeath] Detected that the player was in water at the time of their death.")
            ;/; This doesn't work consistently.
            Utility.Wait(0.0) ; Wait until after the loading screen
            Debug.SendAnimationEvent(akTarget, "staggerStart") ; testing shows that only certain animations work; in particular, some anims don't work if the player is still moving or trying to move
            ;/;
            Utility.Wait(0.0) ; Wait until after the loading screen
            akTarget.PushActorAway(akTarget, 0.0)
         EndIf
         akTarget.DispelSpell(pkSpell)
      EndIf
   EndIf
EndEvent