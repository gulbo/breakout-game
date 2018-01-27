#ifdef __cplusplus
extern "C" {
#endif
	
extern int32_t DAC_Initialize (void);
extern int32_t DAC_Uninitialize (void);
extern int32_t DAC_SetValue (uint32_t val);
extern uint32_t DAC_GetResolution (void);	
	
	
#ifdef __cplusplus
}
#endif
