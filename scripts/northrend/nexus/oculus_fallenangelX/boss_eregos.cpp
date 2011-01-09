/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: oculus
SD%Complete: 45%
SDComment: // dev // FallenAngelX    
SDCategory: Ley-Guardian Eregos
ToDo:: summons 4 whelps during the fight  use the whelps id in zone  and during his planar shift he summons 2 planar anomalies
planar shifts spells need implented
EndScriptData */

#include "precompiled.h"
#include "oculus.h"


/*###
#Ley-Guardian Eregos
####*/
enum
{
	SAY_AGGRO               = -1000048,
	SAY_DEATH               = -1000043,
	
	SPELL_ARCANE_BARRAGE    = 50804,
	SPELL_ARCANE_BARRAGE_H  = 59381,
	SPELL_ARCANE_VOLLEY     = 51153,
	SPELL_ARCANE_VOLLEY_H   = 59382,
	SPELL_ENRAGE            = 51170,
	SPELL_PLANAR_SHIFT      = 51162,
	SPELL_ARCANE_SHIELD     = 53813,
    SPELL_PLANAR_BLAST      = 57976,
	SPELL_PLANAR_ANOMALIES  = 57959
};

enum adds
{
	PLANAR_ANOMALY				= 30879
};

struct MANGOS_DLL_DECL boss_eregosAI: public ScriptedAI
{
	boss_eregosAI(Creature* c): ScriptedAI(c)
	{
		m_pInstance = (ScriptedInstance*)m_creature->GetInstanceData();
		m_bIsRegularMode = m_creature->GetMap()->IsRegularDifficulty();
		Aggroed = false;
		Reset();
	}

	ScriptedInstance* m_pInstance;
	bool m_bIsRegularMode;

	uint32 ArcaneBarrageTimer;
	uint32 ArcaneVolleyTimer;
	uint32 EnrageTimer;
	uint8 PlanarShiftTimer;
	
	uint32 HeightEnrageTimer;

	bool Aggroed;
	bool Moving;
    bool PlanarShift60;
	bool PlanarShift20;
	
	void Reset()
	{
		ArcaneBarrageTimer = 15000;
		ArcaneVolleyTimer = 15000;
		EnrageTimer = urand(15000,20000);
		HeightEnrageTimer = 5000;
		Moving = false;
		PlanarShift60 = false;
	    PlanarShift20 = false;
	}

	void JustDied(Unit* pKiller)
	{
	    DoScriptText(SAY_DEATH, m_creature);
		if(m_pInstance)
			m_pInstance->SetData(TYPE_EREGOS,DONE);
	}

	void Aggro(Unit*)
	{
	    DoScriptText(SAY_AGGRO,m_creature);
		Aggroed = true;
	}

	void SpellHit(Unit* pCaster, const SpellEntry* spell)
	{
	}

	void UpdateAI(const uint32 diff)
	{
		if(!m_creature->getVictim() || !m_creature->SelectHostileTarget())
		{
			if(Aggroed)
			{
				Aggroed = false;
				EnterEvadeMode();
			}
			return;
		}

		if(!(m_creature->getVictim()->IsWithinDistInMap(m_creature,40.0f)))
		{
			if(!Moving)
			{
				m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
				Moving = true;
			}
		}
		else
		{
			if(Moving)
			{
				m_creature->GetMotionMaster()->MoveIdle();
				Moving = false;
			}
		}

		if(m_creature->GetPositionZ() < 645.0f)
		{
			if(HeightEnrageTimer <= diff)
			{
				DoCastSpellIfCan(m_creature->getVictim(),5,true);
				DoCastSpellIfCan(m_creature,23257,true);
				HeightEnrageTimer = 5000;
			} else HeightEnrageTimer -= diff;
		}
		else
			HeightEnrageTimer = 5000;

		if(ArcaneBarrageTimer <= diff)
		{
			if(DoCastSpellIfCan(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0),m_bIsRegularMode?SPELL_ARCANE_BARRAGE:SPELL_ARCANE_BARRAGE_H) == CAST_OK)
				ArcaneBarrageTimer = urand(15000,16000);
		} else ArcaneBarrageTimer -= diff;

		if(ArcaneVolleyTimer <= diff)
		{
			if(DoCastSpellIfCan(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0),m_bIsRegularMode?SPELL_ARCANE_VOLLEY:SPELL_ARCANE_VOLLEY_H) == CAST_OK)
				ArcaneVolleyTimer = urand(15000,16000);
		} else ArcaneVolleyTimer -= diff;

		if(EnrageTimer <= diff)
		{
			DoCastSpellIfCan(m_creature,SPELL_ENRAGE,true);
			EnrageTimer = urand(20000,25000);
		} else EnrageTimer -= diff;
	
    	DoMeleeAttackIfReady();
	}
	
};

CreatureAI* GetAI_boss_eregos(Creature* pCreature)
{
	return new boss_eregosAI(pCreature);
}

void AddSC_boss_eregos()
{
	Script* newscript;

	newscript = new Script;
	newscript->Name = "boss_eregos";
	newscript->GetAI = &GetAI_boss_eregos;
	newscript->RegisterSelf();
}
