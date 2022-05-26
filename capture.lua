local cond_clk = require('io.github.USN484259.cond_clk')

cond_clk.sleep(3000)
local img = cond_clk.capture()
img.save("capture.png")

cond_clk.sleep(1000)
img.view()
