#include "User.h"


void PWM(uint8_t Passage, int16_t duty)
{
	/* 限幅: ±PWM_PERIOD */
	if (duty >  PWM_PERIOD) duty =  PWM_PERIOD;
	if (duty < -PWM_PERIOD) duty = -PWM_PERIOD;

	uint16_t a = (duty >= 0) ?  duty : 0;
	uint16_t b = (duty >= 0) ? 0 : -duty;

	switch (Passage) {
	case 1: /* TIMG8: CC0=正转, CC1=反转 */

	
	DL_TimerA_setCaptureCompareValue(PWM_2_INST, b, DL_TIMER_CC_1_INDEX);
		DL_TimerA_setCaptureCompareValue(PWM_2_INST, a, DL_TIMER_CC_3_INDEX);
		break;
	case 2: /* 电机2: TIMA0 CC0=正转, CC2=反转 */
		DL_TimerA_setCaptureCompareValue(PWM_2_INST, b, DL_TIMER_CC_0_INDEX);
		DL_TimerA_setCaptureCompareValue(PWM_2_INST, a, DL_TIMER_CC_2_INDEX);
		break;
	case 3: /* 电机3: TIMA1 CC1=反转, CC0=正转 */
		
	DL_TimerA_setCaptureCompareValue(PWM_0_INST, b, DL_TIMER_CC_0_INDEX);
		DL_TimerA_setCaptureCompareValue(PWM_0_INST, a, DL_TIMER_CC_1_INDEX);
		break;
	case 4: /* 电机4: TIMG8 CC0=正转, CC1=反转 */
		
	DL_TimerG_setCaptureCompareValue(PWM_1_INST, b, DL_TIMER_CC_0_INDEX);
		DL_TimerG_setCaptureCompareValue(PWM_1_INST, a, DL_TIMER_CC_1_INDEX);
		break;
	default:
		DL_TimerG_setCaptureCompareValue(PWM_1_INST, 0, DL_TIMER_CC_0_INDEX);
		DL_TimerG_setCaptureCompareValue(PWM_1_INST, 0, DL_TIMER_CC_1_INDEX);
		DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, DL_TIMER_CC_0_INDEX);
		DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, DL_TIMER_CC_1_INDEX);
		DL_TimerA_setCaptureCompareValue(PWM_2_INST, 0, DL_TIMER_CC_0_INDEX);
		DL_TimerA_setCaptureCompareValue(PWM_2_INST, 0, DL_TIMER_CC_1_INDEX);
		DL_TimerA_setCaptureCompareValue(PWM_2_INST, 0, DL_TIMER_CC_2_INDEX);
		DL_TimerA_setCaptureCompareValue(PWM_2_INST, 0, DL_TIMER_CC_3_INDEX);
		break;
	}
}
