/** ####  PROJETO 2 ############################################
**
**     Filename  : Pong_1D.ino
**     Project   : Projeto_02-LAB. de APLIC. COM MICROCOMPUTADORES
**     Professor : André Borges Cavalcante
**     Processor : Atmega328P
**     Version   : 1.00
**     Compiler  : Arduino IDE
**     Date/Time : 24/05/2018, 09:24
**     Aluno     : Antonio Gabriel Sousa Borralho
**     Last Mod  : 28/06/2018 (DIP-SW OK)
**
** ###########################################################*/


#define NOTE_C6  1047 //Define a frencia para o Buzzer

//#### VARIAVEIS GLOBAIS #######################################
unsigned long tempoAtual = 0;
unsigned long tempoAnterior = 0;

//500 mili segundos para que um led mude de estado
const unsigned long inicialTempoPorLED = 500;
//Valor decrementado para aumento da velocidade 
const unsigned long inicialDeltaTempoPorLED = 50; 
unsigned long TempoPorLED = inicialTempoPorLED;
unsigned long deltaTempoPorLED = inicialDeltaTempoPorLED;
//Simboliza o zero do valor digital de DIPSW 
int zero = 100; 

int EstadoAtualBotaoJogador1 = 1;
int EstadoAnteriorBotaoJogador1 = 1;
int EstadoAtualBotaoJogador2 = 1;
int EstadoAnteriorBotaoJogador2 = 1;
int Buzzer=0; //Buzzer ligado a porta 0
int Porta;


//DIP SWITCH
int bit1=0; //Bit 1 do DIP-SW
int bit2=0;
int bit3=0;
int bit4=0;

const int Jogador1 = 12; //Led amarelo do Jogador 1
const int golJogador1 = 13; 
/*Se o led vermelho(Porta 13) acender marca ponto para 
adversário significa que a bola passou do led amarelo*/
const int BotaoJogador1 = 3;

const int Jogador2 = 5; //Led amarelo do Jogador 2
const int golJogador2 = 4; 
/*Se o led vermelho(Porta 4) acender marca ponto para 
adversário significa que a bola passou do led amarelo*/
const int BotaoJogador2 = 2;

int PosicaoAtual = Jogador1; //Jogador 1 inicia o jogo.
int PosicaoAnterior = Jogador1 + 1;
int deltaPosicao = 0; 
//Indica qual o led anterior a ser apagado em PosicaoAnterior.

int scoreJogador1 = 0;
int scoreJogador2 = 0;

//Sentido Convencional é do Jogador 1 para o 2
bool SentidoContrario = true; 
bool EntradaPermitida = true;

//#### PROTOTIPOS DAS FUNCOES ##################################
void VerificarEntradas();
void MudaDirecaoDaBola();
void DeterminaProximaPosicao();
void MoveBola();
void ChecaGols();
void PontuacaoParaJogador(int pontuador);
void ChecaFimDoJogo();
void MostraScores(int Pontuador);
void ResetaJogada();
void FimDoJogo(int Vencedor);
void ExibeVencedor(int Jogador);
void InibeJogador(int Jogador);
void PiscaLEDs(int n_vezes, int Jogador);
void BemVindo(int n_vezes);
void LerDIPSW();
void tempoDIPSW();

//#### LOOP E SETUP ############################################
void setup() {
  pinMode(Buzzer,OUTPUT);
  //Configura Portas 4 a 13 como SAÍDAS (LEDS)
  for(Porta=4;Porta<=13;Porta++){
     pinMode(Porta, OUTPUT);
  }
  //Configura Portas 2 e 3 como ENTRADAS (botoes)
  pinMode(2, INPUT_PULLUP); //(Resistor PullUp ativado)
  pinMode(3, INPUT_PULLUP);
  //Cerimônia de StartGame
  BemVindo(30);
  //Aguarda Jogador 1 iniciar
  digitalWrite(Jogador1, HIGH);
  while(digitalRead(BotaoJogador1)==1){}
  delay(500); //Remove buffer do teclado
}

