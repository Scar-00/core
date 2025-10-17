---@param build Build
return function (build)
    ---@type ToolChain
    local tool_chain = "Clang";
    local warnings = { "Error", "Pedantic", "All", "Extra" };
    local no_warnings = { "DeprecatedDeclarations" };
    local custom = {};
    local libs = {};

    if tool_chain == "Clang" and build:host_os() == "Linux" then
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
        files = { "test.c" },
        output = "test",
        includes = {},
        libs = libs,
        args = { warnings = warnings, no_warnings = no_warnings, custom = custom }
    });
    local test_exe = test:build_and_install();
    if not test_exe then
        error(test_exe);
    end
    if build:wants_run() then
        build:run(test_exe, { "test", "foo", "bar" });
    end
end
