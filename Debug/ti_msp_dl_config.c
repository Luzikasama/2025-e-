/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerA_backupConfig gENCODER1ABackup;
DL_TimerG_backupConfig gENCODER2ABackup;
DL_TimerG_backupConfig gTIMER_STEPPER_XBackup;
DL_TimerA_backupConfig gTIMER_STEPPER_YBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_PWM_0_init();
    SYSCFG_DL_ENCODER1A_init();
    SYSCFG_DL_ENCODER2A_init();
    SYSCFG_DL_TIMER_STEPPER_X_init();
    SYSCFG_DL_TIMER_STEPPER_Y_init();
    SYSCFG_DL_UART_HMI_init();
    SYSCFG_DL_UART_RASPBERRYPI_init();
    SYSCFG_DL_UART_DEBUG_init();
    SYSCFG_DL_DMA_init();
    /* Ensure backup structures have no valid state */

	gENCODER1ABackup.backupRdy 	= false;
	gENCODER2ABackup.backupRdy 	= false;
	gTIMER_STEPPER_XBackup.backupRdy 	= false;
	gTIMER_STEPPER_YBackup.backupRdy 	= false;


}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_saveConfiguration(ENCODER1A_INST, &gENCODER1ABackup);
	retStatus &= DL_TimerG_saveConfiguration(ENCODER2A_INST, &gENCODER2ABackup);
	retStatus &= DL_TimerG_saveConfiguration(TIMER_STEPPER_X_INST, &gTIMER_STEPPER_XBackup);
	retStatus &= DL_TimerA_saveConfiguration(TIMER_STEPPER_Y_INST, &gTIMER_STEPPER_YBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_restoreConfiguration(ENCODER1A_INST, &gENCODER1ABackup, false);
	retStatus &= DL_TimerG_restoreConfiguration(ENCODER2A_INST, &gENCODER2ABackup, false);
	retStatus &= DL_TimerG_restoreConfiguration(TIMER_STEPPER_X_INST, &gTIMER_STEPPER_XBackup, false);
	retStatus &= DL_TimerA_restoreConfiguration(TIMER_STEPPER_Y_INST, &gTIMER_STEPPER_YBackup, false);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerG_reset(PWM_0_INST);
    DL_TimerA_reset(ENCODER1A_INST);
    DL_TimerG_reset(ENCODER2A_INST);
    DL_TimerG_reset(TIMER_STEPPER_X_INST);
    DL_TimerA_reset(TIMER_STEPPER_Y_INST);
    DL_UART_Main_reset(UART_HMI_INST);
    DL_UART_Main_reset(UART_RASPBERRYPI_INST);
    DL_UART_Main_reset(UART_DEBUG_INST);


    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerG_enablePower(PWM_0_INST);
    DL_TimerA_enablePower(ENCODER1A_INST);
    DL_TimerG_enablePower(ENCODER2A_INST);
    DL_TimerG_enablePower(TIMER_STEPPER_X_INST);
    DL_TimerA_enablePower(TIMER_STEPPER_Y_INST);
    DL_UART_Main_enablePower(UART_HMI_INST);
    DL_UART_Main_enablePower(UART_RASPBERRYPI_INST);
    DL_UART_Main_enablePower(UART_DEBUG_INST);

    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_0_C0_IOMUX,GPIO_PWM_0_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_0_C0_PORT, GPIO_PWM_0_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_0_C1_IOMUX,GPIO_PWM_0_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_0_C1_PORT, GPIO_PWM_0_C1_PIN);

    DL_GPIO_initPeripheralInputFunction(GPIO_ENCODER1A_C0_IOMUX,GPIO_ENCODER1A_C0_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_ENCODER2A_C0_IOMUX,GPIO_ENCODER2A_C0_IOMUX_FUNC);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_HMI_IOMUX_TX, GPIO_UART_HMI_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_HMI_IOMUX_RX, GPIO_UART_HMI_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_RASPBERRYPI_IOMUX_TX, GPIO_UART_RASPBERRYPI_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_RASPBERRYPI_IOMUX_RX, GPIO_UART_RASPBERRYPI_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_DEBUG_IOMUX_TX, GPIO_UART_DEBUG_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_DEBUG_IOMUX_RX, GPIO_UART_DEBUG_IOMUX_RX_FUNC);

    DL_GPIO_initDigitalOutput(GPIO_RELAY_RELAY_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_GW_SENSOR_R4_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_GW_SENSOR_R3_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_GW_SENSOR_R2_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_GW_SENSOR_R1_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_GW_SENSOR_L1_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_GW_SENSOR_L2_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_GW_SENSOR_L3_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_GW_SENSOR_L4_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_MOTOR_AIN1_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_MOTOR_AIN2_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_MOTOR_BIN1_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_MOTOR_BIN2_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_ENCODER_ENCODER1B_IOMUX);

    DL_GPIO_initDigitalInput(GPIO_ENCODER_ENCODER2B_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_STEPPER_STEPPER_X_STP_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_STEPPER_STEPPER_X_DIR_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_STEPPER_STEPPER_X_EN_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_STEPPER_STEPPER_Y_STP_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_STEPPER_STEPPER_Y_DIR_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_STEPPER_STEPPER_Y_EN_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_LED_LED_RED_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_LED_LED_GREEN_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_LED_LED_YELLOW_IOMUX);

    DL_GPIO_clearPins(GPIOA, GPIO_MOTOR_AIN1_PIN |
		GPIO_MOTOR_BIN2_PIN |
		GPIO_STEPPER_STEPPER_X_DIR_PIN |
		GPIO_STEPPER_STEPPER_Y_STP_PIN |
		GPIO_STEPPER_STEPPER_Y_DIR_PIN |
		GPIO_STEPPER_STEPPER_Y_EN_PIN);
    DL_GPIO_setPins(GPIOA, GPIO_LED_LED_RED_PIN |
		GPIO_LED_LED_YELLOW_PIN);
    DL_GPIO_enableOutput(GPIOA, GPIO_MOTOR_AIN1_PIN |
		GPIO_MOTOR_BIN2_PIN |
		GPIO_STEPPER_STEPPER_X_DIR_PIN |
		GPIO_STEPPER_STEPPER_Y_STP_PIN |
		GPIO_STEPPER_STEPPER_Y_DIR_PIN |
		GPIO_STEPPER_STEPPER_Y_EN_PIN |
		GPIO_LED_LED_RED_PIN |
		GPIO_LED_LED_YELLOW_PIN);
    DL_GPIO_clearPins(GPIOB, GPIO_RELAY_RELAY_PIN |
		GPIO_MOTOR_AIN2_PIN |
		GPIO_MOTOR_BIN1_PIN |
		GPIO_STEPPER_STEPPER_X_STP_PIN |
		GPIO_STEPPER_STEPPER_X_EN_PIN);
    DL_GPIO_setPins(GPIOB, GPIO_LED_LED_GREEN_PIN);
    DL_GPIO_enableOutput(GPIOB, GPIO_RELAY_RELAY_PIN |
		GPIO_MOTOR_AIN2_PIN |
		GPIO_MOTOR_BIN1_PIN |
		GPIO_STEPPER_STEPPER_X_STP_PIN |
		GPIO_STEPPER_STEPPER_X_EN_PIN |
		GPIO_LED_LED_GREEN_PIN);

}


