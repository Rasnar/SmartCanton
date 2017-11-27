/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
 !!GlobalInfo
 product: Pins v3.0
 processor: MKW41Z512xxx4
 package_id: MKW41Z512VHT4
 mcu_data: ksdk2_0
 processor_version: 2.0.1
 pin_labels:
 - {pin_num: '4', pin_signal: TSI0_CH10/PTA16/LLWU_P4/SPI1_SOUT/TPM0_CH0, label: USR_LED0, identifier: USR_LED0}
 - {pin_num: '6', pin_signal: TSI0_CH12/PTA18/LLWU_P6/SPI1_SCK/TPM2_CH0, label: USR_LED2, identifier: USR_LED2}
 - {pin_num: '23', pin_signal: DAC0_OUT/ADC0_SE4/CMP0_IN2/PTB18/I2C1_SCL/TPM_CLKIN0/TPM0_CH0/NMI_b, label: BAT_LEVEL, identifier: BAT_LEVEL}
 - {pin_num: '18', pin_signal: ADC0_SE3/CMP0_IN3/PTB2/RF_NOT_ALLOWED/DTM_TX/TPM1_CH0, label: USR_BTN0, identifier: USR_BTN0}
 - {pin_num: '19', pin_signal: ADC0_SE2/CMP0_IN4/PTB3/CLKOUT/TPM1_CH1/RTC_CLKOUT, label: USR_BTN1, identifier: USR_BTN1}
 - {pin_num: '7', pin_signal: TSI0_CH13/ADC0_SE5/PTA19/LLWU_P7/SPI1_PCS0/TPM2_CH1, label: USR_LED3, identifier: USR_LED3}
 - {pin_num: '5', pin_signal: TSI0_CH11/PTA17/LLWU_P5/RF_RESET/SPI1_SIN/TPM_CLKIN1, label: USR_LED1, identifier: USR_LED1}
 - {pin_num: '40', pin_signal: TSI0_CH0/PTC4/LLWU_P12/ANT_A/EXTRG_IN/UART0_CTS_b/TPM1_CH0/BSM_DATA, label: PMOD_SSEL, identifier: PMOD_SSEL1}
 - {pin_num: '41', pin_signal: TSI0_CH1/PTC5/LLWU_P13/RF_NOT_ALLOWED/LPTMR0_ALT2/UART0_RTS_b/TPM1_CH1/BSM_CLK, label: GPS_nCS, identifier: GPS_nCS}
 - {pin_num: '37', pin_signal: PTC1/ANT_B/I2C0_SDA/UART0_RTS_b/TPM0_CH2/BLE_RF_ACTIVE, label: TIMEPULSE_GPS, identifier: TIMEPULSE_GPS}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

#include "fsl_common.h"
#include "fsl_port.h"
#include "pin_mux.h"

/*FUNCTION**********************************************************************
 * 
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 * 
 *END**************************************************************************/
void BOARD_InitBootPins(void)
{
	BOARD_InitPins();
}

#define PCR_SRE_SLOW                  0x01u   /*!< Slew Rate Enable: Slow slew rate is configured on the corresponding pin, if the pin is configured as a digital output. */
#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port */
#define PIN2_IDX                         2u   /*!< Pin number for pin 2 in a port */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port */
#define PIN4_IDX                         4u   /*!< Pin number for pin 4 in a port */
#define PIN5_IDX                         5u   /*!< Pin number for pin 5 in a port */
#define PIN6_IDX                         6u   /*!< Pin number for pin 6 in a port */
#define PIN7_IDX                         7u   /*!< Pin number for pin 7 in a port */
#define PIN16_IDX                       16u   /*!< Pin number for pin 16 in a port */
#define PIN17_IDX                       17u   /*!< Pin number for pin 17 in a port */
#define PIN18_IDX                       18u   /*!< Pin number for pin 18 in a port */
#define PIN19_IDX                       19u   /*!< Pin number for pin 19 in a port */
#define SOPT5_LPUART0RXSRC_LPUART_RX  0x00u   /*!< LPUART0 Receive Data Source Select: LPUART_RX pin */
#define SOPT5_LPUART0TXSRC_LPUART_TX  0x00u   /*!< LPUART0 Transmit Data Source Select: LPUART0_TX pin */

