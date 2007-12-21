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
SDName: Boss_Mother_Shahraz
SD%Complete: 80
SDComment: Prismatic shields randomly applied instead of correct "based on damage taken"
SDCategory: Black Temple
EndScriptData */

#include "../../sc_defines.h"
#include "../../../../../game/Player.h"
#include "../../../../../game/TargetedMovementGenerator.h"

//Spells
#define SPELL_BEAM_SINISTER          40859
#define SPELL_BEAM_VILE          40860
#define SPELL_BEAM_WICKED          40861
#define SPELL_BEAM_SINFUL          40827
#define SPELL_PRISMATIC_SHADOW          40880
#define SPELL_PRISMATIC_FIRE          40882
#define SPELL_PRISMATIC_NATURE          40883
#define SPELL_PRISMATIC_ARCANE          40891
#define SPELL_PRISMATIC_FROST          40896
#define SPELL_PRISMATIC_HOLY          40897
#define SPELL_ATTRACTION          40871
#define SPELL_ENRAGE          23537

//Speech'n'Sounds
#define SAY_TAUNT1          "You play, you pay."
#define SOUND_TAUNT1          11501

#define SAY_TAUNT2          "I'm not impressed."
#define SOUND_TAUNT2          11502

#define SAY_TAUNT3          "Enjoying yourselves?"
#define SOUND_TAUNT3          11503

#define SAY_AGGRO          "So, business... Or pleasure?"
#define SOUND_AGGRO          11504

#define SAY_SPELL1          "You seem a little tense."
#define SOUND_SPELL1          11505

#define SAY_SPELL2          "Don't be shy."
#define SOUND_SPELL2          11506

#define SAY_SPELL3          "I'm all... yours."
#define SOUND_SPELL3          11507

#define SAY_SLAY1          "Easy come, easy go."
#define SOUND_SLAY1          11508

#define SAY_SLAY2          "So much for a happy ending."
#define SOUND_SLAY2          11509

#define SAY_ENRAGE          "Stop toying with my emotions!"
#define SOUND_ENRAGE          11510

#define SAY_DEATH          "I wasn't... finished."
#define SOUND_DEATH          11511


struct MANGOS_DLL_DECL boss_shahrazAI : public ScriptedAI
{
    boss_shahrazAI(Creature *c) : ScriptedAI(c) 
    {
        if(c->GetInstanceData())
            pInstance = ((ScriptedInstance*)c->GetInstanceData());
        else pInstance = NULL;
        SetVariables();
    }

    ScriptedInstance* pInstance;

    uint64 BombGUID;

    uint32 BeamTimer;
    uint32 BeamCounter;
    uint32 ElementalShieldTimer;
    uint32 FatalAttractionTimer;
    uint32 FatalAttractionExplodeTimer;
    uint32 RandomYellTimer;
    uint32 EnrageTimer;
    uint32 Multiplier;
    uint32 ExplosionCount;

    bool InCombat;
    bool HasSummonedBomb;

    void SetVariables()
    {
        if(pInstance)
            pInstance->SetData("MotherShahrazEvent", 0);

        BombGUID = 0;

        BeamTimer = 60000;
        BeamCounter = 4;
        ElementalShieldTimer = 0;
        FatalAttractionTimer = 60000;
        FatalAttractionExplodeTimer = 3000;
        RandomYellTimer = 70000 + rand()%41 * 1000;
        EnrageTimer = 600000;
        Multiplier = 1;
        ExplosionCount = 0;

        HasSummonedBomb = false;
        InCombat = false;
    }

    void EnterEvadeMode()
    {
        InCombat = false;

        (*m_creature).GetMotionMaster()->Clear(false);
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop();
        DoGoHome();
    }

    void AttackStart(Unit *who)
    {
        if(!who)
            return;

        if(who->isTargetableForAttack() && who!= m_creature)
        {
            DoStartMeleeAttack(who);

            if(!InCombat)
            {
                if(pInstance)
                    pInstance->SetData("MotherShahrazEvent", 1);

                DoYell(SAY_AGGRO,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature, SOUND_AGGRO);
                InCombat = true;
            }
        }
    }

