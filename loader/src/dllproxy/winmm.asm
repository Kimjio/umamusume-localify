.code

extern mciExecute_Original:QWORD
extern CloseDriver_Original:QWORD
extern DefDriverProc_Original:QWORD
extern DriverCallback_Original:QWORD
extern DrvGetModuleHandle_Original:QWORD
extern GetDriverModuleHandle_Original:QWORD
extern OpenDriver_Original:QWORD
extern PlaySound_Original:QWORD
extern PlaySoundA_Original:QWORD
extern PlaySoundW_Original:QWORD
extern SendDriverMessage_Original:QWORD
extern WOWAppExit_Original:QWORD
extern auxGetDevCapsA_Original:QWORD
extern auxGetDevCapsW_Original:QWORD
extern auxGetNumDevs_Original:QWORD
extern auxGetVolume_Original:QWORD
extern auxOutMessage_Original:QWORD
extern auxSetVolume_Original:QWORD
extern joyConfigChanged_Original:QWORD
extern joyGetDevCapsA_Original:QWORD
extern joyGetDevCapsW_Original:QWORD
extern joyGetNumDevs_Original:QWORD
extern joyGetPos_Original:QWORD
extern joyGetPosEx_Original:QWORD
extern joyGetThreshold_Original:QWORD
extern joyReleaseCapture_Original:QWORD
extern joySetCapture_Original:QWORD
extern joySetThreshold_Original:QWORD
extern mciDriverNotify_Original:QWORD
extern mciDriverYield_Original:QWORD
extern mciFreeCommandResource_Original:QWORD
extern mciGetCreatorTask_Original:QWORD
extern mciGetDeviceIDA_Original:QWORD
extern mciGetDeviceIDFromElementIDA_Original:QWORD
extern mciGetDeviceIDFromElementIDW_Original:QWORD
extern mciGetDeviceIDW_Original:QWORD
extern mciGetDriverData_Original:QWORD
extern mciGetErrorStringA_Original:QWORD
extern mciGetErrorStringW_Original:QWORD
extern mciGetYieldProc_Original:QWORD
extern mciLoadCommandResource_Original:QWORD
extern mciSendCommandA_Original:QWORD
extern mciSendCommandW_Original:QWORD
extern mciSendStringA_Original:QWORD
extern mciSendStringW_Original:QWORD
extern mciSetDriverData_Original:QWORD
extern mciSetYieldProc_Original:QWORD
extern midiConnect_Original:QWORD
extern midiDisconnect_Original:QWORD
extern midiInAddBuffer_Original:QWORD
extern midiInClose_Original:QWORD
extern midiInGetDevCapsA_Original:QWORD
extern midiInGetDevCapsW_Original:QWORD
extern midiInGetErrorTextA_Original:QWORD
extern midiInGetErrorTextW_Original:QWORD
extern midiInGetID_Original:QWORD
extern midiInGetNumDevs_Original:QWORD
extern midiInMessage_Original:QWORD
extern midiInOpen_Original:QWORD
extern midiInPrepareHeader_Original:QWORD
extern midiInReset_Original:QWORD
extern midiInStart_Original:QWORD
extern midiInStop_Original:QWORD
extern midiInUnprepareHeader_Original:QWORD
extern midiOutCacheDrumPatches_Original:QWORD
extern midiOutCachePatches_Original:QWORD
extern midiOutClose_Original:QWORD
extern midiOutGetDevCapsA_Original:QWORD
extern midiOutGetDevCapsW_Original:QWORD
extern midiOutGetErrorTextA_Original:QWORD
extern midiOutGetErrorTextW_Original:QWORD
extern midiOutGetID_Original:QWORD
extern midiOutGetNumDevs_Original:QWORD
extern midiOutGetVolume_Original:QWORD
extern midiOutLongMsg_Original:QWORD
extern midiOutMessage_Original:QWORD
extern midiOutOpen_Original:QWORD
extern midiOutPrepareHeader_Original:QWORD
extern midiOutReset_Original:QWORD
extern midiOutSetVolume_Original:QWORD
extern midiOutShortMsg_Original:QWORD
extern midiOutUnprepareHeader_Original:QWORD
extern midiStreamClose_Original:QWORD
extern midiStreamOpen_Original:QWORD
extern midiStreamOut_Original:QWORD
extern midiStreamPause_Original:QWORD
extern midiStreamPosition_Original:QWORD
extern midiStreamProperty_Original:QWORD
extern midiStreamRestart_Original:QWORD
extern midiStreamStop_Original:QWORD
extern mixerClose_Original:QWORD
extern mixerGetControlDetailsA_Original:QWORD
extern mixerGetControlDetailsW_Original:QWORD
extern mixerGetDevCapsA_Original:QWORD
extern mixerGetDevCapsW_Original:QWORD
extern mixerGetID_Original:QWORD
extern mixerGetLineControlsA_Original:QWORD
extern mixerGetLineControlsW_Original:QWORD
extern mixerGetLineInfoA_Original:QWORD
extern mixerGetLineInfoW_Original:QWORD
extern mixerGetNumDevs_Original:QWORD
extern mixerMessage_Original:QWORD
extern mixerOpen_Original:QWORD
extern mixerSetControlDetails_Original:QWORD
extern mmDrvInstall_Original:QWORD
extern mmGetCurrentTask_Original:QWORD
extern mmTaskBlock_Original:QWORD
extern mmTaskCreate_Original:QWORD
extern mmTaskSignal_Original:QWORD
extern mmTaskYield_Original:QWORD
extern mmioAdvance_Original:QWORD
extern mmioAscend_Original:QWORD
extern mmioClose_Original:QWORD
extern mmioCreateChunk_Original:QWORD
extern mmioDescend_Original:QWORD
extern mmioFlush_Original:QWORD
extern mmioGetInfo_Original:QWORD
extern mmioInstallIOProcA_Original:QWORD
extern mmioInstallIOProcW_Original:QWORD
extern mmioOpenA_Original:QWORD
extern mmioOpenW_Original:QWORD
extern mmioRead_Original:QWORD
extern mmioRenameA_Original:QWORD
extern mmioRenameW_Original:QWORD
extern mmioSeek_Original:QWORD
extern mmioSendMessage_Original:QWORD
extern mmioSetBuffer_Original:QWORD
extern mmioSetInfo_Original:QWORD
extern mmioStringToFOURCCA_Original:QWORD
extern mmioStringToFOURCCW_Original:QWORD
extern mmioWrite_Original:QWORD
extern mmsystemGetVersion_Original:QWORD
extern sndPlaySoundA_Original:QWORD
extern sndPlaySoundW_Original:QWORD
extern timeBeginPeriod_Original:QWORD
extern timeEndPeriod_Original:QWORD
extern timeGetDevCaps_Original:QWORD
extern timeGetSystemTime_Original:QWORD
extern timeGetTime_Original:QWORD
extern timeKillEvent_Original:QWORD
extern timeSetEvent_Original:QWORD
extern waveInAddBuffer_Original:QWORD
extern waveInClose_Original:QWORD
extern waveInGetDevCapsA_Original:QWORD
extern waveInGetDevCapsW_Original:QWORD
extern waveInGetErrorTextA_Original:QWORD
extern waveInGetErrorTextW_Original:QWORD
extern waveInGetID_Original:QWORD
extern waveInGetNumDevs_Original:QWORD
extern waveInGetPosition_Original:QWORD
extern waveInMessage_Original:QWORD
extern waveInOpen_Original:QWORD
extern waveInPrepareHeader_Original:QWORD
extern waveInReset_Original:QWORD
extern waveInStart_Original:QWORD
extern waveInStop_Original:QWORD
extern waveInUnprepareHeader_Original:QWORD
extern waveOutBreakLoop_Original:QWORD
extern waveOutClose_Original:QWORD
extern waveOutGetDevCapsA_Original:QWORD
extern waveOutGetDevCapsW_Original:QWORD
extern waveOutGetErrorTextA_Original:QWORD
extern waveOutGetErrorTextW_Original:QWORD
extern waveOutGetID_Original:QWORD
extern waveOutGetNumDevs_Original:QWORD
extern waveOutGetPitch_Original:QWORD
extern waveOutGetPlaybackRate_Original:QWORD
extern waveOutGetPosition_Original:QWORD
extern waveOutGetVolume_Original:QWORD
extern waveOutMessage_Original:QWORD
extern waveOutOpen_Original:QWORD
extern waveOutPause_Original:QWORD
extern waveOutPrepareHeader_Original:QWORD
extern waveOutReset_Original:QWORD
extern waveOutRestart_Original:QWORD
extern waveOutSetPitch_Original:QWORD
extern waveOutSetPlaybackRate_Original:QWORD
extern waveOutSetVolume_Original:QWORD
extern waveOutUnprepareHeader_Original:QWORD
extern waveOutWrite_Original:QWORD

