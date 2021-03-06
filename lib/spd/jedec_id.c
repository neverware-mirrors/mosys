/* Copyright 2012, Google Inc.
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
 *
 * JEDEC Solid State Technology Association, formerly known as Joint Electron
 * Device Engineering Council (JEDEC) or Joint Electron Device Engineering
 * Councils, is the semiconductor engineering standardization body of the
 * Electronic Industries Alliance (EIA), a trade association that represents
 * all areas of the electronics industry in the United States.
 */

#include <stdlib.h>

#include "lib/string.h"

#include "jedec_id.h"

/*
 * JEDEC Standard Manufacturer's Identification Code
 * from JEP106. Download the latest revision from jedec.org.
 */
static const char *jedec_id1[] = {
	[1] = "AMD",
	[2] = "AMI",
	[3] = "Fairchild",
	[4] = "Fujitsu",
	[5] = "GTE",
	[6] = "Harris",
	[7] = "Hitachi",
	[8] = "Inmos",
	[9] = "Intel",
	[10] = "I.T.T.",
	[11] = "Intersil",
	[12] = "Monolithic Memories",
	[13] = "Mostek",
	[14] = "Freescale",
	[15] = "National",
	[16] = "NEC",
	[17] = "RCA",
	[18] = "Raytheon",
	[19] = "Conexant",
	[20] = "Seeq",
	[21] = "NXP (Philips Semi)",
	[22] = "Synertek",
	[23] = "Texas Instruments",
	[24] = "Toshiba",
	[25] = "Xicor",
	[26] = "Zilog",
	[27] = "Eurotechnique",
	[28] = "Mitsubishi",
	[29] = "Lucent (AT&T)",
	[30] = "Exel",
	[31] = "Atmel",
	[32] = "STMicroelectronics (SGS/Thomson)",
	[33] = "Lattice Semi.",
	[34] = "NCR",
	[35] = "Wafer Scale Integration",
	[36] = "IBM",
	[37] = "Tristar",
	[38] = "Visic",
	[39] = "Intl. CMOS Technology",
	[40] = "SSSI",
	[41] = "Microchip Technology",
	[42] = "Ricoh Ltd.",
	[43] = "VLSI",
	[44] = "Micron Technology",
	[45] = "SK Hynix (Hyundai)",
	[46] = "OKI Semiconductor",
	[47] = "ACTEL",
	[48] = "Sharp",
	[49] = "Catalyst",
	[50] = "Panasonic",
	[51] = "IDT",
	[52] = "Cypress",
	[53] = "DEC",
	[54] = "LSI Logic",
	[55] = "Zarlink",
	[56] = "UTMC",
	[57] = "Thinking Machine",
	[58] = "Thomson CSF",
	[59] = "Integrated CMOS(Vertex)",
	[60] = "Honeywell",
	[61] = "Tektronix",
	[62] = "Oracle Corporation (Sun Microsystems)",
	[63] = "SST",
	[64] = "ProMos/Mosel Vitelic (Mosel)",
	[65] = "Infineon",
	[66] = "Macronix",
	[67] = "Xerox",
	[68] = "Plus Logic",
	[69] = "SanDisk",
	[70] = "Elan Circuit Tech.",
	[71] = "European Silicon Str.",
	[72] = "Apple Computer",
	[73] = "Xilinx",
	[74] = "Compaq",
	[75] = "Protocol Engines",
	[76] = "SCI",
	[77] = "Seiko Instruments",
	[78] = "Samsung",
	[79] = "I3 Design System",
	[80] = "Klic",
	[81] = "Crosspoint Solutions",
	[82] = "Alliance Semiconductor",
	[83] = "Tandem",
	[84] = "Hewlett-Packard",
	[85] = "Intg. Silicon Solutions",
	[86] = "Brooktree",
	[87] = "New Media",
	[88] = "MHS Electronic",
	[89] = "Performance Semi.",
	[90] = "Winbond Electronic",
	[91] = "Kawasaki Steel",
	[92] = "Bright Micro",
	[93] = "TECMAR",
	[94] = "Exar",
	[95] = "PCMCIA",
	[96] = "Goldstar (LG Semi)",
	[97] = "Northern Telecom",
	[98] = "Sanyo",
	[99] = "Array Microsystems",
	[100] = "Crystal Semiconductor",
	[101] = "Analog Devices",
	[102] = "PMC-Sierra",
	[103] = "Asparix",
	[104] = "Convex Computer",
	[105] = "Quality Semiconductor",
	[106] = "Nimbus Technology",
	[107] = "Transwitch",
	[108] = "Micronas (ITT Intermetall)",
	[109] = "Cannon",
	[110] = "Altera",
	[111] = "NEXCOM",
	[112] = "QUALCOMM",
	[113] = "Sony",
	[114] = "Cray Research",
	[115] = "AMS (Austria Micro)",
	[116] = "Vitesse",
	[117] = "Aster Electronics",
	[118] = "Bay Networks (Synoptic)",
	[119] = "Zentrum",
	[120] = "TRW",
	[121] = "Thesys",
	[122] = "Solbourne Computer",
	[123] = "Allied-Signal",
	[124] = "Dialog",
	[125] = "Media Vision",
	[126] = "Numonyx (Level One Communication)",
};

