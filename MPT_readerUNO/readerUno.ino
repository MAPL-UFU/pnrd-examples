#include <Pn532NfcReader.h>
#include <PN532_HSU.h>
#include <PN532.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <NfcAdapter.h>

          
// MPT_readerUno/readerUno.ino

uint32_t tagId3 = 0xFF;
uint32_t readerId = 0x02;
uint16_t * tokenDesejado;
bool tagReadyToContinue = false;


// --------------------------Iniciando as comunicações HSU -----------------------------------

//Rotines related with the configuration of the RFID reader PN532 
PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc3 = NfcAdapter(pn532spi);     
//Creation of the reader and PNRD objects
Pn532NfcReader* reader3 = new Pn532NfcReader(&nfc3);
Pnrd pnrd3 = Pnrd(reader3,4,4,false,false,true);
// -----------------------------------------------------------------------

void setup() {  
  //Initialization of the communication with the reader and with the computer Serial bus 
  Serial.begin(9600);    
  reader3->initialize();      

  pnrd3.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);
  pnrd3.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);
  pnrd3.setAsTagInformation(PetriNetInformation::GOAL_TOKEN);

  // pnrd3.getGoalToken(tokenDesejado);
}
void serial_com(uint32_t tagId, uint32_t readerId, int antenna, char ErrorType[100] ){
    Serial.println(String("id:") + String(tagId,HEX) + String(" | ant:") + String(antenna) + String(" | readerId:") + String(readerId,HEX) + String(" | pnrd:")+ String(ErrorType));
    Serial.println("end");

}
void loop() {
  delay(100);

  ReadError readError3 = pnrd3.getData();
  delay(100);
  // switch(readError3){
  //   case ReadError::NO_ERROR:
  //      Serial.println("Sem erro");
  //   case ReadError::TAG_NOT_PRESENT:
  //       Serial.println("Sem Tag");
  //   case ReadError::INFORMATION_NOT_PRESENT:
  //     Serial.println("INFORMATION_NOT_PRESENT");
  //   case ReadError::NOT_AUTORIZED:
  //     Serial.println("NOT_AUTORIZED");
  //       case ReadError::NOT_AUTORIZED:
  //     Serial.println("NOT_AUTORIZED");
  // }
  if(readError3 == ReadError::NO_ERROR){
    FireError fireError3;
    Serial.println("Sem erro");
    
    // pnrd3.printTokenVector();

    //verifica se é uma nova tag
      if(tagId3 != pnrd3.getTagId()){
          tagId3 = pnrd3.getTagId();
          //Dispara a transição t3
          FireError fireError3 = pnrd3.fire(0);

          switch (fireError3){
              case FireError::NO_ERROR :
                //Atualizando a tag
                if(pnrd3.saveData() == WriteError::NO_ERROR){
                      pnrd3.printGoalToken();
                      serial_com(tagId3,readerId,6,"NO_ERROR");
                      return;
                }else{
                      serial_com(tagId3,readerId,6,"ERROR_TAG_UPDATE");
                      return;
                }

              case FireError::PRODUCE_EXCEPTION :
                  serial_com(tagId3,readerId,6,"PRODUCE_EXCEPTION");
              return;

              case FireError::CONDITIONS_ARE_NOT_APPLIED :
                  serial_com(tagId3,readerId,6,"CONDITIONS_ARE_NOT_APPLIED");
              break;

              case FireError::NOT_KNOWN:
                serial_com(tagId3,readerId,6,"NOT_KNOWN");
              break; 
          }  
      }
  }
  Serial.flush();           
}