mciExecute_EXPORT proc
  jmp QWORD ptr mciExecute_Original
mciExecute_EXPORT endp

CloseDriver_EXPORT proc
  jmp QWORD ptr CloseDriver_Original
CloseDriver_EXPORT endp

DefDriverProc_EXPORT proc
  jmp QWORD ptr DefDriverProc_Original
DefDriverProc_EXPORT endp

DriverCallback_EXPORT proc
  jmp QWORD ptr DriverCallback_Original
DriverCallback_EXPORT endp

DrvGetModuleHandle_EXPORT proc
  jmp QWORD ptr DrvGetModuleHandle_Original
DrvGetModuleHandle_EXPORT endp

GetDriverModuleHandle_EXPORT proc
  jmp QWORD ptr GetDriverModuleHandle_Original
GetDriverModuleHandle_EXPORT endp

OpenDriver_EXPORT proc
  jmp QWORD ptr OpenDriver_Original
OpenDriver_EXPORT endp

PlaySound_EXPORT proc
  jmp QWORD ptr PlaySound_Original
PlaySound_EXPORT endp

PlaySoundA_EXPORT proc
  jmp QWORD ptr PlaySoundA_Original
PlaySoundA_EXPORT endp

PlaySoundW_EXPORT proc
  jmp QWORD ptr PlaySoundW_Original
