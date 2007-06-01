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

#include "../../sc_defines.h"

// **** This script is still under Developement ****

    
#define SPELL_THUNDERCLAP                8732              
#define SPELL_FIREBALLVOLLEY                22425
#define SPELL_MIGHTYBLOW                14099

     

      
struct MANGOS_DLL_DECL phalanxAI : public ScriptedAI
{
    phalanxAI(Creature *c) : ScriptedAI(c) {EnterEvadeMode();}

    uint32 ThunderClap_Timer;
    uint32 FireballVolley_Timer;
    uint32 MightyBlow_Timer;
    bool InCombat;

    void EnterEvadeMode()
    {       
        ThunderClap_Timer = 12000;
        FireballVolley_Timer =0;
	MightyBlow_Timer = 15000;
        InCombat = false;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop();
        DoGoHome();
    }

    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && who!= m_creature)
        {
            //Begin melee attack if we are within range
            DoStartMeleeAttack(who);
            InCombat = true;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
        {
            float attackRadius = m_creature->GetAttackDistance(who);
            if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE)
            {
                if(who->HasStealthAura())
                    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                DoStartMeleeAttack(who);
                InCombat = true;

            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget())
            return;

        //Check if we have a current target
        if( m_creature->getVictim() && m_creature->isAlive())
        {
            
            //ThunderClap_Timer
            if (ThunderClap_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_THUNDERCLAP);

                //10 seconds
               ThunderClap_Timer = 10000;
            }else ThunderClap_Timer -= diff;

            //FireballVolley_Timer
            if ( m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 51 )
            {
		if (FireballVolley_Timer < diff)
	        {
                DoCast(m_creature->getVictim(),SPELL_FIREBALLVOLLEY);

                //15 seconds
                FireballVolley_Timer = 15000;
                }else FireballVolley_Timer -= diff;
            }

            //MightyBlow_Timer
            if (MightyBlow_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_MIGHTYBLOW);

                //10 seconds
               MightyBlow_Timer = 10000;
            }else MightyBlow_Timer -= diff;



            //If we are within range melee the target
            if( m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DIST))
            {
                //Make sure our attack is ready and we arn't currently casting
                if( m_creature->isAttackReady() && !m_creature->m_currentSpell)
                {
                    m_creature->AttackerStateUpdate(m_creature->getVictim());
                    m_creature->resetAttackTimer();
                }
            }
        }
    }
}; 
CreatureAI* GetAI_phalanx(Creature *_Creature)
{
    return new phalanxAI (_Creature);
}


void AddSC_phalanx()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="phalanx";
    newscript->GetAI = GetAI_phalanx;
    m_scripts[nrscripts++] = newscript;
}
