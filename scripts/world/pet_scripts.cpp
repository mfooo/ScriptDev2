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
SDName: Pet_Scripts
SD%Complete: 20%
SDComment: Here is the place for scripted AI for guardian and mini pets.
SDCategory: Pets
EndScriptData */

#include "precompiled.h"
#include "PetAI.h"
#include "Pet.h"
#include "Totem.h"
#include "SpellMgr.h"
#include "ObjectGuid.h"


struct MANGOS_DLL_DECL pet_simple_guardianAI : public PetAI
{
    /* This AI is more a kind of additional initialisation,
       that adds spells to the autocast list, defined in
       CreatureInfo::Spells.
       The AI functions are processed by PetAI.
    */
    pet_simple_guardianAI(Pet* pPet): PetAI(pPet)
    {
        // add spells if any
         if (CreatureInfo const* pPetInfo = pPet->GetCreatureInfo())
            for (uint8 i = 0; i<CREATURE_MAX_SPELLS; i++)
            {
                if (!pPetInfo->spells[i])
                  continue;

                if (SpellEntry const *spellInfo = GetSpellStore()->LookupEntry(pPetInfo->spells[i]))
                {
                    // skip spells without any cooldown
                    if (!spellInfo->StartRecoveryTime && !GetSpellRecoveryTime(spellInfo) && !(spellInfo->Attributes & SPELL_ATTR_PASSIVE))
                        continue;
                    // in case applying stat auras, we need to set it modifiable temporary
                    pPet->SetCanModifyStats(true);
                    pPet->addSpell(pPetInfo->spells[i], spellInfo->Attributes & SPELL_ATTR_PASSIVE ? ACT_PASSIVE : ACT_ENABLED);
                    pPet->SetCanModifyStats(false);
                }
            }
    }
};


/*######
## pet_dk_ghoul
#####*/

enum
{
    SPELL_DK_SCALING_01 = 54566,
    SPELL_DK_SCALING_02 = 51996,
    SPELL_LEAP          = 47482
};

struct MANGOS_DLL_DECL pet_dk_ghoulAI : public pet_simple_guardianAI
{
    pet_dk_ghoulAI(Pet* pPet) : pet_simple_guardianAI(pPet)
    {
        pPet->SetCanModifyStats(true);
        pPet->addSpell(SPELL_DK_SCALING_01);
        pPet->addSpell(SPELL_DK_SCALING_02);
        pPet->SetCanModifyStats(false);
    }

    // some hacky-hacky for "Leap" :-/
    void AttackStart(Unit *u)
    {
        Unit* oldTarget = m_creature->getVictim();
        PetAI::AttackStart(u);

        // PetAI::AttackStart was successfull
        if (m_creature->getVictim() != oldTarget && m_creature->getVictim() == u
            && !m_creature->IsNonMeleeSpellCasted(false))
        {
            float dist = m_creature->GetDistance(m_creature->getVictim());
            if (dist > 5.0f && dist < 30.0f)
                // self cast (works only like this, because target mode is buggy)
                m_creature->CastSpell(m_creature, SPELL_LEAP, false);
        }
    }
};

/*######
## pet_greater_earth_elemental
######*/

enum
{
    SPELL_AOE_TAUNT         = 36213,
    SPELL_IMMUNITY_NATURE   = 7941
};

struct MANGOS_DLL_DECL pet_greater_earth_elementalAI : public PetAI
{
    pet_greater_earth_elementalAI(Pet* pPet) : PetAI(pPet)
    {
        pPet->addSpell(SPELL_IMMUNITY_NATURE);
        Reset();
    }

    uint32 m_timer;

    void Reset()
    {
        m_timer = 500;
    }

    Unit* GetOriginalOwner()
    {
        Unit* owner = ((Pet*)m_creature)->GetOwner();
        if (!owner)
            return NULL;

        if (owner->GetTypeId() != TYPEID_UNIT || !((Creature*)owner)->IsTotem())
            return NULL;

        return ((Totem*)owner)->GetOwner();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        PetAI::UpdateAI(uiDiff);

        // kind of workaround to react also on non-hostile attackers
        if (!m_creature->getVictim())
            if (Unit* origOwner = GetOriginalOwner())
            {
                Unit* pTarget = origOwner->getAttackerForHelper();
                if (pTarget && pTarget->IsWithinDist(m_creature, 35.0f))
                    AttackStart(pTarget);
            }

        if (m_timer < uiDiff)
        {
            if (m_creature->getVictim() && !m_creature->IsNonMeleeSpellCasted(false))
            {
                m_creature->CastSpell(m_creature->getVictim(), SPELL_AOE_TAUNT, false);
                m_timer = urand(2000, 6000);
            }
        }
        else
            m_timer -= uiDiff;
    }
};


/*######
## pet_greater_fire_elemental
######*/