static const char *jedec_id2[] = {
	[1] = "Cirrus Logic",
	[2] = "National Instruments",
	[3] = "ILC Data Device",
	[4] = "Alcatel Mietec",
	[5] = "Micro Linear",
	[6] = "Univ. of NC",
	[7] = "JTAG Technologies",
	[8] = "BAE Systems",
	[9] = "Nchip",
	[10] = "Galileo Tech",
	[11] = "Bestlink Systems",
	[12] = "Graychip",
	[13] = "GENNUM",
	[14] = "VideoLogic",
	[15] = "Robert Bosch",
	[16] = "Chip Express",
	[17] = "DATARAM",
	[18] = "United Microelec Corp.",
	[19] = "TCSI",
	[20] = "Smart Modular",
	[21] = "Hughes Aircraft",
	[22] = "Lanstar Semiconductor",
	[23] = "Qlogic",
	[24] = "Kingston",
	[25] = "Music Semi",
	[26] = "Ericsson Components",
	[27] = "SpaSE",
	[28] = "Eon Silicon Devices",
	[29] = "Programmable Micro Corp",
	[30] = "DoD",
	[31] = "Integ. Memories Tech.",
	[32] = "Corollary Inc.",
	[33] = "Dallas Semiconductor",
	[34] = "Omnivision",
	[35] = "EIV(Switzerland)",
	[36] = "Novatel Wireless",
	[37] = "Zarlink (formerly Mitel)",
	[38] = "Clearpoint",
	[39] = "Cabletron",
	[40] = "STEC (Silicon Technology)",
	[41] = "Vanguard",
	[42] = "Hagiwara Sys-Com",
	[43] = "Vantis",
	[44] = "Celestica",
	[45] = "Century",
	[46] = "Hal Computers",
	[47] = "Rohm Company Ltd.",
	[48] = "Juniper Networks",
	[49] = "Libit Signal Processing",
	[50] = "Mushkin Enhanced Memory (Ramtron)",
	[51] = "Tundra Semiconductor",
	[52] = "Adaptec Inc.",
	[53] = "LightSpeed Semi.",
	[54] = "ZSP Corp.",
	[55] = "AMIC Technology",
	[56] = "Adobe Systems",
	[57] = "Dynachip",
	[58] = "PNY Electronics",
	[59] = "Newport Digital",
	[60] = "MMC Networks",
	[61] = "T Square",
	[62] = "Seiko Epson",
	[63] = "Broadcom",
	[64] = "Viking Components",
	[65] = "V3 Semiconductor",
	[66] = "Flextronics",
	[67] = "Suwa Electronics",
	[68] = "Transmeta",
	[69] = "Micron CMS",
	[70] = "American Computer & Digital Components Inc",
	[71] = "Enhance 3000 Inc",
	[72] = "Tower Semiconductor",
	[73] = "CPU Design",
	[74] = "Price Point",
	[75] = "Maxim Integrated Product",
	[76] = "Tellabs",
	[77] = "Centaur Technology",
	[78] = "Unigen Corporation",
	[79] = "Transcend Information",
	[80] = "Memory Card Technology",
	[81] = "CKD Corporation Ltd.",
	[82] = "Capital Instruments, Inc.",
	[83] = "Aica Kogyo, Ltd.",
	[84] = "Linvex Technology",
	[85] = "MSC Vertriebs GmbH",
	[86] = "AKM Company, Ltd.",
	[87] = "Dynamem, Inc.",
	[88] = "NERA ASA",
	[89] = "GSI Technology",
	[90] = "Dane-Elec (C Memory)",
	[91] = "Acorn Computers",
	[92] = "Lara Technology",
	[93] = "Oak Technology, Inc.",
	[94] = "Itec Memory",
	[95] = "Tanisys Technology",
	[96] = "Truevision",
	[97] = "Wintec Industries",
	[98] = "Super PC Memory",
	[99] = "MGV Memory",
	[100] = "Galvantech",
	[101] = "Gadzoox Nteworks",
	[102] = "Multi Dimensional Cons.",
	[103] = "GateField",
	[104] = "Integrated Memory System",
	[105] = "Triscend",
	[106] = "XaQti",
	[107] = "Goldenram",
	[108] = "Clear Logic",
	[109] = "Cimaron Communications",
	[110] = "Nippon Steel Semi. Corp.",
	[111] = "Advantage Memory",
	[112] = "AMCC",
	[113] = "LeCroy",
	[114] = "Yamaha Corporation",
	[115] = "Digital Microwave",
	[116] = "NetLogic Microsystems",
	[117] = "MIMOS Semiconductor",
	[118] = "Advanced Fibre",
	[119] = "BF Goodrich Data.",
	[120] = "Epigram",
	[121] = "Acbel Polytech Inc.",
	[122] = "Apacer Technology",
	[123] = "Admor Memory",
	[124] = "FOXCONN",
	[125] = "Quadratics Superconductor",
	[126] = "3COM",
};

