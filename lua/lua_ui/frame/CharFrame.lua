local layoutName = 'CharWindow.layout'

ui.defineFrame(..., layoutName)
function Init(self)
    self.name = 'char window'
    self.event = 'click event'
    self.char = 'robot'
    --print(self._name_ .. ':' .. self.name .. ': ' ..self.layout)
end

function UnInit(self)
    --print('uninit ' .. self.name)
    
end

function Subscribe(self)
    --print(self.name .. ' subscribe ' .. self.event)
    self:SubscribeEvents()
end

function ShowChar(self)
    --print(self.name .. ', char is ' .. self.char)
    self:Show()
end