PlaySoundW_EXPORT endp

SendDriverMessage_EXPORT proc
  jmp QWORD ptr SendDriverMessage_Original
SendDriverMessage_EXPORT endp

WOWAppExit_EXPORT proc
  jmp QWORD ptr WOWAppExit_Original
WOWAppExit_EXPORT endp

auxGetDevCapsA_EXPORT proc
  jmp QWORD ptr auxGetDevCapsA_Original
auxGetDevCapsA_EXPORT endp

auxGetDevCapsW_EXPORT proc
  jmp QWORD ptr auxGetDevCapsW_Original
auxGetDevCapsW_EXPORT endp

auxGetNumDevs_EXPORT proc
  jmp QWORD ptr auxGetNumDevs_Original
auxGetNumDevs_EXPORT endp

auxGetVolume_EXPORT proc
  jmp QWORD ptr auxGetVolume_Original
auxGetVolume_EXPORT endp

auxOutMessage_EXPORT proc
  jmp QWORD ptr auxOutMessage_Original
auxOutMessage_EXPORT endp

auxSetVolume_EXPORT proc
  jmp QWORD ptr auxSetVolume_Original
auxSetVolume_EXPORT endp

joyConfigChanged_EXPORT proc
  jmp QWORD ptr joyConfigChanged_Original
joyConfigChanged_EXPORT endp

joyGetDevCapsA_EXPORT proc
  jmp QWORD ptr joyGetDevCapsA_Original
joyGetDevCapsA_EXPORT endp

joyGetDevCapsW_EXPORT proc
  jmp QWORD ptr joyGetDevCapsW_Original
joyGetDevCapsW_EXPORT endp

joyGetNumDevs_EXPORT proc
  jmp QWORD ptr joyGetNumDevs_Original
joyGetNumDevs_EXPORT endp

joyGetPos_EXPORT proc
  jmp QWORD ptr joyGetPos_Original
joyGetPos_EXPORT endp

joyGetPosEx_EXPORT proc
  jmp QWORD ptr joyGetPosEx_Original
joyGetPosEx_EXPORT endp

joyGetThreshold_EXPORT proc
  jmp QWORD ptr joyGetThreshold_Original
joyGetThreshold_EXPORT endp

joyReleaseCapture_EXPORT proc
  jmp QWORD ptr joyReleaseCapture_Original
joyReleaseCapture_EXPORT endp

joySetCapture_EXPORT proc
  jmp QWORD ptr joySetCapture_Original
joySetCapture_EXPORT endp

joySetThreshold_EXPORT proc
  jmp QWORD ptr joySetThreshold_Original
joySetThreshold_EXPORT endp

mciDriverNotify_EXPORT proc
  jmp QWORD ptr mciDriverNotify_Original
mciDriverNotify_EXPORT endp

mciDriverYield_EXPORT proc
  jmp QWORD ptr mciDriverYield_Original
mciDriverYield_EXPORT endp

mciFreeCommandResource_EXPORT proc
  jmp QWORD ptr mciFreeCommandResource_Original
