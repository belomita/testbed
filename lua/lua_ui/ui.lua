local modname = ...

module(modname, package.seeall)

_M.frame_classes = {}
_M.frame_instances = {}

function defineFrame(frame_name, layout_name)
    local _module = _M
    local _frame = {}
    _frame.layout = layout_name
    
    setmetatable(_frame, { __index = _G })
    frame_classes[frame_name] = _frame
    
    setfenv(2, _frame)
end

function createFrame(frame_name, ...)
    local _module = _M
    local frame_class = frame_classes[frame_name]
    
    -- Create instance from class
    local frame_instance = {}
    setmetatable(frame_instance, {__index = frame_class})
    
    -- Attatch to c++ object
    local win_ptr = UIUtility.CreateCWindow(frame_class.layout)
    tolua.setpeer(win_ptr, frame_instance)
    frame_instance = win_ptr
    
    -- Store instance's name
    if arg.n == 0 then
        frame_instance._name_ = frame_name
    elseif type(arg[1]) == 'string' then
        local new_name = frame_name..arg[1]
        frame_instance._name_ = new_name
    end
    -- Export to ui module, thus can refer like ui.XXFrame
    _module[frame_instance._name_] = frame_instance

    -- Store for management
    frame_instances[frame_instance._name_] = frame_instance
    
    -- Call Init method if defined
    if frame_instance.Init then
        frame_instance:Init()
    end
    
    -- Call Subscribe if defined
    if frame_instance.Subscribe then
        frame_instance:Subscribe()
    end
    
    return frame_instance
end

function createFrames()
    for k, v in pairs(frame_classes) do
        createFrame(k)
    end
end

function destroyFrames()
    local _module = _M
    for k, v in pairs(frame_instances) do
        UIUtility.DestroyCWindow(v)
        _module[k] = nil
        frame_instances = nil
    end
end

