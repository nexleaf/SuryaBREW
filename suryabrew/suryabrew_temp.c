

#include "suryabrew.h"


/***********************************************
* TEMPERATURE INTERFACE AND OPERATION
************************************************/

static void HaveDataCB(uint16 numFrames, void * usrPtr);
static void NeedDataCB(uint16 numFrames, void * usrPtr);
static void PlayedDataCB(uint16 numFrames, void * usrPtr);
static void ReadyCB(void * usrPtr);


double natural_log(const double in)
{
	// Compute the natural log using newtons method!
	// x_(n+1) = x_n - (f(x_n)/f'(x_n))
	// where f(x) == e^x - in
	double exp_1 = FASSIGN_STR("2.718281828");
	double guess = FASSIGN_INT(10); // start value of 10 provides the right balance
	double thepow = FASSIGN_INT(0);
	int i = 0;
	//AECHAR strout[64];
	//FLOATTOWSTR(in, strout, 64);
	//DBGPRINTF("input is %S", strout);
	for (i = 0; i < 10; i++) { // do 10 iterations. tested out, seems like less is bad
		thepow = FPOW(exp_1, guess);		
		guess = FSUB(guess, FDIV(FSUB(thepow, in), thepow));
	}
	//FLOATTOWSTR(guess, strout, 64);
	//DBGPRINTF("guess %S", strout);
	return guess;
}

// wanted to inline, but __inline giving trouble
int suryabrew_TempCalcTemp(suryabrew* pMe)
{
	AECHAR strout[64];
	int temp_int = 0;
	// Convert PCM to ohms using circuit equation, formual is ((res1*inputpcm)/mkaxPCM) - (res1 + res2)
	// res1 is from circuit
	// res2 is currently zero... next revision of circuit will have this
	// inputpcm is computed during calibration step
	// maxPCM is value from audio stream
	int res1 = 10000;
	int res2 = 0;
	int inputpcm = 2508;
	double ohms_d = FASSIGN_INT(((res1*inputpcm)/pMe->maxSound) - (res1 + res2));
	//DBGPRINTF("Maxsnd is %d, res: %d", pMe->maxSound, ((res1*inputpcm)/pMe->maxSound) - (res1 + res2));
	//FLOATTOWSTR(ohms_d, strout, 64);
	//DBGPRINTF("ohms_d is %S", strout);

	// A, B, and C are the thermistor parameters for our particular thermistor:
	// http://mcshaneinc.com/html/TS165_Specs.html
	double trA = FASSIGN_STR("0.000693546");
	double trB = FASSIGN_STR("0.000201139");
	double trC = FASSIGN_STR("0.0000000933821");

	// we need ln(ohms) and ln^3(ohms)
	double lnohms = natural_log(ohms_d);
	double lnohms_cubed = FPOW(lnohms, FASSIGN_INT(3));
	//FLOATTOWSTR(lnohms, strout, 64);
	//DBGPRINTF("lnohms is %S", strout);
	//FLOATTOWSTR(lnohms_cubed, strout, 64);
	//DBGPRINTF("lnohms^3 is %S", strout);

	// convert the ohms into temperature using thermistor equation
	// T = (1/(A + B*ln(ohms) + C*ln^3(ohms)) - 273.15
	double temp_K = FASSIGN_INT(0); // Kelvin
	double temp_C = FASSIGN_INT(0); // Celcius

	temp_K = FDIV(FASSIGN_INT(1),FADD(trA, FADD(FMUL(trB, lnohms), FMUL(trC, lnohms_cubed))));
	temp_C  = FSUB(temp_K,FASSIGN_STR("273.15"));
	FLOATTOWSTR(temp_C, strout, 64);
	DBGPRINTF("temp_c is %S", strout);
	temp_int = FLTTOINT(temp_C);

	return temp_int;
	
	// One place to calc temp from PCM values... can also use user input calibration here eventually
	//return (pMe->maxSound * 449 + 168995)/10000;
	//return (pMe->maxSound * 568 + 400396)/10000;
	//return (pMe->maxSound * 465 + 405156)/10000;
	//return (pMe->maxSound * 258 + 360000)/10000;
	//WSPRINTF(pMe->tempDisp, 64, pMe->tempDispFmt, res);
	//WSPRINTF(pMe->tempDispDBG, 64, pMe->tempDispDBGFmt, pMe->maxSound, pMe->minSound, (pMe->maxSound + pMe->minSound)/2);
}

