/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
    kPIN_MUX_DirectionInput = 0U,        /* Input direction */
    kPIN_MUX_DirectionOutput = 1U,       /* Output direction */
    kPIN_MUX_DirectionInputOrOutput = 2U /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

#define IOCON_PIO_DIGITAL_EN 0x0100u  /*!<@brief Enables digital function */
#define IOCON_PIO_FUNC1 0x01u         /*!<@brief Selects pin function 1 */
#define IOCON_PIO_INPFILT_OFF 0x0200u /*!<@brief Input filter disabled */
#define IOCON_PIO_INV_DI 0x00u        /*!<@brief Input function is not inverted */
#define IOCON_PIO_MODE_INACT 0x00u    /*!<@brief No addition pin function */
#define IOCON_PIO_OPENDRAIN_DI 0x00u  /*!<@brief Open drain is disabled */
#define IOCON_PIO_SLEW_STANDARD 0x00u /*!<@brief Standard mode, output slew rate control is enabled */
#define PIO016_DIGIMODE_ANALOG 0x00u  /*!<@brief Select Analog/Digital mode.: Analog mode. */
#define PIO016_FUNC_ALT0 0x00u        /*!<@brief Selects pin function.: Alternative connection 0. */
#define PIO031_DIGIMODE_ANALOG 0x00u  /*!<@brief Select Analog/Digital mode.: Analog mode. */
#define PIO031_FUNC_ALT0 0x00u        /*!<@brief Selects pin function.: Alternative connection 0. */
#define PIO10_DIGIMODE_ANALOG 0x00u   /*!<@brief Select Analog/Digital mode.: Analog mode. */
#define PIO10_FUNC_ALT0 0x00u         /*!<@brief Selects pin function.: Alternative connection 0. */
#define PIO20_DIGIMODE_ANALOG 0x00u   /*!<@brief Select Analog/Digital mode.: Analog mode. */
#define PIO20_FUNC_ALT0 0x00u         /*!<@brief Selects pin function.: Alternative connection 0. */
#define PIO22_DIGIMODE_DIGITAL 0x01u  /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO22_FUNC_ALT0 0x00u         /*!<@brief Selects pin function.: Alternative connection 0. */
#define PIO314_DIGIMODE_DIGITAL 0x01u /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO314_FUNC_ALT0 0x00u        /*!<@brief Selects pin function.: Alternative connection 0. */
#define PIO320_DIGIMODE_DIGITAL 0x01u /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO320_FUNC_ALT3 0x03u        /*!<@brief Selects pin function.: Alternative connection 3. */
#define PIO326_DIGIMODE_DIGITAL 0x01u /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO326_FUNC_ALT3 0x03u        /*!<@brief Selects pin function.: Alternative connection 3. */
#define PIO327_DIGIMODE_DIGITAL 0x01u /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO327_FUNC_ALT3 0x03u        /*!<@brief Selects pin function.: Alternative connection 3. */
#define PIO328_DIGIMODE_DIGITAL 0x01u /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO328_FUNC_ALT3 0x03u        /*!<@brief Selects pin function.: Alternative connection 3. */
#define PIO329_DIGIMODE_DIGITAL 0x01u /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO329_FUNC_ALT3 0x03u        /*!<@brief Selects pin function.: Alternative connection 3. */
#define PIO33_DIGIMODE_DIGITAL 0x01u  /*!<@brief Select Analog/Digital mode.: Digital mode. */
#define PIO33_FUNC_ALT0 0x00u         /*!<@brief Selects pin function.: Alternative connection 0. */

/*! @name FC0_RXD_SDA_MOSI (coord B13), U24[13]/P0_29-ISP_FC0_RXD
  @{ */
#define BOARD_INITPINS_ISP_FC0_RXD_PERIPHERAL FLEXCOMM0              /*!<@brief Device name: FLEXCOMM0 */
#define BOARD_INITPINS_ISP_FC0_RXD_SIGNAL RXD_SDA_MOSI               /*!<@brief FLEXCOMM0 signal: RXD_SDA_MOSI */
#define BOARD_INITPINS_ISP_FC0_RXD_PIN_NAME FC0_RXD_SDA_MOSI         /*!<@brief Pin name */
#define BOARD_INITPINS_ISP_FC0_RXD_LABEL "U24[13]/P0_29-ISP_FC0_RXD" /*!<@brief Label */
#define BOARD_INITPINS_ISP_FC0_RXD_NAME "ISP_FC0_RXD"                /*!<@brief Identifier name */
                                                                     /* @} */

/*! @name FC0_TXD_SCL_MISO (coord A2), U24[12]/P0_30-ISP_FC0_TXD
  @{ */
