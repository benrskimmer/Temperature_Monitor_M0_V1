/*
 * spi_drv.c
 *
 * Created: 6/23/2021 12:14:25 AM
 *  Author: Ben
 */ 
#include <asf.h>
#include <spi.h>
#include <port.h>
#include <sercom.h>
#include <pinmux.h>
#include <stdio.h>
#include "spi_drv.h"
#include "millis.h"


#define SLAVESELECT PIN_PA14 // ss
#define SLAVEREADY PIN_PA28 // handshake pin - NOT SURE ABOUT THIS ONE!
#define SLAVERESET PIN_PA08  // reset pin
#define NINA_GPIO0 PIN_PA27

bool inverted_reset = false;

//#define DELAY_TRANSFER()

#ifndef SPIWIFI
#define SPIWIFI SPI
#endif

struct spi_module spi_master_instance;

struct spi_slave_inst slave;

bool wifi_initialized = false;

// typedef enum
// {
// 	uninitialized = 0,
// 	wait_before_init,
// 	initialized
// } wifi_spi_state_t;

//testing
volatile int test_result = 0;

// local function prototypes
void wifi_spi_waitForSlaveSign(void);
void wifi_spi_getParam(uint8_t* param);

void configure_spi_master(void)
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
	/* Configure and initialize software device instance of peripheral slave */
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = SLAVESELECT;
	spi_attach_slave(&slave, &slave_dev_config);
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.mux_setting = SPI_SIGNAL_MUX_SETTING_N;
	config_spi_master.pinmux_pad0 = PIN_PA12;
	config_spi_master.pinmux_pad1 = PIN_PA13;
	config_spi_master.pinmux_pad2 = PINMUX_UNUSED;
	config_spi_master.pinmux_pad3 = PIN_PA15;
	test_result = spi_init(&spi_master_instance, SERCOM2, &config_spi_master);
	spi_enable(&spi_master_instance);
}

void wifi_spi_begin()
{

      //SPIWIFI.begin();
	  
	  struct system_pinmux_config wifi_mosi;
	  system_pinmux_get_config_defaults(&wifi_mosi);
	  wifi_mosi.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	  wifi_mosi.mux_position = MUX_PA12C_SERCOM2_PAD0;
	  system_pinmux_pin_set_config(PIN_PA12D_SERCOM4_PAD0, &wifi_mosi);
	  
	  struct system_pinmux_config wifi_miso;
	  system_pinmux_get_config_defaults(&wifi_miso);
	  wifi_miso.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
	  wifi_miso.mux_position = MUX_PA13C_SERCOM2_PAD1;
	  system_pinmux_pin_set_config(PIN_PA13D_SERCOM4_PAD1, &wifi_miso);
	  
	  struct system_pinmux_config wifi_sck;
	  system_pinmux_get_config_defaults(&wifi_sck);
	  wifi_sck.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	  wifi_sck.mux_position = MUX_PA15C_SERCOM2_PAD3;
	  system_pinmux_pin_set_config(PIN_PA15D_SERCOM4_PAD3, &wifi_sck);
	  
	  struct port_config output_pin;
	  port_get_config_defaults(&output_pin);
	  output_pin.direction = PORT_PIN_DIR_OUTPUT;
	  port_pin_set_config(SLAVESELECT, &output_pin);
	  
	  port_pin_set_config(SLAVERESET, &output_pin);
	  
	  port_pin_set_config(NINA_GPIO0, &output_pin);
	  
//	  port_pin_set_output_level(NINA_GPIO0, HIGH);
	  
	  struct port_config input_pin;
	  port_get_config_defaults(&input_pin);
	  input_pin.direction = PORT_PIN_DIR_INPUT;
	  port_pin_set_config(SLAVEREADY, &input_pin);
//	  port_pin_set_config(NINA_GPIO0, &input_pin); // this is not right and needs to be fixed...
	  
	  configure_spi_master();
	  
	  
	  // set support pin states
	  port_pin_set_output_level(NINA_GPIO0, HIGH);
//	  port_pin_set_output_level(SLAVESELECT, 1); not needed, handled by asf
	  port_pin_set_output_level(SLAVERESET, HIGH);
	  // time for state machine + scheduler...
	  
	  // dumb blocking wait... cause we don't have time to write a task scheduler, too busy with chicks rn - or trying to be anyways...
	  //for(unsigned long i = 0; i < 5000000; i++) nop();
	  delay(10);
	  
	  port_pin_set_output_level(SLAVERESET, LOW);
	  
	 // for(unsigned long i = 0; i < 50000000; i++) nop();
	  delay(750);
	  
	  port_pin_set_output_level(NINA_GPIO0, LOW);
	  port_pin_set_config(NINA_GPIO0, &input_pin);
	  
	  //putting this here for testing only
	  //spi_select_slave(&spi_master_instance, &slave, true);
	  
	  wifi_initialized = true;
	  
	  /*
	  
      pinMode(SLAVESELECT, OUTPUT);
      pinMode(SLAVEREADY, INPUT);
      pinMode(SLAVERESET, OUTPUT);
      pinMode(NINA_GPIO0, OUTPUT);

      digitalWrite(NINA_GPIO0, HIGH);
      digitalWrite(SLAVESELECT, HIGH);
      digitalWrite(SLAVERESET, inverted_reset ? HIGH : LOW);
      delay(10);
      digitalWrite(SLAVERESET, inverted_reset ? LOW : HIGH);
      delay(750);

      digitalWrite(NINA_GPIO0, LOW);
      pinMode(NINA_GPIO0, INPUT);

#ifdef _DEBUG_
	  INIT_TRIGGER()
#endif
*/
      
}