void suryabrew_TempSetActive(suryabrew* pMe, boolean active)
{
   int status;

   // Check to see if activity is already set
   if (active == pMe->tempActive)
   {
      return;
   }

   // If we are setting the IVocoder active...
   if (active) {

      // Data Integrity checks
      if (pMe->pIVocoder && pMe->pISound) {

         // Unmute the microphone and earpiece
         pMe->soundInfo.eDevice     = AEE_SOUND_DEVICE_CURRENT; //AEE_SOUND_DEVICE_SPEAKER; //AEE_SOUND_DEVICE_CURRENT;
         pMe->soundInfo.eMethod     = AEE_SOUND_METHOD_VOICE;
         pMe->soundInfo.eAPath      = AEE_SOUND_APATH_MUTE; // AEE_SOUND_APATH_BOTH;
         pMe->soundInfo.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
         pMe->soundInfo.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;

         // Set configurations via ISOUND interface
		 DBGPRINTF("!!!! Set in setactive");
         ISOUND_Set(pMe->pISound, &pMe->soundInfo);
         ISOUND_SetDevice(pMe->pISound);

         // Set necessary config data, Description in API
         pMe->vocConfig.needDataCB = NeedDataCB;
         pMe->vocConfig.haveDataCB = HaveDataCB;
		 pMe->vocConfig.playedDataCB = PlayedDataCB;
         //pMe->vocConfig.playedDataCB = NULL;
         pMe->vocConfig.readyCB = ReadyCB;
         pMe->vocConfig.usrPtr = pMe;
         pMe->vocConfig.max = FULL_RATE; // HALF_RATE;
         pMe->vocConfig.min = FULL_RATE; //EIGHTH_RATE;
         pMe->vocConfig.overwrite = TRUE;
         pMe->vocConfig.txReduction = 0;
         pMe->vocConfig.vocoder = VOC_PCM; //// VOC_IS127;
         pMe->vocConfig.watermark = 24;

         // Configure IVocoder
         status = IVOCODER_VocConfigure(pMe->pIVocoder, pMe->vocConfig, &pMe->vocInfo);

         // check status
         if(status != SUCCESS) {
               DBGPRINTF("Problem w/ Voc %d",status);
         }

         //Set buffer
         DBGPRINTF("bufsize %d framedur %d", pMe->vocInfo.maxFrameSize, pMe->vocInfo.frameDuration);

         // if on and configured
         if(pMe->tempVocOn) {
			 DBGPRINTF("From setactive");
			 ReadyCB(pMe);
         }
      }

      // Display status to screen
      // DisplayOutput(pMe, IDS_STREAM);
	  DBGPRINTF("Confed");
   }

   // If we are deactivating the IVocoder
   else if (!active) {

         // Data Integrity Checks
         if (pMe->pIVocoder && pMe->pISound) {
			 DBGPRINTF("Deactiveate");
            // Reset data in Vocoder
            IVOCODER_VocOutReset(pMe->pIVocoder); 
            IVOCODER_VocInReset(pMe->pIVocoder); 

            // We stop incoming and outgoing data
            IVOCODER_VocInStop(pMe->pIVocoder);
            IVOCODER_VocOutStop(pMe->pIVocoder);

			// Try again since sound does not always seem to stop
			IVOCODER_VocOutReset(pMe->pIVocoder); 
			IVOCODER_VocInReset(pMe->pIVocoder); 

         }
		 DBGPRINTF("Set inactive");

   }

   // Set boolean value
   pMe->tempActive = active;
   //pMe->playing = FALSE;
}


