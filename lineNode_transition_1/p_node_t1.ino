#include <Pn532NfcReader.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

//programa/lineNode_transition_1/p_node_t1.ino


uint16_t fire_vector[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t ifTypeA[] = {1, 0, 0, 0, 0, 0, 0, 0};
uint16_t ifNotTypeA[] = {0, 0, 1, 0, 0, 0, 0, 0};
uint16_t ifApproved[] = {0, 0, 0, 0, 1, 0, 0, 0};
uint16_t ifNotApproved[] = {0, 0, 0, 0, 0, 0, 1, 0};
uint16_t mFireVector[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint32_t tagId = 0xFF;
uint32_t readerId = 0xFF;
String readString;
String flagType = "EMPTY";
const int buttonPass = 4;
int ipnrdTrans;

// Setup PNRD and/or IPNRD TAG 
PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
Pn532NfcReader *reader = new Pn532NfcReader(&nfc);
Pnrd pnrd = Pnrd(reader, 5, 8, true, true);
//--------------------------------------------------

void setup()
{
  Serial.begin(9600); // initialize serial communications at 9600 bps
  reader->initialize();
  pinMode(buttonPass, INPUT); //Signal for transition trigger
}

void loop()
{
  if (Serial.available())
  {
    if (Serial.available() > 0)
    {
      char c = Serial.read();
      Serial.flush();
      readString = c;
      if (readString.length() > 0)
      {
        if (readString == "P")
        {
          flagType = "PNRD";
          Serial.println("PNRD");
          pnrd.resetAsTagInformation();
          pnrd.setAsTagInformation(PetriNetInformation::TOKEN_VECTOR);
          pnrd.setAsTagInformation(PetriNetInformation::ADJACENCY_LIST);
        }
        else if (readString == "I")
        {
          Serial.println("IPNRD");
          flagType = "IPNRD";
          for (int i = 0; i < 8; i++)
          { 
            if (ipnrdTrans == 0){
              mFireVector[i] = ifTypeA[i];}
            else if (ipnrdTrans == 2){
              mFireVector[i] = ifNotTypeA[i];}
            else if (ipnrdTrans == 4){
              mFireVector[i] = ifApproved[i];}
            else if (ipnrdTrans == 6){
              mFireVector[i] = ifNotApproved[i];}
            else{
              mFireVector[i] = fire_vector[i];}
          }
          pnrd.resetAsTagInformation();
          pnrd.setFireVector(mFireVector);
          pnrd.setAsTagInformation(PetriNetInformation::FIRE_VECTOR);
          pnrd.setAsTagInformation(PetriNetInformation::TAG_HISTORY);
        }
      }
    }
  }
  else if (flagType != "EMPTY")
  {
    delay(1000);
    if (flagType == "IPNRD")
    {
      if (pnrd.saveData() == WriteError::NO_ERROR)
      {
        tagId = pnrd.getTagId();
        Serial.println("IPNRD Tag CONFIGURED.");
        Serial.println("end");
        delay(1000);
        return;
      }
    }
    else if (flagType == "PNRD")
    {
      pnrd.conditionUpdate(1, digitalRead(buttonPass));
      ReadError readError = pnrd.getData();
      switch (readError)
      {
      case ReadError::NO_ERROR:
        FireError fireError;
        if (tagId != pnrd.getTagId())
        { ipnrdTrans =999; //reseta a indicação da ipnrdTransição
          tagId = pnrd.getTagId();
          FireError fireError = pnrd.fire(1);

          switch (fireError)
          {
          case FireError::NO_ERROR:

            //Salvar a nova informação na etiqueta
            if (pnrd.saveData() == WriteError::NO_ERROR)
            { ipnrdTrans = 4;
              Serial.println(String("id:") + String(tagId) + String(" | readerId:") + String(readerId) + String(" | pnrd:NO_ERROR"));
              pnrd.printTagHistory();
              pnrd.printTokenVector();
              Serial.println("end");
              return;
            }
            else
            {
              Serial.println(String("id:") + String(tagId) + String(" | readerId:") + String(readerId) + String(" | pnrd:ERROR_TAG_UPDATE"));
              Serial.println("end");
              return;
            }
          case FireError::PRODUCE_EXCEPTION:
            Serial.println(String("id:") + String(tagId) + String(" | readerId:") + String(readerId) + String(" | pnrd:PRODUCE_EXCEPTION"));
            pnrd.printTagHistory();
            Serial.println("end");
            break;
          case FireError::CONDITIONS_ARE_NOT_APPLIED:
            ipnrdTrans = 6;
            Serial.println(String("id:") + String(tagId) + String(" | readerId:") + String(readerId) + String(" | pnrd:CONDITIONS_ARE_NOT_APPLIED"));
            Serial.println("end");
            break;
          case FireError::NOT_KNOWN:
            Serial.println(String("id:") + String(tagId) + String(" | readerId:") + String(readerId) + String(" | pnrd:NOT_KNOWN"));
            Serial.println("end");
            break;
          }
        }
      
      }
      delay(1000);
    }
  }
}