mciFreeCommandResource_EXPORT endp

mciGetCreatorTask_EXPORT proc
  jmp QWORD ptr mciGetCreatorTask_Original
mciGetCreatorTask_EXPORT endp

mciGetDeviceIDA_EXPORT proc
  jmp QWORD ptr mciGetDeviceIDA_Original
mciGetDeviceIDA_EXPORT endp

mciGetDeviceIDFromElementIDA_EXPORT proc
  jmp QWORD ptr mciGetDeviceIDFromElementIDA_Original
mciGetDeviceIDFromElementIDA_EXPORT endp

mciGetDeviceIDFromElementIDW_EXPORT proc
  jmp QWORD ptr mciGetDeviceIDFromElementIDW_Original
mciGetDeviceIDFromElementIDW_EXPORT endp

mciGetDeviceIDW_EXPORT proc
  jmp QWORD ptr mciGetDeviceIDW_Original
mciGetDeviceIDW_EXPORT endp

mciGetDriverData_EXPORT proc
  jmp QWORD ptr mciGetDriverData_Original
mciGetDriverData_EXPORT endp

mciGetErrorStringA_EXPORT proc
  jmp QWORD ptr mciGetErrorStringA_Original
mciGetErrorStringA_EXPORT endp

mciGetErrorStringW_EXPORT proc
  jmp QWORD ptr mciGetErrorStringW_Original
mciGetErrorStringW_EXPORT endp

mciGetYieldProc_EXPORT proc
  jmp QWORD ptr mciGetYieldProc_Original
mciGetYieldProc_EXPORT endp

mciLoadCommandResource_EXPORT proc
  jmp QWORD ptr mciLoadCommandResource_Original
mciLoadCommandResource_EXPORT endp

mciSendCommandA_EXPORT proc
  jmp QWORD ptr mciSendCommandA_Original
mciSendCommandA_EXPORT endp

mciSendCommandW_EXPORT proc
  jmp QWORD ptr mciSendCommandW_Original
mciSendCommandW_EXPORT endp

mciSendStringA_EXPORT proc
  jmp QWORD ptr mciSendStringA_Original
mciSendStringA_EXPORT endp

mciSendStringW_EXPORT proc
  jmp QWORD ptr mciSendStringW_Original
mciSendStringW_EXPORT endp

mciSetDriverData_EXPORT proc
  jmp QWORD ptr mciSetDriverData_Original
mciSetDriverData_EXPORT endp

mciSetYieldProc_EXPORT proc
  jmp QWORD ptr mciSetYieldProc_Original
mciSetYieldProc_EXPORT endp

midiConnect_EXPORT proc
  jmp QWORD ptr midiConnect_Original
midiConnect_EXPORT endp

midiDisconnect_EXPORT proc
  jmp QWORD ptr midiDisconnect_Original
midiDisconnect_EXPORT endp

midiInAddBuffer_EXPORT proc
  jmp QWORD ptr midiInAddBuffer_Original
midiInAddBuffer_EXPORT endp

midiInClose_EXPORT proc
  jmp QWORD ptr midiInClose_Original
midiInClose_EXPORT endp

midiInGetDevCapsA_EXPORT proc
  jmp QWORD ptr midiInGetDevCapsA_Original
midiInGetDevCapsA_EXPORT endp

midiInGetDevCapsW_EXPORT proc
  jmp QWORD ptr midiInGetDevCapsW_Original
midiInGetDevCapsW_EXPORT endp

midiInGetErrorTextA_EXPORT proc
  jmp QWORD ptr midiInGetErrorTextA_Original
midiInGetErrorTextA_EXPORT endp

midiInGetErrorTextW_EXPORT proc
  jmp QWORD ptr midiInGetErrorTextW_Original
midiInGetErrorTextW_EXPORT endp

midiInGetID_EXPORT proc
  jmp QWORD ptr midiInGetID_Original
midiInGetID_EXPORT endp

midiInGetNumDevs_EXPORT proc
  jmp QWORD ptr midiInGetNumDevs_Original
midiInGetNumDevs_EXPORT endp

midiInMessage_EXPORT proc
  jmp QWORD ptr midiInMessage_Original
midiInMessage_EXPORT endp

midiInOpen_EXPORT proc
  jmp QWORD ptr midiInOpen_Original
midiInOpen_EXPORT endp

midiInPrepareHeader_EXPORT proc
  jmp QWORD ptr midiInPrepareHeader_Original
midiInPrepareHeader_EXPORT endp