static void HaveDataCB(uint16 numFrames, void * usrPtr)
{
   suryabrew *pMe = (suryabrew*)usrPtr;
   DataRateType rate;
   uint16 length;
   int i;
   int status;
   int j;
   int16 *i16;
   int16 tempint;
   //uint16 *u16;
   int16 max = 0;
   int16 min = 8000;


   // Data integrity checks
   if (!pMe || !pMe->pIVocoder)
   {
      return;
   }
   // Read in each frame and send to network
   //DBGPRINTF("Entering HAVEDATA !!!");
   for (i = 0; i < numFrames; i++) {
      status = IVOCODER_VocInRead(pMe->pIVocoder,
                                  &rate, &length, pMe->frameDataIN);

	  //if (pMe->playing == FALSE)
	  //{
	 //DBGPRINTF("HaveData %d %d %d %d", numFrames, length, rate, status);
	  //		  MEMCPY(pMe->frameDataOUT, pMe->frameDataIN, length);
		  // If we succesfully read in data, then write to IVocoder
      
	  if (status == SUCCESS) {
		  //for (j = 0; j < length; j++) {
		  //	  DBGPRINTF("%x", pMe->frameDataIN[j]);
		  //}
		  // Send recived data to IVocoder
		  //IVOCODER_VocOutWrite(pMe->pIVocoder, rate, length, pMe->frameDataOUT);
		  i16 = (int16 *) pMe->frameDataIN;
		  //u16 = (uint16 *) pMe->frameDataIN;
		  //IFILE_Write(pMe->pIFileAudioOut, pMe->frameDataIN, length);

		  for (j = 0; j < length / 2; j++)
		  {
			  //tempint = HTONS(i16[j]);
			  tempint = i16[j];
			  if (tempint > max)
			  {
				  max = tempint;
			  }
			  if (tempint < min)
			  {
				  min = tempint;
			  }
			 // DBGPRINTF("%d %u", i16[j], u16[j]);
		  }
		  //DBGPRINTF("Max: %d %d", max, pMe->maxSound);
		  if (pMe->cycleCount >= pMe->cycleMax)
		  {
			  // Display the old one, and then set the new one
			  suryabrew_DrawTempTemp(pMe);
			  if (pMe->enable_datalogger) {
				  suryabrew_DataLoggerLog(pMe);
			  }
			  pMe->cycleCount = 0;
			  pMe->maxSound = 0;
			  pMe->minSound = 8000;
		  } 
		  if (pMe->enable_datalogger) {
				suryabrew_DataLoggerIncremenetSamples(pMe, length / 2); 
		  }
		  if (max > pMe->maxSound)
		  {
			  pMe->maxSound = max;
		  }	
		  if (min < pMe->minSound)
		  {
			  pMe->minSound = min;
		  }
		  pMe->cycleCount += 1;


		  if (pMe->soundMode == SOUNDMODE_PLAYBACK)
		  {
			  IVOCODER_VocOutWrite(pMe->pIVocoder, rate, length, pMe->frameDataIN);
		  } else {
			  IVOCODER_VocOutWrite(pMe->pIVocoder, FULL_RATE, 320, pMe->tempSoundOut);
		  }


	  }

	  //}
	  //pMe->playing = TRUE;
	  //}
   }

}

static void PlayedDataCB(uint16 numFrames, void * usrPtr) {
   suryabrew *pMe = (suryabrew *)usrPtr;
   //
   // NOTE!!! This NEVER seems to get called no matter what I do!
   //

  // DBGPRINTF("PlayedData %d", numFrames);
   //pMe->playing = FALSE;
   //if (pMe->soundMode != SOUNDMODE_PLAYBACK)
   //{
	  
   //}

}

static void NeedDataCB(uint16 numFrames, void * usrPtr) {
   suryabrew *pMe = (suryabrew *)usrPtr;
   DBGPRINTF("NeedData %d", numFrames);

   //
   // NOTE!!! This NEVER seems to get called no matter what I do!
   //

   // Display pause message if ivocoder needs data
   //if (pMe->tempVocOn) {
	   //IVOCODER_VocOutWrite(pMe->pIVocoder, 0, 320, pMe->tempSoundOut);
	   //DisplayOutput(pMe, IDS_PAUSE);
	   //DBGPRINTF("NeedData tempVocOn");
	   //if (pMe->soundMode != SOUNDMODE_PLAYBACK)
	   //{
	   //}
   //}
}



static void ReadyCB(void * usrPtr) {
   suryabrew *pMe = (suryabrew *) usrPtr;
   DBGPRINTF("!!!!!!!!!! READYCB !!!!!!!!!!!");
   // Start Reading in
   IVOCODER_VocInStart(pMe->pIVocoder);

   // Allow for playing
   IVOCODER_VocOutStart(pMe->pIVocoder);

   // Set Vocoder variable to on
   pMe->tempVocOn = TRUE;
}



void suryabrew_TempLoadSound(suryabrew *pMe)
{
   if (pMe->tempSoundOut != NULL)
   {
	   ISHELL_FreeResData(pMe->pIShell, pMe->tempSoundOut);
	   pMe->tempSoundOut = NULL;
   }


   switch (pMe->soundMode)
   {
   case SOUNDMODE_PLAYBACK:
	   // do nothing
	   break;
   case SOUNDMODE_ONE:
	   pMe->tempSoundOut = ISHELL_LoadResData(pMe->pIShell, SURYABREW_RES_FILE, IDB_BINARY_8KHZ_400HZ_16BIT_01, RESTYPE_BINARY);
	   break;
   case SOUNDMODE_LOW:
	   pMe->tempSoundOut = ISHELL_LoadResData(pMe->pIShell, SURYABREW_RES_FILE, IDB_BINARY_8KHZ_400HZ_16BIT_05, RESTYPE_BINARY);
	   break;
   case SOUNDMODE_MED:
	   pMe->tempSoundOut = ISHELL_LoadResData(pMe->pIShell, SURYABREW_RES_FILE, IDB_BINARY_8KHZ_400HZ_16BIT_10, RESTYPE_BINARY);
	   break;
   case SOUNDMODE_HIGH:
	   pMe->tempSoundOut = ISHELL_LoadResData(pMe->pIShell, SURYABREW_RES_FILE, IDB_BINARY_8KHZ_400HZ_16BIT_15, RESTYPE_BINARY);
	   break;
   default:
	   break;
   }


}

