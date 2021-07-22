---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by sergey.
--- DateTime: 31/05/2021 14:14
---

local DEBUG_MODE = "DEBUG"
local PROD_MODE = "PROD"

RUN_MODE = PROD_MODE
WORKING_DIRECTORY = getScriptPath()

if RUN_MODE == PROD_MODE then
    package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\?.dll;"
    package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\cmake-build-release\\?.dll;"
else
    package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\?.dll;"
    package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\cmake-build-debug\\?.dll;"
end

require("quik_connector")
