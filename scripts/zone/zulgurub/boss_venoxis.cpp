/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* ScriptData
SDName: boss_venoxis
SD%Complete: 100
SDComment:
EndScriptData */


#include "../../sc_defines.h"

#define SPELL_HOLY_FIRE     23860
#define SPELL_ASPECTOFVENOXIS   25053
#define SPELL_HOLY_NOVA     23858 
#define SPELL_POISON_CLOUD  24840
#define SPELL_SNAKE_FORM    23849
#define SPELL_RENEW         23895
#define SPELL_BERSERK       23537

#define SAY_AGGRO         "Let the coils of hate unfurl!"

#define SOUND_AGGRO       8421

struct MANGOS_DLL_DECL boss_venoxisAI : public ScriptedAI
{
    boss_venoxisAI(Creature *c) : ScriptedAI(c) {EnterEvadeMode();}

    uint32 HolyFire_Timer;
    uint32 Aspect_Timer;
    uint32 Renew_Timer;
    uint32 PoisonCloud_Timer;
    uint32 HolyNova_Timer;
    uint32 TargetInRange;

  bool InCombat;
    bool PhaseTwo;
    bool InBerserk;

    void EnterEvadeMode()
    {
        HolyFire_Timer = 10000;
        Aspect_Timer = 14000;
        Renew_Timer = 41000;
        PoisonCloud_Timer = 2000;
        HolyNova_Timer = 5000;
        TargetInRange = 0;


        InCombat = false;
        PhaseTwo = false;
        InBerserk= false;

    m_creature->RemoveAllAuras();
    m_creature->DeleteThreatList();
    m_creature->CombatStop();
    DoGoHome();
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);       
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CONFUSED, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM , true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR , true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DAZE, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SLEEP, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
    }

    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && who!= m_creature)
        {
            DoStartMeleeAttack(who);
            //Say our dialog on initial aggro
      if (!InCombat)
            {
                DoYell(SAY_AGGRO,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature,SOUND_AGGRO);
                InCombat = true;
            }
        }
    }


    void JustDied(Unit* Killer)
    {
        ScriptedInstance *pInstance = (m_creature->GetInstanceData()) ? ((ScriptedInstance*)m_creature->GetInstanceData()) : NULL;
        if(pInstance)
            pInstance->SetData("Venoxis_Death", 0);
    }

  void ResetThreat()
  {
    std::list<HostilReference*>& m_threatlist = m_creature->getThreatManager().getThreatList();
 
    for(uint32 i = 0; i <= (m_threatlist.size()-1); i++)
    {
      Unit* pUnit = SelectUnit(SELECT_TARGET_TOPAGGRO, i);
      if(pUnit)
        (m_creature->getThreatManager()).modifyThreatPercent(pUnit, -99);
    }
 
  }
    void MoveInLineOfSight(Unit *who)
    {
        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
        {
            float attackRadius = m_creature->GetAttackDistance(who);
            if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && m_creature->IsWithinLOSInMap(who))
            {
                if(who->HasStealthAura())
                    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                DoStartMeleeAttack(who);
                      //Say our dialog on initial aggro
                if (!InCombat)
                {
                DoYell(SAY_AGGRO,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature,SOUND_AGGRO);
                InCombat = true;
                }

            }
        }
    }

     void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget())
            return;

        if( m_creature->getVictim() && m_creature->isAlive())
        {
            if ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth() > 50))
            {

        if (Aspect_Timer < diff)
                {
                    DoCast(m_creature->getVictim(),SPELL_ASPECTOFVENOXIS);
                    Aspect_Timer = 18000;
                }else Aspect_Timer -= diff;
                
                if (Renew_Timer < diff)
                {
                    DoCast(m_creature,SPELL_RENEW);
                    Renew_Timer = 30000;
                }else Renew_Timer -= diff;

                if (HolyNova_Timer < diff)
                {
                    Unit* target = NULL;
                    TargetInRange = 0;
                    for(int i=0; i<10; i++)
                    {
                        target = SelectUnit(SELECT_TARGET_TOPAGGRO,i);
                        if(target)
                            if(m_creature->IsWithinDistInMap(target, ATTACK_DISTANCE))
                                TargetInRange++;
                    }
                    if(TargetInRange > 1)
                    {
                        DoCast(m_creature->getVictim(),SPELL_HOLY_NOVA);
                        HolyNova_Timer = 1000;
                    }
                    else
                    {
                        HolyNova_Timer = 5000;
                    }
                    
                }else HolyNova_Timer -= diff;
                
        if (HolyFire_Timer < diff && TargetInRange < 3)
                {
                    DoCast(m_creature->getVictim(),SPELL_HOLY_FIRE);
                    HolyFire_Timer = 8000;
                }else HolyFire_Timer -= diff;               
                
                
                
            }
            else
            {
                if(!PhaseTwo)
                {
                    DoCast(m_creature,SPELL_SNAKE_FORM);
          const CreatureInfo *cinfo = m_creature->GetCreatureInfo();
          m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 25)));
          m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 25)));
          m_creature->UpdateDamagePhysical(BASE_ATTACK);
          ResetThreat();
          PhaseTwo = true;
                }

                if(PhaseTwo && PoisonCloud_Timer < diff)
                {
                    DoCast(m_creature->getVictim(), SPELL_POISON_CLOUD);
                    PoisonCloud_Timer = 16000;
                }PoisonCloud_Timer -=diff;

                if (PhaseTwo && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 11) && !InBerserk)
                {
                    DoCast(m_creature, SPELL_BERSERK);
                    InBerserk = true;
                }
            }
            DoMeleeAttackIfReady();
        }
    }
}; 
CreatureAI* GetAI_boss_venoxis(Creature *_Creature)
{
    return new boss_venoxisAI (_Creature);
}


void AddSC_boss_venoxis()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_venoxis";
    newscript->GetAI = GetAI_boss_venoxis;
    m_scripts[nrscripts++] = newscript;
}
