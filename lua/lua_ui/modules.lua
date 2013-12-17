local modname = ...

module(modname, package.seeall)

_M._modules = {}

function load(name)
    unLoad(name)
    _modules[name] = true
    require(name)
end

function unLoad(name)
    if _modules[name] then
        _modules[name] = false
        package.loaded[name] = nil
        _G[name] = nil
    end
end

function loadList(list)
    for i, v in ipairs(list) do
        load(v)
    end
end

function unLoadAll()
    for k, v in pairs(_modules) do
        unLoad(k)
    end
    _modules = nil
end