void suryabrew_ISoundCallback(void *pUser, AEESoundCmd eCBType, AEESoundStatus eSPStatus, uint32 dwParam)
{
	suryabrew *pMe = (suryabrew *) pUser;
	AEESoundCmdData *scd = NULL;

	if (eCBType == AEE_SOUND_VOLUME_CB) 
	{
		DBGPRINTF("SOUND_CALLBACK-VOLUME: %d", eSPStatus);
		if (eSPStatus == AEE_SOUND_SUCCESS)
		{
			scd = (AEESoundCmdData *) dwParam;
			DBGPRINTF("Volumne is: %d", scd->wVolume);
			pMe->currVolume = scd->wVolume;
		} else {
			DBGPRINTF("Failed getting volume");
		}
	} else {
		DBGPRINTF("SOUND_CALLBACK-STATUS: %d %x", eSPStatus, dwParam);
		if (dwParam != 0)
		{
			scd = (AEESoundCmdData *) dwParam;
			DBGPRINTF("Vol is: %d %d", pMe->currVolume, scd->wVolume);
		} else 
		{
			DBGPRINTF("Vol at: %d", pMe->currVolume);
		}
	}

}


void suryabrew_TempGetVolume(suryabrew *pMe)
{
	if (pMe->pISound == NULL)
	{
		return;
	}
	ISOUND_GetVolume(pMe->pISound);
}
void suryabrew_TempVolUp(suryabrew *pMe)
{
	if (pMe->pISound == NULL)
	{
		return;
	}
	if (pMe->allow_volume != 1)
	{
		return;
	}
	if (pMe->currVolume >= AEE_MAX_VOLUME)
	{
		DBGPRINTF("VOLUME AT MAX");
	} else {
		pMe->currVolume = pMe->currVolume + 1;
		ISOUND_SetVolume(pMe->pISound, pMe->currVolume);
	}
}
void suryabrew_TempVolDown(suryabrew *pMe)
{
	if (pMe->pISound == NULL)
	{
		return;
	}
	if (pMe->allow_volume != 1)
	{
		return;
	}
	if (pMe->currVolume == 0)
	{
		DBGPRINTF("VOLUME AT MMIN");
	} else {
		pMe->currVolume = pMe->currVolume - 1;
		ISOUND_SetVolume(pMe->pISound, pMe->currVolume);
	}
}
void suryabrew_TempPlayTone(suryabrew *pMe)
{
	AEESoundToneData std;
	if (pMe->pISound == NULL)
	{
		return;
	}
	std.eTone = AEE_TONE_RING_A4;
	std.wDuration = 0;
	ISOUND_PlayTone(pMe->pISound, std);
}
void suryabrew_TempStopTone(suryabrew *pMe)
{
	if (pMe->pISound == NULL)
	{
		return;
	}
	ISOUND_StopTone(pMe->pISound);
}

void suryabrew_TempUnloadVOC(suryabrew *pMe)
{
	suryabrew_TempSetActive(pMe, FALSE);

   
	// Release IVocoder
   if (pMe->pIVocoder) {
	   IVOCODER_VocOutReset(pMe->pIVocoder); 
	   IVOCODER_VocInReset(pMe->pIVocoder); 
	   IVOCODER_Release(pMe->pIVocoder);
	   pMe->pIVocoder = NULL;
   }

   // Release Sound interface
   if (pMe->pISound) {
      ISOUND_Release(pMe->pISound);
      pMe->pISound = NULL;
   }

   if (pMe->tempSoundOut != NULL)
   {
	   ISHELL_FreeResData(pMe->pIShell, pMe->tempSoundOut);
	   pMe->tempSoundOut = NULL;
   }

   if (pMe->pIFileAudioOut != NULL)
   {
	   IFILE_Release(pMe->pIFileAudioOut);
	   pMe->pIFileAudioOut = NULL;
   }

   if (pMe->enable_datalogger) {
	   suryabrew_DataLoggerUnload(pMe);
   }

}

