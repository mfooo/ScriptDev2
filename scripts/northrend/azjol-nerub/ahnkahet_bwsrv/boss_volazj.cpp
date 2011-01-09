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
SDName: Boss_Volazj
SD%Complete: 100%
SDComment: Full
SDAuthor: MaxXx2021 aka Mioka
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"
#include "ahnkahet.h"
#include "TemporarySummon.h"

enum
{
    SPELL_MIND_FLAY                 = 57941,
    SPELL_MIND_FLAY_H               = 59974,
    SPELL_SHADOW_BOLT               = 57942,
    SPELL_SHADOW_BOLT_H             = 59975,

    //Shiver - horrible ability
    SPELL_SHIVER                    = 57949, //Jump Aura 
    SPELL_SHIVER_H                  = 59978,
    SPELL_SHIVER_DMG                = 57952, //Damage self target
    SPELL_SHIVER_DMG_H              = 59979,

    SPELL_INSANITY                  = 57496, //This is what volajz casts, it should trigger Twisted Visage spawn spells
    SPELL_INSANITY_COPY_CASTER      = 57507, //For use in code
    SPELL_INSANITY_SUMMON_1         = 57500, //copy weapon and summon creature
    SPELL_INSANITY_SUMMON_2         = 57501,
    SPELL_INSANITY_SUMMON_3         = 57502,
    SPELL_INSANITY_SUMMON_4         = 57503,
    SPELL_INSANITY_SUMMON_5         = 57504,
    SPELL_INSANITY_PHASE_1          = 57508, //invis spells
    SPELL_INSANITY_PHASE_2          = 57509,
    SPELL_INSANITY_PHASE_3          = 57510,
    SPELL_INSANITY_PHASE_4          = 57511,
    SPELL_INSANITY_PHASE_5          = 57512,
    SPELL_INSANITY_CHANNEL          = 57561, //Just for visual, Volazj cast this when players are in insanity
    
    NPC_TWISTED_VISAGE              = 30621,

    SAY_AGGRO                       = -1619033,
    SAY_INSANITY                    = -1619034,
    SAY_SLAY_1                      = -1619035,
    SAY_SLAY_2                      = -1619036,
    SAY_SLAY_3                      = -1619037,
    SAY_DEATH_1                     = -1619038,
    SAY_DEATH_2                     = -1619039,

    PHASE_FIGHT                     = 1,
    PHASE_INSANITY_1                = 2,  // Wait five seconds until cast is complete, set unattackable
    PHASE_INSANITY_2                = 3, 

    //Class Spells
    //DRUID
    //Caster
    SPELL_WRATH                     = 57466,
    SPELL_ROOT                      = 57095,
    SPELL_SPIKE                     = 53307,
    SPELL_HEAL                      = 53607,
    SPELL_MOON_FIRE                 = 8925,

    //Fury
    SPELL_CATFANGS                  = 67982,
    SPELL_BEARSTUN                  = 43612,
    SPELL_BEARBASH                  = 53498,
    SPELL_BEARBLOOD                 = 61896,

    //HUNTER
    SPELL_MONKEY                    = 13163,   
    SPELL_POISON                    = 13549,
    SPELL_ARCANE                    = 14282,
    SPELL_STEDY                     = 49052,
    SPELL_SHOOT                     = 65868,

    //WARLOCK
    SPELL_SHIELD                    = 47893,
    SPELL_MESHOK                    = 47813,
    SPELL_AGONY                     = 47864,
    SPELL_FIRE                      = 1094,
    SPELL_SB                        = 18111,
    SPELL_FIRE_BONUS                = 38401,  
    SPELL_MAGIC_STUN                = 56733,

    //MAGE
    SPELL_FIREBALL                  = 21159,
    SPELL_FREEZE_ARMOR              = 7301,
    SPELL_ARCANE_MISSILE            = 10273,
    SPELL_ARCANE_BLOW               = 59245,

    //ROGUE
    SPELL_SINISTER                  = 8621,
    SPELL_STUN                      = 72335,
    SPELL_STUN2                     = 31843,
    SPELL_BLOOD                     = 15583,
    SPELL_AMBUSH                    = 39669,
    SPELL_LAST                      = 11299,

