/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_malor_the_zelous
SD%Complete: 100
SDComment: 
EndScriptData */

#include "sc_creature.h"

// **** This script is still under Developement ****

//Status: check for correct spellID(layonhands), send to SVN

#define SPELL_GROUNDSMASH    12734
#define SPELL_LAYONHANDS    10310

struct MANGOS_DLL_DECL boss_malor_the_zealousAI : public ScriptedAI
{
    boss_malor_the_zealousAI(Creature *c) : ScriptedAI(c) {Reset();}

    uint32 GroundSmash_Timer;
    bool LayOnHands;
    bool InCombat;

    void Reset()
    {
        GroundSmash_Timer = 4000;
        LayOnHands = false;
        InCombat = false;

        //m_creature->RemoveAllAuras();
        //m_creature->DeleteThreatList();
        //m_creature->CombatStop();
        //DoGoHome();
    }

    void Aggro(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //GroundSmash
        if (GroundSmash_Timer < diff)
        {
            //Cast
            DoCast(m_creature->getVictim(),SPELL_GROUNDSMASH);

            //10 seconds until we should cast this again
            GroundSmash_Timer = 10000;
        }else GroundSmash_Timer -= diff;

        //LayOnHands
        if (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() < 3)
        {
            if (!LayOnHands)
            {
                //Cast
                DoCast(m_creature,SPELL_LAYONHANDS);
                LayOnHands = true;
            }
        }

        DoMeleeAttackIfReady();
    }
}; 
CreatureAI* GetAI_boss_malor_the_zealous(Creature *_Creature)
{
    return new boss_malor_the_zealousAI (_Creature);
}


void AddSC_boss_malor_the_zealous()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_malor_the_zealous";
    newscript->GetAI = GetAI_boss_malor_the_zealous;
    m_scripts[nrscripts++] = newscript;
}