int suryabrew_TempInitVOC(suryabrew *pMe)
{
	pMe->tempActive = FALSE;
	pMe->tempVocOn = FALSE;
	//	pMe->playing = FALSE;
	
	pMe->tempSoundOut = ISHELL_LoadResData(pMe->pIShell, SURYABREW_RES_FILE, IDB_BINARY_8KHZ_400HZ_16BIT_01, RESTYPE_BINARY);

	// Create IVocoder Interface
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_VOCODER, (void **)&pMe->pIVocoder) != SUCCESS)
   {
      DBGPRINTF("Error Creating IVocoder interface");
	  return EFAILED;
   }

   // Create ISound interface
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOUND, (void **)&pMe->pISound) != SUCCESS)
   {
      DBGPRINTF("Error Creating ISound interface");
	  return EFAILED;
   }

   ISound_RegisterNotify(pMe->pISound, suryabrew_ISoundCallback, pMe);
   suryabrew_TempGetVolume(pMe);

   // Assumes CurrDate set! Debug output... save file to SD card
   /*
   suryabrew_FileCreateFileNamePrefix(pMe, "AUDIO");
   SPRINTF(pMe->AudioFilename, "%sTEST.raw", pMe->FileNamePrefix);

   DBGPRINTF("to: %s", pMe->ImageFilename);
   pMe->pIFileAudioOut = suryabrew_FileCreateFile(pMe, pMe->AudioFilename, NULL);
   */

   if (pMe->enable_datalogger) {
	   suryabrew_DataLoggerInit(pMe);
   }

   return SUCCESS;
}


/*******************************************************/
/************************* OLD *************************/
/*******************************************************/

#if 0
void suryabrew_TempInit(suryabrew *pMe)
{


	int nErr = 0;
	AEEMediaData amd;

	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MEDIAUTIL, (void **) &pMe->pIMediaUtilPlay);
	if (nErr)
	{
		DBGPRINTF("TempInit CreateInstance MediaUtil: %d", nErr);
		return;
	}

	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MEDIAUTIL, (void **) &pMe->pIMediaUtilRec);
	if (nErr)
	{
		DBGPRINTF("TempInit CreateInstance MediaUtil: %d", nErr);
		if (pMe->pIMediaUtilPlay != NULL)
		{
			IMEDIAUTIL_Release(pMe->pIMediaUtilPlay);
		}
		return;
	}


	amd.clsData = MMD_FILE_NAME;
	amd.pData = "fs:/card0/tempsound.wav";
	amd.dwSize = 0;
	nErr = IMEDIAUTIL_CreateMedia(pMe->pIMediaUtilPlay, &amd, &pMe->pIMediaPlay);

	if (nErr)
	{
		DBGPRINTF("TempInit CreateInstance MediaPlay: %d", nErr);
		if (pMe->pIMediaUtilRec != NULL)
		{
			IMEDIAUTIL_Release(pMe->pIMediaUtilRec);
		}
		if (pMe->pIMediaUtilPlay != NULL)
		{
			IMEDIAUTIL_Release(pMe->pIMediaUtilPlay);
		}
		return;
	}

	IMEDIA_Play(pMe->pIMediaPlay);
	DBGPRINTF("TempInit Now Setup Recording: %d", nErr);


	amd.clsData = MMD_FILE_NAME;
	amd.pData = "fs:/card0/rectemp.wav";
	amd.dwSize = 0;
	nErr = IMEDIAUTIL_CreateMedia(pMe->pIMediaUtilRec, &amd, &pMe->pIMediaRec);

	if (nErr)
	{
		DBGPRINTF("TempInit CreateInstance MediaRec: %d", nErr);
		if (pMe->pIMediaUtilRec != NULL)
		{
			IMEDIAUTIL_Release(pMe->pIMediaUtilRec);
		}
		if (pMe->pIMediaUtilPlay != NULL)
		{
			IMEDIAUTIL_Release(pMe->pIMediaUtilPlay);
		}

		return;
	}

	IMEDIA_Record(pMe->pIMediaRec);
	DBGPRINTF("TempInit Now Recording: %d", nErr);


	if (pMe->pIMediaUtilRec != NULL)
	{
		IMEDIAUTIL_Release(pMe->pIMediaUtilRec);
	}
	if (pMe->pIMediaUtilPlay != NULL)
	{
		IMEDIAUTIL_Release(pMe->pIMediaUtilPlay);
	}

}
#endif