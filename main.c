#include <lua.h>
#include <lauxlib.h>

#include "aergo.h"
#include <string.h>

static const char * aergo_lua_to_json(lua_State* L, int args_pos);

aergo *instance = NULL;

static void aergo_lua_account_to_table(lua_State* L, aergo_account account) {
    int index = -2;
    lua_newtable(L);
    lua_pushboolean(L, account.use_ledger);
    lua_setfield(L, index, "use_ledger");
    lua_pushinteger(L, account.index);
    lua_setfield(L, index, "index");
    lua_pushlstring(L, (const char*) &account.privkey, sizeof(account.privkey));
    lua_setfield(L, index, "privkey");
    lua_pushlstring(L, (const char*) &account.pubkey, sizeof(account.pubkey));
    lua_setfield(L, index, "pubkey");
    lua_pushlstring(L, (const char*) &account.address, sizeof(account.address));
    lua_setfield(L, index, "address");
    lua_pushinteger(L, account.nonce);
    lua_setfield(L, index, "nonce");
    lua_pushnumber(L, account.balance);
    lua_setfield(L, index, "balance");

    lua_createtable(L, sizeof(account.state_root), 0);
    for (int i = 0; i < sizeof(account.state_root); i++) {
      lua_pushinteger(L, account.state_root[i]);
      lua_rawseti(L, index, i + 1);
    }
    lua_setfield(L, index, "state_root");
}

static aergo_account aergo_lua_table_to_account(lua_State* L, int index) {
    aergo_account account = {0};
    const unsigned char *data;
    size_t len;

    lua_getfield(L, index, "use_ledger");
    account.use_ledger = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "index");
    account.index = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "privkey");
    len = 0;
    data = (const unsigned char*) lua_tolstring(L, -1, &len);
    memcpy(account.privkey, data, len);
    lua_pop(L, 1);

    lua_getfield(L, index, "pubkey");
    len = 0;
    data = (const unsigned char*) lua_tolstring(L, -1, &len);
    memcpy(account.pubkey, data, len);
    lua_pop(L, 1);

    lua_getfield(L, index, "address");
    len = 0;
    const char *address = (const char*) lua_tolstring(L, -1, &len);
    memcpy(account.address, address, len);
    lua_pop(L, 1);

    lua_getfield(L, index, "nonce");
    account.nonce = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, "balance");
    account.balance = lua_tonumber(L, -1);
    lua_pop(L, 1);

    /*
    lua_getfield(L, index, "state_root");
    size_t N = lua_objlen(L, -1);
    for (int i = 1; i <= N; ++i) {
        lua_rawgeti(L, -1, i);
        account.state_root[i - 1] = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    */

    account.is_updated = true;

    return account;
}

static int aergo_lua_connect(lua_State* L) {
  const char *host = luaL_checkstring(L, 1);
  int port = luaL_checkinteger(L, 2);
  instance = aergo_connect(host, port);
  lua_pushboolean(L, instance != NULL);
  return 1;
}

static int aergo_lua_query(lua_State* L) {
  const char *contract_address = luaL_checkstring(L, 1);
  const char *function_name = luaL_checkstring(L, 2);
  const char *args = aergo_lua_to_json(L, 3);
  char response[1024];

  bool success = aergo_query_smart_contract_json(
    instance,
    response, sizeof response,
    contract_address,
    function_name,
    args
  );

  // returns a table with 2 fields: success & result
  lua_newtable(L);
  lua_pushboolean(L, success);
  lua_setfield(L, -2, "success");
  lua_pushstring(L, response);
  lua_setfield(L, -2, "result");

  //lua_pushstring(L, response);
  return 1;
}

static int aergo_lua_get_account_state(lua_State* L) {
    aergo_account account = {0};
    char error[256];

    bool use_ledger = lua_toboolean(L, 1);
    account.use_ledger = use_ledger;
    if (use_ledger) {
      int index = lua_tointeger(L, 2);
      account.index = index;
    } else {
      size_t len;
      const unsigned char *privkey;
      privkey = (const unsigned char*) lua_tolstring(L, 2, &len);
      memcpy(account.privkey, privkey, len);
    }

    if (aergo_get_account_state(instance, &account, error) == true) {
        aergo_lua_account_to_table(L, account);
    } else {
        lua_createtable(L, 0, 1);
        lua_pushstring(L, error);
        lua_setfield(L, -2, "error");
    }
    return 1;
}