void wifi_spi_end() {
    port_pin_set_output_level(SLAVERESET, 0);

	struct port_config input_pin; // set select pin to input
	port_get_config_defaults(&input_pin);
	input_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(SLAVESELECT, &input_pin);
	
	spi_disable(&spi_master_instance);
	
    wifi_initialized = false;
}

// void wifi_spi_update(void) {
// 	//if(!initialized && )
// 	port_pin_set_output_level(SLAVERESET, 0);
// }


void wifi_spi_spiSlaveSelect()
{
    //SPIWIFI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    //port_pin_set_output_level(SLAVESELECT, 0);
	spi_select_slave(&spi_master_instance, &slave, true);

    // wait for up to 5 ms for the NINA to indicate it is not ready for transfer
    // the timeout is only needed for the case when the shield or module is not present
    for (unsigned long start = millis(); (port_pin_get_input_level(SLAVEREADY) != HIGH) && (millis() - start) < 5;);
}


void wifi_spi_spiSlaveDeselect()
{
    //port_pin_set_output_level(SLAVESELECT, 1);
	spi_select_slave(&spi_master_instance, &slave, false);
   // SPIWIFI.endTransaction();
}


char wifi_spi_spiTransfer(volatile char data)
{
     char result = 0; //SPIWIFI.transfer(data);
//     DELAY_TRANSFER();
	spi_transceive_buffer_wait(&spi_master_instance, (uint8_t*)&data, (uint8_t*)&result, 1);

    return result;                    // return the received byte
}

int wifi_spi_waitSpiChar(unsigned char waitChar)
{
    int timeout = TIMEOUT_CHAR;
    unsigned char _readChar = 0;
    do{
        _readChar = wifi_spi_readChar(); //get data byte
        if (_readChar == ERR_CMD)
        {
        	//WARN("Err cmd received\n");
        	return -1;
        }
    }while((timeout-- > 0) && (_readChar != waitChar));
    return  (_readChar == waitChar);
}

int wifi_spi_readAndCheckChar(char checkChar, char* readChar)
{
    wifi_spi_getParam((uint8_t*)readChar);

    return  (*readChar == checkChar);
}

char wifi_spi_readChar(void)
{
	uint8_t readChar = 0;
	wifi_spi_getParam(&readChar);
	return readChar;
}

#define WAIT_START_CMD(x) wifi_spi_waitSpiChar(START_CMD)

// #define IF_CHECK_START_CMD(x)				\
// if (!WAIT_START_CMD(_data))				\
// {										\
// 	TOGGLE_TRIGGER()
//  WARN("Error waiting START_CMD");
// 	return 0;							\
// }else

#define IF_CHECK_START_CMD(x)				\
    if (!WAIT_START_CMD(_data))				\
	{										\
        return 0;							\
    }else									\

// #define CHECK_DATA(check, x)                   \
// if (!wifi_spi_readAndCheckChar(check, &x))   \
// {                                               \
// 	//TOGGLE_TRIGGER()
// 	//WARN("Reply error");
// 	//INFO2(check, (uint8_t)x);
// 	return 0;                                   \
// }else

#define CHECK_DATA(check, x)						\
    if (!wifi_spi_readAndCheckChar(check, &x))		\
    {                                               \
        return 0;                                   \
    }else                                           \

#define waitSlaveReady() (port_pin_get_input_level(SLAVEREADY) == LOW)
#define waitSlaveSign() (port_pin_get_input_level(SLAVEREADY) == HIGH)
#define waitSlaveSignalH() while(port_pin_get_input_level(SLAVEREADY) != HIGH){}
#define waitSlaveSignalL() while(port_pin_get_input_level(SLAVEREADY) != LOW){}

void wifi_spi_waitForSlaveSign(void)
{
	while (!waitSlaveSign());
}

