local layoutName = 'MapWindow.layout'

ui.defineFrame(..., layoutName)
function Init(self)
    self.name = 'map window'
    self.event = 'update event'
    --print(self._name_ .. ':' .. self.name .. ': ' ..self.layout)
end

function UnInit(self)
    --print('uninit ' .. self.name)
end

function Subscribe(self)
    --print(self.name .. ' subscribe ' .. self.event)
end

function PrintPosition(self)
    --print(self.name .. ', position: (35, 60)')

end