enum
{
    SPELL_FIRE_NOVA = 12470,
    SPELL_FIRE_BLAST = 57984,
    SPELL_IMMUNITY_FIRE = 7942
};

struct MANGOS_DLL_DECL pet_greater_fire_elementalAI : public PetAI
{
    pet_greater_fire_elementalAI(Pet* pPet) : PetAI(pPet)
    {
        // Immunity: Fire
        pPet->addSpell(SPELL_IMMUNITY_FIRE);
        Reset();
    }

    uint32 m_timer_blast, m_timer_nova;
    int32 m_damage_blast, m_damage_nova;

    void Reset()
    {
        m_timer_blast = 500;
        m_timer_nova  = 500;

        // custom damage, as the core calculated damage of this spells is quite high
        // (SPELL_ATTR_LEVEL_DAMAGE_CALCULATION)
        m_damage_blast  = 47;
        m_damage_nova   = 76;
    }

    Unit* GetOriginalOwner()
    {
        Unit* owner = ((Pet*)m_creature)->GetOwner();
        if (!owner)
            return NULL;

        if (owner->GetTypeId() != TYPEID_UNIT || !((Creature*)owner)->IsTotem())
            return NULL;

        return ((Totem*)owner)->GetOwner();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        PetAI::UpdateAI(uiDiff);

        // kind of workaround to react also on non-hostile attackers
        if (!m_creature->getVictim())
            if (Unit* origOwner = GetOriginalOwner())
            {
                Unit* pTarget = origOwner->getAttackerForHelper();
                if (pTarget && pTarget->IsWithinDist(m_creature, 35.0f))
                    AttackStart(pTarget);
            }

        // fire nova
        if (m_timer_nova < uiDiff)
        {
            if (m_creature->getVictim() && m_creature->getVictim()->IsWithinDist(m_creature, 6.0f) && !m_creature->IsNonMeleeSpellCasted(false))
            {
                m_creature->CastCustomSpell(m_creature->getVictim(), SPELL_FIRE_NOVA, &m_damage_nova, 0, 0, false);
                m_timer_nova = urand(5000, 10000);
                m_timer_blast = 3000;
            }
        }
        else
            m_timer_nova -= uiDiff;

        // fire blast
        if (m_timer_blast < uiDiff)
        {
            if (m_creature->getVictim() && !m_creature->IsNonMeleeSpellCasted(false))
            {
                // cast fireblast always if out of range, and only sometimes when in melee range
                bool cast = true;;
                if (m_creature->canReachWithAttack(m_creature->getVictim()))
                    cast = !urand(0,2) ? false : true;
                if (cast)
                    m_creature->CastCustomSpell(m_creature->getVictim(), SPELL_FIRE_BLAST, &m_damage_blast, 0, 0, false);
                m_timer_blast = 3000;
                return;
            }
        }
        else
            m_timer_blast -= uiDiff;
    }
};


CreatureAI* GetAI_pet_simple_guardian(Creature* pCreature)
{
    if (pCreature->IsPet())
        return new pet_simple_guardianAI((Pet*)pCreature);
    else
        return NULL;
}

CreatureAI* GetAI_pet_dk_ghoul(Creature* pCreature)
{
    if (pCreature->IsPet())
        return new pet_dk_ghoulAI((Pet*)pCreature);
    else
        return NULL;
}

CreatureAI* GetAI_pet_greater_earth_elemental(Creature* pCreature)
{
    if (pCreature->IsPet())
        return new pet_greater_earth_elementalAI((Pet*)pCreature);
    else
        return NULL;
}

CreatureAI* GetAI_pet_greater_fire_elemental(Creature* pCreature)
{
    if (pCreature->IsPet())
        return new pet_greater_fire_elementalAI((Pet*)pCreature);
    else
        return NULL;
}

/*######
## Risen Ghoul, Army of the Dead Ghoul
######*/

enum
{
    ENTRY_AOTD_GHOUL    = 24207,
    SPELL_CLAW          = 47468,
    SPELL_LEAP1          = 47482,
    SPELL_AURA_TAUNT    = 43264
};

struct MANGOS_DLL_DECL pet_risen_ghoulAI : public ScriptedAI
{
    pet_risen_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsReady = false;
        m_bIsSpawned = false;
        fDist = (m_creature->GetEntry() == ENTRY_AOTD_GHOUL) ? float(urand(1, 5) ) : PET_FOLLOW_DIST;
        fAngle = PET_FOLLOW_ANGLE;
        m_uiCreatorGUID = m_creature->GetCreatorGuid();
        if (Unit* pOwner = m_creature->GetMap()->GetUnit(m_uiCreatorGUID))
            fAngle = m_creature->GetAngle(pOwner);