midiInReset_EXPORT proc
  jmp QWORD ptr midiInReset_Original
midiInReset_EXPORT endp

midiInStart_EXPORT proc
  jmp QWORD ptr midiInStart_Original
midiInStart_EXPORT endp

midiInStop_EXPORT proc
  jmp QWORD ptr midiInStop_Original
midiInStop_EXPORT endp

midiInUnprepareHeader_EXPORT proc
  jmp QWORD ptr midiInUnprepareHeader_Original
midiInUnprepareHeader_EXPORT endp

midiOutCacheDrumPatches_EXPORT proc
  jmp QWORD ptr midiOutCacheDrumPatches_Original
midiOutCacheDrumPatches_EXPORT endp

midiOutCachePatches_EXPORT proc
  jmp QWORD ptr midiOutCachePatches_Original
midiOutCachePatches_EXPORT endp

midiOutClose_EXPORT proc
  jmp QWORD ptr midiOutClose_Original
midiOutClose_EXPORT endp

midiOutGetDevCapsA_EXPORT proc
  jmp QWORD ptr midiOutGetDevCapsA_Original
midiOutGetDevCapsA_EXPORT endp

midiOutGetDevCapsW_EXPORT proc
  jmp QWORD ptr midiOutGetDevCapsW_Original
midiOutGetDevCapsW_EXPORT endp

midiOutGetErrorTextA_EXPORT proc
  jmp QWORD ptr midiOutGetErrorTextA_Original
midiOutGetErrorTextA_EXPORT endp

midiOutGetErrorTextW_EXPORT proc
  jmp QWORD ptr midiOutGetErrorTextW_Original
midiOutGetErrorTextW_EXPORT endp

midiOutGetID_EXPORT proc
  jmp QWORD ptr midiOutGetID_Original
midiOutGetID_EXPORT endp

midiOutGetNumDevs_EXPORT proc
  jmp QWORD ptr midiOutGetNumDevs_Original
midiOutGetNumDevs_EXPORT endp

midiOutGetVolume_EXPORT proc
  jmp QWORD ptr midiOutGetVolume_Original
midiOutGetVolume_EXPORT endp

midiOutLongMsg_EXPORT proc
  jmp QWORD ptr midiOutLongMsg_Original
midiOutLongMsg_EXPORT endp

midiOutMessage_EXPORT proc
  jmp QWORD ptr midiOutMessage_Original
midiOutMessage_EXPORT endp

midiOutOpen_EXPORT proc
  jmp QWORD ptr midiOutOpen_Original
midiOutOpen_EXPORT endp

midiOutPrepareHeader_EXPORT proc
  jmp QWORD ptr midiOutPrepareHeader_Original
midiOutPrepareHeader_EXPORT endp

midiOutReset_EXPORT proc
  jmp QWORD ptr midiOutReset_Original
midiOutReset_EXPORT endp

midiOutSetVolume_EXPORT proc
  jmp QWORD ptr midiOutSetVolume_Original
midiOutSetVolume_EXPORT endp

midiOutShortMsg_EXPORT proc
  jmp QWORD ptr midiOutShortMsg_Original
midiOutShortMsg_EXPORT endp

midiOutUnprepareHeader_EXPORT proc
  jmp QWORD ptr midiOutUnprepareHeader_Original
midiOutUnprepareHeader_EXPORT endp

midiStreamClose_EXPORT proc
  jmp QWORD ptr midiStreamClose_Original
midiStreamClose_EXPORT endp

midiStreamOpen_EXPORT proc
  jmp QWORD ptr midiStreamOpen_Original
midiStreamOpen_EXPORT endp

midiStreamOut_EXPORT proc
  jmp QWORD ptr midiStreamOut_Original
midiStreamOut_EXPORT endp

midiStreamPause_EXPORT proc
  jmp QWORD ptr midiStreamPause_Original
midiStreamPause_EXPORT endp

midiStreamPosition_EXPORT proc
  jmp QWORD ptr midiStreamPosition_Original
midiStreamPosition_EXPORT endp

midiStreamProperty_EXPORT proc
  jmp QWORD ptr midiStreamProperty_Original
midiStreamProperty_EXPORT endp

midiStreamRestart_EXPORT proc
  jmp QWORD ptr midiStreamRestart_Original
midiStreamRestart_EXPORT endp

midiStreamStop_EXPORT proc
  jmp QWORD ptr midiStreamStop_Original
midiStreamStop_EXPORT endp