void loop(){
  VerificarEntradas();
  tempoAtual = millis();
  if (tempoAtual - tempoAnterior >= TempoPorLED){
    LerDIPSW();
    tempoDIPSW();
    
    ChecaGols();
    DeterminaProximaPosicao();
    MoveBola();

    tempoAnterior = tempoAtual;
  }
}
//#### FUNCOES ##################################################
void VerificarEntradas(){
  LerDIPSW();    
  EstadoAtualBotaoJogador1 = digitalRead(BotaoJogador1);
  EstadoAtualBotaoJogador2 = digitalRead(BotaoJogador2);
  if (EstadoAtualBotaoJogador1 != EstadoAnteriorBotaoJogador1 && EntradaPermitida){
    if (EstadoAtualBotaoJogador1 == 1){
      if (PosicaoAtual == Jogador1){
        MudaDirecaoDaBola();
        AumentaVelocidade();       
      }else{
        PontuacaoParaJogador(2);
      }
    }
    EstadoAnteriorBotaoJogador1 = EstadoAtualBotaoJogador1;
  }
  if (EstadoAtualBotaoJogador2 != EstadoAnteriorBotaoJogador2 && EntradaPermitida){
    if (EstadoAtualBotaoJogador2 == 1){
      if (PosicaoAtual == Jogador2){
        MudaDirecaoDaBola();
        AumentaVelocidade();
      }else{
        PontuacaoParaJogador(1);
      }
    }
    EstadoAnteriorBotaoJogador2 = EstadoAtualBotaoJogador2;
  }
}
//_______________________________________________________________
void MudaDirecaoDaBola(){
  /*Muda a direcao convencional (de Jogador1 para Jogador2) para 
    o SentidoContrario contrário*/
  SentidoContrario = !SentidoContrario; 
  //Apenas uma mudança de direção por jogada é permitida 
  //para uma jogabilidade consistente
  EntradaPermitida = false;   
}
//_______________________________________________________________
void AumentaVelocidade(){
    TempoPorLED -= deltaTempoPorLED;
    if (deltaTempoPorLED > 1){
    /*Quanto menor este valor mais rápido a valocidade aumenta.
      Isto impede que atinja uma velocidade insana rapidamente. 
      Ajuste ou remova isto se as rodadas ficarem muito longas.*/
      deltaTempoPorLED -= 1; 
    //}
  }  
}
//_______________________________________________________________
void MoveBola(){      //Move a bola uma posição
  PosicaoAnterior = PosicaoAtual;
  digitalWrite(PosicaoAnterior, 0);
  PosicaoAtual = PosicaoAtual + deltaPosicao;
  digitalWrite(PosicaoAtual, 1);
  EntradaPermitida = true;
}
//_______________________________________________________________
void DeterminaProximaPosicao(){
  //Se a direção estiver no SentidoContrario convencional
  if (SentidoContrario){ 
    deltaPosicao = -1; 
  }else{
    deltaPosicao = 1;
  }
}
//_______________________________________________________________
void ChecaGols(){
  if (PosicaoAtual == golJogador2){
    PontuacaoParaJogador(1);
  }else if (PosicaoAtual == golJogador1){
    PontuacaoParaJogador(2);
  }
}
//_______________________________________________________________
void PontuacaoParaJogador(int pontuador){
  EntradaPermitida = false;
  PiscaLEDs(1, 0);
  if (pontuador == 1){
    scoreJogador1++;
    MostraScores(1);
  }else if (pontuador == 2){
    scoreJogador2++;
    MostraScores(2);
  }
  ChecaFimDoJogo();
}
//_______________________________________________________________
void ChecaFimDoJogo(){
  if (scoreJogador1 == 3){
    FimDoJogo(1);
  }
  if (scoreJogador2 == 3){
    FimDoJogo(2);
  }
}
//_______________________________________________________________
void MostraScores(int Pontuador){

  if (Pontuador == 1){
    digitalWrite(Jogador1, 1);
    PosicaoAtual = Jogador1;
    //Habilita a direção para o adversário
    SentidoContrario = true; 
  }else if (Pontuador == 2){
    digitalWrite(Jogador2, 1);
    PosicaoAtual = Jogador2;
    //Habilita o SentidoContrario convencional.
    SentidoContrario = false; 
  }
  /*Usamos os seis LEDs no meio para mostrar a pontuação. 
  Cada Jogador tem tres LEDs verdes para mostrar a pontuacao. 
  Com tres gols vence o jogo.*/
  for (int i = 0; i < scoreJogador1; i++){   
    digitalWrite((11 - i), 1);
  }
  for (int i = 0; i < scoreJogador2; i++){
    digitalWrite((6 + i), 1);
  }
  //Tres segundos para que os Jogadors vizualizem a pontuacao
  delay(3000);                

  InibeJogador(1);
  InibeJogador(2);
  
  if (Pontuador == 1){
    ChecaFimDoJogo();
    digitalWrite(Jogador1, HIGH);
    while(digitalRead(BotaoJogador1)==1){}
    delay(800);
  }else if (Pontuador == 2){
    ChecaFimDoJogo();
    digitalWrite(Jogador2, HIGH);
    while(digitalRead(BotaoJogador2)==1){}
    delay(800);
  }  
  ResetaJogada();
}
//_______________________________________________________________
void ResetaJogada(){
  //Velocidade para o valor inicial de cada rodada
  TempoPorLED = inicialTempoPorLED;
  //Velocidade delta para o valor inicial de cada rodada            
  deltaTempoPorLED = inicialDeltaTempoPorLED;  
}
//_______________________________________________________________
void FimDoJogo(int Vencedor){
  PiscaLEDs(10, Vencedor);
  if(Vencedor==1)
    InibeJogador(2);
  else
    InibeJogador(1);
  scoreJogador1 = 0;
  scoreJogador2 = 0;
}
//_______________________________________________________________
void ExibeVencedor(int Jogador){
  if (Jogador != 1){
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
  }
  if (Jogador != 2){
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(13, HIGH);
  }
}
//_______________________________________________________________
void InibeJogador(int Jogador){
  if (Jogador != 1){
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
  }
  if (Jogador != 2){
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
  }
}
//_______________________________________________________________
void PiscaLEDs(int n_vezes, int Jogador){
  for (int i = 0; i < n_vezes; i++){
    ExibeVencedor(Jogador);
    tone(Buzzer, NOTE_C6);
    delay(50);
    InibeJogador(Jogador);
    noTone(Buzzer);
    delay(50);
  }
  InibeJogador(1);
  InibeJogador(2);
}
//_______________________________________________________________
void BemVindo(int Count){
  for (int i=0; i < Count; i++){
    ExibeVencedor(1);
    ExibeVencedor(2);
    tone(Buzzer, i*exp(-i/10000));//Oscilacao para Buzzer
    delay(35);
    InibeJogador(1);
    InibeJogador(2);
    noTone(Buzzer);
    delay(35);    
  }  
  InibeJogador(1);
  InibeJogador(2);
}
//_______________________________________________________________
void LerDIPSW(){
  bit1 = analogRead(1);
  bit2 = analogRead(2);
  bit3 = analogRead(3);
  bit4 = analogRead(4);
}
//_______________________________________________________________
void tempoDIPSW(){
  //0001
  if ((bit1 < zero)&&(bit2 < zero)&&(bit3 < zero)&&(bit4 > zero)){
      TempoPorLED=inicialTempoPorLED;
      deltaTempoPorLED = inicialDeltaTempoPorLED; 
  }
  //0010
  if ((bit1 < zero)&&(bit2 < zero)&&(bit3 > zero)&&(bit4 < zero)){
      TempoPorLED=inicialTempoPorLED-(inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-1;
  }
  //0011
  if ((bit1 < zero)&&(bit2 < zero)&&(bit3 > zero)&&(bit4 > zero)){
      TempoPorLED=inicialTempoPorLED-(2*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-2; 
  }

  //0100
  if ((bit1 < zero)&&(bit2 > zero)&&(bit3 < zero)&&(bit4 < zero)){
      TempoPorLED=inicialTempoPorLED-(3*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-3; 
  }
  //0101
  if ((bit1 < zero)&&(bit2 > zero)&&(bit3 < zero)&&(bit4 > zero)){
      TempoPorLED=inicialTempoPorLED-(4*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-4; 
  }

  //0110
  if ((bit1 < zero)&&(bit2 > zero)&&(bit3 > zero)&&(bit4 < zero)){
      TempoPorLED=inicialTempoPorLED-(5*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-5; 
  }

  //0111
  if ((bit1 < zero)&&(bit2 > zero)&&(bit3 > zero)&&(bit4 > zero)){
      TempoPorLED=inicialTempoPorLED-(6*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-6; 
  }

  //1000
  if ((bit1 > zero)&&(bit2 < zero)&&(bit3 < zero)&&(bit4 < zero)){
      TempoPorLED=inicialTempoPorLED-(7*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-7; 
  }

  //1001
  if ((bit1 > zero)&&(bit2 < zero)&&(bit3 < zero)&&(bit4 > zero)){
      TempoPorLED=inicialTempoPorLED-(8*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-8; 
  }

  //1010
  if ((bit1 > zero)&&(bit2 < zero)&&(bit3 > zero)&&(bit4 < zero)){
      TempoPorLED=inicialTempoPorLED-(9*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-9; 
  }

  //1011
  if ((bit1 > zero)&&(bit2 < zero)&&(bit3 > zero)&&(bit4 > zero)){
      TempoPorLED=inicialTempoPorLED-(10*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-10; 
  }

  //1100
  if ((bit1 > zero)&&(bit2 > zero)&&(bit3 < zero)&&(bit4 < zero)){
      TempoPorLED=inicialTempoPorLED-(11*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-11; 
  }

  //1101
  if ((bit1 > zero)&&(bit2 > zero)&&(bit3 < zero)&&(bit4 > zero)){
      TempoPorLED=inicialTempoPorLED-(12*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-12; 
  }

  //1110
  if ((bit1 > zero)&&(bit2 > zero)&&(bit3 > zero)&&(bit4 < zero)){
      TempoPorLED=inicialTempoPorLED-(13*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-13; 
  }

  //1111
  if ((bit1 > zero)&&(bit2 > zero)&&(bit3 > zero)&&(bit4 > zero)){
      TempoPorLED=inicialTempoPorLED-(14*inicialTempoPorLED/15);
      deltaTempoPorLED = inicialDeltaTempoPorLED-14; 
  }
}
//#### FIM ######################################################