static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
    .inputFreq              = DL_SYSCTL_SYSPLL_INPUT_FREQ_16_32_MHZ,
	.rDivClk2x              = 3,
	.rDivClk1               = 0,
	.rDivClk0               = 0,
	.enableCLK2x            = DL_SYSCTL_SYSPLL_CLK2X_ENABLE,
	.enableCLK1             = DL_SYSCTL_SYSPLL_CLK1_DISABLE,
	.enableCLK0             = DL_SYSCTL_SYSPLL_CLK0_DISABLE,
	.sysPLLMCLK             = DL_SYSCTL_SYSPLL_MCLK_CLK2X,
	.sysPLLRef              = DL_SYSCTL_SYSPLL_REF_SYSOSC,
	.qDiv                   = 9,
	.pDiv                   = DL_SYSCTL_SYSPLL_PDIV_2
};
SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_2);

    
	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
	/* Set default configuration */
	DL_SYSCTL_disableHFXT();
	DL_SYSCTL_disableSYSPLL();
    DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *) &gSYSPLLConfig);
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_2);
    DL_SYSCTL_setMCLKSource(SYSOSC, HSCLK, DL_SYSCTL_HSCLK_SOURCE_SYSPLL);

}


/*
 * Timer clock configuration to be sourced by  / 1 (40000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   40000000 Hz = 40000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gPWM_0ClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

static const DL_TimerG_PWMConfig gPWM_0Config = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period = 1000,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_0_init(void) {

    DL_TimerG_setClockConfig(
        PWM_0_INST, (DL_TimerG_ClockConfig *) &gPWM_0ClockConfig);

    DL_TimerG_initPWMMode(
        PWM_0_INST, (DL_TimerG_PWMConfig *) &gPWM_0Config);

    DL_TimerG_setCaptureCompareOutCtl(PWM_0_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_0_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, 0, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareOutCtl(PWM_0_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_1_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_0_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_0_INST, 0, DL_TIMER_CC_1_INDEX);

    DL_TimerG_enableClock(PWM_0_INST);


    
    DL_TimerG_setCCPDirection(PWM_0_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );


}



/*
 * Timer clock configuration to be sourced by BUSCLK /  (80000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   312500 Hz = 80000000 Hz / (1 * (255 + 1))
 */
