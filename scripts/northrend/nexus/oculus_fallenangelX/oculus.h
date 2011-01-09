/* Copyright (C) 2010 by /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */
 
/* ScriptData
SDName: oculus.h
SD%Complete: 100%
SDComment: /dev/rsa/ // dev // FallenAngelX
SDCategory: 
EndScriptData */

#ifndef OCULUS_H
#define OCULUS_H
#include "BSW_ai.h"
#include "BSW_instance.h"

enum
{
	MAX_ENCOUNTER        = 4,

	TYPE_DRAKOS          = 1,
	TYPE_VAROS           = 2,
	TYPE_UROM            = 3,
	TYPE_EREGOS          = 4,

	NPC_DRAKOS           = 27654,
	NPC_VAROS            = 27447,
	NPC_UROM             = 27655,
	NPC_EREGOS           = 27656,

	NPC_VERDISA          = 27657,
	NPC_BELGARISTRASZ    = 27658,
	NPC_ETERNOS          = 27659,

	GO_FORCEFIELD        = 591000,
	GO_DRAGON_CAGE_DOOR  = 193995,
	GO_ORB_OF_NEXUS      = 188715,
	GO_NEXUS_PORTAL      = 189985,

	GO_CACHE_OF_EREGOS   = 191349,
	GO_CACHE_OF_EREGOS_H = 193603,
	GO_CACHE_SPOTLIGHT   = 191351
};

#endif