mixerClose_EXPORT proc
  jmp QWORD ptr mixerClose_Original
mixerClose_EXPORT endp

mixerGetControlDetailsA_EXPORT proc
  jmp QWORD ptr mixerGetControlDetailsA_Original
mixerGetControlDetailsA_EXPORT endp

mixerGetControlDetailsW_EXPORT proc
  jmp QWORD ptr mixerGetControlDetailsW_Original
mixerGetControlDetailsW_EXPORT endp

mixerGetDevCapsA_EXPORT proc
  jmp QWORD ptr mixerGetDevCapsA_Original
mixerGetDevCapsA_EXPORT endp

mixerGetDevCapsW_EXPORT proc
  jmp QWORD ptr mixerGetDevCapsW_Original
mixerGetDevCapsW_EXPORT endp

mixerGetID_EXPORT proc
  jmp QWORD ptr mixerGetID_Original
mixerGetID_EXPORT endp

mixerGetLineControlsA_EXPORT proc
  jmp QWORD ptr mixerGetLineControlsA_Original
mixerGetLineControlsA_EXPORT endp

mixerGetLineControlsW_EXPORT proc
  jmp QWORD ptr mixerGetLineControlsW_Original
mixerGetLineControlsW_EXPORT endp

mixerGetLineInfoA_EXPORT proc
  jmp QWORD ptr mixerGetLineInfoA_Original
mixerGetLineInfoA_EXPORT endp

mixerGetLineInfoW_EXPORT proc
  jmp QWORD ptr mixerGetLineInfoW_Original
mixerGetLineInfoW_EXPORT endp

mixerGetNumDevs_EXPORT proc
  jmp QWORD ptr mixerGetNumDevs_Original
mixerGetNumDevs_EXPORT endp

mixerMessage_EXPORT proc
  jmp QWORD ptr mixerMessage_Original
mixerMessage_EXPORT endp

mixerOpen_EXPORT proc
  jmp QWORD ptr mixerOpen_Original
mixerOpen_EXPORT endp

mixerSetControlDetails_EXPORT proc
  jmp QWORD ptr mixerSetControlDetails_Original
mixerSetControlDetails_EXPORT endp

mmDrvInstall_EXPORT proc
  jmp QWORD ptr mmDrvInstall_Original
mmDrvInstall_EXPORT endp

mmGetCurrentTask_EXPORT proc
  jmp QWORD ptr mmGetCurrentTask_Original
mmGetCurrentTask_EXPORT endp

mmTaskBlock_EXPORT proc
  jmp QWORD ptr mmTaskBlock_Original
mmTaskBlock_EXPORT endp

mmTaskCreate_EXPORT proc
  jmp QWORD ptr mmTaskCreate_Original
mmTaskCreate_EXPORT endp

mmTaskSignal_EXPORT proc
  jmp QWORD ptr mmTaskSignal_Original
mmTaskSignal_EXPORT endp

mmTaskYield_EXPORT proc
  jmp QWORD ptr mmTaskYield_Original
mmTaskYield_EXPORT endp

mmioAdvance_EXPORT proc
  jmp QWORD ptr mmioAdvance_Original
mmioAdvance_EXPORT endp

mmioAscend_EXPORT proc
  jmp QWORD ptr mmioAscend_Original
mmioAscend_EXPORT endp

mmioClose_EXPORT proc
  jmp QWORD ptr mmioClose_Original
mmioClose_EXPORT endp

mmioCreateChunk_EXPORT proc
  jmp QWORD ptr mmioCreateChunk_Original
mmioCreateChunk_EXPORT endp

mmioDescend_EXPORT proc
  jmp QWORD ptr mmioDescend_Original
mmioDescend_EXPORT endp

mmioFlush_EXPORT proc
  jmp QWORD ptr mmioFlush_Original
mmioFlush_EXPORT endp

mmioGetInfo_EXPORT proc
  jmp QWORD ptr mmioGetInfo_Original
mmioGetInfo_EXPORT endp

mmioInstallIOProcA_EXPORT proc
  jmp QWORD ptr mmioInstallIOProcA_Original
mmioInstallIOProcA_EXPORT endp

mmioInstallIOProcW_EXPORT proc
  jmp QWORD ptr mmioInstallIOProcW_Original
mmioInstallIOProcW_EXPORT endp

mmioOpenA_EXPORT proc
  jmp QWORD ptr mmioOpenA_Original
mmioOpenA_EXPORT endp