    //WARRIOR
    SPELL_HERO_STRIKE               = 52221,
    SPELL_BLOODY                    = 18106,
    SPELL_CLEAVE                    = 47520,
    SPELL_ROAR                      = 64062,
    SPELL_THUNDER                   = 43583,

    //PALADIN
    SPELL_HOLY_LIGHT                = 3472,
    SPELL_LIGHT_STUN                = 66940,
    SPELL_SPELL_AURA                = 19746,
    SPELL_FIRE_LAND                 = 50915,
    SPELL_STIGMA_FURY               = 69403,
    SPELL_STIGMA_LIGHT              = 31801,
    SPELL_STIGMA                    = 20271,

    //SHAMAN
    SPELL_STORM_STRIKE              = 64757,
    SPELL_LIGHTNING                 = 49240,
    SPELL_TOTEM_TREMOR              = 65992,
    SPELL_TOTEM_MAGMA               = 58735,

    //DEATHKNIGHT
    SPELL_DEATH_GRIP                = 49560,
    SPELL_PLAGUE_STRIKE             = 59133,
    SPELL_ICY_TOUCH                 = 49909,
    SPELL_DEAD_ARMY                 = 42651,
 
    //PRIEST
    SPELL_POWER_WORD_SHIELD         = 48066,
    SPELL_DEVOURING_PLAGUE          = 48300,
    SPELL_DISPEL_MAGIC              = 988,
    SPELL_MIND_BLAST                = 48127,
    SPELL_FLASH_HEAL                = 48071
};

static uint32 InsanityDataList[5][4] = 
{
    {SPELL_INSANITY_PHASE_1, SPELL_INSANITY_SUMMON_1, 16},
    {SPELL_INSANITY_PHASE_2, SPELL_INSANITY_SUMMON_2, 32},
    {SPELL_INSANITY_PHASE_3, SPELL_INSANITY_SUMMON_3, 64},
    {SPELL_INSANITY_PHASE_4, SPELL_INSANITY_SUMMON_4, 128},
    {SPELL_INSANITY_PHASE_5, SPELL_INSANITY_SUMMON_5, 256},
};

/*######
## boss_volazj
######*/

struct MANGOS_DLL_DECL boss_volazjAI : public ScriptedAI
{
    boss_volazjAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint8 m_uiPhase;
    uint8 m_uiLastSacrifaceHP;

    uint32 m_uiMindFlayTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiShiverTimer;
    uint32 m_uiCheckTimer;
    Unit* m_pShiverTarget;
    bool m_bShiverIsHere;
    uint32 m_uiShiverCastTimer;

    //Insanity
    uint32 m_uiCheckIllusionTimer;

    uint8 m_uiHP;
    uint8 m_uiAction;

    bool m_bInsanityUse;
    uint32 m_uiInsanityCastTimer;

    void Reset()
    {
        m_uiPhase = PHASE_FIGHT;

        m_uiHP = 1;
        m_uiAction = 0;
        m_bInsanityUse = false;
        
        m_uiInsanityCastTimer = 5100;
        m_uiMindFlayTimer = 10000;
        m_uiShadowBoltTimer = 5000;
        m_uiShiverTimer = 18000;
        m_uiCheckTimer = 1000;
        m_uiPhase = PHASE_FIGHT;
        m_pShiverTarget = NULL;
        m_bShiverIsHere = false;

        //Insanity
        m_uiCheckIllusionTimer = 2000;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetPhaseMask(497, true);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(urand(0, 1) ? SAY_DEATH_1 : SAY_DEATH_2, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLAZJ, DONE);
    }

    void InsanityPhase()
    {
       m_uiPhase = PHASE_INSANITY_1;
       m_uiAction = 0;
       ++m_uiHP;
       Move(false);
       m_creature->CastSpell(m_creature, SPELL_INSANITY, false);
       m_uiInsanityCastTimer = 5100;
       m_bInsanityUse = true;
    }

