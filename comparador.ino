/**
   --------------------------------------------------------------------------------------------------------------------
   Example sketch/program showing how to read data from more than one PICC to serial.
   --------------------------------------------------------------------------------------------------------------------
   This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
   Example sketch/program showing how to read data from more than one PICC (that is: a RFID Tag or Card) using a
   MFRC522 based RFID Reader on the Arduino SPI interface.
   Warning: This may not work! Multiple devices at one SPI are difficult and cause many trouble! Engineering skill
            and knowledge are required!
   @license Released into the public domain.
   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS 1    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required *
   SPI SS 2    SDA(SS)      ** custom, take a unused pin, only HIGH/LOW required *
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/


//Inclusão de bibliotecas utilizadas no projeto.
#include <SPI.h>
#include <MFRC522.h>

// Número dos Pins utilizados: RESET + SDAs das Leitoras; 
#define RST_PIN         9
#define SS_1_PIN        10
#define SS_2_PIN        8
#define SS_3_PIN        7
#define SS_4_PIN        6


// Pinos dos Componentes Adjascentes.
#define GreenLed        2
#define relayIN         3

// Definição do número total de leitoras. 
#define NR_OF_READERS 8


// Lista de letras que são esperadas na inserção
byte letrasEsperadas [8][4] = {
  {0x4B, 0x17, 0xBC, 0x79},
  {0x8A, 0x2B, 0xBC, 0x79}, 
  {0x81, 0x29, 0xBC, 0x79},
  {0x8A, 0x2B, 0xBC, 0x79},
  {0x8A, 0x2B, 0xBC, 0x79},
  {0x8A, 0x2B, 0xBC, 0x79},
  {0x8A, 0x2B, 0xBC, 0x79},
  {0x8A, 0x2B, 0xBC, 0x79},
};


byte letraAux [4]; 

// Lista de letras que foram inseridas pelo usuário
  //Linhas - Colunas
byte letrasInseridas [8][4] = {
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0}
};

uint32_t vetorTransformado [8] = {0,0,0,0,0,0,0,0};

uint32_t vetorInt [8];

byte tagarray[][4] = {
  {0x4B, 0x17, 0xBC, 0x79},
  {0x8A, 0x2B, 0xBC, 0x79}, 
  {0x81, 0x29, 0xBC, 0x79},
  {0xE6, 0xDF, 0xBB, 0x79},
};

//Vetor auxiliar utilizado nas comparações.
uint8_t vetorDiff[8] = {0,0,0,0,0,0,0,0};

//Vetor auxiliar utilizado para saber quais leitoras serão utilizadas para a palavra proposta.
uint8_t vetorUsadas[8] = {0,0,0,0,0,0,0,0};


// Inlocking status:
int tagcount = 0;
bool access = false;


// Vetor com os pinos que são utilizadas como informação pelas leitoras.
byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN, SS_4_PIN};

// Cria instância MFRC522:
MFRC522 mfrc522[NR_OF_READERS];

/**
   Inicialização.
*/
void setup() {

  Serial.begin(9600);           // Inicialização da comunicação Serial entre a placa e o computador.
  while (!Serial);              // Não fazer nada caso a Serial não seja inicializada.

  SPI.begin();                  // Inicialização da comunicação SPI.

  /* Inicialização de Inputs e Outputs. */
  pinMode(GreenLed, OUTPUT);
  digitalWrite(GreenLed, LOW);
  pinMode(relayIN, OUTPUT);
  digitalWrite(relayIN, HIGH);
  //pinMode(RedLed, OUTPUT);
  //digitalWrite(RedLed, LOW);


  /* Procura por leitoras MFRC522 */
  //uint8_t = Inteiro sem sinal, vai de 0  até 255. Tamanho de 8 bits = 1 byte.
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
    delay(4); //Delay para as placas ficarem prontas. Inserido seguindo recomendação na documentação do GH.
    Serial.print(F("Reader "));
    Serial.print(reader);
    Serial.print(F(": "));
    mfrc522[reader].PCD_DumpVersionToSerial();
    mfrc522[reader].PCD_SetAntennaGain(mfrc522[reader].RxGain_max);
  }
}

