#include "esp_log.h"
#include "lauxlib.h"
#include "lnodeaux.h"
#include "module.h"
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
static const char* ARITHMETIC_METATABLE = NODEMCU_MODULE_METATABLE();
static int idle_cnt = 0;

static void    idle_task(void *parm)

{
    while(1==1)
        {
        int64_t now = esp_timer_get_time();     // time anchor
        vTaskDelay(0 / portTICK_RATE_MS);
        int64_t now2 = esp_timer_get_time();
        idle_cnt += (now2 - now) / 1000;        // diff
        }
}

static void    mon_task(void *parm)

{
    while(1==1)
        {
        // Note the trick of saving it on entry, so print time
        // is not added to our timing.
        
        float new_cnt =  (float)idle_cnt;    // Save the count for printing it ...
        
        // Compensate for the 100 ms delay artifact: 900 ms = 100%
        float cpu_percent = ((99.9 / 90.) * new_cnt) / 10;
        printf("%.0f%%  ", 100 - cpu_percent); fflush(stdout);
        idle_cnt = 0;                        // Reset variable
        vTaskDelay(1000 / portTICK_RATE_MS);
        }
}

static int app_main()
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("ESP32, %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    xTaskCreate(idle_task, "idle_task", 1024 * 2, NULL,  0, NULL);
    xTaskCreate(mon_task, "mon_task", 1024 * 2, NULL, 10, NULL);
    
    printf("CPU Load Demo.\n");
    int cnt = 0;
    while(true)
        {
        // Every 10 seconds, we put the processor to work. 
        if(++cnt % 10 == 0)
            {
            int cnt_dummy = 0;
            
            printf(" work [[ "); fflush(stdout);
            
            //Make sure the watchdog is not triggered ...
            for(int aa = 0 ; aa < 30000000; aa++)
                {
                cnt_dummy += 22;
                }
                
            printf(" ]] rest "); fflush(stdout);
            }
        vTaskDelay(1000 / portTICK_RATE_MS);
        }
    return 1;
}

static int check_err (lua_State *L, esp_err_t err)
{
  switch (err)
  {
    case ESP_ERR_INVALID_ARG: luaL_error (L, "invalid argument");
    case ESP_ERR_INVALID_STATE: luaL_error (L, "internal logic error");
    case ESP_OK: break;
  }
  return 0;
}

// decimal to binary module
static int decimal_binary(lua_State* L){  
  int dec_num=luaL_checknumber(L,1);
  long long bin_num=0;
  int rem, i = 1;
    while (dec_num!=0) {
    rem = dec_num % 2;
    dec_num /= 2;
    bin_num+= rem * i;
    i *= 10;
  }
  lua_pushnumber(L, (lua_Number)bin_num);
    return 1;
}
static int gpio_reading (lua_State *L)
{
  int gpio = luaL_checkint (L, 1);
  if (!GPIO_IS_VALID_GPIO(gpio))
    return check_err (L, ESP_ERR_INVALID_ARG);
  lua_pushinteger (L, gpio_get_level (gpio));
  return 1;
}
static int gpio_writing (lua_State *L)
{
  int gpio = luaL_checkint (L, 1);
  int level = luaL_checkint (L, 2);
  check_err (L, gpio_set_level (gpio, level));
  return 0;
}

static int find_match_array(lua_State *L){
  int  val=luaL_checknumber(L, 2);
  if(lua_istable(L,1)){
    size_t datalen = lua_objlen( L,1);
    int flg=0;
    for(int i=0;i<datalen;i++){
      lua_rawgeti(L,1,i+1);
      if(val==luaL_checknumber(L,-1)){
       flg=i+1;
      break;
    }
     lua_pop(L,1);
    }
    lua_pushinteger(L,(lua_Integer)flg);
  }
  return 1;
 }
static int calib_alg(lua_State* L){
  size_t i;
  float org[3],sort[3],comp1,comp2,a=0.0;
  int pos=0;
  if( lua_istable( L,1)){
    for( i = 0; i < 3; i ++ ){
      lua_rawgeti( L, 1, i + 1 );
      if(lua_type(L,-1)== LUA_TNUMBER){
        org[i]=(lua_Number)luaL_checknumber(L,-1);
        lua_pop( L, 1 );
      }
    }
  }
  if( lua_istable( L,2)){
    for( i = 0; i < 3; i ++ ){
     lua_rawgeti( L, 2, i + 1 );
     if(lua_type(L,-1)== LUA_TNUMBER){
        sort[i]=(lua_Number)luaL_checknumber(L,-1);
        lua_pop( L, 1 );
     }
  }
  }
     comp1=sort[1]-sort[0];
     comp2=sort[2]-sort[1];
    if (comp1>comp2)
        a=sort[1];
    else if(comp2>comp1)
        a=sort[2];
    else if(comp1!=0 && comp2!=0)
           a=sort[2];
    for (int i = 0; i <3;i++)
    if(org[i]==a)
    {
      org[i]=0;
      pos=i;
    }
    for(int i=pos;i>0;i--)
    org[i]=org[i-1];
    org[0]=(org[2]+org[1])/2.0;
    lua_newtable( L );
   for (int i = 0; i <3;i++){
    lua_pushnumber(L, (lua_Number)org[i]);
     lua_rawseti( L,-2, i + 1 );
   }
   return 1;
}
LROT_BEGIN(arith_metatable)
LROT_END(arith_metatable, NULL, 0)

LROT_BEGIN(module)
LROT_FUNCENTRY(calibration_algorithm,calib_alg)
LROT_FUNCENTRY(dectobin,decimal_binary)
LROT_FUNCENTRY(cpu_load,app_main)
LROT_FUNCENTRY(gpio_read,gpio_reading)
LROT_FUNCENTRY(gpio_write,gpio_writing)
LROT_FUNCENTRY(find_match,find_match_array)
LROT_END(module, NULL, 0)

// module_init is invoked on device startup
static int module_init(lua_State* L) {
    luaL_rometatable(L, ARITHMETIC_METATABLE, (void*)arith_metatable_map);  // create metatable for arithmetic
    return 0;
}

NODEMCU_MODULE_STD();  // define Lua entries