void wifi_spi_waitForSlaveReady(void)
{
	while (!waitSlaveReady());
}

void wifi_spi_getParam(uint8_t* param)
{
    // Get Params data
   // *param = spiTransfer(DUMMY_DATA);
	uint8_t dummy_data = DUMMY_DATA;
	spi_transceive_buffer_wait(&spi_master_instance, &dummy_data, param, 1);
 //   DELAY_TRANSFER();
}

int wifi_spi_waitResponseCmd(uint8_t cmd, uint8_t numParam, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
    int ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        CHECK_DATA(numParam, _data)
        {
            wifi_spi_readParamLen8(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                //param[ii] = spiTransfer(DUMMY_DATA);
                wifi_spi_getParam(&param[ii]);
            } 
        }         

        wifi_spi_readAndCheckChar(END_CMD, &_data);
    }     
    
    return 1;
}
/*
int wifi_spi_waitResponse(uint8_t cmd, uint8_t numParam, uint8_t* param, uint16_t* param_len)
{
    char _data = 0;
    int i =0, ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        CHECK_DATA(numParam, _data);
        {
            readParamLen16(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                param[ii] = spiTransfer(DUMMY_DATA);
            } 
        }         

        readAndCheckChar(END_CMD, &_data);
    }     
    
    return 1;
}
*/

int wifi_spi_waitResponseData16(uint8_t cmd, uint8_t* param, uint16_t* param_len)
{
    char _data = 0;
    uint16_t ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = wifi_spi_readChar();
        if (numParam != 0)
        {        
            wifi_spi_readParamLen16(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                param[ii] = wifi_spi_spiTransfer(DUMMY_DATA);
            } 
        }         

        wifi_spi_readAndCheckChar(END_CMD, &_data);
    }     
    
    return 1;
}

int wifi_spi_waitResponseData8(uint8_t cmd, uint8_t* param, uint8_t* param_len)
{
    char _data = 0;
    int ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = wifi_spi_readChar();
        if (numParam != 0)
        {        
            wifi_spi_readParamLen8(param_len);
            for (ii=0; ii<(*param_len); ++ii)
            {
                // Get Params data
                param[ii] = wifi_spi_spiTransfer(DUMMY_DATA);
            } 
        }         

        wifi_spi_readAndCheckChar(END_CMD, &_data);
    }     
    
    return 1;
}

int wifi_spi_waitResponseParams(uint8_t cmd, uint8_t numParam, tParam* params)
{
    char _data = 0;
    int i =0, ii = 0;


    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t _numParam = wifi_spi_readChar();
        if (_numParam != 0)
        {        
            for (i=0; i<_numParam; ++i)
            {
                params[i].paramLen = wifi_spi_readParamLen8(NULL);
                for (ii=0; ii<params[i].paramLen; ++ii)
                {
                    // Get Params data
                    params[i].param[ii] = wifi_spi_spiTransfer(DUMMY_DATA);
                } 
            }
        } else
        {
//            WARN("Error numParam == 0");
            return 0;
        }

        if (numParam != _numParam)
        {
//            WARN("Mismatch numParam");
            return 0;
        }

        wifi_spi_readAndCheckChar(END_CMD, &_data);
    }         
    return 1;
}

/*
int wifi_spi_waitResponse(uint8_t cmd, tParam* params, uint8_t* numParamRead, uint8_t maxNumParams)
{
    char _data = 0;
    int i =0, ii = 0;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = readChar();

        if (numParam > maxNumParams)
        {
            numParam = maxNumParams;
        }
        *numParamRead = numParam;
        if (numParam != 0)
        {
            for (i=0; i<numParam; ++i)
            {
                params[i].paramLen = readParamLen8();

                for (ii=0; ii<params[i].paramLen; ++ii)
                {
                    // Get Params data
                    params[i].param[ii] = spiTransfer(DUMMY_DATA);
                } 
            }
        } else
        {
            WARN("Error numParams == 0");
            Serial.println(cmd, 16);
            return 0;
        }
        readAndCheckChar(END_CMD, &_data);
    }         
    return 1;
}
*/

