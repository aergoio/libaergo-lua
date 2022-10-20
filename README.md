# libaergo-lua

Library to interface with the Aergo blockchain in Lua


## Installation

First [install libaergo](https://github.com/aergoio/libaergo#dependencies)

Then install this library:

```
git clone https://github.com/aergoio/libaergo-lua
cd libaergo-lua
make
sudo make install
```

## Usage

Here is an example connecting to the Aergo test network:

```lua
require "aergo"

if aergo.connect("testnet-api.aergo.io", 7845) == false then
  error("Error connecting to aergo network")
end
```

After the connection is done it is possible to interface with smart contracts.

There are 2 methods to execute functions on them:

* aergo.query
* aergo.call

**Queries** are used to execute read-only functions, those that do not change the contract state.

There is no need for an account to do that.

Example:

```lua
local contract_address = "AmhcceopRiU7r3Gwy5tmtkk4Z3Px53SfsKBifGMvaSSNiyWrvKYe"

local response = aergo.query(contract_address, "hello")

print("Smart Contract Query ", response.success and "OK" or "FAILED")
print("Response:", response.result)
```

**Calls** are used to execute functions that change the contract state.

These calls are made using transactions. They require an account with funds (native aergo tokens).

There are 2 ways to have accounts:

* Using a secret (private) key on your app
* Using hardware wallet (Ledger Nano)

Before making a call we need to get the account state.

Example with a private key:

```lua
local privkey = "\xDB\x85\xDD\x0C\xBA\x47\x32\xA1\x1A\xEB\x3C\x7C\x48\x91\xFB\xD2\xFE\xC4\x5F\xC7\x2D\xB3\x3F\xB6\x1F\x31\xEB\x57\xE7\x24\x61\x76"

local account = aergo.get_account_state(false, privkey)

print("address: ", account.address)
print("nonce: ", account.nonce)
print("balance: ", account.balance)
```

Example with a hardware wallet:

```lua
local account = aergo.get_account_state(true, 0)

print("address: ", account.address)
print("nonce: ", account.nonce)
print("balance: ", account.balance)
```

Afther the account is initialized it is possible to make a call to a function on the smart contract:

```lua
local receipt = aergo.call(account, contract_address, 'set_name', '["Lua!"]')

print("status: ", receipt.status)
print("returned: ", receipt.ret)
print("blockNo: ", receipt.blockNo)
print("blockHash: ", to_hex(receipt.blockHash))
print("txIndex: ", receipt.txIndex)
print("txHash: ", to_hex(receipt.txHash))
print("contractAddress: ", receipt.contractAddress)
print("gasUsed: ", receipt.gasUsed)
print("feeUsed: ", receipt.feeUsed)
```


### Working example

You can check an example code in [test.lua](test.lua)
