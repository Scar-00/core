---@param build Build
return function (build)
    ---@type ToolChain
    local tool_chain = "Clang";
    local warnings = { "Error", "Pedantic", "All", "Extra" };
    local no_warnings = { "DeprecatedDeclarations" };
    local custom = { "-ggdb", "-std=c23" };
    local libs = {};
    local includes = { "." };

    if tool_chain == "Clang" and build:host_os() == "Linux" then
        custom = { "-fsanitize=memory", "-ggdb", "-std=c23" };
    end

    if build:host_os() == "Windows" then
        libs = { "-luser32" };
    end

    if tool_chain == "Msvc" then
        warnings = {};
        no_warnings = {};
    end

    local test = build:add_binary({
        name = "test_core",
        tool_chain = tool_chain,
        opt_level = build:default_opt_level(),
        files = { "./test/test_core.c" },
        output = "test_core",
        includes = includes,
        libs = libs,
        args = { warnings = warnings, no_warnings = no_warnings, custom = custom }
    });
    if build:should_generate_database() then
        build:generate_database();
    end
    local test_exe = test:build_and_install();
    if not test_exe then
        error("failed to build `test_core`");
    end
    if build:wants_run() then
        build:run(test_exe, {});
    end
end