    void AttackStart(Unit* pWho)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;
        else
            ScriptedAI::AttackStart(pWho);
    }

    void Move(bool bNotDisable)
    {
        if (!bNotDisable)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->StopMoving();
        }
        else
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            Unit* pTarget = m_creature->getVictim();
            if (!pTarget)
            {
                m_creature->SetInCombatWithZone();
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            }
            if (pTarget)
                m_creature->GetMotionMaster()->MoveChase(pTarget);
        }
    }

    void CalculatePlayer()
    {
        std::list<HostileReference*> lList = m_creature->getThreatManager().getThreatList();
        if (!lList.empty())
        {
            for (std::list<HostileReference*>::iterator itr = lList.begin(); itr!= lList.end(); ++itr)
            {
                Unit* pTarget = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid());
                if (pTarget && pTarget->isAlive() && pTarget->GetTypeId() == TYPEID_PLAYER)
                {
                    pTarget->SetPhaseMask(InsanityDataList[m_uiAction][2], true);
                    CalculateActions(m_uiAction, pTarget);
                    ++m_uiAction;
                }
            }
        }
        m_uiPhase = PHASE_INSANITY_2;
        m_creature->SetInCombatWithZone();
    }

    void CalculateActions(uint8 uiAction, Unit* pPlayer)
    {
        std::list<HostileReference*> lList = m_creature->getThreatManager().getThreatList();
        if (!lList.empty())
        {
            for (std::list<HostileReference*>::iterator itr = lList.begin(); itr!= lList.end(); ++itr)
            {
                Unit* pTarget = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid());
                if (pTarget && pTarget->isAlive() && pTarget->GetTypeId() == TYPEID_PLAYER && pTarget != pPlayer)
                {
                    if (Creature* pClone = pTarget->SummonCreature(NPC_TWISTED_VISAGE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), pTarget->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0))
                    {
                        pTarget->CastSpell(pClone, SPELL_INSANITY_COPY_CASTER, true);
                        if (pPlayer)
                            pClone->SetPhaseMask(pPlayer->GetPhaseMask(), true);
                    }
                    if (Pet* pPet = pTarget->GetPet())
                    {
                        if (Creature* pClone = pPet->SummonCreature(NPC_TWISTED_VISAGE, pPet->GetPositionX(), pPet->GetPositionY(), pPet->GetPositionZ(), pPet->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0))
                        {
                            pPet->CastSpell(pClone, SPELL_INSANITY_COPY_CASTER, true);
                            if (pPlayer)
                                pClone->SetPhaseMask(pPlayer->GetPhaseMask(), true);
                        }
                    }
                }
            }
        }
    }

    void CheckIllusion()
    {
	    std::list<Creature*> pClones;
        GetCreatureListWithEntryInGrid(pClones, m_creature, NPC_TWISTED_VISAGE, 200.0f);
        if (!pClones.empty())
        {
            for (std::list<Creature*>::iterator iter = pClones.begin(); iter != pClones.end(); ++iter)
            {
                if ((*iter)->isAlive())
                    return;
            }
        }

        std::list<HostileReference*> lList = m_creature->getThreatManager().getThreatList();
        if (!lList.empty())
        {
            for (std::list<HostileReference*>::iterator itr = lList.begin(); itr!= lList.end(); ++itr)
            {
                if (Unit* pTarget = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid()))
                    pTarget->SetPhaseMask(1, true);
            }
        }
        m_uiPhase = PHASE_FIGHT;
        m_creature->RemoveAurasDueToSpell(SPELL_INSANITY_CHANNEL);
        Move(true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || (!m_creature->getVictim() && m_uiPhase != PHASE_INSANITY_2))
            return;

        if (m_bInsanityUse)
        {
            if (m_uiInsanityCastTimer <= uiDiff)
            {
                m_bInsanityUse = false;
                m_creature->CastSpell(m_creature, SPELL_INSANITY_CHANNEL, false);
                CalculatePlayer();
            }
            else
                m_uiInsanityCastTimer -= uiDiff;

            return;
        }

        if (m_bShiverIsHere)
        {
            if (m_uiShiverCastTimer <= uiDiff)
            {
                Map* pMap = m_creature->GetMap();
                if (pMap->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = pMap->GetPlayers();

                    if (m_pShiverTarget && m_pShiverTarget->isAlive() && !PlayerList.isEmpty() &&
                       (m_pShiverTarget->HasAura(SPELL_SHIVER) || m_pShiverTarget->HasAura(SPELL_SHIVER_H)))
                    {
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            Unit* pTemp = i->getSource();
                            if (pTemp->GetGUID() == m_pShiverTarget->GetGUID())
                                continue;
                            if (pTemp->IsWithinDistInMap(m_pShiverTarget, 20.0f))
                                pTemp->CastSpell(pTemp, m_bIsRegularMode ? SPELL_SHIVER_DMG : SPELL_SHIVER_DMG_H, true);
                        }
                    }
                }
                m_bShiverIsHere = false;
            }
            else
                m_uiShiverCastTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case PHASE_FIGHT:
            {
                if (m_uiMindFlayTimer <= uiDiff)
                {
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MIND_FLAY : SPELL_MIND_FLAY_H);
                    m_uiMindFlayTimer = 5000 + urand(3000, 7000);;
                }
                else
                    m_uiMindFlayTimer -= uiDiff;

                //Shadowbolt voley
                if (m_uiShadowBoltTimer <= uiDiff)
                {
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H);
                    m_uiShadowBoltTimer = urand(4000, 6000);
                }
                else
                    m_uiShadowBoltTimer -= uiDiff;

                //Shiver
                if (m_uiShiverTimer <= uiDiff)
                {
                    m_pShiverTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                    if (m_pShiverTarget)
                    {
                        DoCast(m_pShiverTarget, m_bIsRegularMode ? SPELL_SHIVER : SPELL_SHIVER_H);
                        m_pShiverTarget->CastSpell(m_pShiverTarget, m_bIsRegularMode ? SPELL_SHIVER_DMG : SPELL_SHIVER_DMG_H, true);
                    }
                    m_bShiverIsHere = true;
                    m_uiShiverCastTimer = 15000;
                    m_uiShiverTimer = 30000;
                }
                else
                    m_uiShiverTimer -= uiDiff;

                if (m_creature->GetHealthPercent() < 100-(33.4f*m_uiHP))
                    InsanityPhase();
                 
                DoMeleeAttackIfReady();
                break;
            }
            case PHASE_INSANITY_2:
            {
                if (m_uiCheckIllusionTimer < uiDiff)
                {
                    CheckIllusion();
                    m_uiCheckIllusionTimer = 2000;
                }
                else
                    m_uiCheckIllusionTimer -= uiDiff;
                
                break;
            }
        } 
    }
};