/*
   Main loop.
*/

void loop() {

for(int i = 0; i < 8; i++){
  for(int j = 0; j < 4 ; j++){
  letraAux [j] = letrasEsperadas [i][j];
  vetorTransformado [i] = ByteToInt(letraAux);
  }
}

for(int i = 0; i < 8; i++){
  Serial.print("Retorno de posição ");
  Serial.println(i);
  Serial.println(vetorTransformado [i]);
}
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {

    // Looking for new cards
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader "));
      Serial.print(reader);

      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(": Card UID:"));
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println();


      for (int x = 0; x < sizeof(tagarray); x++)                  // tagarray's row
      {
        for (int i = 0; i < mfrc522[reader].uid.size; i++)        //tagarray's columns
        {
          if ( mfrc522[reader].uid.uidByte[i] != tagarray[x][i])  //Comparing the UID in the buffer to the UID in the tag array.
          {
            DenyingTag();
            break;
          }
          else
          {
            if (i == mfrc522[reader].uid.size - 1)                // Test if we browesed the whole UID.
            {
              AllowTag();
            }
            else
            {
              continue;                                           // We still didn't reach the last cell/column : continue testing!
            }
          }
        }
        if (access) break;                                        // If the Tag is allowed, quit the test.
      }


      if (access)
      {
        if (tagcount == NR_OF_READERS)
        {
          OpenDoor();
        }
        else
        {
          MoreTagsNeeded();
        }
      }
      else
      {
        UnknownTag();
      }
      /*Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
        Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));*/
      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC..
  } //for(uint8_t reader..
}

/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte * buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printTagcount() {
  Serial.print("Tag n°");
  Serial.println(tagcount);
}

void DenyingTag()
{
  tagcount = tagcount;
  access = false;
}

void AllowTag()
{
  tagcount = tagcount + 1;
  access = true;
}

void Initialize()
{
  tagcount = 0;
  access = false;
}

void OpenDoor()
{
  Serial.println("Welcome! the door is now open");
  Initialize();
  digitalWrite(relayIN, LOW);
  digitalWrite(GreenLed, HIGH);
  delay(2000);
  digitalWrite(relayIN, HIGH);
  delay(500);
  digitalWrite(GreenLed, LOW);
}

void MoreTagsNeeded()
{
  printTagcount();
  Serial.println("System needs more cards");
  //digitalWrite(RedLed, HIGH);
  delay(1000);
  //digitalWrite(RedLed, LOW);
  access = false;
}

void UnknownTag()
{
  Serial.println("This Tag isn't allowed!");
  printTagcount();
  for (int flash = 0; flash < 5; flash++)
  {
    //digitalWrite(RedLed, HIGH);
    delay(100);
   // digitalWrite(RedLed, LOW);
    delay(100);
  }
}

//void LetterCompare(byte *letrasEsperadas, byte *letrasInseridas, uint8_t posicaoCorrente){
  //for(i=0; i < NR_OF_READERS; i++){
    //for(j=0; j < 4; j++){
    //if(letrasEsperadas[i][j] != letrasInseridas[i][j]){

    //}else{

    //}

  // }
 // }
//}


uint32_t ByteToInt(byte *letra){
  uint32_t tagID;
  //tagId = (rfidReader1.uid.uidByte[3]) | (rfidReader1.uid.uidByte[2] << 8) |( rfidReader1.uid.uidByte[1]<< 16) | (rfidReader1.uid.uidByte[0] << 24);
  tagID = (letra[0] | letra[1]<<8 | letra[2]<<16 | letra[3]<<24);
  //Serial.println(tagID);

  return tagID; 
}