#define BOARD_INITPINS_ISP_FC0_TXD_PERIPHERAL FLEXCOMM0              /*!<@brief Device name: FLEXCOMM0 */
#define BOARD_INITPINS_ISP_FC0_TXD_SIGNAL TXD_SCL_MISO               /*!<@brief FLEXCOMM0 signal: TXD_SCL_MISO */
#define BOARD_INITPINS_ISP_FC0_TXD_PIN_NAME FC0_TXD_SCL_MISO         /*!<@brief Pin name */
#define BOARD_INITPINS_ISP_FC0_TXD_LABEL "U24[12]/P0_30-ISP_FC0_TXD" /*!<@brief Label */
#define BOARD_INITPINS_ISP_FC0_TXD_NAME "ISP_FC0_TXD"                /*!<@brief Identifier name */
                                                                     /* @} */

/*! @name PIO3_14 (coord E3), J13[5]/LED1/P3_14-CT3MAT1-USR_LED1
  @{ */
#define BOARD_INITPINS_LED1_PERIPHERAL GPIO                            /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_LED1_SIGNAL PIO3                                /*!<@brief GPIO signal: PIO3 */
#define BOARD_INITPINS_LED1_GPIO GPIO                                  /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_LED1_GPIO_PIN 14U                               /*!<@brief PIO3 pin index: 14 */
#define BOARD_INITPINS_LED1_PORT 3U                                    /*!<@brief PORT device name: 3U */
#define BOARD_INITPINS_LED1_PIN 14U                                    /*!<@brief 3U pin index: 14 */
#define BOARD_INITPINS_LED1_CHANNEL 14                                 /*!<@brief GPIO PIO3 channel: 14 */
#define BOARD_INITPINS_LED1_PIN_NAME PIO3_14                           /*!<@brief Pin name */
#define BOARD_INITPINS_LED1_LABEL "J13[5]/LED1/P3_14-CT3MAT1-USR_LED1" /*!<@brief Label */
#define BOARD_INITPINS_LED1_NAME "LED1"                                /*!<@brief Identifier name */
#define BOARD_INITPINS_LED1_DIRECTION kPIN_MUX_DirectionOutput         /*!<@brief Direction */
                                                                       /* @} */

/*! @name PIO3_3 (coord A13), J9[8]/LED2/P3_3-USR_LED2
  @{ */
#define BOARD_INITPINS_LED2_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_LED2_SIGNAL PIO3                        /*!<@brief GPIO signal: PIO3 */
#define BOARD_INITPINS_LED2_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_LED2_GPIO_PIN 3U                        /*!<@brief PIO3 pin index: 3 */
#define BOARD_INITPINS_LED2_PORT 3U                            /*!<@brief PORT device name: 3U */
#define BOARD_INITPINS_LED2_PIN 3U                             /*!<@brief 3U pin index: 3 */
#define BOARD_INITPINS_LED2_CHANNEL 3                          /*!<@brief GPIO PIO3 channel: 3 */
#define BOARD_INITPINS_LED2_PIN_NAME PIO3_3                    /*!<@brief Pin name */
#define BOARD_INITPINS_LED2_LABEL "J9[8]/LED2/P3_3-USR_LED2"   /*!<@brief Label */
#define BOARD_INITPINS_LED2_NAME "LED2"                        /*!<@brief Identifier name */
#define BOARD_INITPINS_LED2_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                               /* @} */

/*! @name PIO2_2 (coord C3), J9[6]/LED3/P2_2-CT1MAT1-USR_LED3
  @{ */
#define BOARD_INITPINS_LED3_PERIPHERAL GPIO                          /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_LED3_SIGNAL PIO2                              /*!<@brief GPIO signal: PIO2 */
#define BOARD_INITPINS_LED3_GPIO GPIO                                /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_LED3_GPIO_PIN 2U                              /*!<@brief PIO2 pin index: 2 */
#define BOARD_INITPINS_LED3_PORT 2U                                  /*!<@brief PORT device name: 2U */
#define BOARD_INITPINS_LED3_PIN 2U                                   /*!<@brief 2U pin index: 2 */
#define BOARD_INITPINS_LED3_CHANNEL 2                                /*!<@brief GPIO PIO2 channel: 2 */
#define BOARD_INITPINS_LED3_PIN_NAME PIO2_2                          /*!<@brief Pin name */
#define BOARD_INITPINS_LED3_LABEL "J9[6]/LED3/P2_2-CT1MAT1-USR_LED3" /*!<@brief Label */
#define BOARD_INITPINS_LED3_NAME "LED3"                              /*!<@brief Identifier name */
#define BOARD_INITPINS_LED3_DIRECTION kPIN_MUX_DirectionOutput       /*!<@brief Direction */
                                                                     /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M4F */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
