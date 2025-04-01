//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Declares basic entity communications classes, for input/output of data
//			between entities
//
// $NoKeywords: $
//=============================================================================//

#ifndef ENTITYOUTPUT_H
#define ENTITYOUTPUT_H

#ifdef _WIN32
#pragma once
#endif


#include <string_t.h>
#include <variant_t.h>


// a trimmed down version of https://github.com/ValveSoftware/source-sdk-2013/blob/master/src/game/server/entityoutput.h


#define EVENT_FIRE_ALWAYS	-1


//-----------------------------------------------------------------------------
// Purpose: A COutputEvent consists of an array of these CEventActions.
//			Each CEventAction holds the information to fire a single input in
//			a target entity, after a specific delay.
//-----------------------------------------------------------------------------
class CEventAction
{
public:
	CEventAction( const char *ActionData = NULL );

	string_t m_iTarget; // name of the entity(s) to cause the action in
	string_t m_iTargetInput; // the name of the action to fire
	string_t m_iParameter; // parameter to send, 0 if none
	float m_flDelay; // the number of seconds to wait before firing the action
	int m_nTimesToFire; // The number of times to fire this event, or EVENT_FIRE_ALWAYS.

	int m_iIDStamp;	// unique identifier stamp

	CEventAction *m_pNext;
};


//-----------------------------------------------------------------------------
// Purpose: Stores a list of connections to other entities, for data/commands to be
//			communicated along.
//-----------------------------------------------------------------------------
class CBaseEntityOutput
{
public:
	~CBaseEntityOutput();

public:
	variant_t m_Value;
	CEventAction *m_ActionList;

	CBaseEntityOutput() {} // this class cannot be created, only it's children

private:
	CBaseEntityOutput( CBaseEntityOutput& ); // protect from accidental copying
};

typedef void (*DeleteForMeType)(void* thing);
#if defined(WIN64)
typedef void (*AddEventType)(void* queue, const char* target, const char* targetInput, variant_t Value, float fireDelay, CBaseEntity* pActivator, CBaseEntity* pCaller, int outputID);
#else
typedef void (__fastcall *AddEventType)(void* queue, void* edx, const char* target, const char* targetInput, variant_t Value, float fireDelay, CBaseEntity* pActivator, CBaseEntity* pCaller, int outputID);
#endif

// an edit of https://github.com/ValveSoftware/source-sdk-2013/blob/65aa7c910e1e4a3ef7b39bf4ac4b0f325af7ccf1/src/game/server/cbase.cpp#L301-L418
void CBaseEntityOutput_FireOutput2(CBaseEntityOutput *self, variant_t Value, CBaseEntity *pActivator, CBaseEntity *pCaller, float fDelay, DeleteForMeType DeleteForMe, AddEventType AddEvent)
{
	//
	// Iterate through all eventactions and fire them off.
	//
	CEventAction *ev = self->m_ActionList;
	CEventAction *prev = NULL;

	while (ev != NULL)
	{
		if (ev->m_iParameter == NULL_STRING)
		{
			//
			// Post the event with the default parameter.
			//
#if defined(WIN64)
			AddEvent( NULL, STRING(ev->m_iTarget), STRING(ev->m_iTargetInput), Value, ev->m_flDelay + fDelay, pActivator, pCaller, ev->m_iIDStamp );
#else
			AddEvent( NULL, NULL, STRING(ev->m_iTarget), STRING(ev->m_iTargetInput), Value, ev->m_flDelay + fDelay, pActivator, pCaller, ev->m_iIDStamp );
#endif
		}
		else
		{
			//
			// Post the event with a parameter override.
			//
			variant_t ValueOverride;
			ValueOverride.SetString( ev->m_iParameter );
#if defined(WIN64)
			AddEvent( NULL, STRING(ev->m_iTarget), STRING(ev->m_iTargetInput), ValueOverride, ev->m_flDelay, pActivator, pCaller, ev->m_iIDStamp );
#else
			AddEvent( NULL, NULL, STRING(ev->m_iTarget), STRING(ev->m_iTargetInput), ValueOverride, ev->m_flDelay, pActivator, pCaller, ev->m_iIDStamp );
#endif
		}

		//
		// Remove the event action from the list if it was set to be fired a finite
		// number of times (and has been).
		//
		bool bRemove = false;
		if (ev->m_nTimesToFire != EVENT_FIRE_ALWAYS)
		{
			ev->m_nTimesToFire--;
			if (ev->m_nTimesToFire == 0)
			{
				bRemove = true;
			}
		}

		if (!bRemove)
		{
			prev = ev;
			ev = ev->m_pNext;
		}
		else
		{
			if (prev != NULL)
			{
				prev->m_pNext = ev->m_pNext;
			}
			else
			{
				self->m_ActionList = ev->m_pNext;
			}

			CEventAction *next = ev->m_pNext;
			DeleteForMe(ev);
			ev = next;
		}
	}
}


#endif // ENTITYOUTPUT_H
