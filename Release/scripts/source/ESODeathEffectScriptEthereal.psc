Scriptname ESODeathEffectScriptEthereal extends activemagiceffect Hidden

Spell Property pkSpell Auto ; the spell to which this effect belongs
Spell Property pkVisuals Auto ; the spell we use for effectshaders
Actor _kTarget = None

;
; This script exists to accomplish a few different purposes:
;
;  - Force the player fully out of "ghost mode" if they somehow manage to break 
;    ethereality.
;
;  - Ensure that when the player leaves "ghost mode," our detection overrides 
;    are removed.
;
;  - Handle player controls for combat, and sheathing of the player's weapon 
;    when they enter "ghost mode."
;
; A few assumptions are made here:
;
;  - The DLL will always ragdoll the player when they die. It only HAS to ragdoll 
;    them after a killmove, to fix potential physics/animation issues, but ragdolling 
;    them in other situations is a stylistic choice.
;

Function BackToLife()
   If ESODeath_Miscellaneous.EffectExists(Self) ; if we're dispelled multiple times, the ScriptObject may not wrap a valid ActiveEffect* anymore
      UnregisterForAnimationEvent(_kTarget, "GetUpEnd")
      UnregisterForAnimationEvent(_kTarget, "GetUpExit")
   EndIf
   Game.EnablePlayerControls(False, True, False, False, False, False, False, False)
   ESODeath_Detection.SetActorUnseen(_kTarget, False)
   _kTarget.DispelSpell(pkSpell)
   _kTarget.RemoveSpell(pkSpell)
EndFunction

Event OnEffectStart(Actor akTarget, Actor akCaster)
   _kTarget = akTarget
   RegisterForAnimationEvent(akTarget, "GetUpEnd") ; player has finished going from ragdoll to animated
   RegisterForAnimationEvent(akTarget, "GetUpExit") ; not used when swimming; indicates player has control again?
   Game.DisablePlayerControls(False, True, False, False, False, False, False, False)
   ;
   ; Failsafe for if we missed the events:
   ;
   RegisterForSingleUpdate(Self.GetDuration())
   Utility.Wait(1.5)
   _kTarget.AddSpell(pkVisuals, False)
EndEvent
Event OnUpdate()
   BackToLife()
EndEvent
Event OnEffectFinish(Actor akTarget, Actor akCaster)
   Debug.Trace("[ESODeath] Ethereal effect finished.")
   BackToLife()
   akTarget.DispelSpell(pkVisuals)
EndEvent
Event OnAnimationEvent(ObjectReference akTarget, String asEventName)
   If akTarget != _kTarget
      Return
   EndIf
   UnregisterForAnimationEvent(akTarget, asEventName)
   If asEventName == "GetUpEnd"
      RegisterForSingleUpdate(Self.GetDuration())
   ElseIf asEventName == "GetUpExit"
      _kTarget.SheatheWeapon()
   EndIf
   _kTarget.AddSpell(pkVisuals, False)
EndEvent