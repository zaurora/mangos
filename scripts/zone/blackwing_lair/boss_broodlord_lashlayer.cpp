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
SDName: Boss_Broodlord_Lashlayer
SD%Complete: 100
SDComment: 
SDCategory: Blackwing Lair
EndScriptData */

#include "../../sc_defines.h"

#define SPELL_CLEAVE            26350
#define SPELL_BLASTWAVE         23331
#define SPELL_MORTALSTRIKE      24573
#define SPELL_KNOCKBACK         25778

#define SAY_AGGRO               "None of your kind should be here! You've doomed only yourselves!"
#define SAY_LEASH               "Clever Mortals but I am not so easily lured away from my sanctum!"
#define SOUND_AGGRO             8286
#define SOUND_LEASH             8287

struct MANGOS_DLL_DECL boss_broodlordAI : public ScriptedAI
{
    boss_broodlordAI(Creature *c) : ScriptedAI(c) {Reset();}

    uint32 Cleave_Timer;
    uint32 BlastWave_Timer;
    uint32 MortalStrike_Timer;
    uint32 KnockBack_Timer;
    uint32 LeashCheck_Timer;
    bool InCombat;

    void Reset()
    {
        Cleave_Timer = 8000;      //These times are probably wrong
        BlastWave_Timer = 12000;
        MortalStrike_Timer = 20000;
        KnockBack_Timer = 30000;
        LeashCheck_Timer = 2000;
        InCombat = false;


        //m_creature->RemoveAllAuras();
        //m_creature->DeleteThreatList();
        //m_creature->CombatStop();
        //DoGoHome();
    }

    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && who!= m_creature)
        {
            //Begin melee attack if we are within range
            DoStartMeleeAttack(who);

            if (!InCombat)
            {
                DoYell(SAY_AGGRO, LANG_UNIVERSAL, NULL);
                DoPlaySoundToSet(m_creature, SOUND_AGGRO);
                InCombat = true;
            }
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

                //Begin melee attack if we are within range
                DoStartMeleeAttack(who);

                if (!InCombat)
                {
                    DoYell(SAY_AGGRO, LANG_UNIVERSAL, NULL);
                    DoPlaySoundToSet(m_creature, SOUND_AGGRO);
                    InCombat = true;
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //LeashCheck_Timer
        if (LeashCheck_Timer < diff)
        {
            float rx,ry,rz;
            m_creature->GetRespawnCoord(rx, ry, rz);
            float spawndist_sq = m_creature->GetDistanceSq(rx,ry,rz);
            if ( spawndist_sq > 2500 )
            { 
                EnterEvadeMode();
                return;
            }
            //2 seconds until we should check this agian
            LeashCheck_Timer = 2000;
        }else LeashCheck_Timer -= diff;

        //Cleave_Timer
        if (Cleave_Timer < diff)
        {
            //Cast
            DoCast(m_creature->getVictim(),SPELL_CLEAVE);

            //8-12 seconds until we should cast this agian
            Cleave_Timer = 8000 + rand()%4000;
        }else Cleave_Timer -= diff;

        // BlastWave
        if (BlastWave_Timer < diff)
        {
            //Cast
            DoCast(m_creature->getVictim(),SPELL_BLASTWAVE);                

            //35 seconds until we should cast this again
            BlastWave_Timer = 12000;
        }else BlastWave_Timer -= diff;

        //MortalStrike_Timer
        if (MortalStrike_Timer < diff)
        {
            //Cast
            DoCast(m_creature->getVictim(),SPELL_MORTALSTRIKE);

            //30 seconds until we should cast this agian
            MortalStrike_Timer = 25000 + rand()%10000;
        }else MortalStrike_Timer -= diff;

        if (KnockBack_Timer < diff)
        {
            //Cast
            DoCast(m_creature->getVictim(),SPELL_KNOCKBACK);

            //Drop 50% aggro
            m_creature->getThreatManager().modifyThreatPercent(m_creature->getVictim(),-50);

            //15 seconds until we should cast this agian
            KnockBack_Timer = 15000 + rand()%15000;
        }else KnockBack_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_broodlord(Creature *_Creature)
{
    return new boss_broodlordAI (_Creature);
}


void AddSC_boss_broodlord()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_broodlord";
    newscript->GetAI = GetAI_boss_broodlord;
    m_scripts[nrscripts++] = newscript;
}