    void MoveInLineOfSight(Unit *who)
    {

        if(who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
        {
            float attackRadius = m_creature->GetAttackDistance(who);
            if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE && m_creature->IsWithinLOSInMap(who))
            {
                if(who->HasStealthAura())
                    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                if(who && who->isAlive())
                    m_creature->AddThreat(who, 1.0f);

                if(!InCombat)
                {
                    if(pInstance)
                        pInstance->SetData("MotherShahrazEvent", 1);
                    DoYell(SAY_AGGRO,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature, SOUND_AGGRO);
                    InCombat = true;
                }
            }
        }
    }    

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
        case 0:
            DoYell(SAY_SLAY1,LANG_UNIVERSAL,NULL);
            DoPlaySoundToSet(m_creature, SOUND_SLAY1);
            break;
        case 1:
            DoYell(SAY_SLAY2,LANG_UNIVERSAL,NULL);
            DoPlaySoundToSet(m_creature, SOUND_SLAY2);
            break;
        }
    }

    void JustDied(Unit *victim)
    {
        if(pInstance)
            pInstance->SetData("MotherShahrazEvent", 3);

        DoYell(SAY_DEATH, LANG_UNIVERSAL, NULL);
        DoPlaySoundToSet(m_creature,SOUND_DEATH);
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        //Randomly cast one beam.
        if(BeamTimer < diff)
        {
            if(m_creature->GetHealth()*30 <= m_creature->GetMaxHealth())
            {
                if(BeamCounter < 4)
                {
                    switch(rand()%3)
                    {
                    case 0:
                        DoCast(m_creature->getVictim(), SPELL_BEAM_SINISTER);
                        break;
                    case 1:
                        DoCast(m_creature->getVictim(), SPELL_BEAM_WICKED);
                        break;
                    case 2:
                        DoCast(m_creature->getVictim(), SPELL_BEAM_VILE);
                        break;
                    }
                    BeamCounter++;
                }
                else
                {
                    DoCast(m_creature->getVictim(), SPELL_BEAM_SINFUL);
                    BeamCounter = 0;
                    DoYell(SAY_SPELL1,LANG_UNIVERSAL,NULL);
                    DoPlaySoundToSet(m_creature, SOUND_SPELL1);
                }
            }
            else
            {
                switch(rand()%3)
                {
                case 0:
                    DoCast(m_creature->getVictim(), SPELL_BEAM_SINISTER);
                    break;
                case 1:
                    DoCast(m_creature->getVictim(), SPELL_BEAM_WICKED);
                    break;
                case 2:
                    DoCast(m_creature->getVictim(), SPELL_BEAM_VILE);
                    break;
                }
            }
            BeamTimer = 40000 / Multiplier;
        }else BeamTimer -= diff;

        // Random Prismatic Shield every 15 seconds. 
        // This is a workaround as it should be cast on the player depending on what spell school he damaged with
        // It should stack based on damage.
        if(ElementalShieldTimer < diff)
        {
            switch(rand()%6)
            {
            case 0:
                DoCast(m_creature, SPELL_PRISMATIC_SHADOW);
                break;
            case 1:
                DoCast(m_creature, SPELL_PRISMATIC_FIRE);
                break;
            case 2:
                DoCast(m_creature, SPELL_PRISMATIC_NATURE);
                break;
            case 3:
                DoCast(m_creature, SPELL_PRISMATIC_ARCANE);
                break;
            case 4:
                DoCast(m_creature, SPELL_PRISMATIC_FROST);
                break;
            case 5:
                DoCast(m_creature, SPELL_PRISMATIC_HOLY);
                break;
            }
            ElementalShieldTimer = 15000;
        }else ElementalShieldTimer -= diff;

        // Select 3 random targets (can select same target more than once), teleport to creature then cast an explosion.
        // Targets have ~3 seconds to run away.
        if(FatalAttractionTimer < diff)
        {
            for(uint32 i = 0; i<4; i++)
            {
                Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (target && target->GetTypeId() == TYPEID_PLAYER) // Teleport Z + 2 since they can fall through the ground if VMaps is disabled
                    ((Player*)target)->TeleportTo(m_creature->GetMapId(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+2, target->GetOrientation());
            }

            switch(rand()%2)
            {
            case 0:
                DoYell(SAY_SPELL2,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature, SOUND_SPELL2);
                break;
            case 1:
                DoYell(SAY_SPELL3,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature, SOUND_SPELL3);
                break;
            }
            FatalAttractionExplodeTimer = 4000;
            Creature* Bomb = NULL; // This creature casts the explosion spell.
            FatalAttractionTimer = 40000 + rand()%31 * 1000;
            Bomb = m_creature->SummonCreature(23085, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 5, TEMPSUMMON_TIMED_DESPAWN, 10000);
            if(Bomb)
            {
                BombGUID = Bomb->GetGUID();
                Bomb->setFaction(m_creature->getFaction());
                Bomb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); // Unselectable. Players shouldn't be able to kill it.
                Bomb->SetUInt32Value(UNIT_FIELD_DISPLAYID, 11686); // Invisible
            }              
        }else FatalAttractionTimer -= diff;

        if(FatalAttractionExplodeTimer < diff)
        {
            if(BombGUID)
            {
                Creature* Bomb = NULL;
                Bomb = ((Creature*)Unit::GetUnit((*m_creature), BombGUID));
                if(ExplosionCount < 3)
                {            
                    if(Bomb)
                    {
                        Bomb->CastSpell(Bomb, SPELL_ATTRACTION, false);
                        FatalAttractionExplodeTimer = 1000;
                        ExplosionCount++;
                    }
                }
                else
                {
                    FatalAttractionExplodeTimer = FatalAttractionTimer + 4000;
                    ExplosionCount = 0;
                    BombGUID = NULL;
                }
            }
        }else FatalAttractionExplodeTimer -= diff;


        //Enrage
        if(EnrageTimer < diff)
        {
            DoCast(m_creature, SPELL_ENRAGE);
            DoYell(SAY_ENRAGE,LANG_UNIVERSAL,NULL);
            DoPlaySoundToSet(m_creature, SOUND_ENRAGE);
            Multiplier++;
            EnrageTimer = 600000;
        }else EnrageTimer -= diff;

        //Random taunts
        if(RandomYellTimer < diff)
        {
            switch(rand()%3)
            {
            case 0:
                DoYell(SAY_TAUNT1,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature, SOUND_TAUNT1);
                break;
            case 1:
                DoYell(SAY_TAUNT2,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature, SOUND_TAUNT2);
                break;
            case 2:
                DoYell(SAY_TAUNT3,LANG_UNIVERSAL,NULL);
                DoPlaySoundToSet(m_creature, SOUND_TAUNT3);
                break;
            }
            RandomYellTimer = 60000 + rand()%91 * 1000;
        }else RandomYellTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_shahraz(Creature *_Creature)
{
    return new boss_shahrazAI (_Creature);
}


void AddSC_boss_mother_shahraz()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_mother_shahraz";
    newscript->GetAI = GetAI_boss_shahraz;
    m_scripts[nrscripts++] = newscript;
}
