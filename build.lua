---@alias ToolChain "Msvc" | "Gcc" | "Clang" | "Zig" | { compiler: string, linker: string }
---@alias BinaryType "Executable" | "DynLib" | "StaticLib"
---@alias ErrorFlag "Error" | "Pedantic" | "Extra" | "All" | "DeprecatedDeclarations"
---@alias OptimizationLevel "Debug" | "Release" | "O0" | "O1" | "O2" | "O3" | "OSize"
---@alias Os "Windows" | "Linux" | "MacOs" | "UnixLike"

---@class Args
---@field warnings ?ErrorFlag[]
---@field no_warnings ?ErrorFlag[]
---@field custom ?string[]

---@class JoinHandle

---@class Binary
---@field build async fun(self: Binary): JoinHandle
---@field build_and_install async fun(self: Binary): string?

---@class Graph
---@field tool_chain ToolChain
---@field opt_level OptimizationLevel
---@field type ?BinaryType
---@field files string[]
---@field output ?string
---@field src_dir ?string
---@field includes ?string[]
---@field lib_paths ?string[]
---@field libs ?string[]
---@field args ?Args
---@field excludes ?string[]

---@class CmdOutput
---@field stdout string
---@field stderr string

---@class Build
---@field add_binary fun(self: Build, binary: Graph): Binary
---@field install async fun(self: Build, join_handle: JoinHandle): string?
---@field default_toolchain fun(self: Build): ToolChain
---@field default_opt_level fun(self: Build): OptimizationLevel
---@field wants_run fun(self: Build): boolean
---@field run async fun(self: Build, binary: string, args: string[]?): CmdOutput?
---@field host_os fun(self: Build): Os

---@param build Build
return function (build)
    ---@type ToolChain
    local tool_chain = build:default_toolchain();
    local warnings = { "Error", "Pedantic", "All", "Extra" };
    local no_warnings = { "DeprecatedDeclarations" };
    local custom = {};
    local libs = {};

    if tool_chain == "Clang" and build:host_os() == "Unix" then
        custom = { "-fsanitize=memory" };
    end

    if tool_chain == "Msvc" then
        warnings = {};
        no_warnings = {};
        libs = { "-luser32" };
    end

    local test = build:add_binary({
        tool_chain = tool_chain,
        opt_level = build:default_opt_level(),
        files = {
            "test.c"
        },
        output = "test",
        includes = {},
        libs = libs,
        args = {
            warnings = warnings,
            no_warnings = no_warnings,
            custom = custom
        }
    });
    local test_exe = test:build_and_install();
    if not test_exe then
        error(test_exe);
    end
    if build:wants_run() then
        build:run(test_exe, { "test", "foo", "bar" });
    end
end
