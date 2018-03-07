

			.text
			.equ HPS0,0xFFC08000//100MHZ
			.equ HPS1,0xFFC09000//100MHZ
			.equ HPS2,0xFFD00000//25MHZ
			.equ HPS3,0xFFD01000//25MHZ
			.equ BDATA,0xFF200050 
			.equ EDATA,0xFF20005C
			.global HPS_TIM_config_ASM
			.global HPS_TIM_read_INT_ASM
			.global HPS_TIM_clear_INT_ASM
			.global EDGECAP_ASM
			.global CLEAR_EDGECAP_ASM

HPS_TIM_config_ASM:	
				
				PUSH {LR}
				LDR R1,[R0]//r1 is 4bits
				BIC R1,#0xFFFFFFF0
//B END
//END:B END
				AND R2,R1,#0x1 // configue timer 0
				CMP R2,#0x1
				LDREQ R7,=HPS0
				PUSH {R0-R10}
				BLEQ CONFIG
				POP {R0-R10}

				AND R2,R1,#0x2 // configue timer 1
				CMP R2,#0x2
				LDREQ R7,=HPS1
				PUSH {R0-R10}
				BLEQ CONFIG
				POP {R0-R10}

				AND R2,R1,#0x4 // configue timer 2
				CMP R2,#0x4
				LDREQ R7,=HPS2
				PUSH {R0-R10}
				BLEQ CONFIG
				POP {R0-R10}

				AND R2,R1,#0x8 // configue timer 3
				CMP R2,#0x8
				LDREQ R7,=HPS3
				PUSH {R0-R10}
				BLEQ CONFIG
				POP {R0-R10}

				POP {LR}

				BX LR

CONFIG:			MOV R1,#0//set control E to 0 to config
				LDR R2,[R7,#0x8]
				BIC R2,#0x1
				ORR R1,R1,R2
				STR R1,[R7,#0x8]
					

				LDR R1,[R0,#4]	//timeout =load value with correct scale
				PUSH {R3}
				MOV R3,#100
				MUL R1,R3
				POP {R3}

				LDR R2,=HPS1
				CMP R7,R2
				PUSHEQ {R3}
				LDREQ R3,=0x186A0
				SUBEQ R1,R3
				POPEQ {R3}
				BEQ SKIP
				CMP R7,#0xFFC08000
				LSRNE R1,#2
SKIP:

				STR R1,[R7,#0x0]	//store time out

				LDR R1,[R0,#8]	//Load_en =M bit
				LDR R2,[R7,#0x8]
				LSL R1,#1
				BIC R2,#0x2
				ORR R1,R1,R2
				STR R1,[R7,#0x8]

				LDR R1,[R0,#12]	//Interupt_en =I-bit
				LDR R2,[R7,#0x8]
				LSL R1,#2
				BIC R2,#0x4
				ORR R1,R1,R2
				STR R1,[R7,#0x8]

				LDR R1,[R0,#16]	//set enable to Enable works 07=111
				LDR R2,[R7,#0x8]
				BIC R2,#0x1
				ORR R1,R1,R2
				STR R1,[R7,#0x8]

				BX LR

HPS_TIM_read_INT_ASM: //Return the read from timers
				
				PUSH {LR}
				MOV R1,R0//r1 is 4bits

				AND R2,R1,#0x1
				CMP R2,#0x1
				LDREQ R7,=HPS0
				BLEQ READ

				AND R2,R1,#0x2
				CMP R2,#0x2
				LDREQ R7,=HPS1
				BLEQ READ

				AND R2,R1,#0x4
				CMP R2,#0x4
				LDREQ R7,=HPS2
				BLEQ READ

				AND R2,R1,#0x8
				CMP R2,#0x8
				LDREQ R7,=HPS3
				BLEQ READ

READ:			LDR R1,[R7,#0x10]
				BIC R1,#0xFFFFFFFE
				MOV R0,R1 //RETURN R1
				
				POP {LR}
				BX LR


HPS_TIM_clear_INT_ASM: // clear timers

				PUSH {LR}
				MOV R1,R0//r1 is 4bits

				AND R2,R1,#0x1
				CMP R2,#0x1
				LDREQ R7,=HPS0
				BLEQ WRITE

				AND R2,R1,#0x2
				CMP R2,#0x2
				LDREQ R7,=HPS1
				BLEQ WRITE

				AND R2,R1,#0x4
				CMP R2,#0x4
				LDREQ R7,=HPS2
				BLEQ WRITE

				AND R2,R1,#0x8
				CMP R2,#0x8
				LDREQ R7,=HPS3
				BLEQ WRITE

				POP {LR}
				BX LR

WRITE:			MOV R1,#0//set control E to 0 to config
				LDR R2,[R7,#0x8]
				BIC R2,#0x1
				ORR R1,R1,R2
				STR R1,[R7,#0x8]


				LDR R1,[R7,#0x10]
				BIC R1,#0x1
				STR R1,[R7,#0x10]

				LDR R1,[R7,#0xC]
				BIC R1,#0x1
				STR R1,[R7,#0xC]


				MOV R1,#1//set control E to 1
				LDR R2,[R7,#0x8]
				BIC R2,#0x1
				ORR R1,R1,R2
				STR R1,[R7,#0x8]
				
				BX LR

EDGECAP_ASM:	//Read from edgecapture
				PUSH {R1,R2}
				LDR R1,=EDATA
				LDR R0,[R1]
				BIC R0,#0xFFFFFFF0
				POP {R1,R2}
				BX LR

CLEAR_EDGECAP_ASM:  //Clear edgecapture
				PUSH {R1-R3}
				LDR R2,=EDATA
				LDR	R3,[R2]
				STR R3,[R2]
				POP {R1-R3}
				BX LR
	
