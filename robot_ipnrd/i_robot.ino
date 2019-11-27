#include <Pn532NfcReader.h>
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include "string.h"
//programa/robot_ipnrd/i_robot.ino
int8_t mIncidenceMatrix[] = {-1, 1, -1, 1, -1, 1, -1, 1,
                             1, -1, 0, 0, 0, 0, 0, 0,
                             0, 0, 1, -1, 0, 0, 0, 0,
                             0, 0, 0, 0, 1, -1, 0, 0,
                             0, 0, 0, 0, 0, 0, 1, -1};

uint16_t mStartingTokenVector[] = {1, 0, 0, 0, 0, 0};
uint16_t fire_vector[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint16_t ifTypeA[] = {1, 0, 0, 0, 0, 0, 0, 0};
uint16_t ifNotTypeA[] = {0, 0, 1, 0, 0, 0, 0, 0};
uint16_t ifApproved[] = {0, 0, 0, 0, 1, 0, 0, 0};
uint16_t ifNotApproved[] = {0, 0, 0, 0, 0, 0, 1, 0};
//Rotines related with the configuration of the RFID reader PN532
PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);

//Creation of the reader and PNRD objects
Pn532NfcReader *reader = new Pn532NfcReader(&nfc);
Pnrd pnrd = Pnrd(reader, 5, 8, true, true);

uint32_t tagId = 0xFF;

void setup()
{
  //Initialization of the communication with the reader and with the computer Serial bus
  Serial.begin(9600);
  reader->initialize();

  pnrd.setTokenVector(mStartingTokenVector);
  pnrd.setIncidenceMatrix(mIncidenceMatrix);

  pnrd.setAsTagInformation(PetriNetInformation::FIRE_VECTOR);
  pnrd.setAsTagInformation(PetriNetInformation::TAG_HISTORY);
  pnrd.setDeviceId(1);
}

void loop()
{
  delay(1000);

  //Tag reading
  ReadError readError = pnrd.getData();
  //In case of a successful reading
  if (readError == ReadError::NO_ERROR)
  {
    FireError fireError;
    //Checks if it's a new tag
    if (tagId != pnrd.getTagId())
    {
      tagId = pnrd.getTagId();
      FireError fireError = pnrd.fire();

      switch (fireError)
      {
      case FireError::NO_ERROR:

        //Salvar a nova informação na etiqueta
        if (pnrd.saveData() == WriteError::NO_ERROR)
        {
          pnrd.getFireVector(fire_vector);
          if (memcmp((const void *)fire_vector, (const void *)ifTypeA, sizeof(fire_vector)) == 0)
          {flagX1 = 1
            Serial.println(0b001); //Robo vai para (1,1,1)
            Serial.println("end");
          }
          else if (memcmp((const void *)fire_vector, (const void *)ifNotTypeA, sizeof(fire_vector)) == 0)
          {
            Serial.println(0b010); //Robo vai para (2,2,2)
            Serial.println("end");
          }
 
          else if (memcmp((const void *)fire_vector, (const void *)ifApproved, sizeof(fire_vector)) == 0)
          {
            Serial.println(0b101); //Robo vai para (4,4,4)
            Serial.println("end");
          }
          else if (memcmp((const void *)fire_vector, (const void *)ifNotApproved, sizeof(fire_vector)) == 0)
          {
            Serial.println(0b110); //Robo vai para (6,6,6)
            Serial.println("end");
          }
          else
          {
              Serial.println(0b000);
          }

          // if the arrays are equal execute the code here
          // ...
          return;
        }
        else
        {
          Serial.println("Error in the tag update.");
          pnrd.printTagHistory();
          return;
        };
        return;

      case FireError::PRODUCE_EXCEPTION:
        Serial.println(0b000);
        break;
      }
    }
  }
  else
  {
      Serial.println(0b000);
  }
}
