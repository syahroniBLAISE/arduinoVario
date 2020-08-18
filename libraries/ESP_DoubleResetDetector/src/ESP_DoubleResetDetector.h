/****************************************************************************************************************************
 * ESP_DoubleResetDetector.h
 * For ESP8266 / ESP32 boards
 *
 * ESP_DoubleResetDetector is a library for the ESP8266/Arduino platform
 * to enable trigger configure mode by resetting ESP32 / ESP8266 twice.
 *
 * Forked from DataCute https://github.com/datacute/DoubleResetDetector
 * 
 * Built by Khoi Hoang https://github.com/khoih-prog/ESP_DoubleResetDetector
 * Licensed under MIT license
 * Version: 1.0.0
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.0   K Hoang      15/12/2019 Initial coding
 *  1.0.1   K Hoang      30/12/2019 Now can use EEPROM or SPIFFS for both ESP8266 and ESP32. RTC still OK for ESP8266 
 *****************************************************************************************************************************/

#ifndef ESP_DoubleResetDetector_H
#define ESP_DoubleResetDetector_H

#if defined(ARDUINO) && (ARDUINO >= 100)
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

//#define ESP_DRD_USE_EEPROM      false
//#define ESP_DRD_USE_SPIFFS      false
//#define ESP8266_DRD_USE_RTC     false   //true

#ifdef ESP32
  #if (!ESP_DRD_USE_EEPROM && !ESP_DRD_USE_SPIFFS)
    #warning Neither EEPROM nor SPIFFS selected. Default to EEPROM
    #ifdef ESP_DRD_USE_EEPROM
      #undef ESP_DRD_USE_EEPROM
      #define ESP_DRD_USE_EEPROM      true
    #endif
  #endif
#endif

#ifdef ESP8266
  #if (!ESP8266_DRD_USE_RTC && !ESP_DRD_USE_EEPROM && !ESP_DRD_USE_SPIFFS)
    #warning Neither RTC, EEPROM nor SPIFFS selected. Default to EEPROM
    #ifdef ESP_DRD_USE_EEPROM
      #undef ESP_DRD_USE_EEPROM
      #define ESP_DRD_USE_EEPROM      true
    #endif
  #endif
#endif

//default to use EEPROM, otherwise, use SPIFFS
#if ESP_DRD_USE_EEPROM
  #include <EEPROM.h>
   
  #define  FLAG_DATA_SIZE     4

  #ifndef EEPROM_SIZE
    #define EEPROM_SIZE     512
  #endif  

  #ifndef EEPROM_START
    #define EEPROM_START    256
  #endif    
#elif ESP_DRD_USE_SPIFFS
  #include <FS.h>
  #ifdef ESP32
    #include "SPIFFS.h"
  #endif
  
  #define  DRD_FILENAME     "/drd.dat"
  
#endif    //#if ESP_DRD_USE_EEPROM

#ifndef DOUBLERESETDETECTOR_DEBUG
  #define DOUBLERESETDETECTOR_DEBUG       false
#endif

#define ESP_DOUBLERESETDETECTOR_VERSION "1.0.1"

#define DOUBLERESETDETECTOR_FLAG_SET    0xD0D01234
#define DOUBLERESETDETECTOR_FLAG_CLEAR  0xD0D04321

class DoubleResetDetector
{
public:
	DoubleResetDetector(int timeout, int address)
	{
    #if ESP_DRD_USE_EEPROM
      #if (DOUBLERESETDETECTOR_DEBUG)
        Serial.println("EEPROM size = " + String(EEPROM_SIZE) + ", start = " + String(EEPROM_START));
      #endif
      
      EEPROM.begin(EEPROM_SIZE);
    #elif ESP_DRD_USE_SPIFFS
      // SPIFFS code
      if (!SPIFFS.begin()) 
      {
        #if (DOUBLERESETDETECTOR_DEBUG)
          Serial.println("SPIFFS failed!. Please use EEPROM.");
        #endif
      }      
    #else
      #ifdef ESP8266
        //RTC only for ESP8266
      #endif
    #endif
    
	  this->timeout = timeout * 1000;
	  this->address = address;
	  doubleResetDetected = false;
	  waitingForDoubleReset = false;
	};
	
	bool detectDoubleReset()
	{
  	doubleResetDetected = detectRecentlyResetFlag();
	
	  if (doubleResetDetected) 
	  {
	    #if (DOUBLERESETDETECTOR_DEBUG)
	      Serial.println("doubleResetDetected");
	    #endif
		
		  clearRecentlyResetFlag();
	  } 
	  else 
	  {
	    #if (DOUBLERESETDETECTOR_DEBUG)
	      Serial.println("No doubleResetDetected");
	    #endif
		
		  setRecentlyResetFlag();
		  waitingForDoubleReset = true;
	  }
	  
	  return doubleResetDetected;

	};
	
	void loop()
	{
  	if (waitingForDoubleReset && millis() > timeout)
	  {
	    #if (DOUBLERESETDETECTOR_DEBUG)
	      Serial.println("Stop doubleResetDetecting");
	    #endif
	    
	    stop();
	  }
	};
	
	void stop()
	{
		clearRecentlyResetFlag();
	  waitingForDoubleReset = false;
	};
	
  bool doubleResetDetected;

	
private:
  uint32_t DOUBLERESETDETECTOR_FLAG;
	int timeout;
	int address;
	bool waitingForDoubleReset;
	
