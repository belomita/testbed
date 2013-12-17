--
package.path='./frame/?.lua;' .. package.path

function Init()
    require('modules')
    
    local ui_module = 'ui'
    local frame_modules = {'MapFrame', 'CharFrame'}
    
    modules.load(ui_module)
    modules.loadList(frame_modules)
end

function UnInit()
    modules.unLoadAll()
    package.loaded['modules'] = nil
    _G['modules'] = nil
end

function LoadUI()
    --ui.createFrame('MapFrame')
    --ui.createFrame('CharFrame', '1')
    ui.createFrames()
end

function UnLoadUI()
    ui.destroyFrames()
end

function RunTest()
    --ui.MapFrame:PrintPosition()
    --ui.CharFrame:ShowChar()
end

function print_indent(num)
    if num < 1 then return end
    for i = 1, num do io.write('\t') end
end

function stat(name, tab, depth)
    print_indent(depth)
    io.write(name .. '= {\n')
    for k, v in pairs(tab) do
        if type(v) == 'table' then
            if depth < 3 and name ~= k then
                stat(k, v, depth + 1)
            else
                print_indent(depth + 1)
                io.write(tostring(k) .. ' = "' .. tostring(v) .. '",\n')
            end
        else
            print_indent(depth + 1)
            io.write(tostring(k) .. ' = "' .. tostring(v)  .. '",\n')
        end
    end
    print_indent(depth)
    io.write('}, \n')
end


function inv_state(inv_dict, name, key, tab, depth)
    for k, v in pairs(tab) do
        if not inv_dict[tostring(v)] then
            inv_dict[tostring(v)] =  {}
        end
        local sub_dict = inv_dict[tostring(v)]
        local key_name = name .. '.' ..tostring(k)
        
        sub_dict[key_name] = true
                
        if type(v) == 'table' then
            if depth < 3 and k ~= key then
                inv_state(inv_dict, key_name, k, v, depth + 1)
            end
        end
    end

end

function Run()
    Init()
    LoadUI()
    print(os.date('%Y-%m-%d %H:%M:%S'))
    -- local inv_dict = {}
    -- inv_state(inv_dict, '_G', '_G', _G, 0)
    -- for k, v in pairs(inv_dict) do
        -- print(k .. ':')
        -- for subk, _ in pairs(v) do 
            -- print('\t' .. subk)
        -- end
    -- end
    
    local cwindow = CWindow('test')
    --cwindow =  nil
    UnLoadUI()
    UnInit()
    collectgarbage()
    print ('-------------------')
    -- inv_dict = {}
    -- inv_state(inv_dict, '_G', '_G', _G, 0)
    -- for k, v in pairs(inv_dict) do
        -- print(k .. ':')
        -- for subk, _ in pairs(v) do 
            -- print('\t' .. subk)
        -- end
    -- end
end

Run()