/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
 BOARD_InitPins:
 - options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
 - pin_list:
 - {pin_num: '47', peripheral: SPI0, signal: SIN, pin_signal: TSI0_CH6/PTC18/LLWU_P2/SPI0_SIN/I2C1_SDA/UART0_TX/BSM_DATA/DTM_TX}
 - {pin_num: '46', peripheral: SPI0, signal: SOUT, pin_signal: TSI0_CH5/PTC17/LLWU_P1/SPI0_SOUT/I2C1_SCL/UART0_RX/BSM_FRAME/DTM_RX}
 - {pin_num: '4', peripheral: GPIOA, signal: 'GPIO, 16', pin_signal: TSI0_CH10/PTA16/LLWU_P4/SPI1_SOUT/TPM0_CH0, direction: OUTPUT}
 - {pin_num: '6', peripheral: GPIOA, signal: 'GPIO, 18', pin_signal: TSI0_CH12/PTA18/LLWU_P6/SPI1_SCK/TPM2_CH0, direction: OUTPUT}
 - {pin_num: '2', peripheral: SWD, signal: CLK, pin_signal: TSI0_CH9/PTA1/SPI1_PCS0/TPM1_CH1/SWD_CLK}
 - {pin_num: '3', peripheral: RCM, signal: RESET, pin_signal: PTA2/TPM0_CH3/RESET_b}
 - {pin_num: '1', peripheral: SWD, signal: DIO, pin_signal: TSI0_CH8/PTA0/SPI0_PCS1/TPM1_CH0/SWD_DIO}
 - {pin_num: '5', peripheral: GPIOA, signal: 'GPIO, 17', pin_signal: TSI0_CH11/PTA17/LLWU_P5/RF_RESET/SPI1_SIN/TPM_CLKIN1, direction: OUTPUT}
 - {pin_num: '7', peripheral: GPIOA, signal: 'GPIO, 19', pin_signal: TSI0_CH13/ADC0_SE5/PTA19/LLWU_P7/SPI1_PCS0/TPM2_CH1, direction: OUTPUT}
 - {pin_num: '8', peripheral: DCDC, signal: PSWITCH, pin_signal: PSWITCH}
 - {pin_num: '9', peripheral: DCDC, signal: CFG, pin_signal: DCDC_CFG}
 - {pin_num: '10', peripheral: DCDC, signal: IN, pin_signal: VDCDC_IN}
 - {pin_num: '14', peripheral: DCDC, signal: VDD_1P8OUT, pin_signal: VDD_1P8OUT}
 - {pin_num: '13', peripheral: DCDC, signal: GND, pin_signal: DCDC_GND}
 - {pin_num: '15', peripheral: SUPPLY, signal: VDD_PMC_IN, pin_signal: VDD_1P5OUT_PMCIN}
 - {pin_num: '18', peripheral: GPIOB, signal: 'GPIO, 2', pin_signal: ADC0_SE3/CMP0_IN3/PTB2/RF_NOT_ALLOWED/DTM_TX/TPM1_CH0, direction: INPUT}
 - {pin_num: '19', peripheral: GPIOB, signal: 'GPIO, 3', pin_signal: ADC0_SE2/CMP0_IN4/PTB3/CLKOUT/TPM1_CH1/RTC_CLKOUT, direction: INPUT, slew_rate: slow}
 - {pin_num: '20', peripheral: SUPPLY, signal: 'VDD_0, 0', pin_signal: VDD_0}
 - {pin_num: '21', peripheral: RTC, signal: EXTAL32K, pin_signal: EXTAL32K/PTB16/I2C1_SCL/TPM2_CH0}
 - {pin_num: '22', peripheral: RTC, signal: XTAL32K, pin_signal: XTAL32K/PTB17/I2C1_SDA/TPM2_CH1/BSM_CLK}
 - {pin_num: '23', peripheral: ADC0, signal: 'SE, 4', pin_signal: DAC0_OUT/ADC0_SE4/CMP0_IN2/PTB18/I2C1_SCL/TPM_CLKIN0/TPM0_CH0/NMI_b}
 - {pin_num: '26', peripheral: SUPPLY, signal: 'VSSA, 0', pin_signal: VSSA28}
 - {pin_num: '27', peripheral: ADC0, signal: VREFH, pin_signal: VREFH/VREF_OUT}
 - {pin_num: '28', peripheral: SUPPLY, signal: 'VDDA, 0', pin_signal: VDDA}
 - {pin_num: '30', peripheral: RADIO, signal: EXTAL, pin_signal: EXTAL}
 - {pin_num: '31', peripheral: RADIO, signal: XTAL, pin_signal: XTAL}
 - {pin_num: '32', peripheral: RADIO, signal: PWR_RF3, pin_signal: VDD_RF3}
 - {pin_num: '33', peripheral: RADIO, signal: ANT, pin_signal: ANT}
 - {pin_num: '34', peripheral: RADIO, signal: GANT, pin_signal: GANT}
 - {pin_num: '35', peripheral: RADIO, signal: PWR_RF2, pin_signal: VDD_RF2}
 - {pin_num: '36', peripheral: RADIO, signal: PWR_RF1, pin_signal: VDD_RF1}
 - {pin_num: '37', peripheral: GPIOC, signal: 'GPIO, 1', pin_signal: PTC1/ANT_B/I2C0_SDA/UART0_RTS_b/TPM0_CH2/BLE_RF_ACTIVE}
 - {pin_num: '40', peripheral: GPIOC, signal: 'GPIO, 4', pin_signal: TSI0_CH0/PTC4/LLWU_P12/ANT_A/EXTRG_IN/UART0_CTS_b/TPM1_CH0/BSM_DATA, direction: OUTPUT}
 - {pin_num: '41', peripheral: GPIOC, signal: 'GPIO, 5', pin_signal: TSI0_CH1/PTC5/LLWU_P13/RF_NOT_ALLOWED/LPTMR0_ALT2/UART0_RTS_b/TPM1_CH1/BSM_CLK}
 - {pin_num: '42', peripheral: I2C1, signal: CLK, pin_signal: TSI0_CH2/PTC6/LLWU_P14/XTAL_OUT_EN/I2C1_SCL/UART0_RX/TPM2_CH0/BSM_FRAME}
 - {pin_num: '43', peripheral: I2C1, signal: SDA, pin_signal: TSI0_CH3/PTC7/LLWU_P15/SPI0_PCS2/I2C1_SDA/UART0_TX/TPM2_CH1/BSM_DATA}
 - {pin_num: '44', peripheral: SUPPLY, signal: 'VDD_1, 0', pin_signal: VDD_148}
 - {pin_num: '48', peripheral: GPIOC, signal: 'GPIO, 19', pin_signal: TSI0_CH7/PTC19/LLWU_P3/SPI0_PCS0/I2C0_SCL/UART0_CTS_b/BSM_CLK/BLE_RF_ACTIVE}
 - {pin_num: '16', peripheral: I2C0, signal: CLK, pin_signal: PTB0/LLWU_P8/XTAL_OUT_EN/I2C0_SCL/CMP0_OUT/TPM0_CH1/CLKOUT}
 - {pin_num: '17', peripheral: I2C0, signal: SDA, pin_signal: ADC0_SE1/CMP0_IN5/PTB1/DTM_RX/I2C0_SDA/LPTMR0_ALT1/TPM0_CH2/CMT_IRO}
 - {pin_num: '45', peripheral: SPI0, signal: SCK, pin_signal: TSI0_CH4/PTC16/LLWU_P0/SPI0_SCK/I2C0_SDA/UART0_RTS_b/TPM0_CH3}
 - {pin_num: '38', peripheral: LPUART0, signal: RX, pin_signal: TSI0_CH14/PTC2/LLWU_P10/TX_SWITCH/I2C1_SCL/UART0_RX/CMT_IRO/DTM_RX}
 - {pin_num: '39', peripheral: LPUART0, signal: TX, pin_signal: TSI0_CH15/PTC3/LLWU_P11/RX_SWITCH/I2C1_SDA/UART0_TX/TPM0_CH1/DTM_TX}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitPins(void)
{
	CLOCK_EnableClock(kCLOCK_PortA); /* Port A Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortB); /* Port B Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortC); /* Port C Clock Gate Control: Clock enabled */

	PORT_SetPinMux(PORTA, PIN0_IDX, kPORT_MuxAlt7); /* PORTA0 (pin 1) is configured as SWD_DIO */
	PORT_SetPinMux(PORTA, PIN1_IDX, kPORT_MuxAlt7); /* PORTA1 (pin 2) is configured as SWD_CLK */
	PORT_SetPinMux(PORTA, PIN16_IDX, kPORT_MuxAsGpio); /* PORTA16 (pin 4) is configured as PTA16 */
	PORT_SetPinMux(PORTA, PIN17_IDX, kPORT_MuxAsGpio); /* PORTA17 (pin 5) is configured as PTA17 */
	PORT_SetPinMux(PORTA, PIN18_IDX, kPORT_MuxAsGpio); /* PORTA18 (pin 6) is configured as PTA18 */
	PORT_SetPinMux(PORTA, PIN19_IDX, kPORT_MuxAsGpio); /* PORTA19 (pin 7) is configured as PTA19 */
	PORT_SetPinMux(PORTA, PIN2_IDX, kPORT_MuxAlt7); /* PORTA2 (pin 3) is configured as RESET_b */
	PORT_SetPinMux(PORTB, PIN0_IDX, kPORT_MuxAlt3); /* PORTB0 (pin 16) is configured as I2C0_SCL */
	PORT_SetPinMux(PORTB, PIN1_IDX, kPORT_MuxAlt3); /* PORTB1 (pin 17) is configured as I2C0_SDA */
	PORT_SetPinMux(PORTB, PIN16_IDX, kPORT_PinDisabledOrAnalog); /* PORTB16 (pin 21) is configured as EXTAL32K */
	PORT_SetPinMux(PORTB, PIN17_IDX, kPORT_PinDisabledOrAnalog); /* PORTB17 (pin 22) is configured as XTAL32K */
	PORT_SetPinMux(PORTB, PIN18_IDX, kPORT_PinDisabledOrAnalog); /* PORTB18 (pin 23) is configured as ADC0_SE4 */
	PORT_SetPinMux(PORTB, PIN2_IDX, kPORT_MuxAsGpio); /* PORTB2 (pin 18) is configured as PTB2 */
	PORT_SetPinMux(PORTB, PIN3_IDX, kPORT_MuxAsGpio); /* PORTB3 (pin 19) is configured as PTB3 */
	PORTB->PCR[3] = ((PORTB->PCR[3] & (~(PORT_PCR_SRE_MASK | PORT_PCR_ISF_MASK))) /* Mask bits to zero which are setting */
	| PORT_PCR_SRE(PCR_SRE_SLOW) /* Slew Rate Enable: Slow slew rate is configured on the corresponding pin, if the pin is configured as a digital output. */
	);
	PORT_SetPinMux(PORTC, PIN1_IDX, kPORT_MuxAsGpio); /* PORTC1 (pin 37) is configured as PTC1 */
	PORT_SetPinMux(PORTC, PIN16_IDX, kPORT_MuxAlt2); /* PORTC16 (pin 45) is configured as SPI0_SCK */
	PORT_SetPinMux(PORTC, PIN17_IDX, kPORT_MuxAlt2); /* PORTC17 (pin 46) is configured as SPI0_SOUT */
	PORT_SetPinMux(PORTC, PIN18_IDX, kPORT_MuxAlt2); /* PORTC18 (pin 47) is configured as SPI0_SIN */
	PORT_SetPinMux(PORTC, PIN19_IDX, kPORT_MuxAsGpio); /* PORTC19 (pin 48) is configured as PTC19 */
	PORT_SetPinMux(PORTC, PIN2_IDX, kPORT_MuxAlt4); /* PORTC2 (pin 38) is configured as UART0_RX */
	PORT_SetPinMux(PORTC, PIN3_IDX, kPORT_MuxAlt4); /* PORTC3 (pin 39) is configured as UART0_TX */
	PORT_SetPinMux(PORTC, PIN4_IDX, kPORT_MuxAsGpio); /* PORTC4 (pin 40) is configured as PTC4 */
	PORT_SetPinMux(PORTC, PIN5_IDX, kPORT_MuxAsGpio); /* PORTC5 (pin 41) is configured as PTC5 */
	PORT_SetPinMux(PORTC, PIN6_IDX, kPORT_MuxAlt3); /* PORTC6 (pin 42) is configured as I2C1_SCL */
	PORT_SetPinMux(PORTC, PIN7_IDX, kPORT_MuxAlt3); /* PORTC7 (pin 43) is configured as I2C1_SDA */
	SIM->SOPT5 = ((SIM->SOPT5 & (~(SIM_SOPT5_LPUART0TXSRC_MASK | SIM_SOPT5_LPUART0RXSRC_MASK))) /* Mask bits to zero which are setting */
	| SIM_SOPT5_LPUART0TXSRC(SOPT5_LPUART0TXSRC_LPUART_TX) /* LPUART0 Transmit Data Source Select: LPUART0_TX pin */
	| SIM_SOPT5_LPUART0RXSRC(SOPT5_LPUART0RXSRC_LPUART_RX) /* LPUART0 Receive Data Source Select: LPUART_RX pin */
	);
}

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
 BOARD_InitButtons:
 - options: {coreID: singlecore, enableClock: 'true'}
 - pin_list:
 - {pin_num: '40', peripheral: GPIOC, signal: 'GPIO, 4', pin_signal: TSI0_CH0/PTC4/LLWU_P12/ANT_A/EXTRG_IN/UART0_CTS_b/TPM1_CH0/BSM_DATA}
 - {pin_num: '41', peripheral: GPIOC, signal: 'GPIO, 5', pin_signal: TSI0_CH1/PTC5/LLWU_P13/RF_NOT_ALLOWED/LPTMR0_ALT2/UART0_RTS_b/TPM1_CH1/BSM_CLK}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitButtons
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitButtons(void)
{
	CLOCK_EnableClock(kCLOCK_PortB); /* Port C Clock Gate Control: Clock enabled */

	PORT_SetPinMux(PORTB, PIN2_IDX, kPORT_MuxAsGpio); /* PORTB2 (pin 18) is configured as PTB2 */
	PORT_SetPinMux(PORTB, PIN3_IDX, kPORT_MuxAsGpio); /* PORTB3 (pin 19) is configured as PTB3 */
}

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
 BOARD_InitLEDs:
 - options: {coreID: singlecore, enableClock: 'true'}
 - pin_list:
 - {pin_num: '4', peripheral: GPIOA, signal: 'GPIO, 16', pin_signal: TSI0_CH10/PTA16/LLWU_P4/SPI1_SOUT/TPM0_CH0, direction: OUTPUT}
 - {pin_num: '6', peripheral: GPIOA, signal: 'GPIO, 18', pin_signal: TSI0_CH12/PTA18/LLWU_P6/SPI1_SCK/TPM2_CH0, direction: OUTPUT}
 - {pin_num: '5', peripheral: GPIOA, signal: 'GPIO, 17', pin_signal: TSI0_CH11/PTA17/LLWU_P5/RF_RESET/SPI1_SIN/TPM_CLKIN1, direction: OUTPUT}
 - {pin_num: '7', peripheral: GPIOA, signal: 'GPIO, 19', pin_signal: TSI0_CH13/ADC0_SE5/PTA19/LLWU_P7/SPI1_PCS0/TPM2_CH1, direction: OUTPUT}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitLEDs
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitLEDs(void)
{
	CLOCK_EnableClock(kCLOCK_PortA); /* Port A Clock Gate Control: Clock enabled */

	PORT_SetPinMux(PORTA, PIN16_IDX, kPORT_MuxAsGpio); /* PORTA16 (pin 4) is configured as PTA16 */
	PORT_SetPinMux(PORTA, PIN17_IDX, kPORT_MuxAsGpio); /* PORTA17 (pin 5) is configured as PTA17 */
	PORT_SetPinMux(PORTA, PIN18_IDX, kPORT_MuxAsGpio); /* PORTA18 (pin 6) is configured as PTA18 */
	PORT_SetPinMux(PORTA, PIN19_IDX, kPORT_MuxAsGpio); /* PORTA19 (pin 7) is configured as PTA19 */

}

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
 BOARD_InitSPI:
 - options: {coreID: singlecore, enableClock: 'true'}
 - pin_list:
 - {pin_num: '47', peripheral: SPI0, signal: SIN, pin_signal: TSI0_CH6/PTC18/LLWU_P2/SPI0_SIN/I2C1_SDA/UART0_TX/BSM_DATA/DTM_TX}
 - {pin_num: '46', peripheral: SPI0, signal: SOUT, pin_signal: TSI0_CH5/PTC17/LLWU_P1/SPI0_SOUT/I2C1_SCL/UART0_RX/BSM_FRAME/DTM_RX}
 - {pin_num: '45', peripheral: SPI0, signal: SCK, pin_signal: TSI0_CH4/PTC16/LLWU_P0/SPI0_SCK/I2C0_SDA/UART0_RTS_b/TPM0_CH3}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitSPI
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitSPI(void)
{
	CLOCK_EnableClock(kCLOCK_PortC); /* Port C Clock Gate Control: Clock enabled */

	PORT_SetPinMux(PORTC, PIN16_IDX, kPORT_MuxAlt2); /* PORTC16 (pin 45) is configured as SPI0_SCK */
	PORT_SetPinMux(PORTC, PIN17_IDX, kPORT_MuxAlt2); /* PORTC17 (pin 46) is configured as SPI0_SOUT */
	PORT_SetPinMux(PORTC, PIN18_IDX, kPORT_MuxAlt2); /* PORTC18 (pin 47) is configured as SPI0_SIN */
}

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
 BOARD_InitLPUART:
 - options: {coreID: singlecore, enableClock: 'true'}
 - pin_list:
 - {pin_num: '38', peripheral: LPUART0, signal: RX, pin_signal: TSI0_CH14/PTC2/LLWU_P10/TX_SWITCH/I2C1_SCL/UART0_RX/CMT_IRO/DTM_RX}
 - {pin_num: '39', peripheral: LPUART0, signal: TX, pin_signal: TSI0_CH15/PTC3/LLWU_P11/RX_SWITCH/I2C1_SDA/UART0_TX/TPM0_CH1/DTM_TX}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitLPUART
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitLPUART(void)
{
	CLOCK_EnableClock(kCLOCK_PortC); /* Port C Clock Gate Control: Clock enabled */

	PORT_SetPinMux(PORTC, PIN2_IDX, kPORT_MuxAlt4); /* PORTC2 (pin 38) is configured as UART0_RX */
	PORT_SetPinMux(PORTC, PIN3_IDX, kPORT_MuxAlt4); /* PORTC3 (pin 39) is configured as UART0_TX */
	SIM->SOPT5 = ((SIM->SOPT5 & (~(SIM_SOPT5_LPUART0TXSRC_MASK | SIM_SOPT5_LPUART0RXSRC_MASK))) /* Mask bits to zero which are setting */
	| SIM_SOPT5_LPUART0TXSRC(SOPT5_LPUART0TXSRC_LPUART_TX) /* LPUART0 Transmit Data Source Select: LPUART0_TX pin */
	| SIM_SOPT5_LPUART0RXSRC(SOPT5_LPUART0RXSRC_LPUART_RX) /* LPUART0 Receive Data Source Select: LPUART_RX pin */
	);
}