mmioOpenW_EXPORT proc
  jmp QWORD ptr mmioOpenW_Original
mmioOpenW_EXPORT endp

mmioRead_EXPORT proc
  jmp QWORD ptr mmioRead_Original
mmioRead_EXPORT endp

mmioRenameA_EXPORT proc
  jmp QWORD ptr mmioRenameA_Original
mmioRenameA_EXPORT endp

mmioRenameW_EXPORT proc
  jmp QWORD ptr mmioRenameW_Original
mmioRenameW_EXPORT endp

mmioSeek_EXPORT proc
  jmp QWORD ptr mmioSeek_Original
mmioSeek_EXPORT endp

mmioSendMessage_EXPORT proc
  jmp QWORD ptr mmioSendMessage_Original
mmioSendMessage_EXPORT endp

mmioSetBuffer_EXPORT proc
  jmp QWORD ptr mmioSetBuffer_Original
mmioSetBuffer_EXPORT endp

mmioSetInfo_EXPORT proc
  jmp QWORD ptr mmioSetInfo_Original
mmioSetInfo_EXPORT endp

mmioStringToFOURCCA_EXPORT proc
  jmp QWORD ptr mmioStringToFOURCCA_Original
mmioStringToFOURCCA_EXPORT endp

mmioStringToFOURCCW_EXPORT proc
  jmp QWORD ptr mmioStringToFOURCCW_Original
mmioStringToFOURCCW_EXPORT endp

mmioWrite_EXPORT proc
  jmp QWORD ptr mmioWrite_Original
mmioWrite_EXPORT endp

mmsystemGetVersion_EXPORT proc
  jmp QWORD ptr mmsystemGetVersion_Original
mmsystemGetVersion_EXPORT endp

sndPlaySoundA_EXPORT proc
  jmp QWORD ptr sndPlaySoundA_Original
sndPlaySoundA_EXPORT endp

sndPlaySoundW_EXPORT proc
  jmp QWORD ptr sndPlaySoundW_Original
sndPlaySoundW_EXPORT endp

timeBeginPeriod_EXPORT proc
  jmp QWORD ptr timeBeginPeriod_Original
timeBeginPeriod_EXPORT endp

timeEndPeriod_EXPORT proc
  jmp QWORD ptr timeEndPeriod_Original
timeEndPeriod_EXPORT endp

timeGetDevCaps_EXPORT proc
  jmp QWORD ptr timeGetDevCaps_Original
timeGetDevCaps_EXPORT endp

timeGetSystemTime_EXPORT proc
  jmp QWORD ptr timeGetSystemTime_Original
timeGetSystemTime_EXPORT endp

timeGetTime_EXPORT proc
  jmp QWORD ptr timeGetTime_Original
timeGetTime_EXPORT endp

timeKillEvent_EXPORT proc
  jmp QWORD ptr timeKillEvent_Original
timeKillEvent_EXPORT endp

timeSetEvent_EXPORT proc
  jmp QWORD ptr timeSetEvent_Original
timeSetEvent_EXPORT endp

waveInAddBuffer_EXPORT proc
  jmp QWORD ptr waveInAddBuffer_Original
waveInAddBuffer_EXPORT endp

waveInClose_EXPORT proc
  jmp QWORD ptr waveInClose_Original
waveInClose_EXPORT endp

waveInGetDevCapsA_EXPORT proc
  jmp QWORD ptr waveInGetDevCapsA_Original
waveInGetDevCapsA_EXPORT endp

waveInGetDevCapsW_EXPORT proc
  jmp QWORD ptr waveInGetDevCapsW_Original
waveInGetDevCapsW_EXPORT endp

waveInGetErrorTextA_EXPORT proc
  jmp QWORD ptr waveInGetErrorTextA_Original
waveInGetErrorTextA_EXPORT endp

waveInGetErrorTextW_EXPORT proc
  jmp QWORD ptr waveInGetErrorTextW_Original
waveInGetErrorTextW_EXPORT endp

waveInGetID_EXPORT proc
  jmp QWORD ptr waveInGetID_Original
waveInGetID_EXPORT endp

waveInGetNumDevs_EXPORT proc
  jmp QWORD ptr waveInGetNumDevs_Original
waveInGetNumDevs_EXPORT endp

waveInGetPosition_EXPORT proc
  jmp QWORD ptr waveInGetPosition_Original
waveInGetPosition_EXPORT endp