static const char *jedec_id3[] = {
	[1] = "Camintonn Corporation",
	[2] = "ISOA Incorporated",
	[3] = "Agate Semiconductor",
	[4] = "ADMtek Incorporated",
	[5] = "HYPERTEC",
	[6] = "Adhoc Technologies",
	[7] = "MOSAID Technologies",
	[8] = "Ardent Technologies",
	[9] = "Switchcore",
	[10] = "Cisco Systems, Inc.",
	[11] = "Allayer Technologies",
	[12] = "WorkX AG",
	[13] = "Oasis Semiconductor",
	[14] = "Novanet Semiconductor",
	[15] = "E-M Solutions",
	[16] = "Power General",
	[17] = "Advanced Hardware Arch.",
	[18] = "Inova Semiconductors GmbH",
	[19] = "Telocity",
	[20] = "Delkin Devices",
	[21] = "Symagery Microsystems",
	[22] = "C-Port Corporation",
	[23] = "SiberCore Technologies",
	[24] = "Southland Microsystems",
	[25] = "Malleable Technologies",
	[26] = "Kendin Communications",
	[27] = "Great Technology Microcomputer",
	[28] = "Sanmina Corporation",
	[29] = "HADCO Corporation",
	[30] = "Corsair",
	[31] = "Actrans System Inc.",
	[32] = "ALPHA Technologies",
	[33] = "Silicon Labratories, Inc. (Cygnal)",
	[34] = "Artesyn Technologies",
	[35] = "Align Manufacturing",
	[36] = "Peregrine Semiconductor",
	[37] = "Chameleon Systems",
	[38] = "Aplus Flash Technology",
	[39] = "MIPS Technologies",
	[40] = "Chrysalis ITS",
	[41] = "ADTEC Corporation",
	[42] = "Kentron Technologies",
	[43] = "Win Technologies",
	[44] = "Tachyon Semiconductor (ASIC Designs Inc)",
	[45] = "Extreme Packet Devices",
	[46] = "RF Micro Devices",
	[47] = "Siemens AG",
	[48] = "Sarnoff Corporation",
	[49] = "Itautec SA (Itautec Philco SA)",
	[50] = "Radiata Inc.",
	[51] = "Benchmark Elect. (AVEX)",
	[52] = "Legend",
	[53] = "SpecTek Incorporated",
	[54] = "Hi/fn",
	[55] = "Enikia Incorporated",
	[56] = "SwitchOn Networks",
	[57] = "AANetcom Incorporated",
	[58] = "Micro Memory Bank",
	[59] = "ESS Technology",
	[60] = "Virata Corporation",
	[61] = "Excess Bandwidth",
	[62] = "West Bay Semiconductor",
	[63] = "DSP Group",
	[64] = "Newport Communications",
	[65] = "Chip2Chip Incorporated",
	[66] = "Phobos Corporation",
	[67] = "Intellitech Corporation",
	[68] = "Nordic VLSI ASA",
	[69] = "Ishoni Networks",
	[70] = "Silicon Spice",
	[71] = "Alchemy Semiconductor",
	[72] = "Agilent Technologies",
	[73] = "Centillium Communications",
	[74] = "W.L. Gore",
	[75] = "HanBit Electronics",
	[76] = "GlobeSpan",
	[77] = "Element 14",
	[78] = "Pycon",
	[79] = "Saifun Semiconductors",
	[80] = "Sibyte, Incorporated",
	[81] = "MetaLink Technologies",
	[82] = "Feiya Technology",
	[83] = "I & C Technology",
	[84] = "Shikatronics",
	[85] = "Elektrobit",
	[86] = "Megic",
	[87] = "Com-Tier",
	[88] = "Malaysia Micro Solutions",
	[89] = "Hyperchip",
	[90] = "Gemstone Communications",
	[91] = "Anadigm (Anadyne)",
	[92] = "3ParData",
	[93] = "Mellanox Technologies",
	[94] = "Tenx Technologies",
	[95] = "Helix AG",
	[96] = "Domosys",
	[97] = "Skyup Technology",
	[98] = "HiNT Corporation",
	[99] = "Chiaro",
	[100] = "MDT Technologies GmbH (MCI Computer)",
	[101] = "Exbit Technology A/S",
	[102] = "Integrated Technology Express",
	[103] = "AVED Memory",
	[104] = "Legerity",
	[105] = "Jasmine Networks",
	[106] = "Caspian Networks",
	[107] = "nCUBE",
	[108] = "Silicon Access Networks",
	[109] = "FDK Corporation",
	[110] = "High Bandwidth Access",
	[111] = "MultiLink Technology",
	[112] = "BRECIS",
	[113] = "World Wide Packets",
	[114] = "APW",
	[115] = "Chicory Systems",
	[116] = "Xstream Logic",
	[117] = "Fast-Chip",
	[118] = "Zucotto Wireless",
	[119] = "Realchip",
	[120] = "Galaxy Power",
	[121] = "eSilicon",
	[122] = "Morphics Technology",
	[123] = "Accelerant Networks",
	[124] = "Silicon Wave",
	[125] = "SandCraft",
	[126] = "Elpida",
};