/*######
## mob_twisted_visage
######*/
struct MANGOS_DLL_DECL mob_twisted_visageAI : public ScriptedAI
{
    mob_twisted_visageAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiClass = 0;
        if (m_creature->IsTemporarySummon())
        {
            TemporarySummon* pSummon = (TemporarySummon*)m_creature;
            if (m_pOwner = m_creature->GetMap()->GetUnit(pSummon->GetSummonerGuid()))
                InitiateImage();
        }
        Reset();
    }

    Unit* m_pOwner;
    uint8 m_uiClass;
    uint8 m_uiTactic;
    uint32 m_uiSpellTimer[12];
    bool m_bIsComplete;

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        m_uiTactic = 1;
        m_uiSpellTimer[0] = 1000;
        m_uiSpellTimer[1] = 6000;
        m_uiSpellTimer[2] = 15000;
        m_uiSpellTimer[3] = 12000;
        m_uiSpellTimer[4] = urand(1000, 16000);
        m_uiSpellTimer[5] = urand(1000, 12000);
    }

    void InitiateImage()
    {
        if (m_pOwner)
        {
            m_uiClass = m_pOwner->getClass();
            m_creature->SetLevel(m_pOwner->getLevel());
            uint32 uiHealth;
            if (m_bIsRegularMode)
                uiHealth = 4000;
            else
                uiHealth = 12000;
            m_creature->SetMaxHealth(uiHealth);
            m_creature->SetHealth(uiHealth);
            m_creature->setPowerType(m_pOwner->getPowerType()); 
            m_creature->SetMaxPower(m_pOwner->getPowerType(), m_pOwner->GetMaxPower(m_pOwner->getPowerType()));
            m_creature->SetPower(m_pOwner->getPowerType(), m_pOwner->GetPower(m_pOwner->getPowerType()));
            m_creature->setFaction(14);
            if (m_pOwner->GetTypeId() == TYPEID_PLAYER)
            { 
                if (Item const* pItem = ((Player*)m_pOwner)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                    m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, pItem->GetProto()->ItemId);
                if (Item const* pItem = ((Player*)m_pOwner)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                    m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, pItem->GetProto()->ItemId);
                if (Item const* pItem = ((Player*)m_pOwner)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
                    m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, pItem->GetProto()->ItemId);
            }
            else
            {
                m_uiClass = 100;
                m_creature->SetCreatorGuid((m_pOwner->GetOwner())->GetObjectGuid());
                m_creature->SetName(m_pOwner->GetName());
                m_creature->SetDisplayId(m_pOwner->GetDisplayId());
                m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, m_pOwner->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID));
                m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, m_pOwner->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1));
                m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, m_pOwner->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2));
            }
        }
        m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        m_creature->SetInCombatWithZone();
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            switch (m_uiClass)
            {
                case CLASS_HUNTER:
                    m_creature->GetMotionMaster()->MoveChase(pWho, 15.0f);
                    break;
                case CLASS_DRUID:
                    if (m_uiTactic > 2)
                        m_creature->GetMotionMaster()->MoveChase(pWho, 2.0f);
                    else
                        m_creature->GetMotionMaster()->MoveChase(pWho, 15.0f);
                    break;
                case CLASS_ROGUE:
                case CLASS_PALADIN:
                case CLASS_WARRIOR:
                case CLASS_DEATH_KNIGHT:
                    m_creature->GetMotionMaster()->MoveChase(pWho, 2.0f);
                    break;
                case CLASS_WARLOCK:
                case CLASS_MAGE:
                case CLASS_PRIEST:
                    m_creature->GetMotionMaster()->MoveChase(pWho, 15.0f);
                    break;
                case CLASS_SHAMAN:
                    m_creature->GetMotionMaster()->MoveChase(pWho, 7.0f);
                    break;
                case 100:
                    m_creature->GetMotionMaster()->MoveChase(pWho);
                    break;
            }
        }
    }

    void Aggro(Unit* pWho)
    { 
        switch (m_uiClass)
        {
            case CLASS_WARLOCK:
                DoCast(m_creature, SPELL_SHIELD);
                break;
            case CLASS_DRUID:
                m_creature->CastSpell(m_creature, SPELL_SPIKE, false);
                if (m_pOwner)
                {
                    if (m_pOwner->HasAura(5487) || m_pOwner->HasAura(9634))
                        m_uiTactic = 1;
                    else if (m_pOwner->HasAura(768))
                        m_uiTactic = 2;
                    else
                        m_uiTactic = 3; 
                }
                break;
            case CLASS_HUNTER:
                DoCast(m_creature, SPELL_MONKEY);
                break;
            case CLASS_MAGE:
                DoCast(m_creature, SPELL_FREEZE_ARMOR);
                break;
            case CLASS_WARRIOR:
                DoCast(m_creature, SPELL_ROAR);
                break;
            case CLASS_PALADIN:
                DoCast(m_creature, SPELL_SPELL_AURA);
                break;
            case CLASS_PRIEST:
                DoCast(m_creature, SPELL_POWER_WORD_SHIELD, true);
                break;
        }
    }

    void JustDied(Unit* pKiller)
    {
	    std::list<Creature*> pClones;
        GetCreatureListWithEntryInGrid(pClones, m_creature, NPC_TWISTED_VISAGE, 200.0f);
        if (pClones.size() <= 1)
        {
            Map* pMap = m_creature->GetMap();
            if (pMap->IsDungeon())
            {
                Map::PlayerList const &PlayerList = pMap->GetPlayers();

                if (!PlayerList.isEmpty())
                {
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        if (Unit* pTemp = i->getSource())
                        {
                            if (pTemp->GetPhaseMask() == m_creature->GetPhaseMask())
                                pTemp->SetPhaseMask(497, true);
                        }
                    }
                }
            }
        }
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            m_creature->ForcedDespawn();
            return;
        }

        switch (m_uiClass)
        {
            case CLASS_WARLOCK:
            {
                if (m_uiSpellTimer[0] < uiDiff)
                {
                  DoCast(m_creature->getVictim(), SPELL_SB, false);
                  m_uiSpellTimer[0] = urand(3000, 8000);
                }
                else
                    m_uiSpellTimer[0] -= uiDiff;

                if (m_uiSpellTimer[1] < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_AGONY, false);
                    m_uiSpellTimer[1] = urand(8000, 16000);
                }
                else
                    m_uiSpellTimer[1] -= uiDiff;

                if (m_uiSpellTimer[2] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_FIRE_BONUS, false);
                    m_uiSpellTimer[2] = urand(10000, 15000);
                }
                else
                    m_uiSpellTimer[2] -= uiDiff;

                if (m_uiSpellTimer[3] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_FIRE, false);
                    m_uiSpellTimer[3] = urand(12000, 17000);
                }
                else
                    m_uiSpellTimer[3] -= uiDiff;

                if (m_uiSpellTimer[4] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_MESHOK, false);
                    m_uiSpellTimer[4] = urand(12000, 20000);
                }
                else
                    m_uiSpellTimer[4] -= uiDiff;

                if (m_uiSpellTimer[5] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_MAGIC_STUN, false);
                    m_uiSpellTimer[4] = urand(17000, 25000);
                }
                else
                    m_uiSpellTimer[5] -= uiDiff;
                
                break;
            }
            case CLASS_DRUID:
            {
                if (m_uiTactic == 1)
                {
                    if (m_uiSpellTimer[0] < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_BEARSTUN, false);
                        m_uiSpellTimer[0] = urand(7000, 19000);
                    }
                    else
                        m_uiSpellTimer[0] -= uiDiff;

                    if (m_uiSpellTimer[1] < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_BEARBASH, false);
                        m_uiSpellTimer[1] = urand(5000, 13000);
                    }
                    else
                        m_uiSpellTimer[1] -= uiDiff;

                    if (m_uiSpellTimer[2] < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_BEARBLOOD, false);
                        m_uiSpellTimer[2] = urand(4000, 17000);
                    }
                    else
                        m_uiSpellTimer[2] -= uiDiff;
                }
                if (m_uiTactic == 2)
                {
                    if (m_uiSpellTimer[0] < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_CATFANGS, false);
                        m_uiSpellTimer[0] = urand(5000, 13000);
                    }
                    else
                        m_uiSpellTimer[0] -= uiDiff;
                }
                if (m_uiTactic == 3)
                {
                    if (m_uiSpellTimer[0] < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(pTarget, SPELL_MOON_FIRE, false);
                        m_uiSpellTimer[0] = urand(7000, 19000);
                    }
                    else
                        m_uiSpellTimer[0] -= uiDiff;

                    if (m_uiSpellTimer[1] < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_WRATH, false);
                        m_uiSpellTimer[1] = urand(3000, 9000);
                    }
                    else
                        m_uiSpellTimer[1] -= uiDiff;
 
                    if (m_uiSpellTimer[2] < uiDiff)
                    {
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(pTarget, SPELL_ROOT, false);
                        m_uiSpellTimer[2] = urand(12000, 22000);
                    }
                    else
                        m_uiSpellTimer[2] -= uiDiff;

                    if (m_uiSpellTimer[3] < uiDiff)
                    {
                        if (m_creature->GetHealthPercent() < 40.0f)
                        {
                            m_creature->InterruptNonMeleeSpells(false);
                            DoCast(m_creature, SPELL_HEAL, false);
                            m_uiSpellTimer[3] = urand(6000, 9000);
                        }
                    }
                    else
                        m_uiSpellTimer[3] -= uiDiff;
                }
                break;
            }
            case CLASS_HUNTER:
            {
                if (m_uiSpellTimer[0] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_ARCANE, false);
                    m_uiSpellTimer[0] = urand(7000, 12000);
                }
                else
                    m_uiSpellTimer[0] -= uiDiff;

                if (m_uiSpellTimer[1] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_STEDY, false);
                    m_uiSpellTimer[1] = urand(2000, 7000);
                }
                else
                    m_uiSpellTimer[1] -= uiDiff;

                if (m_uiSpellTimer[2] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_POISON, false);
                    m_uiSpellTimer[2] = urand(10000, 15000);
                }
                else
                    m_uiSpellTimer[2] -= uiDiff;

                if (m_uiSpellTimer[3] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_SHOOT, false);
                    m_uiSpellTimer[3] = urand(6000, 9000);
                }
                else
                    m_uiSpellTimer[3] -= uiDiff;
                break;
            }
            case CLASS_MAGE:
            {
                if (m_uiSpellTimer[0] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_FIREBALL, false);
                    m_uiSpellTimer[0] = urand(3000, 8000);
                }
                else
                    m_uiSpellTimer[0] -= uiDiff;

                if (m_uiSpellTimer[1] < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_ARCANE_MISSILE, false);
                    m_uiSpellTimer[1] = urand(12000, 19000);
                }
                else
                    m_uiSpellTimer[1] -= uiDiff;

                if (m_uiSpellTimer[2] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_ARCANE_BLOW, false);
                    m_uiSpellTimer[2] = urand(12000, 20000);
                }
                else
                    m_uiSpellTimer[2] -= uiDiff;
                
                break;
            }
            case CLASS_ROGUE:
            {
                if (m_creature->GetVisibility() != VISIBILITY_OFF)
                {
                    if (m_uiSpellTimer[0] < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_SINISTER, false);
                        m_uiSpellTimer[0] = urand(2000, 7000);
                    }
                    else
                        m_uiSpellTimer[0] -= uiDiff;

                    if (m_uiSpellTimer[1] < uiDiff)
                    {
                        m_creature->SetVisibility(VISIBILITY_OFF);
                        m_uiSpellTimer[1] = urand(18000, 25000);
                    }
                    else
                        m_uiSpellTimer[1] -= uiDiff;

                    if (m_uiSpellTimer[2] < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_BLOOD, false);
                        m_uiSpellTimer[2] = urand(8000, 15000);
                    }
                    else
                        m_uiSpellTimer[2] -= uiDiff;

                    if (m_uiSpellTimer[3] < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_STUN, false);
                        m_uiSpellTimer[3] = urand(8000, 15000);
                    }
                    else
                        m_uiSpellTimer[3] -= uiDiff;

                    if (m_uiSpellTimer[4] < uiDiff)
                    {
                        DoCast(m_creature->getVictim(), SPELL_LAST, false);
                        m_uiSpellTimer[4] = urand(15000, 20000);
                    }
                    else
                        m_uiSpellTimer[4] -= uiDiff;
                }
                else
                {
                    if (m_uiSpellTimer[4] < uiDiff)
                    {
                        m_creature->SetVisibility(VISIBILITY_ON);
                        switch (urand(1, 2))
                        {
                            case 1:
                                DoCast(m_creature->getVictim(), SPELL_AMBUSH, false); break;
                            case 2:
                                DoCast(m_creature->getVictim(), SPELL_STUN2, false); break;
                        }
                        m_uiSpellTimer[4] = urand(3000, 5000);
                    }
                    else
                        m_uiSpellTimer[4] -= uiDiff;
                }
                break;
            }
            case CLASS_WARRIOR:
            {
                if (m_uiSpellTimer[0] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_BLOODY, false);
                    m_uiSpellTimer[0] = urand(7000, 19000);
                }
                else
                    m_uiSpellTimer[0] -= uiDiff;

                if (m_uiSpellTimer[1] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_HERO_STRIKE, false);
                    m_uiSpellTimer[1] = urand(3000, 9000);
                }
                else
                    m_uiSpellTimer[1] -= uiDiff;

                if (m_uiSpellTimer[2] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_THUNDER, false);
                    m_uiSpellTimer[2] = urand(12000, 22000);
                }
                else
                    m_uiSpellTimer[2] -= uiDiff;

                if (m_uiSpellTimer[3] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_CLEAVE, false);
                    m_uiSpellTimer[3] = urand(11000, 19000);
                }
                else
                    m_uiSpellTimer[3] -= uiDiff;
              
                break;
            }
            case CLASS_PALADIN:
            {
                if (m_uiSpellTimer[0] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_LIGHT_STUN, false);
                    m_uiSpellTimer[0] = urand(12000, 19000);
                }
                else
                    m_uiSpellTimer[0] -= uiDiff;

                if (m_uiSpellTimer[1] < uiDiff)
                {
                    DoCast(m_creature, SPELL_FIRE_LAND, false);
                    m_uiSpellTimer[1] = urand(9000, 19000);
                }
                else
                    m_uiSpellTimer[1] -= uiDiff;

                if (m_uiSpellTimer[2] < uiDiff)
                {
                    switch (urand(1, 2))
                    {
                        case 1: DoCast(m_creature, SPELL_STIGMA_FURY, false); break;
                        case 2: DoCast(m_creature, SPELL_STIGMA_LIGHT, false); break;
                    }
                    m_uiSpellTimer[2] = urand(14000, 17000);
                }
                else
                    m_uiSpellTimer[2] -= uiDiff;

                if (m_uiSpellTimer[3] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_STIGMA, false);
                    m_uiSpellTimer[3] = urand(5000, 12000);
                }
                else
                    m_uiSpellTimer[3] -= uiDiff;

                if (m_uiSpellTimer[4] < uiDiff)
                {
                    if(m_creature->GetHealthPercent() < 35.0f)
                    {
                        DoCast(m_creature, SPELL_HOLY_LIGHT, false);
                        m_uiSpellTimer[4] = 20000;
                    }
                }
                else
                    m_uiSpellTimer[4] -= uiDiff;

                break;
            }
            case CLASS_SHAMAN:
            {
                if (m_uiSpellTimer[0] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_LIGHTNING, false);
                    m_uiSpellTimer[0] = 2000;
                }
                else
                    m_uiSpellTimer[0] -= uiDiff;

                if (m_uiSpellTimer[1] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_STORM_STRIKE, false);
                    m_uiSpellTimer[1] = 6000;
                }
                else
                    m_uiSpellTimer[1] -= uiDiff;

                if (m_uiSpellTimer[4] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_TOTEM_TREMOR, true);
                    m_uiSpellTimer[4] = 30000;
                }
                else
                    m_uiSpellTimer[4] -= uiDiff;

                if (m_uiSpellTimer[5] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_TOTEM_MAGMA, true);
                    m_uiSpellTimer[5] = 30000;
                }
                else
                    m_uiSpellTimer[5] -= uiDiff;

                break;
            }
            case CLASS_DEATH_KNIGHT:
            {
                if (m_uiSpellTimer[0] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_DEAD_ARMY, true);
                    m_uiSpellTimer[0] = 90000;
                }
                else
                    m_uiSpellTimer[0] -= uiDiff;

                if (m_uiSpellTimer[1] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_ICY_TOUCH, true);
                    m_uiSpellTimer[1] = 12000;
                }
                else
                    m_uiSpellTimer[1] -= uiDiff;

                if (m_uiSpellTimer[2] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_DEATH_GRIP, true);
                    m_uiSpellTimer[2] = 15000;
                }
                else
                    m_uiSpellTimer[2] -= uiDiff;

                if (m_uiSpellTimer[3] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_PLAGUE_STRIKE, true);
                    m_uiSpellTimer[3] = 5000;
                }
                else
                    m_uiSpellTimer[3] -= uiDiff;

                break;
            }
            case CLASS_PRIEST:
            {
                if (m_uiSpellTimer[0] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_MIND_BLAST, false);
                    m_uiSpellTimer[0] = 8000;
                }
                else
                    m_uiSpellTimer[0] -= uiDiff;

                if (m_uiSpellTimer[1] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_DEVOURING_PLAGUE, false);
                    m_uiSpellTimer[1] = 24000;
                }
                else
                    m_uiSpellTimer[1] -= uiDiff;

                if (m_uiSpellTimer[2] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_POWER_WORD_SHIELD, false);
                    m_uiSpellTimer[2] = 20000;
                }
                else
                    m_uiSpellTimer[2] -= uiDiff;

                if (m_uiSpellTimer[3] < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_DISPEL_MAGIC, false);
                    m_uiSpellTimer[3] = 40000;
                }
                else
                    m_uiSpellTimer[3] -= uiDiff;

                if (m_uiSpellTimer[4] < uiDiff)
                {
                    DoCast(m_creature, SPELL_FLASH_HEAL, false);
                    m_uiSpellTimer[4] = 15000;
                }
                else
                    m_uiSpellTimer[4] -= uiDiff;

                break;
            }
            case 100:
                break;
        }
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_volazj(Creature* pCreature)
{
    return new boss_volazjAI(pCreature);
}

CreatureAI* GetAI_mob_twisted_visage(Creature* pCreature)
{
    return new mob_twisted_visageAI(pCreature);
}

void AddSC_boss_volazj()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_volazj";
    pNewScript->GetAI = &GetAI_boss_volazj;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_twisted_visage";
    pNewScript->GetAI = &GetAI_mob_twisted_visage;
    pNewScript->RegisterSelf();
}