static int aergo_lua_call_smart_contract(lua_State* L) {
    aergo_account account = aergo_lua_table_to_account(L, 1);
    const char *contract_address = lua_tostring(L, 2);
    const char *function = lua_tostring(L, 3);
    const char *args = aergo_lua_to_json(L, 4);

    struct transaction_receipt receipt = {0};
    lua_newtable(L);

    if (aergo_call_smart_contract_json(instance, &receipt, &account, contract_address, function, args)) {
        lua_pushstring(L, receipt.blockHash);
        lua_setfield(L, -2, "blockHash");
        lua_pushinteger(L, receipt.blockNo);
        lua_setfield(L, -2, "blockNo");
        lua_pushstring(L, receipt.contractAddress);
        lua_setfield(L, -2, "contractAddress");
        lua_pushboolean(L, receipt.feeDelegation);
        lua_setfield(L, -2, "feeDelegation");
        lua_pushnumber(L, receipt.feeUsed);
        lua_setfield(L, -2, "feeUsed");
        lua_pushinteger(L, receipt.gasUsed);
        lua_setfield(L, -2, "gasUsed");
        lua_pushstring(L, receipt.ret);
        lua_setfield(L, -2, "ret");
        lua_pushstring(L, receipt.status);
        lua_setfield(L, -2, "status");
        lua_pushstring(L, receipt.txHash);
        lua_setfield(L, -2, "txHash");
        lua_pushinteger(L, receipt.txIndex);
        lua_setfield(L, -2, "txIndex");
        // update the nonce on the account table
        lua_pushinteger(L, account.nonce);
        lua_setfield(L, 1, "nonce");
    } else {
        lua_pushstring(L, "FAILED");
        lua_setfield(L, -2, "status");
        lua_pushstring(L, receipt.ret);
        lua_setfield(L, -2, "ret");
    }
    return 1;
}

int luaopen_aergo(lua_State* L) {

  static const struct luaL_Reg functions [] = {
    {"connect", aergo_lua_connect},
    {"get_account_state", aergo_lua_get_account_state},
    {"query", aergo_lua_query},
    {"call", aergo_lua_call_smart_contract},
    {NULL, NULL}
  };

#if LUA_VERSION_NUM >= 502
  lua_newtable(L);
  luaL_setfuncs(L, functions, 0);
  // makes the table a global variable
  lua_pushvalue(L, -1);
  lua_setglobal(L, "aergo");
#else
  luaL_register(L, "aergo", functions); /* 5.1 */
#endif

  return 1;
}

static const char * aergo_lua_to_json(lua_State* L, int args_pos) {
  const char *args;

  switch (lua_type(L, args_pos)) {
  case LUA_TNONE:  // no arguments given
    args = NULL;
    break;
  case LUA_TSTRING:
    args = lua_tostring(L, args_pos);
    break;
#if 0
  case LUA_TTABLE:
    lua_pushnil(L); /* first key */
    while(lua_next(L, args_pos)) {
      //uint8_t value = lua_tonumber(L, -1);
      //string key    = lua_tostring(L, -2);
      switch (lua_type(L, -1)) {  // process only the value
      case LUA_TSTRING:
        json_list_add_string(&jx, lua_tostring(L,-1));
        break;
      case LUA_TNUMBER:
        json_list_add_number(&jx, lua_tonumber(L,-1));
        break;
      case LUA_TBOOLEAN:
        json_list_add_bool(&jx, lua_toboolean(L,-1));
        break;
      case LUA_TNIL:
        json_list_add_null(&jx);
        break;
      case LUA_TTABLE:  // also object?
        ...  recursive
        json_list_add_list(&jx, lua_...(L,-1));
        break;
      default:
        ...
      }
      lua_pop(L, 1); /* pop value, keeps key */
    }
    lua_pop(L, 1); /* pop key */
    args = json_xx(&jx);
#endif
    break;
  default:
    args = "invalid";
  }

  return args;
}