#define PCR_PE_ENABLED                0x01u   /*!< Pull Enable: Internal pullup or pulldown resistor is enabled on the corresponding pin, if the pin is configured as a digital input. */
#define PCR_PS_UP                     0x01u   /*!< Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the corresponding PE field is set. */
#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port */
#define PIN2_IDX                         2u   /*!< Pin number for pin 2 in a port */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port */

/*
 * TEXT BELOW IS USED AS SETTING FOR THE PINS TOOL *****************************
 BOARD_InitI2C:
 - options: {coreID: singlecore, enableClock: 'true'}
 - pin_list:
 - {pin_num: '16', peripheral: I2C0, signal: CLK, pin_signal: PTB0/LLWU_P8/XTAL_OUT_EN/I2C0_SCL/CMP0_OUT/TPM0_CH1/CLKOUT}
 - {pin_num: '17', peripheral: I2C0, signal: SDA, pin_signal: ADC0_SE1/CMP0_IN5/PTB1/DTM_RX/I2C0_SDA/LPTMR0_ALT1/TPM0_CH2/CMT_IRO}
 - {pin_num: '42', peripheral: I2C1, signal: CLK, pin_signal: TSI0_CH2/PTC6/LLWU_P14/XTAL_OUT_EN/I2C1_SCL/UART0_RX/TPM2_CH0/BSM_FRAME}
 - {pin_num: '43', peripheral: I2C1, signal: SDA, pin_signal: TSI0_CH3/PTC7/LLWU_P15/SPI0_PCS2/I2C1_SDA/UART0_TX/TPM2_CH1/BSM_DATA}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR THE PINS TOOL ***
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : BOARD_InitI2C
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 *END**************************************************************************/
void BOARD_InitI2C(void)
{
	CLOCK_EnableClock(kCLOCK_PortB); /* Port B Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortC); /* Port C Clock Gate Control: Clock enabled */

	PORT_SetPinMux(PORTB, PIN0_IDX, kPORT_MuxAlt3); /* PORTB0 (pin 16) is configured as I2C0_SCL */
	PORT_SetPinMux(PORTB, PIN1_IDX, kPORT_MuxAlt3); /* PORTB1 (pin 17) is configured as I2C0_SDA */
	PORT_SetPinMux(PORTC, PIN6_IDX, kPORT_MuxAlt3); /* PORTC6 (pin 42) is configured as I2C1_SCL */
	PORT_SetPinMux(PORTC, PIN7_IDX, kPORT_MuxAlt3); /* PORTC7 (pin 43) is configured as I2C1_SDA */

}

/*******************************************************************************
 * EOF
 ******************************************************************************/
