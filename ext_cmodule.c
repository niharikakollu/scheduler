#include "esp_log.h"
#include "lauxlib.h"
#include "lnodeaux.h"
#include "module.h"
#include<string.h>
#include<stdlib.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora.h"
static const char* ARITHMETIC_METATABLE = NODEMCU_MODULE_METATABLE();

static void lora_initialise(lua_State *L){
	printf("check1");
	lora_init();
	printf("check2");
	long freq=luaL_checknumber(L,1);
	freq=freq*1e6;
	lora_set_frequency(freq);
	printf("check3");
	int sf=luaL_checkinteger(L,2);
	if (sf>0)
		lora_set_spreading_factor(sf);
	printf("check4");
	long bw=luaL_checkinteger(L,3);
	if (bw>0)
		lora_set_bandwidth(bw);
	printf("check5");
	lora_enable_crc();
	return 0;
}

static void lora_sending(lua_State *L){
	int size=luaL_checknumber(L,1);
	char *str = luaL_checkstring( L,2 );
	printf("check6");
	lora_send_packet((uint8_t*)str, size);
	printf("check7");
	return 0;
}

static void lora_receiving(lua_State *L){
	int size=0,x=0;
	size=luaL_checknumber(L,1);
	char buf[size];
	printf("check8");
	lora_receive();    // put into receive mode
	while(available()) {
	 printf("check9");
         x= lora_receive_packet((uint8_t*) buf, sizeof(buf));
         printf("Received: %s, length: %d\n", buf,x);
         //lora_receive();
      }
	lua_pushstring(L,buf);
	return 1;
}

LROT_BEGIN(arith_metatable)
LROT_END(arith_metatable, NULL, 0)

LROT_BEGIN(module)
LROT_FUNCENTRY(lora_init,lora_initialise)
LROT_FUNCENTRY(lora_transmit,lora_sending)
LROT_FUNCENTRY(lora_receive,lora_receiving)
LROT_END(module, NULL, 0)

// module_init is invoked on device startup
static int module_init(lua_State* L) {
    luaL_rometatable(L, ARITHMETIC_METATABLE, (void*)arith_metatable_map);  // create metatable for arithmetic
    return 0;
}

NODEMCU_MODULE_STD();  // define Lua entries