static const DL_TimerA_ClockConfig gENCODER1AClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 255U
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * ENCODER1A_INST_LOAD_VALUE = (1ms * 312500 Hz) - 1
 */
static const DL_TimerA_CaptureConfig gENCODER1ACaptureConfig = {
    .captureMode    = DL_TIMER_CAPTURE_MODE_EDGE_TIME,
    .period         = ENCODER1A_INST_LOAD_VALUE,
    .startTimer     = DL_TIMER_STOP,
    .edgeCaptMode   = DL_TIMER_CAPTURE_EDGE_DETECTION_MODE_RISING,
    .inputChan      = DL_TIMER_INPUT_CHAN_0,
    .inputInvMode   = DL_TIMER_CC_INPUT_INV_NOINVERT,
};

SYSCONFIG_WEAK void SYSCFG_DL_ENCODER1A_init(void) {

    DL_TimerA_setClockConfig(ENCODER1A_INST,
        (DL_TimerA_ClockConfig *) &gENCODER1AClockConfig);

    DL_TimerA_initCaptureMode(ENCODER1A_INST,
        (DL_TimerA_CaptureConfig *) &gENCODER1ACaptureConfig);
    DL_TimerA_enableInterrupt(ENCODER1A_INST , DL_TIMERA_INTERRUPT_CC0_DN_EVENT);

    DL_TimerA_enableClock(ENCODER1A_INST);

}

/*
 * Timer clock configuration to be sourced by BUSCLK /  (80000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   312500 Hz = 80000000 Hz / (1 * (255 + 1))
 */
static const DL_TimerG_ClockConfig gENCODER2AClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 255U
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * ENCODER2A_INST_LOAD_VALUE = (1ms * 312500 Hz) - 1
 */
static const DL_TimerG_CaptureConfig gENCODER2ACaptureConfig = {
    .captureMode    = DL_TIMER_CAPTURE_MODE_EDGE_TIME,
    .period         = ENCODER2A_INST_LOAD_VALUE,
    .startTimer     = DL_TIMER_STOP,
    .edgeCaptMode   = DL_TIMER_CAPTURE_EDGE_DETECTION_MODE_RISING,
    .inputChan      = DL_TIMER_INPUT_CHAN_0,
    .inputInvMode   = DL_TIMER_CC_INPUT_INV_NOINVERT,
};

SYSCONFIG_WEAK void SYSCFG_DL_ENCODER2A_init(void) {

    DL_TimerG_setClockConfig(ENCODER2A_INST,
        (DL_TimerG_ClockConfig *) &gENCODER2AClockConfig);

    DL_TimerG_initCaptureMode(ENCODER2A_INST,
        (DL_TimerG_CaptureConfig *) &gENCODER2ACaptureConfig);
    DL_TimerG_enableInterrupt(ENCODER2A_INST , DL_TIMERG_INTERRUPT_CC0_DN_EVENT);

    DL_TimerG_enableClock(ENCODER2A_INST);

}


/*
 * Timer clock configuration to be sourced by BUSCLK /  (40000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   40000000 Hz = 40000000 Hz / (2 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gTIMER_STEPPER_XClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_2,
    .prescale    = 0U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_STEPPER_X_INST_LOAD_VALUE = (1ms * 40000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTIMER_STEPPER_XTimerConfig = {
    .period     = TIMER_STEPPER_X_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_STEPPER_X_init(void) {

    DL_TimerG_setClockConfig(TIMER_STEPPER_X_INST,
        (DL_TimerG_ClockConfig *) &gTIMER_STEPPER_XClockConfig);

    DL_TimerG_initTimerMode(TIMER_STEPPER_X_INST,
        (DL_TimerG_TimerConfig *) &gTIMER_STEPPER_XTimerConfig);
    DL_TimerG_enableInterrupt(TIMER_STEPPER_X_INST , DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TIMER_STEPPER_X_INST);





}

/*
 * Timer clock configuration to be sourced by BUSCLK /  (40000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   40000000 Hz = 40000000 Hz / (2 * (0 + 1))
 */
static const DL_TimerA_ClockConfig gTIMER_STEPPER_YClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_2,
    .prescale    = 0U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TIMER_STEPPER_Y_INST_LOAD_VALUE = (1 ms * 40000000 Hz) - 1
 */
