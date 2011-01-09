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
SD%Complete: 80%
SDComment: // dev // FallenAngelX  
SDCategory:
ToDo:  Varous Forcefield needs fixed  suppose to have to kill 10 centrifuses constructs before varous will engage on retail
other Varoius Quest and achievements
EndScriptData */

#include "precompiled.h"
#include "oculus.h"

struct MANGOS_DLL_DECL instance_oculus : public ScriptedInstance
{
    instance_oculus(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint64 m_uiDrakosGUID;
	uint64 m_uiVarosGUID;
	uint64 m_uiVarosForcefieldGUID;
	uint64 m_uiUromGUID;
	uint64 m_uiEregosGUID;
	std::list<uint64> m_uiCageDoorList;
	uint64 m_uiEregosCacheGUID;
	uint64 m_uiCacheSpotlightGUID;

	uint32 m_uiVarosShieldCharges;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

	    m_uiDrakosGUID  = 0;
		m_uiVarosGUID   = 0;
		m_uiVarosForcefieldGUID = 0;
		m_uiUromGUID    = 0;
		m_uiEregosGUID  = 0;
		m_uiCageDoorList.clear();
		m_uiEregosCacheGUID = 0;
		m_uiCacheSpotlightGUID = 0;

		m_uiVarosShieldCharges = 10;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;
        }

        return false;
    }

	void OnObjectCreate(GameObject* pGo)
	{
		switch(pGo->GetEntry())
		{
		case GO_FORCEFIELD:
			m_uiVarosForcefieldGUID = pGo->GetGUID();
			break;
		case GO_DRAGON_CAGE_DOOR:
			m_uiCageDoorList.push_back(pGo->GetGUID());
			break;
		case GO_CACHE_OF_EREGOS:
		case GO_CACHE_OF_EREGOS_H:
			m_uiEregosCacheGUID = pGo->GetGUID();
			break;
		case GO_CACHE_SPOTLIGHT:
			m_uiCacheSpotlightGUID = pGo->GetGUID();
			break;
		}
	}

	void OpenCageDoors()
	{
		if(m_uiCageDoorList.empty())
			return;

		for(std::list<uint64>::const_iterator itr = m_uiCageDoorList.begin(); itr != m_uiCageDoorList.end(); ++itr)
		{
			GameObject* pDoor = instance->GetGameObject(*itr);
			if(pDoor)
				DoUseDoorOrButton(*itr);
		}
	}

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
		case NPC_DRAKOS:
			m_uiDrakosGUID = pCreature->GetGUID();
			break;
		case NPC_VAROS:
			m_uiVarosGUID = pCreature->GetGUID();
			break;
		case NPC_UROM:
			m_uiUromGUID = pCreature->GetGUID();
			break;
		case NPC_EREGOS:
			m_uiEregosGUID = pCreature->GetGUID();
			break;
        }
    }

    uint64 GetData64(uint32 uiType)
    {
        switch(uiType)
        {
		case NPC_DRAKOS:
			return m_uiDrakosGUID;
		case NPC_VAROS:
			return m_uiVarosGUID;
		case NPC_UROM:
			return m_uiUromGUID;
		case NPC_EREGOS:
			return m_uiEregosGUID;
        }

        return 0;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_DRAKOS:
                return m_auiEncounter[0];
            case TYPE_VAROS:
                return m_auiEncounter[1];
            case TYPE_UROM:
                return m_auiEncounter[2];
            case TYPE_EREGOS:
                return m_auiEncounter[3];
        }

        return 0;
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance Nexus: SetData received for type %u with data %u", uiType, uiData);

        switch(uiType)
        {
            case TYPE_DRAKOS:
                m_auiEncounter[0] = uiData;
				if(uiData == DONE)
					OpenCageDoors();
                break;
            case TYPE_VAROS:
				if(uiData != SPECIAL)
					m_auiEncounter[1] = uiData;
				else
				{
					m_uiVarosShieldCharges--;
					if(m_uiVarosShieldCharges == 0 && m_uiVarosForcefieldGUID)
						DoUseDoorOrButton(m_uiVarosForcefieldGUID);
				}
                break;
            case TYPE_UROM:
                m_auiEncounter[2] = uiData;
                break;
            case TYPE_EREGOS:
                m_auiEncounter[3] = uiData;
				if(uiData == DONE && m_uiEregosCacheGUID)
				{
					DoRespawnGameObject(m_uiEregosCacheGUID,MINUTE*30);
					if(m_uiCacheSpotlightGUID)
						DoRespawnGameObject(m_uiCacheSpotlightGUID,MINUTE*30);
				}
                break;
            default:
                error_log("SD2: Instance Oculus: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

	bool CheckAchievementCriteriaMeet(uint32, Player const*, Unit const*, uint32)
	{
		return false;
	}
};

InstanceData* GetInstanceData_instance_oculus(Map* pMap)
{
    return new instance_oculus(pMap);
}

struct MANGOS_DLL_DECL mob_centrifuge_constructAI : public ScriptedAI
{
	mob_centrifuge_constructAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
	}
 
	ScriptedInstance* m_pInstance;
	bool m_bIsRegularMode;
 
	void Reset()
	{
	}

	void JustDied(Unit* pKiller)
	{
		if(m_pInstance)
			m_pInstance->SetData(TYPE_VAROS,SPECIAL);
	}
 
	void UpdateAI(const uint32 diff)
	{
		if(!m_creature->getVictim() || !m_creature->SelectHostileTarget())
			return;

		DoMeleeAttackIfReady();
	}
};
 
CreatureAI* GetAI_mob_centrifuge_construct(Creature* pCreature)
{
	return new mob_centrifuge_constructAI(pCreature);
}

bool GOUse_go_oculus_portal(Player* pPlayer, GameObject* pGo)
{
	switch(pGo->GetEntry())
	{
	case GO_ORB_OF_NEXUS:
		pPlayer->TeleportTo(571,3876.159912f,6984.439941f,106.32f,6.279f);
		return true;
	case GO_NEXUS_PORTAL:
		pPlayer->NearTeleportTo(1001.3419f,1051.692f,359.476776f,3.189315f);
		return true;
	}

	return false;
}

void AddSC_instance_oculus()
{
    Script* newscript;
	
	newscript = new Script;
	newscript->Name = "mob_centrifuge_construct";
	newscript->GetAI = &GetAI_mob_centrifuge_construct;
	newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "instance_oculus";
    newscript->GetInstanceData = &GetInstanceData_instance_oculus;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_oculus_portal";
	newscript->pGOUse = GOUse_go_oculus_portal;
	newscript->RegisterSelf();
}