int wifi_spi_waitResponse(uint8_t cmd, uint8_t* numParamRead, uint8_t** params, uint8_t maxNumParams)
{
    char _data = 0;
    int i =0, ii = 0;

    char    *index[WL_SSID_MAX_LENGTH];

    for (i = 0 ; i < WL_NETWORKS_LIST_MAXNUM ; i++)
            index[i] = (char *)params + WL_SSID_MAX_LENGTH*i;

    IF_CHECK_START_CMD(_data)
    {
        CHECK_DATA(cmd | REPLY_FLAG, _data){};

        uint8_t numParam = wifi_spi_readChar();

        if (numParam > maxNumParams)
        {
            numParam = maxNumParams;
        }
        *numParamRead = numParam;
        if (numParam != 0)
        {
            for (i=0; i<numParam; ++i)
            {
            	uint8_t paramLen = wifi_spi_readParamLen8(NULL);
                for (ii=0; ii<paramLen; ++ii)
                {
                	//ssid[ii] = spiTransfer(DUMMY_DATA);
                    // Get Params data
                    index[i][ii] = (uint8_t)wifi_spi_spiTransfer(DUMMY_DATA);

                }
                index[i][ii]=0;
            }
        } else
        {
//            WARN("Error numParams == 0");
            wifi_spi_readAndCheckChar(END_CMD, &_data);
            return 0;
        }
        wifi_spi_readAndCheckChar(END_CMD, &_data);
    }
    return 1;
}

void wifi_spi_sendParamNoLen(uint8_t* param, size_t param_len, uint8_t lastParam)
{
    size_t i = 0;
    // Send Spi paramLen
    wifi_spi_sendParamLen8(0);

    // Send Spi param data
    for (i=0; i<param_len; ++i)
    {
        wifi_spi_spiTransfer(param[i]);
    }

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        wifi_spi_spiTransfer(END_CMD);
}

void wifi_spi_sendParamLen(uint8_t* param, uint8_t param_len, uint8_t lastParam)
{
    int i = 0;
    // Send Spi paramLen
    wifi_spi_sendParamLen8(param_len);

    // Send Spi param data
    for (i=0; i<param_len; ++i)
    {
        wifi_spi_spiTransfer(param[i]);
    }

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        wifi_spi_spiTransfer(END_CMD);
}

void wifi_spi_sendParamLen8(uint8_t param_len)
{
    // Send Spi paramLen
    wifi_spi_spiTransfer(param_len);
}

void wifi_spi_sendParamLen16(uint16_t param_len)
{
    // Send Spi paramLen
    wifi_spi_spiTransfer((uint8_t)((param_len & 0xff00)>>8));
    wifi_spi_spiTransfer((uint8_t)(param_len & 0xff));
}

uint8_t wifi_spi_readParamLen8(uint8_t* param_len)
{
    uint8_t _param_len = wifi_spi_spiTransfer(DUMMY_DATA);
    if (param_len != NULL)
    {
        *param_len = _param_len;
    }
    return _param_len;
}

uint16_t wifi_spi_readParamLen16(uint16_t* param_len)
{
    uint16_t _param_len = wifi_spi_spiTransfer(DUMMY_DATA)<<8 | (wifi_spi_spiTransfer(DUMMY_DATA)& 0xff);
    if (param_len != NULL)
    {
        *param_len = _param_len;
    }
    return _param_len;
}


void wifi_spi_sendBuffer(uint8_t* param, uint16_t param_len, uint8_t lastParam) // lastParam should be set to NO_LAST_PARAM if there's no last param
{
    uint16_t i = 0;

    // Send Spi paramLen
    wifi_spi_sendParamLen16(param_len);

    // Send Spi param data
    for (i=0; i<param_len; ++i)
    {
        wifi_spi_spiTransfer(param[i]);
    }

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        wifi_spi_spiTransfer(END_CMD);
}


void wifi_spi_sendParam(uint16_t param, uint8_t lastParam)
{
    // Send Spi paramLen
    wifi_spi_sendParamLen8(2);

    wifi_spi_spiTransfer((uint8_t)((param & 0xff00)>>8));
    wifi_spi_spiTransfer((uint8_t)(param & 0xff));

    // if lastParam==1 Send Spi END CMD
    if (lastParam == 1)
        wifi_spi_spiTransfer(END_CMD);
}

/* Cmd Struct Message */
/* _________________________________________________________________________________  */
/*| START CMD | C/R  | CMD  |[TOT LEN]| N.PARAM | PARAM LEN | PARAM  | .. | END CMD | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */
/*|   8 bit   | 1bit | 7bit |  8bit   |  8bit   |   8bit    | nbytes | .. |   8bit  | */
/*|___________|______|______|_________|_________|___________|________|____|_________| */

void wifi_spi_sendCmd(uint8_t cmd, uint8_t numParam)
{
    // Send Spi START CMD
    wifi_spi_spiTransfer(START_CMD);

    // Send Spi C + cmd
    wifi_spi_spiTransfer(cmd & ~(REPLY_FLAG));

    // Send Spi totLen
    //spiTransfer(totLen);

    // Send Spi numParam
    wifi_spi_spiTransfer(numParam);

    // If numParam == 0 send END CMD
    if (numParam == 0)
        wifi_spi_spiTransfer(END_CMD);

}

int wifi_spi_available(void)
{
    return (port_pin_get_input_level(NINA_GPIO0) != LOW);
}