Scriptname ESODeathEffectScriptFailsafe extends ActiveMagicEffect Hidden 

Event OnEffectStart(Actor akTarget, Actor akCaster)
   ESODeath_Detection.SetActorUnseen(Game.GetPlayer(), False)
EndEvent
