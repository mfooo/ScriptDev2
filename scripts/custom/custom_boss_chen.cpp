/* ScriptData
SDName: custom_boss_chen
SD%Complete: 
SDComment: 
SDCategory: Lore shiz
EndScriptData */

#include "precompiled.h"

enum
{
	SPELL_WHIRLWIND        = 41056, /* 33238 */
	SPELL_KICK             = 24671,
	SPELL_SHADOWLANCE      = 69058, // unlimited range
	SPELL_RAINOFFIRE       = 57757, // 40 yd
	SPELL_ARCANEVOLLEY     = 59483, // 20 yd
	SPELL_DRAINLIFE        = 70213, // 20 yd
	SPELL_ENRAGE           = 28131,
	SPELL_BERSERK          = 47008,

	NPC_STORM_FURY         = 7105001,
	NPC_FROSTBLAZE         = 7105002,

	SPELL_AURA_WHIRL     = 38464,
	SPELL_AURA_SPARK     = 54141,
	SPELL_AURA_SPARK_2   = 55845,

	WHIRLWIND_CD		= 10000,
	KICK_CD				= 7000,
	SHADOWLANCE_CD      = 10000,
	DRAINLIFE_CD		= 25000,
	RAINOFFIRE_CD       = 30000,
	ARCANEVOLLEY_CD     = 35000,
	SUMMON_STORMFURY_CD = 1*MINUTE*IN_MILLISECONDS,
	BERSERK_CD          = 10*MINUTE*IN_MILLISECONDS/*,


	EVENT_FAIL				= 0,
	EVENT_IN_PROGRESS		= 1,
	EVENT_DONE				= 2
*/
};

/*
#define SPELL_SHIELD
#define SPELL_BOOM_VISUAL
#define SPELL_BOOM_DAMAGE
#define SPELL_EVADE
#define SPELL_CLOAK
#define SPELL_KNOCKBACK
*/

	// uint8 Event;