static const char *jedec_id4[] = {
	[1] = "Solectron",
	[2] = "Optosys Technologies",
	[3] = "Buffalo (Formerly Melco)",
	[4] = "TriMedia Technologies",
	[5] = "Cyan Technologies",
	[6] = "Global Locate",
	[7] = "Optillion",
	[8] = "Terago Communications",
	[9] = "Ikanos Communications",
	[10] = "Princeton Technology",
	[11] = "Nanya Technology",
	[12] = "Elite Flash Storage",
	[13] = "Mysticom",
	[14] = "LightSand Communications",
	[15] = "ATI Technologies",
	[16] = "Agere Systems",
	[17] = "NeoMagic",
	[18] = "AuroraNetics",
	[19] = "Golden Empire",
	[20] = "Muskin",
	[21] = "Tioga Technologies",
	[22] = "Netlist",
	[23] = "TeraLogic",
	[24] = "Cicada Semiconductor",
	[25] = "Centon Electronics",
	[26] = "Tyco Electronics",
	[27] = "Magis Works",
	[28] = "Zettacom",
	[29] = "Cogency Semiconductor",
	[30] = "Chipcon AS",
	[31] = "Aspex Technology",
	[32] = "F5 Networks",
	[33] = "Programmable Silicon Solutions",
	[34] = "ChipWrights",
	[35] = "Acorn Networks",
	[36] = "Quicklogic",
	[37] = "Kingmax Semiconductor",
	[38] = "BOPS",
	[39] = "Flasys",
	[40] = "BitBlitz Communications",
	[41] = "eMemory Technology",
	[42] = "Procket Networks",
	[43] = "Purple Ray",
	[44] = "Trebia Networks",
	[45] = "Delta Electronics",
	[46] = "Onex Communications",
	[47] = "Ample Communications",
	[48] = "Memory Experts Intl",
	[49] = "Astute Networks",
	[50] = "Azanda Network Devices",
	[51] = "Dibcom",
	[52] = "Tekmos",
	[53] = "API NetWorks",
	[54] = "Bay Microsystems",
	[55] = "Firecron Ltd",
	[56] = "Resonext Communications",
	[57] = "Tachys Technologies",
	[58] = "Equator Technology",
	[59] = "Concept Computer",
	[60] = "SILCOM",
	[61] = "3Dlabs",
	[62] = "ct Magazine",
	[63] = "Sanera Systems",
	[64] = "Silicon Packets",
	[65] = "Viasystems Group",
	[66] = "Simtek",
	[67] = "Semicon Devices Singapore",
	[68] = "Satron Handelsges",
	[69] = "Improv Systems",
	[70] = "INDUSYS GmbH",
	[71] = "Corrent",
	[72] = "Infrant Technologies",
	[73] = "Ritek Corp",
	[74] = "empowerTel Networks",
	[75] = "Hypertec",
	[76] = "Cavium Networks",
	[77] = "PLX Technology",
	[78] = "Massana Design",
	[79] = "Intrinsity",
	[80] = "Valence Semiconductor",
	[81] = "Terawave Communications",
	[82] = "IceFyre Semiconductor",
	[83] = "Primarion",
	[84] = "Picochip Designs Ltd",
	[85] = "Silverback Systems",
	[86] = "Jade Star Technologies",
	[87] = "Pijnenburg Securealink",
	[88] = "takeMS - Ultron AG (TakeMS International AG)",
	[89] = "Cambridge Silicon Radio",
	[90] = "Swissbit",
	[91] = "Nazomi Communications",
	[92] = "eWave System",
	[93] = "Rockwell Collins",
	[94] = "Picocel Co., Ltd.",
	[95] = "Alphamosaic Ltd",
	[96] = "Sandburst",
	[97] = "SiCon Video",
	[98] = "NanoAmp Solutions",
	[99] = "Ericsson Technology",
	[100] = "PrairieComm",
	[101] = "Mitac International",
	[102] = "Layer N Networks",
	[103] = "MtekVision (Atsana)",
	[104] = "Allegro Networks",
	[105] = "Marvell Semiconductors",
	[106] = "Netergy Microelectronic",
	[107] = "NVIDIA",
	[108] = "Internet Machines",
	[109] = "Peak Electronics",
	[111] = "Accton Technology",
	[112] = "Teradiant Networks",
	[113] = "Scaleo Chip (Europe Technologies)",
	[114] = "Cortina Systems",
	[115] = "RAM Components",
	[116] = "Raqia Networks",
	[117] = "ClearSpeed",
	[118] = "Matsushita Battery",
	[119] = "Xelerated",
	[120] = "SimpleTech",
	[121] = "Utron Technology",
	[122] = "Astec International",
	[123] = "AVM gmbH",
	[124] = "Redux Communications",
	[125] = "Dot Hill Systems",
	[126] = "TeraChip",
};

