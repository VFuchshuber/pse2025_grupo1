ORG 000h

MOV TMOD, #001h						;configura o modo do timer
MOV R7, #000h 

IDLE:
	JNB P2.7, CONFIG				;se acionado -> configura
	SJMP IDLE

CONFIG:
	MOV a, #000h					;zera acumulador
	MOV b, #014h					;set B para 20d (para multiplicar e obter o numero de contagens para o tempo de comutação)

	;logica para configurar o tempo de comutacao
		JNB P2.2, ADD2
		ADD a, #004h
	ADD2:
		JNB P2.1, ADD1
		ADD a, #002h
	ADD1:
		JNB P2.0, RETURN_ADD		
		ADD a, #001h
RETURN_ADD:
	ADD a, #001h					;adiciona 1 para obter tempo de comutacao entre 1 e 8s
	MUL AB						;multiplica o valor em acc por 2
	MOV R0, a					;o resultado da multiplicação para R0 (numero de iterações do timer para o tempo de comutação)
	JB P2.7, SHUTDOWN				;se desliga motor dutante a configuração
	JB P2.6, CW_START_STAR				;inicia sentido horario
	JNB P2.6, CCW_START_STAR			;inicia sentido anti-horario

CW_START_STAR: 
	MOV R7, #001h					;sinaliza sentido horario
	CLR P1.7
	SJMP TIMER_COUNTER
CW_STOP_STAR:
	SETB P1.7
	MOV R0, #002h					;CONFIGURA PARA 100ms
	SJMP TIMER_COUNTER
CW_START_DELTA:
	CLR P1.5
	SJMP CW_READY

CCW_START_STAR:
	MOV R7, #000h					;sinaliza anti-horario
	CLR P1.6
	SJMP TIMER_COUNTER
CCW_STOP_STAR:
	SETB P1.6
	MOV R0, #002h					;CONFIGURA PARA 100ms
	SJMP TIMER_COUNTER
CCW_START_DELTA:
	CLR P1.4
	SJMP CCW_READY

TIMER50MS:
	MOV TL0, #0B0h					;configura bit menos significativo do timer
	MOV TH0, #03Ch					;configura bit mais significativo do timer
	MOV TCON, #010h					;inicializa timer
	JNB TCON.5, $					;mantém nessa linha até terminar o tempo do timer
	DEC R0						;conta uma passagem pelo timer
	SJMP TIMER_COUNTER

TIMER_COUNTER:
	JNB P1.0, IGNORE_SHUTDOWN			;se estiver na espera dos 3s (motor desligado), pula para ignore_shutdown para evitar loop infinito
	JB P2.7, SHUTDOWN				;se o motor for desligado durante o decorrer do timer, pula para shutdown
CONTINUE_TIMER_COUNTER:
	CJNE R0, #000h, TIMER50mS			;verifica se o R0 esta zerado, caso contrario retorna para o timer de 50 ms
	MOV TCON, #000h					;para timer
FINISH_TIMER:
	MOV a, R7					;copia R7 para acc para testar condicoes
	JNB P1.7, CW_STOP_STAR				;se estiver operando em estrela, desliga estrela
	JNB P1.6, CCW_STOP_STAR				;se estiver operando em estrela, desliga estrela
	JNB P1.0, WAIT3S_STOP				;se estiver na espera de 3s
	JB ACC.0, CW_START_DELTA			;se R7.0 for 1, sentido horario
	JNB ACC.0, CCW_START_DELTA			;se R7.0 for 0, sentido anti-horario

IGNORE_SHUTDOWN:
	JNB P2.7, START_MOTOR_3S_WAIT			;se ligar o motor dentro dos 3s de espera
	SJMP CONTINUE_TIMER_COUNTER

START_MOTOR_3S_WAIT:					;CASO O MOTOR SEJA LIGADO DURANTE OS 3S DE ESPERA, VERIFICA SE O SENTIDO FOI INVERTIDO. SE NÃO FOI LIGA IMEDIATAMENTE
	MOV C, P2.6					;copia bit sentido de rotacao para C
	MOV ACC.0, C					;copia bit sentido para ACC.0
	XRL a, R7					;faz xor com R7 e verifica se foi invertido o sentido
	MOV C, ACC.0					;copia resultado do xor para C (se foi invertido C=1)
	JC CONTINUE_TIMER_COUNTER			;se o sentido foi invertido, continua a contagem de 3s
	LJMP WAIT3S_STOP				;se o sentido não foi invertido encerra a espera de 3s
	
CW_READY:						;ESTADO DO MOTOR FUNCIONANDO NA CONFIGURACAO DELTA ATE QUE SEJA DESLIGADO
	JB P2.7, SHUTDOWN
	JNB P2.6, INVERT
	SJMP CW_READY
CCW_READY:						;ESTADO DO MOTOR FUNCIONANDO NA CONFIGURACAO DELTA ATE QUE SEJA DESLIGADO
	JB P2.7, SHUTDOWN
	JB P2.6, INVERT
	SJMP CCW_READY

SHUTDOWN:						;DESLIGA TODOS OS LEDS
	SETB P1.7
	SETB P1.6
	SETB P1.5
	SETB P1.4
	SJMP WAIT3S

INVERT:
	SJMP SHUTDOWN

WAIT3S:
	MOV R0, #03Ch					;Configura R0 para 60d (3s)
	CLR P1.0					;sinaliza espera de 3s
	SJMP TIMER_COUNTER

WAIT3S_STOP:
	SETB P1.0					;sinaliza espera 3s acabou
	LJMP IDLE
	

	
	


	