struct MANGOS_DLL_DECL custom_boss_chenAI : public ScriptedAI
{
    custom_boss_chenAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		Reset();
	}
	
    uint32 Whirlwind_Timer;
	uint32 Kick_Timer;
	uint32 Shadowlance_Timer;
	uint32 Drainlife_Timer;
	uint32 Rainoffire_Timer;
	uint32 Arcanevolley_Timer;
	uint32 Summon_Stormfury_Timer;
    uint32 Berserk_Timer;
	bool Enraged;
	bool Berserk;
	bool Aggroed;
	uint8 stage;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);

		Whirlwind_Timer = urand(9000,11000);
		Kick_Timer = urand(12000,14000);
		Shadowlance_Timer = urand(500,1000);
		Drainlife_Timer = urand(28000,32000);
		Rainoffire_Timer = urand(10000,15000);
		Arcanevolley_Timer = urand(19000,21000);
		Summon_Stormfury_Timer = urand(5000,6000);
        Berserk_Timer = BERSERK_CD;

		Enraged = false;
		Berserk = false;
		Aggroed = false;
		
		stage = 0;
		// Event = EVENT_FAIL;
    }

	void JustReachedHome()
	{
		Aggroed = false;
		// Event = EVENT_FAIL;
		return;
	}

	void Aggro(Unit *who) 
    {
        if (!Aggroed)
			DoScriptText(-1568017,m_creature,who);
		Aggroed = true;
		// Event = EVENT_IN_PROGRESS;
		return;
    }

	void JustDied(Unit *)
	{
		// Event = EVENT_DONE;
		return;
	}

    void UpdateAI(const uint32 diff)
    {
        //return if no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
		
		if (stage == 0 && m_creature->GetHealthPercent() < 50.0f) { stage = 1; }

			//whirlwind near main target
		    if (Whirlwind_Timer < diff)
			{
				DoCastSpellIfCan(m_creature->getVictim(),SPELL_WHIRLWIND);
				Whirlwind_Timer = WHIRLWIND_CD + urand(0,2000);
			}else Whirlwind_Timer -= diff;

			//kick on random melee
			if (Kick_Timer < diff)
			{
				if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
					if (!m_creature->IsWithinDistInMap(target, 5.0f))
					{
						DoCastSpellIfCan(target,SPELL_KICK);
						Kick_Timer = KICK_CD + urand(0,2000);
					}
			}else if (Kick_Timer > 0) Kick_Timer -= diff;

/*
		//kick on highest aggro
        if (Kick_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO,0))
				{
					DoCastSpellIfCan(target,SPELL_KICK);
					Kick_Timer = KICK_CD + urand(0,2000);
				}
        }else if (Kick_Timer > 0) Kick_Timer -= diff;
*/


			//shadowlance on random attacker more than 20 yd away if hp > 50%
			if (stage==0 && Shadowlance_Timer < diff)
			{
				if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
					if (!m_creature->IsWithinDistInMap(target, 20.0f))
				{
					DoCastSpellIfCan(target,SPELL_SHADOWLANCE);
					Shadowlance_Timer = SHADOWLANCE_CD + urand(0,200);
				}
			}else if (Shadowlance_Timer > 0) Shadowlance_Timer -= diff;

			//rain of fire on random attacker more than 20 yd away
			if (Rainoffire_Timer < diff)
			{
				if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
					if (!m_creature->IsWithinDistInMap(target, 20.0f))
					{
						DoCastSpellIfCan(target,SPELL_RAINOFFIRE);
						Rainoffire_Timer = RAINOFFIRE_CD + urand(0,100);
					}
			}else if (Rainoffire_Timer > 0) Rainoffire_Timer -= diff;

			//arcane volley on all enemies
			if (Arcanevolley_Timer < diff)
			{
				DoCastSpellIfCan(m_creature->getVictim(),SPELL_ARCANEVOLLEY);
				Arcanevolley_Timer = ARCANEVOLLEY_CD + urand(0,2000);
			}else Arcanevolley_Timer -= diff;

			//drain random attacker if hp < 50%
			if (stage != 0 && Drainlife_Timer < diff)
			{
				if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
				{
					DoCastSpellIfCan(target,SPELL_DRAINLIFE);
					Drainlife_Timer = DRAINLIFE_CD + urand(-2000,2000);
				}
			}else Drainlife_Timer -= diff;

/*
	void DoHatefulStrike()
    {
        // The ability is used on highest HP target choosen of the top 2 (3 heroic) targets on threat list being in melee range
        Unit* pTarget = NULL;
        uint32 uiHighestHP = 0;
        uint32 uiTargets = m_bIsRegularMode ? 2 : 3;

        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator iter = tList.begin();iter != tList.end(); ++iter)
        {
            if (!uiTargets)
                break;

            if (Unit* pTempTarget = m_creature->GetMap()->GetUnit((*iter)->getUnitGuid()))
            {
                if (pTempTarget->GetHealth() > uiHighestHP && m_creature->IsWithinDistInMap(pTempTarget, ATTACK_DISTANCE))
                {
                    uiHighestHP = pTempTarget->GetHealth();
                    pTarget = pTempTarget;
                }
            }
            --uiTargets;
        }
        if (pTarget)
            DoCast(pTarget, m_bIsRegularMode ? SPELL_HATEFULSTRIKE : SPELL_HATEFULSTRIKE_H);
    }
*/

			//soft enrage at 5%
			if (!Enraged)
			{
				if (m_creature->GetHealthPercent() < 5.0f)
				{
					if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
					{
						Enraged = true;
					}
				}
			}

			//berserk once
			if (!Berserk)
			{
				if (Berserk_Timer < diff)
				{
					if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
					{
						Berserk = true;
					}
				}
				else
					Berserk_Timer -= diff;
			}

			// summon tornado chasing random target
			if (Summon_Stormfury_Timer < diff)
			{
				if (Unit* Chased = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
					DoScriptText(-1632006,m_creature);
                    float fPosX, fPosY, fPosZ;
                    m_creature->GetPosition(fPosX, fPosY, fPosZ);
                    // doSummon(NPC_SOUL_FRAGMENT,fPosX, fPosY, fPosZ);
					m_creature->SummonCreature(NPC_STORM_FURY, fPosX + 10.0f, fPosY, fPosZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, DAY);
                }
				Summon_Stormfury_Timer = SUMMON_STORMFURY_CD + urand(-2000,2000);

			} else Summon_Stormfury_Timer -= diff;


/*
			if  (timedQuery(SPELL_CORRUPT_SOUL, diff))
                        {
                            if (Unit* pTarget= m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                {
                                if (doCast(SPELL_CORRUPT_SOUL, pTarget) == CAST_OK)
                                    {
                                    DoScriptText(-1632006,m_creature,pTarget);
                                    float fPosX, fPosY, fPosZ;
                                    pTarget->GetPosition(fPosX, fPosY, fPosZ);
                                    doSummon(NPC_SOUL_FRAGMENT,fPosX, fPosY, fPosZ);
                                    }
                                }
                        }
*/

			DoMeleeAttackIfReady();
		}

};


// add chased player
struct MANGOS_DLL_DECL custom_mob_storm_furyAI : public ScriptedAI
{
    custom_mob_storm_furyAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    Unit* Chased;
    uint32 m_uiRangeCheck_Timer;

	void Reset()
    {
        m_uiRangeCheck_Timer = 1000;
        m_creature->SetSpeedRate(MOVE_RUN, 0.2f);
		Chased = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
		m_creature->GetMotionMaster()->MoveChase(Chased);
        m_creature->SetRespawnDelay(DAY);
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!m_creature->SelectHostileTarget())
		{
			m_creature->ForcedDespawn();
			return;
		}

/*
        if (Event != EVENT_IN_PROGRESS)
		{
				m_creature->ForcedDespawn();
				return;
		}
*/

        if (m_uiRangeCheck_Timer < uiDiff)
        {
            if (Chased->IsWithinDistInMap(m_creature, 2.0f))
            {
                m_creature->ForcedDespawn();
            } else m_creature->GetMotionMaster()->MoveChase(Chased);

            m_uiRangeCheck_Timer = 1000;
        }
        else m_uiRangeCheck_Timer -= uiDiff;
    }

};


CreatureAI* GetAI_custom_boss_chen(Creature* pCreature)
{
    return new custom_boss_chenAI(pCreature);
}

CreatureAI* GetAI_custom_mob_storm_fury(Creature* pCreature)
{
    return new custom_mob_storm_furyAI (pCreature);
}

void AddSC_custom_boss_chen()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "custom_boss_chen";
    newscript->GetAI = &GetAI_custom_boss_chen;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "custom_mob_storm_fury";
    newscript->GetAI = &GetAI_custom_mob_storm_fury;
    newscript->RegisterSelf();
}