static const char *jedec_id5[] = {
	[1] = "T-RAM Incorporated",
	[2] = "Innovics Wireless",
	[3] = "Teknovus",
	[4] = "KeyEye Communications",
	[5] = "Runcom Technologies",
	[6] = "RedSwitch",
	[7] = "Dotcast",
	[8] = "Silicon Mountain Memory",
	[9] = "Signia Technologies",
	[10] = "Pixim",
	[11] = "Galazar Networks",
	[12] = "White Electronic Designs",
	[13] = "Patriot Scientific",
	[14] = "Neoaxiom Corporation",
	[15] = "3Y Power Technology",
	[16] = "Scaleo Chip (Europe Technologies)",
	[17] = "Potentia Power Systems",
	[18] = "C-guys Incorporated",
	[19] = "Digital Communications Technology Incorporated",
	[20] = "Silicon-Based Technology",
	[21] = "Fulcrum Microsystems",
	[22] = "Positivo Informatica Ltd",
	[23] = "XIOtech Corporation",
	[24] = "PortalPlayer",
	[25] = "Zhiying Software",
	[26] = "ParkerVision, Inc. (Direct2Data)",
	[27] = "Phonex Broadband",
	[28] = "Skyworks Solutions",
	[29] = "Entropic Communications",
	[30] = "I'M Intelligent Memory Ltd. (Pacific Force Technology)",
	[31] = "Zensys A/S",
	[32] = "Legend Silicon Corp.",
	[33] = "sci-worx GmbH",
	[34] = "SMSC (Oasis Silicon Systems)",
	[35] = "Renesas Electronics",
	[36] = "Raza Microelectronics",
	[37] = "Phyworks",
	[38] = "MediaTek",
	[39] = "Non-cents Productions",
	[40] = "US Modular",
	[41] = "Wintegra Ltd",
	[42] = "Mathstar",
	[43] = "StarCore",
	[44] = "Oplus Technologies",
	[45] = "Mindspeed",
	[46] = "Just Young Computer",
	[47] = "Radia Communications",
	[48] = "OCZ",
	[49] = "Emuzed",
	[50] = "LOGIC Devices",
	[51] = "Inphi Corporation",
	[52] = "Quake Technologies",
	[53] = "Vixel",
	[54] = "SolusTek",
	[55] = "Kongsberg Maritime",
	[56] = "Faraday Technology",
	[57] = "Altium Ltd.",
	[58] = "Insyte",
	[59] = "ARM Ltd.",
	[60] = "DigiVision",
	[61] = "Vativ Technologies",
	[62] = "Endicott Interconnect Technologies",
	[63] = "Pericom",
	[64] = "Bandspeed",
	[65] = "LeWiz Communications",
	[66] = "CPU Technology",
	[67] = "Ramaxel Technology",
	[68] = "DSP Group",
	[69] = "Axis Communications",
	[70] = "Legacy Electronics",
	[71] = "Chrontel",
	[72] = "Powerchip Semiconductor",
	[73] = "MobilEye Technologies",
	[74] = "Excel Semiconductor",
	[75] = "A-DATA Technology",
	[76] = "VirtualDigm",
	[77] = "G Skill Intl",
	[78] = "Quanta Computer",
	[79] = "Yield Microelectronics",
	[80] = "Afa Technologies",
	[81] = "KINGBOX Technology Co. Ltd.",
	[82] = "Ceva",
	[83] = "iStor Networks",
	[84] = "Advance Modules",
	[85] = "Microsoft",
	[86] = "Open-Silicon",
	[87] = "Goal Semiconductor",
	[88] = "ARC International",
	[89] = "Simmtec",
	[90] = "Metanoia",
	[91] = "Key Stream",
	[92] = "Lowrance Electronics",
	[93] = "Adimos",
	[94] = "SiGe Semiconductor",
	[95] = "Fodus Communications",
	[96] = "Credence Systems Corp.",
	[97] = "Genesis Microchip Inc.",
	[98] = "Vihana, Inc.",
	[99] = "WIS Technologies",
	[100] = "GateChange Technologies",
	[101] = "High Density Devices AS",
	[102] = "Synopsys",
	[103] = "Gigaram",
	[104] = "Enigma Semiconductor Inc.",
	[105] = "Centry Micro Inc.",
	[106] = "Icera Semiconductor",
	[107] = "Mediaworks Integrated Systems",
	[108] = "O'Neil Product Development",
	[109] = "Supreme Top Technology Ltd.",
	[110] = "MicroDisplay Corporation",
	[111] = "Team Group Inc.",
	[112] = "Sinett Corporation",
	[113] = "Toshiba Corporation",
	[114] = "Tensilica",
	[115] = "SiRF Technology",
	[116] = "Bacoc Inc.",
	[117] = "SMaL Camera Technologies",
	[118] = "Thompson SC",
	[119] = "Airgo Networks",
	[120] = "Wisair Ltd.",
	[121] = "SigmaTel",
	[122] = "Arkados",
	[123] = "Compete IT gmbH Co. KG",
	[124] = "Eudar Technology Inc.",
	[125] = "Focus Enhancements",
	[126] = "Xyratex",
};

