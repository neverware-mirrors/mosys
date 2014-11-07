/*
 * Copyright 2014, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of Google Inc. nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NVIDIA_TEGRA124_GPIO_H
#define NVIDIA_TEGRA124_GPIO_H

#include <stddef.h>
#include <stdint.h>
#include "mosys/platform.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint32_t gpio_t;
typedef uint32_t gpio_t;

#define TEGRA_GPIO_BASE 0x6000D000
#define TEGRA_APB_PINMUX_BASE 0x70003000

#define GPIO_PINMUX_SHIFT 16
#define GPIO(name) ((gpio_t)(GPIO_##name##_INDEX | \
			     (PINMUX_GPIO_##name << GPIO_PINMUX_SHIFT)))

#define GPIO_PORT_CONSTANTS(port) \
	GPIO_##port##0_INDEX, GPIO_##port##1_INDEX, GPIO_##port##2_INDEX, \
	GPIO_##port##3_INDEX, GPIO_##port##4_INDEX, GPIO_##port##5_INDEX, \
	GPIO_##port##6_INDEX, GPIO_##port##7_INDEX

enum {
	GPIO_GPIOS_PER_PORT = 8,
	GPIO_PORTS_PER_BANK = 4,
	GPIO_BANKS = 8,

	GPIO_GPIOS_PER_BANK = GPIO_GPIOS_PER_PORT * GPIO_PORTS_PER_BANK,
	GPIO_GPIOS = GPIO_BANKS * GPIO_GPIOS_PER_BANK
};

enum gpio_mode {
	GPIO_MODE_SPIO = 0,
	GPIO_MODE_GPIO = 1
};

struct gpio_bank {
	// Values
	u32 config[GPIO_PORTS_PER_BANK];
	u32 out_enable[GPIO_PORTS_PER_BANK];
	u32 out_value[GPIO_PORTS_PER_BANK];
	u32 in_value[GPIO_PORTS_PER_BANK];
	u32 int_status[GPIO_PORTS_PER_BANK];
	u32 int_enable[GPIO_PORTS_PER_BANK];
	u32 int_level[GPIO_PORTS_PER_BANK];
	u32 int_clear[GPIO_PORTS_PER_BANK];

	// Masks
	u32 config_mask[GPIO_PORTS_PER_BANK];
	u32 out_enable_mask[GPIO_PORTS_PER_BANK];
	u32 out_value_mask[GPIO_PORTS_PER_BANK];
	u32 in_value_mask[GPIO_PORTS_PER_BANK];
	u32 int_status_mask[GPIO_PORTS_PER_BANK];
	u32 int_enable_mask[GPIO_PORTS_PER_BANK];
	u32 int_level_mask[GPIO_PORTS_PER_BANK];
	u32 int_clear_mask[GPIO_PORTS_PER_BANK];
};

enum {
	PINMUX_FUNC_MASK = 3 << 0,

	PINMUX_PULL_MASK = 3 << 2,
	PINMUX_PULL_NONE = 0 << 2,
	PINMUX_PULL_DOWN = 1 << 2,
	PINMUX_PULL_UP = 2 << 2,

	PINMUX_TRISTATE = 1 << 4,
	PINMUX_INPUT_ENABLE = 1 << 5,
	PINMUX_OPEN_DRAIN = 1 << 6,
	PINMUX_LOCK = 1 << 7,
	PINMUX_IO_RESET = 1 << 8,
	PINMUX_RCV_SEL = 1 << 9
};

enum {
	GPIO_PORT_CONSTANTS(A),
	GPIO_PORT_CONSTANTS(B),
	GPIO_PORT_CONSTANTS(C),
	GPIO_PORT_CONSTANTS(D),
	GPIO_PORT_CONSTANTS(E),
	GPIO_PORT_CONSTANTS(F),
	GPIO_PORT_CONSTANTS(G),
	GPIO_PORT_CONSTANTS(H),
	GPIO_PORT_CONSTANTS(I),
	GPIO_PORT_CONSTANTS(J),
	GPIO_PORT_CONSTANTS(K),
	GPIO_PORT_CONSTANTS(L),
	GPIO_PORT_CONSTANTS(M),
	GPIO_PORT_CONSTANTS(N),
	GPIO_PORT_CONSTANTS(O),
	GPIO_PORT_CONSTANTS(P),
	GPIO_PORT_CONSTANTS(Q),
	GPIO_PORT_CONSTANTS(R),
	GPIO_PORT_CONSTANTS(S),
	GPIO_PORT_CONSTANTS(T),
	GPIO_PORT_CONSTANTS(U),
	GPIO_PORT_CONSTANTS(V),
	GPIO_PORT_CONSTANTS(W),
	GPIO_PORT_CONSTANTS(X),
	GPIO_PORT_CONSTANTS(Y),
	GPIO_PORT_CONSTANTS(Z),
	GPIO_PORT_CONSTANTS(AA),
	GPIO_PORT_CONSTANTS(BB),
	GPIO_PORT_CONSTANTS(CC),
	GPIO_PORT_CONSTANTS(DD),
	GPIO_PORT_CONSTANTS(EE),
	GPIO_PORT_CONSTANTS(FF)
};

#define PINMUX_CONSTANTS(index, name, gpio, func0, func1, func2, func3) \
	PINMUX_##name##_INDEX = index, \
	PINMUX_##name##_FUNC_##func0 = 0, \
	PINMUX_##name##_FUNC_##func1 = 1, \
	PINMUX_##name##_FUNC_##func2 = 2, \
	PINMUX_##name##_FUNC_##func3 = 3, \
	PINMUX_GPIO_##gpio = PINMUX_##name##_INDEX

enum {
	PINMUX_CONSTANTS(0, ULPI_DATA0, O1, SPI3, HSI, UA3, ULPI),
	PINMUX_CONSTANTS(1, ULPI_DATA1, O2, SPI3, HSI, UA3, ULPI),
	PINMUX_CONSTANTS(2, ULPI_DATA2, O3, SPI3, HSI, UA3, ULPI),
	PINMUX_CONSTANTS(3, ULPI_DATA3, O4, SPI3, HSI, UA3, ULPI),
	PINMUX_CONSTANTS(4, ULPI_DATA4, O5, SPI2, HSI, UA3, ULPI),
	PINMUX_CONSTANTS(5, ULPI_DATA5, O6, SPI2, HSI, UA3, ULPI),
	PINMUX_CONSTANTS(6, ULPI_DATA6, O7, SPI2, HSI, UA3, ULPI),
	PINMUX_CONSTANTS(7, ULPI_DATA7, O0, SPI2, HSI, UA3, ULPI),
	PINMUX_CONSTANTS(8, ULPI_CLK, Y0, SPI1, SPI5, UD3, ULPI),
	PINMUX_CONSTANTS(9, ULPI_DIR, Y1, SPI1, SPI5, UD3, ULPI),
	PINMUX_CONSTANTS(10, ULPI_NXT, Y2, SPI1, SPI5, UD3, ULPI),
	PINMUX_CONSTANTS(11, ULPI_STP, Y3, SPI1, SPI5, UD3, ULPI),
	PINMUX_CONSTANTS(12, DAP3_FS, P0, I2S2, SPI5, DCA, DCB),
	PINMUX_CONSTANTS(13, DAP3_DIN, P1, I2S2, SPI5, DCA, DCB),
	PINMUX_CONSTANTS(14, DAP3_DOUT, P2, I2S2, SPI5, DCA, RES3),
	PINMUX_CONSTANTS(15, DAP3_SCLK, P3, I2S2, SPI5, RES2, DCB),
	PINMUX_CONSTANTS(16, GPIO_PV0, V0, RES0, RES1, RES2, RES3),
	PINMUX_CONSTANTS(17, GPIO_PV1, V1, RES0, RES1, RES2, RES3),
	PINMUX_CONSTANTS(18, SDMMC1_CLK, Z0, SDMMC1, CLK12M, RES2, RES3),
	PINMUX_CONSTANTS(19, SDMMC1_CMD, Z1, SDMMC1, SPDIF, SPI4, UA3),
	PINMUX_CONSTANTS(20, SDMMC1_DAT3, Y4, SDMMC1, SPDIF, SPI4, UA3),
	PINMUX_CONSTANTS(21, SDMMC1_DAT2, Y5, SDMMC1, PWM0, SPI4, UA3),
	PINMUX_CONSTANTS(22, SDMMC1_DAT1, Y6, SDMMC1, PWM1, SPI4, UA3),
	PINMUX_CONSTANTS(23, SDMMC1_DAT0, Y7, SDMMC1, RES1, SPI4, UA3),
	PINMUX_CONSTANTS(26, CLK2_OUT, W5, EXTPERIPH2, RES1, RES2, RES3),
	PINMUX_CONSTANTS(27, CLK2_REQ, CC5, DAP, RES1, RES2, RES3),
	PINMUX_CONSTANTS(68, HDMI_INT, N7, RES0, RES1, RES2, RES3),
	PINMUX_CONSTANTS(69, DDC_SCL, V4, I2C4, RES1, RES2, RES3),
	PINMUX_CONSTANTS(70, DDC_SDA, V5, I2C4, RES1, RES2, RES3),
	PINMUX_CONSTANTS(89, UART2_RXD, C3, IR3, SPDIF, UA3, SPI4),
	PINMUX_CONSTANTS(90, UART2_TXD, C2, IR3, SPDIF, UA3, SPI4),
	PINMUX_CONSTANTS(91, UART2_RTS_N, J6, UA3, UB3, NOR, SPI4),
	PINMUX_CONSTANTS(92, UART2_CTS_N, J5, UA3, UB3, NOR, SPI4),
	PINMUX_CONSTANTS(93, UART3_TXD, W6, UC3, RES1, NOR, SPI4),
	PINMUX_CONSTANTS(94, UART3_RXD, W7, UC3, RES1, NOR, SPI4),
	PINMUX_CONSTANTS(95, UART3_CTS_N, A1, UC3, SDMMC1, DTV, NOR),
	PINMUX_CONSTANTS(96, UART3_RTS_N, C0, UC3, PWM0, DTV, NOR),
	PINMUX_CONSTANTS(97, GPIO_PU0, U0, OWR, UA3, NOR, RES3),
	PINMUX_CONSTANTS(98, GPIO_PU1, U1, RES0, UA3, NOR, RES3),
	PINMUX_CONSTANTS(99, GPIO_PU2, U2, RES0, UA3, NOR, RES3),
	PINMUX_CONSTANTS(100, GPIO_PU3, U3, PWM0, UA3, NOR, DCB),
	PINMUX_CONSTANTS(101, GPIO_PU4, U4, PWM1, UA3, NOR, DCB),
	PINMUX_CONSTANTS(102, GPIO_PU5, U5, PWM2, UA3, NOR, DCB),
	PINMUX_CONSTANTS(103, GPIO_PU6, U6, PWM3, UA3, RES2, NOR),
	PINMUX_CONSTANTS(104, GEN1_I2C_SDA, C5, I2C1, RES1, RES2, RES3),
	PINMUX_CONSTANTS(105, GEN1_I2C_SCL, C4, I2C1, RES1, RES2, RES3),
	PINMUX_CONSTANTS(106, DAP4_FS, P4, I2S3, NOR, DTV, RES3),
	PINMUX_CONSTANTS(107, DAP4_DIN, P5, I2S3, NOR, RES2, RES3),
	PINMUX_CONSTANTS(108, DAP4_DOUT, P6, I2S3, NOR, DTV, RES3),
	PINMUX_CONSTANTS(109, DAP4_SCLK, P7, I2S3, NOR, RES2, RES3),
	PINMUX_CONSTANTS(110, CLK3_OUT, EE0, EXTPERIPH3, RES1, RES2, RES3),
	PINMUX_CONSTANTS(111, CLK3_REQ, EE1, DEV3, RES1, RES2, RES3),
	PINMUX_CONSTANTS(112, GPIO_PC7, C7, RES0, RES1, NOR_WP_N, NOR_INT1),
	PINMUX_CONSTANTS(113, GPIO_PI5, I5, SDMMC2, RES1, NOR, RES3),
	PINMUX_CONSTANTS(114, GPIO_PI7, I7, RES0, TRACE, NOR, DTV),
	PINMUX_CONSTANTS(115, GPIO_PK0, K0, RES0, SDMMC3, NOR, SOC_THERM),
	PINMUX_CONSTANTS(116, GPIO_PK1, K1, SDMMC2, TRACE, NOR, RES3),
	PINMUX_CONSTANTS(117, GPIO_PJ0, J0, RES0, RES1, NOR, USB),
	PINMUX_CONSTANTS(118, GPIO_PJ2, J2, RES0, RES1, NOR, SOC_THERM),
	PINMUX_CONSTANTS(119, GPIO_PK3, K3, SDMMC2, TRACE, NOR, CCLA),
	PINMUX_CONSTANTS(120, GPIO_PK4, K4, SDMMC2, RES1, NOR_AD22, NOR_INT1),
	PINMUX_CONSTANTS(121, GPIO_PK2, K2, RES0, RES1, NOR, RES3),
	PINMUX_CONSTANTS(122, GPIO_PI3, I3, RES0, RES1, NOR, SPI4),
	PINMUX_CONSTANTS(123, GPIO_PI6, I6, RES0, RES1, NOR, SDMMC2),
	PINMUX_CONSTANTS(124, GPIO_PG0, G0, RES0, RES1, NOR, RES3),
	PINMUX_CONSTANTS(125, GPIO_PG1, G1, RES0, RES1, NOR, RES3),
	PINMUX_CONSTANTS(126, GPIO_PG2, G2, RES0, TRACE, NOR, RES3),
	PINMUX_CONSTANTS(127, GPIO_PG3, G3, RES0, TRACE, NOR, RES3),
	PINMUX_CONSTANTS(128, GPIO_PG4, G4, RES0, TMDS, NOR, SPI4),
	PINMUX_CONSTANTS(129, GPIO_PG5, G5, RES0, RES1, NOR, SPI4),
	PINMUX_CONSTANTS(130, GPIO_PG6, G6, RES0, RES1, NOR, SPI4),
	PINMUX_CONSTANTS(131, GPIO_PG7, G7, RES0, RES1, NOR, SPI4),
	PINMUX_CONSTANTS(132, GPIO_PH0, H0, PWM0, TRACE, NOR, DTV),
	PINMUX_CONSTANTS(133, GPIO_PH1, H1, PWM1, TMDS, NOR, DCA),
	PINMUX_CONSTANTS(134, GPIO_PH2, H2, PWM2, TDMS, NOR, CLDVFS),
	PINMUX_CONSTANTS(135, GPIO_PH3, H3, PWM3, SPI4, NOR, CLDVFS),
	PINMUX_CONSTANTS(136, GPIO_PH4, H4, SDMMC2, RES1, NOR, RES3),
	PINMUX_CONSTANTS(137, GPIO_PH5, H5, SDMMC2, RES1, NOR, RES3),
	PINMUX_CONSTANTS(138, GPIO_PH6, H6, SDMMC2, TRACE, NOR, DTV),
	PINMUX_CONSTANTS(139, GPIO_PH7, H7, SDMMC2, TRACE, NOR, DTV),
	PINMUX_CONSTANTS(140, GPIO_PJ7, J7, UD3, RES1, NOR_AD16, NOR_INT2),
	PINMUX_CONSTANTS(141, GPIO_PB0, B0, UD3, RES1, NOR, RES3),
	PINMUX_CONSTANTS(142, GPIO_PB1, B1, UD3, RES1, NOR, RES3),
	PINMUX_CONSTANTS(143, GPIO_PK7, K7, UD3, RES1, NOR, RES3),
	PINMUX_CONSTANTS(144, GPIO_PI0, I0, RES0, RES1, NOR, RES3),
	PINMUX_CONSTANTS(145, GPIO_PI1, I1, RES0, RES1, NOR, RES3),
	PINMUX_CONSTANTS(146, GPIO_PI2, I2, SDMMC2, TRACE, NOR, RES3),
	PINMUX_CONSTANTS(147, GPIO_PI4, I4, SPI4, TRACE, NOR, DCA),
	PINMUX_CONSTANTS(148, GEN2_I2C_SCL, T5, I2C2, RES1, NOR, RES3),
	PINMUX_CONSTANTS(149, GEN2_I2C_SDA, T6, I2C2, RES1, NOR, RES3),
	PINMUX_CONSTANTS(150, SDMMC4_CLK, CC4, SDMMC4, RES1, NOR, RES3),
	PINMUX_CONSTANTS(151, SDMMC4_CMD, T7, SDMMC4, RES1, NOR, RES3),
	PINMUX_CONSTANTS(152, SDMMC4_DAT0, AA0, SDMMC4, SPI3, NOR, RES3),
	PINMUX_CONSTANTS(153, SDMMC4_DAT1, AA1, SDMMC4, SPI3, NOR, RES3),
	PINMUX_CONSTANTS(154, SDMMC4_DAT2, AA2, SDMMC4, SPI3, NOR, RES3),
	PINMUX_CONSTANTS(155, SDMMC4_DAT3, AA3, SDMMC4, SPI3, NOR, RES3),
	PINMUX_CONSTANTS(156, SDMMC4_DAT4, AA4, SDMMC4, SPI3, NOR, RES3),
	PINMUX_CONSTANTS(157, SDMMC4_DAT5, AA5, SDMMC4, SPI3, RES2, RES3),
	PINMUX_CONSTANTS(158, SDMMC4_DAT6, AA6, SDMMC4, SPI3, NOR, RES3),
	PINMUX_CONSTANTS(159, SDMMC4_DAT7, AA7, SDMMC4, RES1, NOR, RES3),
	PINMUX_CONSTANTS(161, CAM_MCLK, CC0, VIMCLK_PRI, VIMCLK_ALT1,
			 VIMCLK_ALT3, SDMMC2),
	PINMUX_CONSTANTS(162, GPIO_PCC1, CC1, I2S4, RES1, RES2, SDMMC2),
	PINMUX_CONSTANTS(163, GPIO_PBB0, BB0, VGP6, VIMCLK2_PRI, SDMMC2,
			 VIMCLK2_ALT3),
	PINMUX_CONSTANTS(164, CAM_I2C_SCL, BB1, VGP1, I2C3, RES2, SDMMC2),
	PINMUX_CONSTANTS(165, CAM_I2C_SDA, BB2, VGP2, I2C3, RES2, SDMMC2),
	PINMUX_CONSTANTS(166, GPIO_PBB3, BB3, VGP3, DCA, DCB, SDMMC2),
	PINMUX_CONSTANTS(167, GPIO_PBB4, BB4, VGP4, DCA, DCB, SDMMC2),
	PINMUX_CONSTANTS(168, GPIO_PBB5, BB5, VGP5, DCA, RES2, SDMMC2),
	PINMUX_CONSTANTS(169, GPIO_PBB6, BB6, I2S4, RES1, DCB, SDMMC2),
	PINMUX_CONSTANTS(170, GPIO_PBB7, BB7, I2S4, RES1, RES2, SDMMC2),
	PINMUX_CONSTANTS(171, GPIO_PCC2, CC2, I2S4, RES1, SDMMC3, SDMMC2),
	PINMUX_CONSTANTS(172, JTAG_RTCK, NONE172, RTCK, RES1, RES2, RES3),
	PINMUX_CONSTANTS(173, PWR_I2C_SCL, Z6, I2CPMU, RES1, RES2, RES3),
	PINMUX_CONSTANTS(174, PWR_I2C_SDA, Z7, I2CPMU, RES1, RES2, RES3),
	PINMUX_CONSTANTS(175, KB_ROW0, R0, RES0, RES1, RES2, RES3),
	PINMUX_CONSTANTS(176, KB_ROW1, R1, RES0, RES1, RES2, RES3),
	PINMUX_CONSTANTS(177, KB_ROW2, R2, RES0, RES1, RES2, RES3),
	PINMUX_CONSTANTS(178, KB_ROW3, R3, RES0, DCA, SYS_CLK, DCB),
	PINMUX_CONSTANTS(179, KB_ROW4, R4, RES0, DCA, RES2, DCB),
	PINMUX_CONSTANTS(180, KB_ROW5, R5, RES0, DCA, RES2, DCB),
	PINMUX_CONSTANTS(181, KB_ROW6, R6, RES0, DCA_LSC0, DCA_LSPII, DCB),
	PINMUX_CONSTANTS(182, KB_ROW7, R7, RES0, RES1, CLDVFS, UA3),
	PINMUX_CONSTANTS(183, KB_ROW8, S0, RES0, RES1, CLDVFS, UA3),
	PINMUX_CONSTANTS(184, KB_ROW9, S1, RES0, RES1, RES2, UA3),
	PINMUX_CONSTANTS(185, KB_ROW10, S2, RES0, RES1, RES2, UA3),
	PINMUX_CONSTANTS(186, KB_ROW11, S3, RES0, RES1, RES2, IR3),
	PINMUX_CONSTANTS(187, KB_ROW12, S4, RES0, RES1, RES2, IR3),
	PINMUX_CONSTANTS(188, KB_ROW13, S5, RES0, RES1, SPI2, RES3),
	PINMUX_CONSTANTS(189, KB_ROW14, S6, RES0, RES1, SPI2, RES3),
	PINMUX_CONSTANTS(190, KB_ROW15, S7, RES0, SOC_THERM, RES2, RES3),
	PINMUX_CONSTANTS(191, KB_COL0, Q0, RES0, RES1, SPI2, RES3),
	PINMUX_CONSTANTS(192, KB_COL1, Q1, RES0, RES1, SPI2, RES3),
	PINMUX_CONSTANTS(193, KB_COL2, Q2, RES0, RES1, SPI2, RES3),
	PINMUX_CONSTANTS(194, KB_COL3, Q3, RES0, DCA, PWM2, UA3),
	PINMUX_CONSTANTS(195, KB_COL4, Q4, RES0, OWR, SDMMC3, UA3),
	PINMUX_CONSTANTS(196, KB_COL5, Q5, RES0, RES1, SDMMC3, RES3),
	PINMUX_CONSTANTS(197, KB_COL6, Q6, RES0, RES1, SPI2, UD3),
	PINMUX_CONSTANTS(198, KB_COL7, Q7, RES0, RES1, SPI2, UD3),
	PINMUX_CONSTANTS(199, CLK_32K_OUT, A0, BLINK, SOC_THERM, RES2, RES3),
	PINMUX_CONSTANTS(201, CORE_PWR_REQ, NONE201, PWRON, RES1, RES2, RES3),
	PINMUX_CONSTANTS(202, CPU_PWR_REQ, NONE202, CPU, RES1, RES2, RES3),
	PINMUX_CONSTANTS(203, PWR_INT_N, NONE203, PMICINTR, RES1, RES2, RES3),
	PINMUX_CONSTANTS(204, CLK_32K_IN, NONE204, CLK_32K_IN, RES1, RES2,
			 RES3),
	PINMUX_CONSTANTS(205, OWR, NONE205, OWR, RES1, RES2, RES3),
	PINMUX_CONSTANTS(206, DAP1_FS, N0, I2S0, DAP1, NOR, RES3),
	PINMUX_CONSTANTS(207, DAP1_DIN, N1, I2S0, DAP1, NOR, RES3),
	PINMUX_CONSTANTS(208, DAP1_DOUT, N2, I2S0, DAP1, NOR, SATA),
	PINMUX_CONSTANTS(209, DAP1_SCLK, N3, I2S0, DAP1, NOR, RES3),
	PINMUX_CONSTANTS(210, DAP_MCLK1_REQ, EE2, DAP, DAP1, SATA, RES3),
	PINMUX_CONSTANTS(211, DAP_MCLK1, W4, EXTPERIPH1, DAP2, RES2, RES3),
	PINMUX_CONSTANTS(212, SPDIF_IN, K6, SPDIF, RES1, RES2, I2C3),
	PINMUX_CONSTANTS(213, SPDIF_OUT, K5, SPDIF, RES1, RES2, I2C3),
	PINMUX_CONSTANTS(214, DAP2_FS, A2, I2S1, DAP2, NOR, RES3),
	PINMUX_CONSTANTS(215, DAP2_DIN, A4, I2S1, DAP2, NOR, RES3),
	PINMUX_CONSTANTS(216, DAP2_DOUT, A5, I2S1, DAP2, NOR, RES3),
	PINMUX_CONSTANTS(217, DAP2_SCLK, A3, I2S1, SAP2, NOR, RES3),
	PINMUX_CONSTANTS(218, DVFS_PWM, X0, SPI6, CLDVFS, NOR, RES3),
	PINMUX_CONSTANTS(219, GPIO_X1_AUD, X1, SPI6, RES1, NOR, RES3),
	PINMUX_CONSTANTS(220, GPIO_X3_AUD, X3, SPI6, SPI1, NOR, RES3),
	PINMUX_CONSTANTS(221, DVFS_CLK, X2, SPI6, CLDVFS_CLK, NOR, RES3),
	PINMUX_CONSTANTS(222, GPIO_X4_AUD, X4, NOR, SPI1, SPI2, DAP2),
	PINMUX_CONSTANTS(223, GPIO_X5_AUD, X5, NOR, SPI1, SPI2, RES3),
	PINMUX_CONSTANTS(224, GPIO_X6_AUD, X6, SPI6, SPI1, SPI2, NOR),
	PINMUX_CONSTANTS(225, GPIO_X7_AUD, X7, RES0, SPI1, SPI2, RES3),
	PINMUX_CONSTANTS(228, SDMMC3_CLK, A6, SDMMC3, RES1, RES2, SPI3),
	PINMUX_CONSTANTS(229, SDMMC3_CMD, A7, SDMMC3, PWM3, UA3, SPI3),
	PINMUX_CONSTANTS(230, SDMMC3_DAT0, B7, SDMMC3, RES1, RES2, SPI3),
	PINMUX_CONSTANTS(231, SDMMC3_DAT1, B6, SDMMC3, PWM2, UA3, SPI3),
	PINMUX_CONSTANTS(232, SDMMC3_DAT2, B5, SDMMC3, PWM1, DCA, SPI3),
	PINMUX_CONSTANTS(233, SDMMC3_DAT3, B4, SDMMC3, PWM0, DCB, SPI3),
	PINMUX_CONSTANTS(239, PEX_L0_RST_N, DD1, PE0, RES1, RES2, RES3),
	PINMUX_CONSTANTS(240, PEX_L0_CLKREQ_N, DD2, PE0, RES1, RES2, RES3),
	PINMUX_CONSTANTS(241, PEX_WAKE_N, DD3, PE, RES1, RES2, RES3),
	PINMUX_CONSTANTS(243, PEX_L1_RST_N, DD5, PE1, RES1, RES2, RES3),
	PINMUX_CONSTANTS(244, PEX_L1_CLKREQ_N, DD6, PE1, RES1, RES2, RES3),
	PINMUX_CONSTANTS(248, HDMI_CEC, EE3, CEC, RES1, RES2, RES3),
	PINMUX_CONSTANTS(249, SDMMC1_WP_N, V3, SDMMC1, CLK12M, SPI4, UA3),
	PINMUX_CONSTANTS(250, SDMMC3_CD_N, V2, SDMMC3, OWR, RES2, RES3),
	PINMUX_CONSTANTS(251, GPIO_W2_AUD, W2, SPI6, RES1, SPI2, I2C1),
	PINMUX_CONSTANTS(252, GPIO_W3_AUD, W3, SPI6, SPI1, SPI2, I2C1),
	PINMUX_CONSTANTS(253, USB_VBUS_EN0, N4, USB, RES1, RES2, RES3),
	PINMUX_CONSTANTS(254, USB_VBUS_EN1, N5, USB, RES1, RES2, RES3),
	PINMUX_CONSTANTS(255, SDMMC3_CLK_LB_IN, EE5, SDMMC3, RES1, RES2, RES3),
	PINMUX_CONSTANTS(256, SDMMC3_CLK_LB_OUT, EE4, SDMMC3, RES1, RES2, RES3),
	PINMUX_CONSTANTS(258, RESET_OUT_N, NONE258, RES0, RES1, RES2, RESET),
	PINMUX_CONSTANTS(259, KB_ROW16, T0, RES0, RES1, RES2, UC3),
	PINMUX_CONSTANTS(260, KB_ROW17, T1, RES0, RES1, RES2, UC3),
	PINMUX_CONSTANTS(261, USB_VBUS_EN2, FF1, USB, RES1, RES2, RES3),
	PINMUX_CONSTANTS(262, GPIO_PFF2, FF2, SATA, RES1, RES2, RES3),
	PINMUX_CONSTANTS(268, DP_HPD, FF0, DP, RES1, RES2, RES3),

};

void gpio_write_port(struct platform_intf *intf, int index,
		size_t offset, uint32_t mask, u32 value);

uint32_t gpio_read_port(struct platform_intf *intf, int index, size_t offset);

void gpio_set_int_enable(struct platform_intf *intf,
				gpio_t gpio, int enable);

void gpio_set_out_enable(struct platform_intf *intf,
		gpio_t gpio, int enable);

void gpio_set_mode(struct platform_intf *intf,
		gpio_t gpio, enum gpio_mode mode);

void pinmux_set_config(struct platform_intf *intf,
		int pin_index, uint32_t config);

void gpio_input(struct platform_intf *intf, gpio_t gpio, uint32_t pull);

void gpio_input_pullup(struct platform_intf *intf, gpio_t gpio);

void gpio_input_pulldown(struct platform_intf *intf, gpio_t gpio);

int gpio_get_in_value(struct platform_intf *intf, gpio_t gpio);

int gpio_get_in_tristate_values(struct platform_intf *intf,
		gpio_t gpio[], int num_gpio);

#endif	/* NVIDIA_TEGRA124_GPIO_H */
