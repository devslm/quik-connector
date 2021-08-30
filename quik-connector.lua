---
--- Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
---

local DEBUG_MODE = "DEBUG"
local RELEASE_MODE = "RELEASE"
local DLL_NAME = "quik-connector.dll"

RUN_MODE = DEBUG_MODE
WORKING_DIRECTORY = getScriptPath()

package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\bin\\" .. DLL_NAME .. ";"

if RUN_MODE == RELEASE_MODE then
    package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\cmake-build-release\\" .. DLL_NAME .. ";"
else
    package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\cmake-build-debug\\" .. DLL_NAME .. ";"
end

require("quik_connector")