static const char *jedec_id6[] = {
	[1] = "Specular Networks",
	[2] = "Patriot Memory",
	[3] = "U-Chip Technology Corp",
	[4] = "Silicon Optix",
	[5] = "Greenfield Networks",
	[6] = "CompuRAM GmbH",
	[7] = "Stargen, Inc.",
	[8] = "NetCell Corporation",
	[9] = "Excalibrus Technolgoies Ltd",
	[10] = "SCM Microsystems",
	[11] = "Xsigo Systems, Inc.",
	[12] = "CHIPS & Systems Inc",
	[13] = "Tier 1 Multichip Solutions",
	[14] = "CWRL Labs",
	[15] = "Teradici",
	[16] = "Gigaram, Inc.",
	[17] = "g2 Microsystems",
	[18] = "PowerFlash Semiconductor",
	[19] = "P.A. Semi, Inc.",
	[20] = "NovaTech Solutions, S.A.",
	[21] = "c2 Microsystems, Inc.",
	[22] = "Level5 Networks",
	[23] = "COS Memory AG",
	[24] = "Innovasic Semiconductor",
	[25] = "O2IC Co. Ltd",
	[26] = "Tabula, Inc.",
	[27] = "Crucial Technology",
	[28] = "Chelsio Communications",
	[29] = "Solarflare Communications",
	[30] = "Xambala Inc.",
	[31] = "EADS Astrium",
	[32] = "Terra Semiconductor (ATO Semicon)",
	[33] = "Imaging Works, Inc.",
	[34] = "Astute Networks, Inc.",
	[35] = "Tzero",
	[36] = "Emulex",
	[37] = "Power-One",
	[38] = "Pulse~LINK Inc.",
	[39] = "Hon Hai Precision Industry",
	[40] = "White Rock Networks Inc.",
	[41] = "Telegent Systems USA, Inc.",
	[42] = "Atrua Technologies, Inc.",
	[43] = "Acbel Polytech Inc.",
	[44] = "eRide Inc.",
	[45] = "ULi Electronics Inc.",
	[46] = "Magnum Semiconductor Inc.",
	[47] = "neoOne Technology, Inc.",
	[48] = "Connex Technology, Inc.",
	[49] = "Stream Processors, Inc.",
	[50] = "Focus Enhancements",
	[51] = "Telecis Wireless, Inc.",
	[52] = "uNav Microelectronics",
	[53] = "Tarari, Inc.",
	[54] = "Ambric, Inc.",
	[55] = "Newport Media, Inc.",
	[56] = "VMTS",
	[57] = "Enuclia Semiconductor, Inc.",
	[58] = "Virtium Technology Inc.",
	[59] = "Solid State System Co., Ltd.",
	[60] = "Kian Tech LLC",
	[61] = "Artimi",
	[62] = "Power Quotient International",
	[63] = "Avago Technologies",
	[64] = "ADTechnology",
	[65] = "Sigma Designs",
	[66] = "SiCortex, Inc.",
	[67] = "Ventura Technology Group",
	[68] = "eASIC",
	[69] = "M.H.S SAS",
	[70] = "Micro Star International",
	[71] = "Rapport Inc.",
	[72] = "Makway International",
	[73] = "Broad Reach Engineering Co.",
	[74] = "Semiconductor Mfg Intl Corp",
	[75] = "SiConnect",
	[76] = "FCI USA Inc.",
	[77] = "Validity Sensors",
	[78] = "Coney Technology Co. Ltd.",
	[79] = "Spans Logic",
	[80] = "Neterion Inc.",
	[81] = "Qimonda",
	[82] = "New Japan Radio Co. Ltd.",
	[83] = "Velogix",
	[84] = "Montalvo Systems",
	[85] = "iVivity Inc.",
	[86] = "Walton Chaintech",
	[87] = "AENEON",
	[88] = "Lorom Industrial Co. Ltd.",
	[89] = "Radiospire Networks",
	[90] = "Sensio Technologies, Inc.",
	[91] = "Nethra Imaging",
	[92] = "Hexon Technology Pte Ltd",
	[93] = "CompuStocx (CSX)",
	[94] = "Methode Electronics, Inc.",
	[95] = "Connect One Ltd.",
	[96] = "Opulan Technologies",
	[97] = "Septentrio NV",
	[98] = "Goldenmars Technology Inc.",
	[99] = "Kreton Corporation",
	[100] = "Cochlear Ltd.",
	[101] = "Altair Semiconductor",
	[102] = "NetEffect, Inc.",
	[103] = "Spansion, Inc.",
	[104] = "Taiwan Semiconductor Mfg",
	[105] = "Emphany Systems Inc.",
	[106] = "ApaceWave Technologies",
	[107] = "Mobilygen Corporation",
	[108] = "Tego",
	[109] = "Cswitch Corporation",
	[110] = "Haier (Beijing) IC Design Co.",
	[111] = "MetaRAM",
	[112] = "Axel Electronics Co. Ltd.",
	[113] = "Tilera Corporation",
	[114] = "Aquantia",
	[115] = "Vivace Semiconductor",
	[116] = "Redpine Signals",
	[117] = "Octalica",
	[118] = "InterDigital Communications",
	[119] = "Avant Technology",
	[120] = "Asrock, Inc.",
	[121] = "Availink",
	[122] = "Quartics, Inc.",
	[123] = "Element CXI",
	[124] = "Innovaciones Microelectronicas",
	[125] = "VeriSilicon Microelectronics",
	[126] = "W5 Networks",
};