        Reset();
    }

    Unit* pTarget;

    ObjectGuid m_uiCreatorGUID;
    uint64 m_uiTargetGUID;

    uint32 m_uiReadyTimer;
    uint32 m_uiClawTimer;
    uint32 m_uiLeapTimer;

    bool m_bIsReady;
    bool m_bIsSpawned;

    float fDist;
    float fAngle;


    void Reset()
    {
        pTarget         = NULL;
        m_uiTargetGUID  = 0;
        m_uiReadyTimer  = 4000;
        m_uiClawTimer   = urand(3000, 5000);
        m_uiLeapTimer   = urand(1000, 5000);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!m_bIsReady)
            return;

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackStart(Unit *pWho)
    {
        if (!m_bIsReady)
            return;

        ScriptedAI::AttackStart(pWho);
    }
/*  this needs further research
    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (m_creature->GetEntry() == ENTRY_AOTD_GHOUL)
            return;

        switch(emote)
        {
            case TEXTEMOTE_GLARE:
                m_creature->HandleEmote(EMOTE_ONESHOT_COWER);
                break;
            case TEXTEMOTE_COWER:
                m_creature->HandleEmote(EMOTE_ONESHOT_OMNICAST_GHOUL);
                break;
        }
    }
*/
    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_bIsReady)
        {
            if (!m_bIsSpawned)
            {
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                m_bIsSpawned = true;
            }

            if (m_uiReadyTimer <= uiDiff)
            {
                m_bIsReady = true;
                if (m_creature->GetEntry() == ENTRY_AOTD_GHOUL)
                    DoCastSpellIfCan(m_creature, SPELL_AURA_TAUNT, CAST_TRIGGERED);
            }
            else m_uiReadyTimer -= uiDiff;

            return;
        }

        Unit* pOwner = m_creature->GetMap()->GetUnit(m_uiCreatorGUID);
        if (!pOwner || !pOwner->IsInWorld())
        {
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
            return;
        }

        // check if current target still exists and is atatckable
        if (m_creature->getVictim() )
            m_uiTargetGUID = m_creature->getVictim()->GetGUID();

        pTarget = m_creature->GetMap()->GetUnit(m_uiTargetGUID);

        if (!pTarget || !m_creature->CanInitiateAttack() || !pTarget->isTargetableForAttack() ||
        !m_creature->IsHostileTo(pTarget) || !pTarget->isInAccessablePlaceFor(m_creature))
        {
            // we have no target, so look for the new one
            if (Unit *pTmp = m_creature->SelectRandomUnfriendlyTarget(0, 30.0f) )
                m_uiTargetGUID = pTmp->GetGUID();

            pTarget = m_creature->GetMap()->GetUnit(m_uiTargetGUID);

            // now check again. if no target found then there is nothing to attack - start following the owner
            if (!pTarget || !m_creature->CanInitiateAttack() || !pTarget->isTargetableForAttack() ||
            !m_creature->IsHostileTo(pTarget) || !pTarget->isInAccessablePlaceFor(m_creature))
            {
                if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE)
                {
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MoveFollow(pOwner, fDist, fAngle);
                    Reset();
                }
                return;
            }
            if (pTarget)
                m_creature->AI()->AttackStart(pTarget);
        }

        // Claw
        if (m_uiClawTimer <= uiDiff)
        {
            DoCastSpellIfCan(pTarget, SPELL_CLAW);
            m_uiClawTimer = urand(3000, 5000);
        }
        else m_uiClawTimer -= uiDiff;

        // Leap
        if (m_uiLeapTimer <= uiDiff)
        {
            if (Unit *pLeapTarget = m_creature->SelectRandomUnfriendlyTarget(m_creature->getVictim(), 30.0f) )
            {
                if (pLeapTarget != pTarget)
                {
                    DoCastSpellIfCan(pLeapTarget, SPELL_LEAP1, CAST_TRIGGERED);
                    m_uiLeapTimer = 20000;
                    m_uiTargetGUID = pLeapTarget->GetGUID();
                    m_creature->AI()->AttackStart(pLeapTarget);
                    return;
                }
            }
        }
        else m_uiLeapTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
 
CreatureAI* GetAI_pet_risen_ghoul(Creature* pCreature)
{
    return new pet_risen_ghoulAI (pCreature);
};

void AddSC_pets()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "pet_simple_guardian";
    newscript->GetAI = &GetAI_pet_simple_guardian;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "pet_dk_ghoul";
    newscript->GetAI = &GetAI_pet_dk_ghoul;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "pet_greater_earth_elemental";
    newscript->GetAI = &GetAI_pet_greater_earth_elemental;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "pet_greater_fire_elemental";
    newscript->GetAI = &GetAI_pet_greater_fire_elemental;
    newscript->RegisterSelf();
	
	newscript = new Script;
    newscript->Name = "pet_risen_ghoul";
    newscript->GetAI = &GetAI_pet_risen_ghoul;
    newscript->RegisterSelf();
}
