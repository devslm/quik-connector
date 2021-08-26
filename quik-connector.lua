---
--- Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
---

local DEBUG_MODE = "DEBUG"
local RELEASE_MODE = "RELEASE"

RUN_MODE = RELEASE_MODE
WORKING_DIRECTORY = getScriptPath()

package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\bin\\?.dll;"

if RUN_MODE == RELEASE_MODE then
    package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\cmake-build-release\\?.dll;"
else
    package.cpath = package.cpath .. ";" .. WORKING_DIRECTORY .. "\\cmake-build-debug\\?.dll;"
end

require("quik_connector")
