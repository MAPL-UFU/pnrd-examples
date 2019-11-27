#include <Pn532NfcReader.h>
#include <PN532_HSU.h>
#include <PN532.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <NfcAdapter.h>

          
// MPT_reader2/reader2.ino

uint32_t tagId1 = 0xFF;
uint32_t tagId2 = 0xFF;
uint32_t tagId3 = 0xFF;
uint32_t readerId = 0x02;
bool tagReadyToContinue = false;

// --------------------------Iniciando as comunicações HSU -----------------------------------

//Rotines related with the configuration of the RFID reader PN532      
PN532_HSU pn532hsu(Serial1);
NfcAdapter nfc1 = NfcAdapter(pn532hsu);

//Creation of the reader and PNRD objects
Pn532NfcReader* reader1 = new Pn532NfcReader(&nfc1);
Pnrd pnrd1 = Pnrd(reader1,9,9,false,true);


//Rotines related with the configuration of the RFID reader PN532      
PN532_HSU pn532hsu2(Serial2);
NfcAdapter nfc2 = NfcAdapter(pn532hsu2);
//Creation of the reader and PNRD objects
Pn532NfcReader* reader2 = new Pn532NfcReader(&nfc2);
Pnrd pnrd2 = Pnrd(reader2,9,9,false,true);

//Rotines related with the configuration of the RFID reader PN532 
PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc3 = NfcAdapter(pn532spi);     
//Creation of the reader and PNRD objects
Pn532NfcReader* reader3 = new Pn532NfcReader(&nfc3);
Pnrd pnrd3 = Pnrd(reader3,9,9,false,true);
// -----------------------------------------------------------------------

void setup() {  
  //Initialization of the communication with the reader and with the computer Serial bus 
  Serial.begin(9600); 
  reader1->initialize();    
  reader2->initialize();     
  reader3->initialize();      

  //Setting of the classic PNRD approach  
  pnrd1.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);
  pnrd1.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);

  pnrd2.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);
  pnrd2.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);

  pnrd3.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);
  pnrd3.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);


  
  Serial.println("\nStart");
}
void serial_com(uint32_t tagId, uint32_t readerId, int antenna, char ErrorType[100] ){
    Serial.println(String("id:") + String(tagId,HEX) + String(" | ant:") + String(antenna) + String(" | readerId:") + String(readerId,HEX) + String(" | pnrd:")+ String(ErrorType));
    Serial.println("end");

}
void loop() {
  delay(100);

// Leitura da tag
  ReadError readError1 = pnrd1.getData();
  delay(100);
  ReadError readError2 = pnrd2.getData();
  delay(100);
  ReadError readError3 = pnrd3.getData();
  delay(100);
  //Se a leitura foi bem sucedida...
  if(readError1 == ReadError::NO_ERROR){
    FireError fireError1;
      // Verifica se é uma nova etiqueta
      if(tagId1 != pnrd1.getTagId()){
        tagId1 = pnrd1.getTagId();
        tagReadyToContinue = false;
        //Realização do disparo da transição T0
        FireError fireError1 = pnrd1.fire(3);

        switch(fireError1){
            case FireError::NO_ERROR :

              //Atualiza a tag
              if(pnrd1.saveData() == WriteError::NO_ERROR){
                serial_com(tagId1,readerId,4,"NO_ERROR");
                return;
              }else{
                serial_com(tagId1,readerId,4,"ERROR_TAG_UPDATE");
                return;
              }
              return;

            case FireError::PRODUCE_EXCEPTION :
                serial_com(tagId1,readerId,4,"PRODUCE_EXCEPTION");
                break;

            case FireError::CONDITIONS_ARE_NOT_APPLIED :
                serial_com(tagId1,readerId,4,"CONDITIONS_ARE_NOT_APPLIED");
            break;

            case FireError::NOT_KNOWN:
                serial_com(tagId1,readerId,4,"NOT_KNOWN");
            break;             
        }
      }
  }
  if(readError2 == ReadError::NO_ERROR){
    FireError fireError2;
      // Verifica se é uma nova etiqueta
      if(tagId2 != pnrd2.getTagId()){
        tagId2 = pnrd2.getTagId();
        tagReadyToContinue = false;
        //Realização do disparo da transição T0
        FireError fireError2 = pnrd2.fire(4);
        switch(fireError2){
            case FireError::NO_ERROR :
              
              //Atualiza a tag
              if(pnrd2.saveData() == WriteError::NO_ERROR){
                serial_com(tagId2,readerId,5,"NO_ERROR");
                return;
              }else{
                serial_com(tagId2,readerId,5,"ERROR_TAG_UPDATE");
                return;
              }
              return;
            case FireError::PRODUCE_EXCEPTION :
                serial_com(tagId2,readerId,5,"PRODUCE_EXCEPTION");
                break;

            case FireError::CONDITIONS_ARE_NOT_APPLIED :
                serial_com(tagId2,readerId,5,"CONDITIONS_ARE_NOT_APPLIED");
            break;
            case FireError::NOT_KNOWN:
                serial_com(tagId2,readerId,5,"NOT_KNOWN");
            break;         
        }
      }
  } 
  if(readError3 == ReadError::NO_ERROR){
    FireError fireError3;

    //verifica se é uma nova tag
      if(tagId3 != pnrd3.getTagId()){
          tagId3 = pnrd3.getTagId();
          //Dispara a transição t3
          FireError fireError3 = pnrd3.fire(5);

          switch (fireError3){
              case FireError::NO_ERROR :
                //Atualizando a tag
                if(pnrd3.saveData() == WriteError::NO_ERROR){
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
}