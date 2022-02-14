#include "esp_log.h"
#include "lauxlib.h"
#include "lnodeaux.h"
#include "module.h"
#include<string.h>
#include<stdlib.h>
#include<math.h>
#define ADD_TABLE_ITEM(L, key, val) \
  lua_pushstring (L, val);      \
  lua_setfield (L, -2, key);
 struct task_table{
  const char *name;
  const char *task;
  const char *status;
};
struct localstruct{
 int *zone_id,*duration_mins,*seq_num;
};
static const char* ARITHMETIC_METATABLE = NODEMCU_MODULE_METATABLE();
void inline structure_ToTable(lua_State *L, struct task_table *test )
{ 
  lua_createtable (L, 0, 3);
  ADD_TABLE_ITEM (L, "name", test->name );
  ADD_TABLE_ITEM (L, "task", test->task);
  ADD_TABLE_ITEM (L, "status", test->status);
}
static int teal_scheduler_table(lua_State *L){
 int i=0,len=0;
size_t datalen;
struct localstruct SPprog_tab;
 if( lua_istable( L,1)){
 lua_getfield(L,1,"zone_id");
 if (lua_type(L, -1)==LUA_TTABLE){
    printf("failed\n");
    datalen = lua_objlen(L,2);
	 len=datalen;
    printf("size of datalen1:%d\n",datalen);
    SPprog_tab.zone_id=(int*)calloc(datalen, sizeof(int));
	for( i = 0; i < datalen; i ++ ){
	printf("entered to for loop1\n");
	lua_rawgeti( L, 2, i + 1 );
     SPprog_tab.zone_id[i]=lua_tointeger (L,-1);
     printf("%d\t",SPprog_tab.zone_id[i]);
      lua_pop (L, 1);
    }
 }
 else
	 luaL_error (L, "Not an array 1");
 lua_getfield(L,1,"duration_mins");
 if (lua_type(L, -1) == LUA_TTABLE){
     datalen = lua_objlen(L,3);
    printf("failed1\n");
    printf("size of datalen2:%d\n",datalen);
    SPprog_tab.duration_mins=(int*)calloc(datalen, sizeof(int));
    for( i = 0; i < datalen; i ++ ){
	    printf("entered into for loop 2\n");
		lua_rawgeti( L, 3, i + 1 );
     SPprog_tab.duration_mins[i]=lua_tointeger (L, -1);
     printf("%d\t",SPprog_tab.duration_mins[i]);
      lua_pop (L, 1);
    }
 }
 else
   luaL_error (L, "Not an array 2");
 lua_getfield(L,1,"seq_num");
 if (lua_type(L, -1) == LUA_TTABLE){
   datalen = lua_objlen( L,4 );
   printf("failed2\n");
printf("size of datalen2:%d\n",datalen);
   SPprog_tab.seq_num=(int*)calloc(datalen, sizeof(int));
   int i=0;
  for( i = 0; i < datalen; i ++ ){
	  printf("entered into loop3\n");
	lua_rawgeti( L, 4, i + 1 );
     SPprog_tab.seq_num[i]=lua_tointeger (L, -1);
     printf("%d\t",SPprog_tab.seq_num[i]);
      lua_pop (L, 1);
  }
 }
 else
   luaL_error (L, "Not an array 3"); 
 }
 printf("length of the table is:%d\n",len);
 lua_createtable(L, 3, 0);
 lua_pushliteral(L,"zone_id");
 lua_createtable(L, 0, len);
 for(int i=0;i<len;i++){
   lua_pushnumber( L, (lua_Number)(i+1));
  lua_setfield(L, -2, i+1);
  }
lua_settable( L, -3 );
 lua_pushliteral(L,"duration_mins");
 lua_createtable(L, 0, len);
 for(int i=0;i<len;i++){
   lua_pushnumber( L, (lua_Number)(i+2));
   lua_setfield(L, -2, i+1);
  }
lua_settable( L, -3 );
 lua_pushliteral(L,"seq");
lua_createtable(L, 0, len);
 for(int i=0;i<len;i++){
   lua_pushnumber( L, (lua_Number)(i+3));
  lua_setfield(L, -2, i+1);
  }
lua_settable( L, -3 );
 return 1;
}
static int l_push_multidim_table(lua_State *L)
{
    /* Creates parent table of size 2 array elements: */
    lua_createtable(L, 2, 0);

    /* Puts key of the first child table on-top of Lua VM stack: */
    lua_pushnumber(L, 1);

    /*Creates first child table of size 3 non-array elements: */
    lua_createtable(L, 0, 3);

    /* Fills the first child table: */
    lua_pushnumber(L, 1);
    lua_setfield(L, -2, "field1");

    lua_pushnumber(L, 2);
    /* setfield() pops the value from Lua VM stack. */
    lua_setfield(L, -2, "field2");

    lua_pushnumber(L, 3);
    lua_setfield(L, -2, "field3");

    /* Remember, child table is on-top of the stack.
     * lua_settable() pops key, value pair from Lua VM stack. */
    lua_settable(L, -3);

    /* Pushes they key value for the second child table: */
    lua_pushnumber(L, 2);

    /*Creates second child table of size  3 non-array elements: */
    lua_createtable(L, 0, 3);

    /* Fills the second child table: */
    lua_pushnumber(L, 10);
    lua_setfield(L, -2, "field1");

    lua_pushnumber(L, 20);
    lua_setfield(L, -2, "field2");

    lua_pushnumber(L, 30);
    lua_setfield(L, -2, "field3");

    /* Remember, child table is still on-top of the stack.
     * lua_settable pops the key, value pair from Lua VM stack
     * And puts child table into the parent. */
    lua_settable(L, -3);

    /* Returns number of output tables:
     * (1 multidimentional)            */
    return 1;
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
//  lopading table with key values.
static int table_key(lua_State* L){  
  struct task_table test;
  luaL_checkanytable (L, 1);
  lua_getfield (L, 1, "name");
  test.name = luaL_optstring(L, -1, "nill");
  lua_getfield (L, 1, "task");
  test.task =  luaL_optstring(L, -1, "nill");
  lua_getfield (L, 1, "status");
  test.status =  luaL_optstring(L, -1, "nill");
  printf("table elemnts are:%s\t%s\t%s\n",test.name,test.task,test.status);
  structure_ToTable(L,&test);
    return 1;
}
 static int load_arr(lua_State* L){
  size_t datalen, i;
   lua_newtable( L ); 
     if( lua_istable( L,1)){
     datalen = lua_objlen( L,1 );
      for( i = 0; i < datalen; i ++ ){
      lua_rawgeti( L, 1, i + 1 );
    if(lua_type(L,-1)== LUA_TNUMBER){ 
    lua_pushnumber(L, (lua_Number)luaL_checknumber(L,-1));
     lua_pop( L, 1 );
     lua_rawseti( L,-2, i + 1 );
    }
    else if(lua_type(L,-1)== LUA_TSTRING){
     lua_pushstring(L,luaL_checkstring(L,-1));
     lua_pop( L, 1 );
     lua_rawseti( L,-2, i + 1 );
    }
      }
     } 
   return 1;
 }
LROT_BEGIN(arith_metatable)
LROT_END(arith_metatable, NULL, 0)

LROT_BEGIN(module)
LROT_FUNCENTRY(larr,load_arr)
LROT_FUNCENTRY(tkey,table_key)
LROT_FUNCENTRY(dectobin,decimal_binary)
LROT_FUNCENTRY(rettab,l_push_multidim_table)
LROT_FUNCENTRY(schedule_table,teal_scheduler_table)
LROT_END(module, NULL, 0)

// module_init is invoked on device startup
static int module_init(lua_State* L) {
    luaL_rometatable(L, ARITHMETIC_METATABLE, (void*)arith_metatable_map);  // create metatable for arithmetic
    return 0;
}

NODEMCU_MODULE_STD();  // define Lua entries