waveInMessage_EXPORT proc
  jmp QWORD ptr waveInMessage_Original
waveInMessage_EXPORT endp

waveInOpen_EXPORT proc
  jmp QWORD ptr waveInOpen_Original
waveInOpen_EXPORT endp

waveInPrepareHeader_EXPORT proc
  jmp QWORD ptr waveInPrepareHeader_Original
waveInPrepareHeader_EXPORT endp

waveInReset_EXPORT proc
  jmp QWORD ptr waveInReset_Original
waveInReset_EXPORT endp

waveInStart_EXPORT proc
  jmp QWORD ptr waveInStart_Original
waveInStart_EXPORT endp

waveInStop_EXPORT proc
  jmp QWORD ptr waveInStop_Original
waveInStop_EXPORT endp

waveInUnprepareHeader_EXPORT proc
  jmp QWORD ptr waveInUnprepareHeader_Original
waveInUnprepareHeader_EXPORT endp

waveOutBreakLoop_EXPORT proc
  jmp QWORD ptr waveOutBreakLoop_Original
waveOutBreakLoop_EXPORT endp

waveOutClose_EXPORT proc
  jmp QWORD ptr waveOutClose_Original
waveOutClose_EXPORT endp

waveOutGetDevCapsA_EXPORT proc
  jmp QWORD ptr waveOutGetDevCapsA_Original
waveOutGetDevCapsA_EXPORT endp

waveOutGetDevCapsW_EXPORT proc
  jmp QWORD ptr waveOutGetDevCapsW_Original
waveOutGetDevCapsW_EXPORT endp

waveOutGetErrorTextA_EXPORT proc
  jmp QWORD ptr waveOutGetErrorTextA_Original
waveOutGetErrorTextA_EXPORT endp

waveOutGetErrorTextW_EXPORT proc
  jmp QWORD ptr waveOutGetErrorTextW_Original
waveOutGetErrorTextW_EXPORT endp

waveOutGetID_EXPORT proc
  jmp QWORD ptr waveOutGetID_Original
waveOutGetID_EXPORT endp

waveOutGetNumDevs_EXPORT proc
  jmp QWORD ptr waveOutGetNumDevs_Original
waveOutGetNumDevs_EXPORT endp

waveOutGetPitch_EXPORT proc
  jmp QWORD ptr waveOutGetPitch_Original
waveOutGetPitch_EXPORT endp

waveOutGetPlaybackRate_EXPORT proc
  jmp QWORD ptr waveOutGetPlaybackRate_Original
waveOutGetPlaybackRate_EXPORT endp

waveOutGetPosition_EXPORT proc
  jmp QWORD ptr waveOutGetPosition_Original
waveOutGetPosition_EXPORT endp

waveOutGetVolume_EXPORT proc
  jmp QWORD ptr waveOutGetVolume_Original
waveOutGetVolume_EXPORT endp

waveOutMessage_EXPORT proc
  jmp QWORD ptr waveOutMessage_Original
waveOutMessage_EXPORT endp

waveOutOpen_EXPORT proc
  jmp QWORD ptr waveOutOpen_Original
waveOutOpen_EXPORT endp

waveOutPause_EXPORT proc
  jmp QWORD ptr waveOutPause_Original
waveOutPause_EXPORT endp

waveOutPrepareHeader_EXPORT proc
  jmp QWORD ptr waveOutPrepareHeader_Original
waveOutPrepareHeader_EXPORT endp

waveOutReset_EXPORT proc
  jmp QWORD ptr waveOutReset_Original
waveOutReset_EXPORT endp

waveOutRestart_EXPORT proc
  jmp QWORD ptr waveOutRestart_Original
waveOutRestart_EXPORT endp

waveOutSetPitch_EXPORT proc
  jmp QWORD ptr waveOutSetPitch_Original
waveOutSetPitch_EXPORT endp

waveOutSetPlaybackRate_EXPORT proc
  jmp QWORD ptr waveOutSetPlaybackRate_Original
waveOutSetPlaybackRate_EXPORT endp

waveOutSetVolume_EXPORT proc
  jmp QWORD ptr waveOutSetVolume_Original
waveOutSetVolume_EXPORT endp

waveOutUnprepareHeader_EXPORT proc
  jmp QWORD ptr waveOutUnprepareHeader_Original
waveOutUnprepareHeader_EXPORT endp

waveOutWrite_EXPORT proc
  jmp QWORD ptr waveOutWrite_Original
waveOutWrite_EXPORT endp

end