static const DL_TimerA_TimerConfig gTIMER_STEPPER_YTimerConfig = {
    .period     = TIMER_STEPPER_Y_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_TIMER_STEPPER_Y_init(void) {

    DL_TimerA_setClockConfig(TIMER_STEPPER_Y_INST,
        (DL_TimerA_ClockConfig *) &gTIMER_STEPPER_YClockConfig);

    DL_TimerA_initTimerMode(TIMER_STEPPER_Y_INST,
        (DL_TimerA_TimerConfig *) &gTIMER_STEPPER_YTimerConfig);
    DL_TimerA_enableInterrupt(TIMER_STEPPER_Y_INST , DL_TIMERA_INTERRUPT_ZERO_EVENT);
	NVIC_SetPriority(TIMER_STEPPER_Y_INST_INT_IRQN, 0);
    DL_TimerA_enableClock(TIMER_STEPPER_Y_INST);





}



static const DL_UART_Main_ClockConfig gUART_HMIClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_HMIConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_HMI_init(void)
{
    DL_UART_Main_setClockConfig(UART_HMI_INST, (DL_UART_Main_ClockConfig *) &gUART_HMIClockConfig);

    DL_UART_Main_init(UART_HMI_INST, (DL_UART_Main_Config *) &gUART_HMIConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART_HMI_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_HMI_INST, UART_HMI_IBRD_40_MHZ_115200_BAUD, UART_HMI_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_HMI_INST,
                                 DL_UART_MAIN_INTERRUPT_RX);
    /* Setting the Interrupt Priority */
    NVIC_SetPriority(UART_HMI_INST_INT_IRQN, 0);


    DL_UART_Main_enable(UART_HMI_INST);
}

static const DL_UART_Main_ClockConfig gUART_RASPBERRYPIClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_RASPBERRYPIConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_RASPBERRYPI_init(void)
{
    DL_UART_Main_setClockConfig(UART_RASPBERRYPI_INST, (DL_UART_Main_ClockConfig *) &gUART_RASPBERRYPIClockConfig);

    DL_UART_Main_init(UART_RASPBERRYPI_INST, (DL_UART_Main_Config *) &gUART_RASPBERRYPIConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART_RASPBERRYPI_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_RASPBERRYPI_INST, UART_RASPBERRYPI_IBRD_40_MHZ_115200_BAUD, UART_RASPBERRYPI_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_RASPBERRYPI_INST,
                                 DL_UART_MAIN_INTERRUPT_RX);
    /* Setting the Interrupt Priority */
    NVIC_SetPriority(UART_RASPBERRYPI_INST_INT_IRQN, 1);


    DL_UART_Main_enable(UART_RASPBERRYPI_INST);
}

static const DL_UART_Main_ClockConfig gUART_DEBUGClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_DEBUGConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_DEBUG_init(void)
{
    DL_UART_Main_setClockConfig(UART_DEBUG_INST, (DL_UART_Main_ClockConfig *) &gUART_DEBUGClockConfig);

    DL_UART_Main_init(UART_DEBUG_INST, (DL_UART_Main_Config *) &gUART_DEBUGConfig);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115190.78
     */
    DL_UART_Main_setOversampling(UART_DEBUG_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_DEBUG_INST, UART_DEBUG_IBRD_40_MHZ_115200_BAUD, UART_DEBUG_FBRD_40_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_DEBUG_INST,
                                 DL_UART_MAIN_INTERRUPT_DMA_DONE_TX |
                                 DL_UART_MAIN_INTERRUPT_RX);
    /* Setting the Interrupt Priority */
    NVIC_SetPriority(UART_DEBUG_INST_INT_IRQN, 1);

    /* Configure DMA Transmit Event */
    DL_UART_Main_enableDMATransmitEvent(UART_DEBUG_INST);
    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART_DEBUG_INST);
    DL_UART_Main_setRXFIFOThreshold(UART_DEBUG_INST, DL_UART_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_UART_Main_setTXFIFOThreshold(UART_DEBUG_INST, DL_UART_TX_FIFO_LEVEL_ONE_ENTRY);

    DL_UART_Main_enable(UART_DEBUG_INST);
}

static const DL_DMA_Config gDMA_CH0Config = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_UNCHANGED,
    .srcIncrement   = DL_DMA_ADDR_INCREMENT,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = UART_DEBUG_INST_DMA_TRIGGER,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_CH0_init(void)
{
    DL_DMA_initChannel(DMA, DMA_CH0_CHAN_ID , (DL_DMA_Config *) &gDMA_CH0Config);
}
SYSCONFIG_WEAK void SYSCFG_DL_DMA_init(void){
    SYSCFG_DL_DMA_CH0_init();
}