	bool detectRecentlyResetFlag()
	{	
		#if (ESP_DRD_USE_EEPROM)
      EEPROM.get(EEPROM_START, DOUBLERESETDETECTOR_FLAG);
	    doubleResetDetectorFlag = DOUBLERESETDETECTOR_FLAG;
	    
	    #if (DOUBLERESETDETECTOR_DEBUG)
	      Serial.println("EEPROM Flag read = 0x" + String(DOUBLERESETDETECTOR_FLAG, HEX) );
	    #endif
	  #elif (ESP_DRD_USE_SPIFFS)
      // SPIFFS code
      if (SPIFFS.exists(DRD_FILENAME))
      { 
        // if config file exists, load
        File file = SPIFFS.open(DRD_FILENAME, "r");
        
        if (!file) 
        {
          #if (DOUBLERESETDETECTOR_DEBUG)  
          Serial.println("Loading config file failed");
          #endif
        }
          
        file.readBytes((char *) &DOUBLERESETDETECTOR_FLAG, sizeof(DOUBLERESETDETECTOR_FLAG));
        doubleResetDetectorFlag = DOUBLERESETDETECTOR_FLAG;
        
	      #if (DOUBLERESETDETECTOR_DEBUG)
	        Serial.println("SPIFFS Flag read = 0x" + String(DOUBLERESETDETECTOR_FLAG, HEX) );
	      #endif

        file.close();                
      }
    #else
      #ifdef ESP8266
        //RTC only for ESP8266
        ESP.rtcUserMemoryRead(address, &doubleResetDetectorFlag, sizeof(doubleResetDetectorFlag));
      #endif
    #endif
	    	
	  doubleResetDetected = (doubleResetDetectorFlag == DOUBLERESETDETECTOR_FLAG_SET);
	  return doubleResetDetected;
	};
	
	void setRecentlyResetFlag()
	{
	  doubleResetDetectorFlag = DOUBLERESETDETECTOR_FLAG_SET;

    DOUBLERESETDETECTOR_FLAG = DOUBLERESETDETECTOR_FLAG_SET;
    
		#if (ESP_DRD_USE_EEPROM)
	    EEPROM.put(EEPROM_START, DOUBLERESETDETECTOR_FLAG);
	    EEPROM.commit();
	    	  
	    #if (DOUBLERESETDETECTOR_DEBUG)
	      delay(1000);
	      EEPROM.get(EEPROM_START, DOUBLERESETDETECTOR_FLAG);
	      Serial.println("SetFlag write = 0x" + String(DOUBLERESETDETECTOR_FLAG, HEX) );
	    #endif
	  #elif (ESP_DRD_USE_SPIFFS)
      // SPIFFS code
      File file = SPIFFS.open(DRD_FILENAME, "w");
      #if (DOUBLERESETDETECTOR_DEBUG)  
        Serial.println("Saving config file...");
      #endif
      
      if (file) 
      {
        file.write((uint8_t *) &DOUBLERESETDETECTOR_FLAG, sizeof(DOUBLERESETDETECTOR_FLAG));  
        file.close();     
        #if (DOUBLERESETDETECTOR_DEBUG)  
          Serial.println("Saving config file OK");
        #endif
      }
      else
      {
        #if (DOUBLERESETDETECTOR_DEBUG)  
        Serial.println("Saving config file failed");
        #endif     
      }        
    #else
      #ifdef ESP8266
        //RTC only for ESP8266
	      ESP.rtcUserMemoryWrite(address, &doubleResetDetectorFlag, sizeof(doubleResetDetectorFlag));
      #endif
    #endif	  	
	};
	
	
	void clearRecentlyResetFlag()
	{
	  doubleResetDetectorFlag = DOUBLERESETDETECTOR_FLAG_CLEAR;
	  DOUBLERESETDETECTOR_FLAG = DOUBLERESETDETECTOR_FLAG_CLEAR;
	
		#if (ESP_DRD_USE_EEPROM)
	    //DOUBLERESETDETECTOR_FLAG = DOUBLERESETDETECTOR_FLAG_CLEAR;
	    EEPROM.put(EEPROM_START, DOUBLERESETDETECTOR_FLAG);
	    EEPROM.commit();
	    
	    #if (DOUBLERESETDETECTOR_DEBUG)
	      delay(1000);
	      EEPROM.get(EEPROM_START, DOUBLERESETDETECTOR_FLAG);
	      Serial.println("ClearFlag write = 0x" + String(DOUBLERESETDETECTOR_FLAG, HEX) );
	    #endif
	  #elif (ESP_DRD_USE_SPIFFS)
      // SPIFFS code      
      File file = SPIFFS.open(DRD_FILENAME, "w");
      #if (DOUBLERESETDETECTOR_DEBUG)  
        Serial.println("Saving config file...");
      #endif
      
      if (file) 
      {
        file.write((uint8_t *) &DOUBLERESETDETECTOR_FLAG, sizeof(DOUBLERESETDETECTOR_FLAG));  
        file.close();     
        #if (DOUBLERESETDETECTOR_DEBUG)  
          Serial.println("Saving config file OK");
        #endif
      }
      else
      {
        #if (DOUBLERESETDETECTOR_DEBUG)  
        Serial.println("Saving config file failed");
        #endif     
      }        
      
    #else
      #ifdef ESP8266
        //RTC only for ESP8266
	      ESP.rtcUserMemoryWrite(address, &doubleResetDetectorFlag, sizeof(doubleResetDetectorFlag));
      #endif
    #endif	
	};
	
	uint32_t doubleResetDetectorFlag;
};
#endif // DoubleResetDetector_H