static const char *jedec_id7[] = {
	[1] = "MOVEKING",
	[2] = "Mavrix Technology, Inc.",
	[3] = "CellGuide Ltd.",
	[4] = "Faraday Technology",
	[5] = "Diablo Technologies, Inc.",
	[6] = "Jennic",
	[7] = "Octasic",
	[8] = "Molex Incorporated",
	[9] = "3Leaf Networks",
	[10] = "Bright Micron Technology",
	[11] = "Netxen",
	[12] = "NextWave Broadband Inc.",
	[13] = "DisplayLink",
	[14] = "ZMOS Technology",
	[15] = "Tec-Hill",
	[16] = "Multigig, Inc.",
	[17] = "Amimon",
	[18] = "Euphonic Technologies, Inc.",
	[19] = "BRN Phoenix",
	[20] = "InSilica",
	[21] = "Ember Corporation",
	[22] = "Avexir Technologies Corporation",
	[23] = "Echelon Corporation",
	[24] = "Edgewater Computer Systems",
	[25] = "XMOS Semiconductor Ltd.",
	[26] = "GENUSION, Inc.",
	[27] = "Memory Corp NV",
	[28] = "SiliconBlue Technologies",
	[29] = "Rambus Inc.",
	[30] = "Andes Technology Corporation",
	[31] = "Coronis Systems",
	[32] = "Achronix Semiconductor",
	[33] = "Siano Mobile Silicon Ltd.",
	[34] = "Semtech Corporation",
	[35] = "Pixelworks Inc.",
	[36] = "Gaisler Research AB",
	[37] = "Teranetics",
	[38] = "Toppan Printing Co. Ltd.",
	[39] = "Kingxcon",
	[40] = "Silicon Integrated Systems",
	[41] = "I-O Data Device, Inc.",
	[42] = "NDS Americas Inc.",
	[43] = "Solomon Systech Limited",
	[44] = "On Demand Microelectronics",
	[45] = "Amicus Wireless Inc.",
	[46] = "SMARDTV SNC",
	[47] = "Comsys Communication Ltd.",
	[48] = "Movidia Ltd.",
	[49] = "Javad GNSS, Inc.",
	[50] = "Montage Technology Group",
	[51] = "Trident Microsystems",
	[52] = "Super Talent",
	[53] = "Optichron, Inc.",
	[54] = "Future Waves UK Ltd.",
	[55] = "SiBEAM, Inc.",
	[56] = "Inicore,Inc.",
	[57] = "Virident Systems",
	[58] = "M2000, Inc.",
	[59] = "ZeroG Wireless, Inc.",
	[60] = "Gingle Technology Co. Ltd.",
	[61] = "Space Micro Inc.",
	[62] = "Wilocity",
	[63] = "Novafora, Ic.",
	[64] = "iKoa Corporation",
	[65] = "ASint Technology",
	[66] = "Ramtron",
	[67] = "Plato Networks Inc.",
	[68] = "IPtronics AS",
	[69] = "Infinite-Memories",
	[70] = "Parade Technologies Inc.",
	[71] = "Dune Networks",
	[72] = "GigaDevice Semiconductor",
	[73] = "Modu Ltd.",
	[74] = "CEITEC",
	[75] = "Northrop Grumman",
	[76] = "XRONET Corporation",
	[77] = "Sicon Semiconductor AB",
	[78] = "Atla Electronics Co. Ltd.",
	[79] = "TOPRAM Technology",
	[80] = "Silego Technology Inc.",
	[81] = "Kinglife",
	[82] = "Ability Industries Ltd.",
	[83] = "Silicon Power Computer & Communications",
	[84] = "Augusta Technology, Inc.",
	[85] = "Nantronics Semiconductors",
	[86] = "Hilscher Gesellschaft",
	[87] = "Quixant Ltd.",
	[88] = "Percello Ltd.",
	[89] = "NextIO Inc.",
	[90] = "Scanimetrics Inc.",
	[91] = "FS-Semi Company Ltd.",
	[92] = "Infinera Corporation",
	[93] = "SandForce Inc.",
	[94] = "Lexar Media",
	[95] = "Teradyne Inc.",
	[96] = "Memory Exchange Corp.",
	[97] = "Suzhou Smartek Electronics",
	[98] = "Avantium Corporation",
	[99] = "ATP Electronics Inc.",
	[100] = "Valens Semiconductor Ltd",
	[101] = "Agate Logic, Inc.",
	[102] = "Netronome",
	[103] = "Zenverge, Inc.",
	[104] = "N-trig Ltd",
	[105] = "SanMax Technologies Inc.",
	[106] = "Contour Semiconductor Inc.",
	[107] = "TwinMOS",
	[108] = "Silicon Systems, Inc.",
	[109] = "V-Color Technology Inc.",
	[110] = "Certicom Corporation",
	[111] = "JSC ICC Milandr",
	[112] = "PhotoFast Global Inc.",
	[113] = "InnoDisk Corporation",
	[114] = "Muscle Power",
	[115] = "Energy Micro",
	[116] = "Innofidei",
	[117] = "CopperGate Communications",
	[118] = "Holtek Semiconductor Inc.",
	[119] = "Myson Century, Inc.",
	[120] = "FIDELIX",
	[121] = "Red Digital Cinema",
	[122] = "Densbits Technology",
	[123] = "Zempro",
	[124] = "MoSys",
	[125] = "Provigent",
	[126] = "Triad Semiconductor, Inc.",
};

