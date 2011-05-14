

#include "suryabrew.h"


void suryabrew_GPSNotify(void *pData)
{
	int nErr = 0;
	suryabrew *pMe = (suryabrew *) pData;
	AECHAR psz[16];

	AEEGPSInfo *g = &pMe->GPSInfo;
	DBGPRINTF("GPSNotify %d %d %d %d %d %d %d %d %x %x %d %d %d %x %d %d %x %x %d",
		g->dwTimeStamp, g->status, g->dwLat, g->dwLon, g->wAltitude, g->wHeading,
		g->wVelocityHor, g->bVelocityVer, g->accuracy, g->fValid, g->bHorUnc,
		g->bHorUncAngle, g->bHorUncPerp, g->Version, g->wVerUnc, g->GPS_UTCOffset,
		g->LocProbability, g->method, g->dwTimeMS);

	pMe->GPSPosition.dwSize = sizeof(AEEPositionInfoEx);
	nErr = IPOSDET_ExtractPositionInfo(pMe->pIPosDet, &pMe->GPSInfo, &pMe->GPSPosition);
	DBGPRINTF("GPSNotify extract pos %d %x %x", nErr, &pMe->GPSInfo, &pMe->GPSPosition);


	//FLOATTOWSTR
	//AEEPositionInfoEx *p = &pMe->GPSPosition;
	DBGPRINTF("GPSNotify %d %d %d %d %d %d %d %d %d %d ", 
		pMe->GPSPosition.dwSize, pMe->GPSPosition.fLatitude, pMe->GPSPosition.fLongitude,
		pMe->GPSPosition.fAltitude, pMe->GPSPosition.fHeading, pMe->GPSPosition.fHorVelocity,
		pMe->GPSPosition.fVerVelocity, pMe->GPSPosition.fHorUnc, pMe->GPSPosition.fAltitudeUnc,
		pMe->GPSPosition.fUTCOffsetValid);
	DBGPRINTF("GPSNotify %d %d %d %d %d ",
		FLTTOINT(FMUL(pMe->GPSPosition.Latitude, 1000.0)),
		FLTTOINT(FMUL(pMe->GPSPosition.Longitude, 1000.0)),
		pMe->GPSPosition.nAltitude,
		FLTTOINT(FMUL(pMe->GPSPosition.Heading, 1000.0)),
		FLTTOINT(FMUL(pMe->GPSPosition.HorVelocity, 1000.0)));
	DBGPRINTF("GPSNotify %d %d %d %d %d %d %x %x",
		FLTTOINT(FMUL(pMe->GPSPosition.VerVelocity, 1000.0)), 
		FLTTOINT(FMUL(pMe->GPSPosition.HorUncAngle, 1000.0)), 
		FLTTOINT(FMUL(pMe->GPSPosition.HorUnc, 1000.0)),
		FLTTOINT(FMUL(pMe->GPSPosition.HorUncPerp, 1000.0)),
		FLTTOINT(FMUL(pMe->GPSPosition.AltitudeUnc, 1000.0)),
		pMe->GPSPosition.GPS_UTCOffset,
		pMe->GPSPosition.LocProbability,
		pMe->GPSPosition.method);


	nErr = ISHELL_SetTimer(pMe->pIShell, SURYABREW_GPS_REQUEST_INTERVAL,
		pMe->GPSTimerCB.pfnNotify, pData);
	DBGPRINTF("GPSNotify set timer: %d", nErr);

}

boolean suryabrew_GPSValid(suryabrew *pMe)
{
	return (pMe->GPSValid = pMe->GPSPosition.fLatitude && pMe->GPSPosition.fLongitude && pMe->GPSPosition.fAltitude) == TRUE;
}


void suryabrew_GPSTimer(void *pData)
{
	int nErr = 0;
	suryabrew *pMe = (suryabrew *) pData;


	nErr = IPOSDET_GetGPSInfo( pMe->pIPosDet, AEEGPS_GETINFO_LOCATION | AEEGPS_GETINFO_ALTITUDE,
		AEEGPS_ACCURACY_LEVEL1, &pMe->GPSInfo, &pMe->GPSDataCB);

	DBGPRINTF("GPSTimer Asked for GPSInfo: %d", nErr);

}

void suryabrew_GPSUnload(suryabrew *pMe)
{
	suryabrew_GPSStop(pMe);
	if ( pMe->pIPosDet != NULL)
	{
		IPOSDET_Release(pMe->pIPosDet);
		pMe->pIPosDet = NULL;
	}
}

int suryabrew_GPSInit(suryabrew* pMe)
{

	int nErr = 0;

	nErr = ISHELL_CreateInstance( pMe->pIShell, AEECLSID_POSDET, (void **)&pMe->pIPosDet );

	if (nErr != SUCCESS)
	{
		DBGPRINTF("GPSInit Unable to init GPS");
	}
	nErr = IPOSDET_GetGPSConfig(pMe->pIPosDet, &pMe->GPSConfig);

	DBGPRINTF("GPSInit %x %d %d %x %x %x %d", pMe->GPSConfig.mode, pMe->GPSConfig.nFixes,
		pMe->GPSConfig.nInterval, pMe->GPSConfig.optim, pMe->GPSConfig.qos, pMe->GPSConfig.server, nErr);

	pMe->GPSConfig.mode = AEEGPS_MODE_ONE_SHOT;
	//pMe->GPSConfig.mode = AEEGPS_MODE_TRACK_STANDALONE;
	pMe->GPSConfig.nFixes = 1;
	pMe->GPSConfig.nInterval = 1;
	pMe->GPSConfig.optim = AEEGPS_OPT_NONE;
	//pMe->GPSConfig.qos = 0; // 0-255
	//pMe->GPSConfig.server = AEEGPS_SERVER_DEFAULT;

	nErr = IPOSDET_SetGPSConfig(pMe->pIPosDet, &pMe->GPSConfig);

	DBGPRINTF("GPSInit SetConfig", nErr);
	DBGPRINTF("GPSInit %x %d %d %x %x %x %d", pMe->GPSConfig.mode, pMe->GPSConfig.nFixes,
		pMe->GPSConfig.nInterval, pMe->GPSConfig.optim, pMe->GPSConfig.qos, pMe->GPSConfig.server, nErr);

	CALLBACK_Init(&pMe->GPSDataCB, suryabrew_GPSNotify, pMe);
	CALLBACK_Init(&pMe->GPSTimerCB, suryabrew_GPSTimer, pMe);
	suryabrew_GPSTimer(pMe);

	return 0;
}

void suryabrew_GPSStop(suryabrew *pMe)
{

	CALLBACK_Cancel(&pMe->GPSDataCB);
	CALLBACK_Cancel(&pMe->GPSTimerCB);
	ISHELL_CancelTimer(pMe->pIShell, pMe->GPSTimerCB.pfnNotify, pMe);

}