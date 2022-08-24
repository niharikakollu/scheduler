#include "esp_log.h"
#include "lauxlib.h"
#include "lnodeaux.h"
#include "module.h"
#include<string.h>
#include<stdlib.h>
#include<math.h>
static const char* ARITHMETIC_METATABLE = NODEMCU_MODULE_METATABLE();
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
    //lua_newtable( L );
     printf("data in array\n ");
   for (int i = 0; i <3;i++){
   // lua_pushnumber(L, (lua_Number)org[i]);
    // lua_pop( L, 1 );
     //lua_rawseti( L,-2, i + 1 );
     printf("%f  ",org[i]);
   }
   return 1;
}
LROT_BEGIN(arith_metatable)
LROT_END(arith_metatable, NULL, 0)

LROT_BEGIN(module)
LROT_FUNCENTRY(calibration_algorithm,calib_alg)
LROT_FUNCENTRY(dectobin,decimal_binary)
LROT_END(module, NULL, 0)

// module_init is invoked on device startup
static int module_init(lua_State* L) {
    luaL_rometatable(L, ARITHMETIC_METATABLE, (void*)arith_metatable_map);  // create metatable for arithmetic
    return 0;
}

NODEMCU_MODULE_STD();  // define Lua entries
