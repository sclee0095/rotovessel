
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "MRFEnergy.h"

#include "instances.inc"

#if !defined(_AFXDLL)
#include <windows.h>
#include <crtdbg.h>
#if defined(DEBUG) | defined(_DEBUG)
#if !defined(DEBUG_NEW)
#define DEBUG_NEW new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
#endif
#endif

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

template <class T> void MRFEnergy<T>::SetMonotonicTrees()
{
	Node* i;
	MRFEdge* e;

	if (!m_isEnergyConstructionCompleted)
	{
		CompleteGraphConstruction();
	}

	for (i=m_nodeFirst; i; i=i->m_next)
	{
		REAL mu;

		int nForward = 0, nBackward = 0;
		for (e=i->m_firstForward; e; e=e->m_nextForward)
		{
			nForward ++;
		}
		for (e=i->m_firstBackward; e; e=e->m_nextBackward)
		{
			nBackward ++;
		}
		int ni = (nForward > nBackward) ? nForward : nBackward;

		mu = (REAL)1 / ni;
		for (e=i->m_firstBackward; e; e=e->m_nextBackward)
		{
			e->m_gammaBackward = mu;
		}
		for (e=i->m_firstForward; e; e=e->m_nextForward)
		{
			e->m_gammaForward = mu;
		}
	}
}

