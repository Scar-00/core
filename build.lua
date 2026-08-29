local warnings = { "Error", "Pedantic", "All", "Extra" };
local no_warnings = { "DeprecatedDeclarations" };
local custom = { "-ggdb", "-std=c23" };
local includes = { "." };

ExamplesDir = "./examples/";

---@param build Build
---@param example string
---@return JoinHandle
local function build_example(build, example)
    local file = ExamplesDir .. example .. ".c";
    local binary = build:add_binary({
        name = example,
        tool_chain = build:default_toolchain(),
        opt_level = build:default_opt_level(),
        files = { file },
        output = ExamplesDir .. example,
        includes = includes,
        args = {
            warnings = warnings,
            no_warnings = no_warnings,
            custom = custom,
        }
    });
    return binary:build();
end

---@param build Build
---@param targets string[]
---@return BuildArtifact[]
local function build_examples(build, targets)
    local tasks = {};
    for _, example in pairs(targets) do
        local artifact = build_example(build, example);
        table.insert(tasks, artifact);
    end
    local artifacts = {};
    for _, task in pairs(tasks) do
        table.insert(artifacts, build:install(task));
    end
    return artifacts;
end

---@param s string
---@return string
function TrimExt(s)
    local sub_str, _ = s:gsub("%.c", "");;
    return sub_str;
end

---@param build Build
---@return string[]
function GetExamples(build)
    local examples = build.read_dir(ExamplesDir, "c");
    local trimmed = {}
    for key, _ in pairs(examples) do
        local t = TrimExt(examples[key]);
        trimmed[key] = t;
    end
    return trimmed;
end

---@param build Build
return function (build)
    local examples = GetExamples(build);

    if build:wants_run() then
        local args = build:unused_cli_args();
        if #args < 1 then
            local message = "no examples provided to run\n";
            message = message .. "available examples:\n";
            for key, value in pairs(examples) do
                message = message .. "    " .. value;
                if key < #examples then
                    message = message .. "\n";
                end
            end
            error(message, 4);
        end

        local target = args[1];
        table.remove(args, 1);
        local is_valid = false;
        for _, example in pairs(examples) do
            if example == target then
                is_valid = true;
                break;
            end
        end
        if not is_valid and target ~= "--all" then
            local message = "`" .. target .. "` is not a valid example\n";
            message = message .. "available examples:\n";
            for key, value in pairs(examples) do
                message = message .. "    " .. value;
                if key < #examples then
                    message = message .. "\n";
                end
            end
            error(message, 4);
        end
        if target == "--all" then
            local artifacts = build_examples(build, examples);
            for _, artifact in pairs(artifacts) do
                build:run(artifact, args);
            end
        else
            local task = build_example(build, target);
            local artifact = build:install(task);
            if not artifact then
                error("failed to build example `" .. target .. "`", 3);
            end
            build:run(artifact, args);
        end
    else
        build_examples(build, examples);
    end
end
