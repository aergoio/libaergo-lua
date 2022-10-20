require "aergo"

function to_hex(data)
  local res = ""
  for i = 1, #data do
    char = string.sub(data, i, i)
    res = res .. string.format("%02x", string.byte(char))
  end
  return res
end

if aergo.connect("testnet-api.aergo.io", 7845) == false then
  error("Error connecting to aergo network")
end

print("Connected. Sending request...")

local contract_address = "AmhcceopRiU7r3Gwy5tmtkk4Z3Px53SfsKBifGMvaSSNiyWrvKYe"

local response = aergo.query(contract_address, "hello")
print("------------------------------------")
print("Smart Contract Query ", response.success and "OK" or "FAILED")
print("Response:", response.result)
print("------------------------------------")

local privkey = "\xDB\x85\xDD\x0C\xBA\x47\x32\xA1\x1A\xEB\x3C\x7C\x48\x91\xFB\xD2\xFE\xC4\x5F\xC7\x2D\xB3\x3F\xB6\x1F\x31\xEB\x57\xE7\x24\x61\x76"

local account = aergo.get_account_state(false, privkey)
print("------------------------------------")
print("use_ledger: ", account.use_ledger)
print("index: ", account.index)
print("privkey: ", to_hex(account.privkey))
print("pubkey: ", to_hex(account.pubkey))
print("address: ", account.address)
print("nonce: ", account.nonce)
print("balance: ", account.balance)
print("state_root: ", account.state_root)
print("------------------------------------")

local receipt = aergo.call(account, contract_address, 'set_name', '["Lua!"]')
print("------------------------------------")
print("status: ", receipt.status)
print("ret: ", receipt.ret)
print("blockNo: ", receipt.blockNo)
print("blockHash: ", to_hex(receipt.blockHash))
print("txIndex: ", receipt.txIndex)
print("txHash: ", to_hex(receipt.txHash))
print("contractAddress: ", receipt.contractAddress)
print("gasUsed: ", receipt.gasUsed)
print("feeUsed: ", receipt.feeUsed)
print("feeDelegation: ", receipt.feeDelegation)
print("------------------------------------")
print("nonce: ", account.nonce)
print("------------------------------------")

local account = aergo.get_account_state(false, privkey)
print("use_ledger: ", account.use_ledger)
print("index: ", account.index)
print("privkey: ", to_hex(account.privkey))
print("pubkey: ", to_hex(account.pubkey))
print("address: ", account.address)
print("nonce: ", account.nonce)
print("balance: ", account.balance)
print("state_root: ", account.state_root)
print("------------------------------------")

local response = aergo.query(contract_address, "hello")
print("------------------------------------")
print("Smart Contract Query ", response.success and "OK" or "FAILED")
print("Response:", response.result)
print("------------------------------------")
