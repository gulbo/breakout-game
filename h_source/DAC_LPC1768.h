#ifdef __cplusplus
extern "C" {
#endif

#define WIN true
#define START true
#define LOSS false
#define WIN_DURATION 120
#define START_DURATION 20
#define LOSS_DURATION 8


extern int32_t DAC_Initialize (void);
extern int32_t DAC_Uninitialize (void);
extern int32_t DAC_SetValue (uint32_t val);
extern uint32_t DAC_GetResolution (void);	
	
extern void sound_delay (int us);
extern void play_sound(void);
extern void play_music(bool win, uint32_t duration);

#ifdef __cplusplus
}
#endif