static const char *jedec_id8[] = {
	[1] = "Siklu Communication Ltd.",
	[2] = "A Force Manufacturing Ltd.",
	[3] = "Strontium",
	[4] = "Abilis Systems",
	[5] = "Siglead, Inc.",
	[6] = "Ubicom, Inc.",
	[7] = "Unifosa Corporation",
	[8] = "Stretch, Inc.",
	[9] = "Lantiq Deutschland GmbH",
	[10] = "Visipro.",
	[11] = "EKMemory",
	[12] = "Microelectronics Institute ZTE",
	[13] = "Cognovo Ltd.",
	[14] = "Carry Technology Co. Ltd.",
	[15] = "Nokia",
	[16] = "King Tiger Technology",
	[17] = "Sierra Wireless",
	[18] = "HT Micron",
	[19] = "Albatron Technology Co. Ltd.",
	[20] = "Leica Geosystems AG",
	[21] = "BroadLight",
	[22] = "AEXEA",
	[23] = "ClariPhy Communications, Inc.",
	[24] = "Green Plug",
	[25] = "Design Art Networks",
	[26] = "Mach Xtreme Technology Ltd.",
	[27] = "ATO Solutions Co. Ltd.",
	[28] = "Ramsta",
	[29] = "Greenliant Systems, Ltd.",
	[30] = "Teikon",
	[31] = "Antec Hadron",
	[32] = "NavCom Technology, Inc.",
	[33] = "Shanghai Fudan Microelectronics",
	[34] = "Calxeda, Inc.",
	[35] = "JSC EDC Electronics",
	[36] = "Kandit Technology Co. Ltd.",
	[37] = "Ramos Technology",
	[38] = "Goldenmars Technology",
	[39] = "XeL Technology Inc.",
	[40] = "Newzone Corporation",
	[41] = "ShenZhen MercyPower Tech",
	[42] = "Nanjing Yihuo Technology",
	[43] = "Nethra Imaging Inc.",
	[44] = "SiTel Semiconductor BV",
	[45] = "SolidGear Corporation",
	[46] = "Topower Computer Ind Co Ltd.",
	[47] = "Wilocity",
	[48] = "Profichip GmbH",
	[49] = "Gerad Technologies",
	[50] = "Ritek Corporation",
	[51] = "Gomos Technology Limited",
	[52] = "Memoright Corporation",
	[53] = "D-Broad, Inc.",
	[54] = "HiSilicon Technologies",
	[55] = "Syndiant Inc..",
	[56] = "Enverv Inc.",
	[57] = "Cognex",
	[58] = "Xinnova Technology Inc.",
	[59] = "Ultron AG",
	[60] = "Concord Idea Corporation",
	[61] = "AIM Corporation",
	[62] = "Lifetime Memory Products",
	[63] = "Ramsway",
	[64] = "Recore Systems B.V.",
	[65] = "Haotian Jinshibo Science Tech",
	[66] = "Being Advanced Memory",
	[67] = "Adesto Technologies",
	[68] = "Giantec Semiconductor, Inc.",
	[69] = "HMD Electronics AG",
	[70] = "Gloway International (HK)",
	[71] = "Kingcore",
	[72] = "Anucell Technology Holding",
	[73] = "Accord Software & Systems Pvt. Ltd.",
	[74] = "Active-Semi Inc.",
	[75] = "Denso Corporation",
	[76] = "TLSI Inc.",
	[77] = "Qidan",
	[78] = "Mustang",
	[79] = "Orca Systems",
	[80] = "Passif Semiconductor",
	[81] = "GigaDevice Semiconductor (Beijing) Inc.",
	[82] = "Memphis Electronic",
	[83] = "Beckhoff Automation GmbH",
	[84] = "Harmony Semiconductor Corp",
	[85] = "Air Computers SRL",
	[86] = "TMT Memory",
	[87] = "Eorex Corporation",
	[88] = "Xingtera",
	[89] = "Netsol",
	[90] = "Bestdon Technology Co. Ltd.",
	[91] = "Baysand Inc.",
	[92] = "Uroad Technology Co. Ltd.",
	[93] = "Wilk Elektronik S.A.",
	[94] = "AAI",
	[95] = "Harman",
	[96] = "Berg Microelectronics Inc.",
	[97] = "ASSIA, Inc.",
	[98] = "Visiontek Products LLC",
	[99] = "OCMEMORY",
	[100] = "Welink Solution Inc.",
	[101] = "Shark Gaming",
	[102] = "Avalanche Technology",
	[103] = "R&D Center ELVEES OJSC",
	[104] = "KingboMars Technology Co. Ltd.",
	[105] = "High Bridge Solutions Industria Eletronica",
	[106] = "Transcend Technology Co. Ltd.",
	[107] = "Everspin Technologies",
	[108] = "Hon-Hai Precision",
	[109] = "Smart Storage Systems",
	[110] = "Toumaz Group",
	[111] = "Zentel Electronics Corporation",
	[112] = "Panram International Corporation",
	[113] = "Silicon Space Technology",
	[114] = "LITE-ON IT Corporation",
	[115] = "Inuitive",
	[116] = "HMicro",
	[117] = "BittWare, Inc.",
	[118] = "GLOBALFOUNDRIES",
	[119] = "ACPI Digital Co. Ltd.",
	[120] = "Annapurna Labs",
	[121] = "AcSiP Technology Corporation",
	[122] = "Idea! Electronic Systems",
	[123] = "Gowe Technology Co. Ltd.",
	[124] = "Hermes Testing Solutions, Inc.",
	[125] = "Positivo BGH",
	[126] = "Intelligence Silicon Technology",
};

static const char *jedec_id9[] = {
	[1] = "3D PLUS",
	[2] = "Diehl Aerospace",
	[3] = "Fairchild",
	[4] = "Mercury Systems",
	[5] = "Sonics, Inc.",
	[6] = "GE Intelligent Platforms GmbH & Co.",
	[7] = "Shenzhen Jinge Information Co.  Ltd.",
	[8] = "SCWW",
	[9] = "Silicon Motion Inc.",
	[10] = "Anurag",
	[11] = "King Kong",
	[12] = "FROM30 Co. Ltd.",
};

static const char **jedec_ids[] = {
	jedec_id1,
	jedec_id2,
	jedec_id3,
	jedec_id4,
	jedec_id5,
	jedec_id6,
	jedec_id7,
	jedec_id8,
	jedec_id9,
};

/*
 * jedec_manufacturer  -  find manufacturer based on id and continuation bytes
 *
 * @idx:        continuation byte index
 * @id:         identifier
 *
 * returns pointer to manufacturer id
 */
const char *jedec_manufacturer(int idx, uint8_t id)
{
	if (0 <= idx &&
            idx < sizeof(jedec_ids) / sizeof(jedec_ids[0]))
		return jedec_ids[idx][id];
	return NULL;
}
