#ifndef _HPS_TIM
#define _HPS_TIM

	typedef enum {
		TIM0 = 0x00000001,
		TIM1 = 0x00000002,
		TIM2 = 0x00000004,
		TIM3 = 0x00000008
	}	HPS_TIM_t;

	typedef struct {
		HPS_TIM_t tim;
		int timeout; //in MIRCOsec 10^-6
		int LD_en;
		int INT_en;
		int enable;
	}	HPS_TIM_config_t;

	// arg is struct pointer so the struct elements are 
	//successive in memory by offest #4
	extern void HPS_TIM_config_ASM(HPS_TIM_config_t *param);

	//read value of the S-bit offset is 0x10(16 dec)
	//nature of return depend on wheter this func can read
	//the S-bit balue of the multiple timers in the same call
	
	extern int HPS_TIM_read_INT_ASM(HPS_TIM_t tim);

	//reset S-bit and F-bit
	//func should support multiple timers in
	// the argument
	
	extern void HPS_TIM_clear_INT_ASM(HPS_TIM_ttime);

	extern int EDGECAP_ASM();//return byte
	extern void CLEAR_EDGECAP_ASM(); // Clear Edgecap

#endif
