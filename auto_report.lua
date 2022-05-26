local cond_clk = require('io.github.USN484259.cond_clk')


local load_failed = cond_clk.image("load_failed.png")


local login_page = cond_clk.image("login_page.png")

login_page.region( {top = 310,bottom = 660,left = 1100,right = 1450,off = 24} )

--local login_blocked = cond_clk.image("login_blocked.png")

--login_blocked.region( {top = 570,bottom = 630,left = 1150,right = 1330,off = 8 } )

local table_loading = cond_clk.image("table_loading.png")

table_loading.region( {top = 400,bottom = 700,left = 400,right = 600,off = 8} )

local table_page = cond_clk.image("table_page.png")

table_page.region( {top = 280,bottom = 370,left = 535,right = 1445,off = 24} )

local function cmp(off)
	return function(a,b)
		local diff = 0
		for i = 1,3,1 do
			diff = diff + math.abs(a[i] - b[i])
		
		end
		
		if diff > off then return false end
		
		return true;
		
	end
end

local function type_string(str,delay)
	for c in string.gmatch(str,".") do
		if delay then cond_clk.sleep(delay) end
		cond_clk.type(c)
	end
end

local last_image


local start,stop,wait_load,login,fill_table,submit,success,fail

local function mail_report(person,message)
	print("sending email")
	local log_file = "log/"..string.format("%x",os.time())..".log"
	local cmd = "java -jar email_sender.jar -d "..person.." -s "..message.." -a "..last_image--.." >>"..log_file.." 2>>&1"
	print(cmd)
	os.execute(cmd)
end

--[[
local function QQ_report(message)
	print("sending QQ")
	
	local qq = require('qq_report')
	qq.report(message)
	
	--os.execute("start cond_clk.exe qq_report.lua")

end
--]]
local attempt_count = 0
start = function ()
	attempt_count = attempt_count + 1

	print("start "..attempt_count)
	
	
	if attempt_count > 20 then
		return fail
	end
	
	os.execute("xdg-open http://example.com")
	cond_clk.sleep(5000)
	return wait_load

end

stop = function ()
	print("stop")

	
end



success = function()
	print("success")
	
	--mail_report(mail_target,"success test")
	--QQ_report('test')
	
	cond_clk.sleep(5000)
	--cond_clk.click(1895,15)
	cond_clk.click(1905,47)
	--os.execute("shutdown -s -t 120")
	--os.execute("shutdown")
	return stop
end

fail = function()
	print("fail")
	--mail_report(mail_target,"fail test")

	return stop

end


local fail_count = 0
wait_load = function ()
	print("wait_load "..fail_count)
	cond_clk.move(200,250)
	cond_clk.sleep(10000)
	local img = cond_clk.capture()
	
	last_image = "log/"..string.format("%x",os.time())..".png"

	img.save(last_image)
	
	load_failed.region( {top = 70,bottom = 95,left = 145,right = 170,off = 8} )

	local res = img.match(load_failed,cmp(8))
	print('cross '..res)
	if res < 0.98 then
		fail_count = fail_count+1
		if fail_count > 5 then
			fail_count = 0
			return start
		end
		return
	end
	
	load_failed.region( {top = 420,bottom = 760,left = 630,right = 1390,off = 8} )

	res = img.match(load_failed,cmp(8))
	print('failed '..res)
	if res > 0.98 then
		fail_count = fail_count+1
		if fail_count > 5 then
			fail_count = 0
			return start
		end
		cond_clk.click(500,120)
		cond_clk.sleep(500)
		cond_clk.type("\n")
		return
	end
	res = img.match(table_loading,cmp(8))
	print('table_loading '..res)
	if res > 0.98 then
		fail_count = fail_count+1
		if fail_count > 5 then
			fail_count = 0
			return start
		end
		return
	end
	
	fail_count = 0
	
	res = img.match(login_page,cmp(24))
	print('login '..res)
	if res > 0.98 then return login end
	
	--res = img.match(login_blocked,cmp(8))
	--print('blocked '..res)
	--if res > 0.99 then return fail end
	
	res = img.match(table_page,cmp(24))
	print('table '..res)
	if res > 0.98 then return fill_table end
	
	
	return start
end


login = function ()
	print("login")

	cond_clk.click(1200,450)
	cond_clk.sleep(200)
	cond_clk.click(1200,450)
	cond_clk.sleep(1000)
	type_string("username",20)
	cond_clk.sleep(500)
	cond_clk.click(1200,505)
	cond_clk.sleep(1000)
	type_string("password",20)
	cond_clk.sleep(500)
	cond_clk.click(1380,565)
	return wait_load
end

fill_table = function()
	print("fill_table")
	
	cond_clk.click(300,240)
	cond_clk.sleep(500)
	cond_clk.scroll(100)
	cond_clk.sleep(2000)
	
	cond_clk.click(740,895)
	cond_clk.sleep(500)
	cond_clk.click(150,120)
	
	return submit


end


local note_popup = cond_clk.image("note_popup.png")

note_popup.region( {top = 510,bottom = 670,left = 682,right = 1300,off = 8} )

local success_popup = cond_clk.image("success_popup.png")

success_popup.region( {top = 530,bottom = 655,left = 772,right = 1210,off = 8} )

local failed_popup = cond_clk.image("failed_popup.png")

failed_popup.region( {top = 495,bottom = 690,left = 592,right = 1390,off = 8} )

submit = function ()
	print("submit "..fail_count)
	
	
	if fail_count > 5 then
		fail_count = 0
		return start
	end
	
	cond_clk.sleep(5000)
	
	local img = cond_clk.capture()
	
	last_image = "log/"..string.format("%x",os.time())..".png"
	
	img.save(last_image)
	
	
	
	local res = img.match(note_popup,cmp(8))
	print('note '..res)
	if res > 0.995 then
		cond_clk.click(1240,640)
		return
	end
	
	res = img.match(failed_popup,cmp(8))
	print('failed '..res)
	if res > 0.995 then
		cond_clk.click(1330,660)
		cond_clk.sleep(500)
		cond_clk.click(150,120)
	end	
	
	res = img.match(success_popup,cmp(8))
	print('success '..res)
	if res > 0.995 then
		fail_count = 0
		return success
	end
	

	
	
	fail_count = fail_count + 1
	
end


local function main()
	print("auto report")
	print("USN484259 2020.2.6")
	
	local state = start
	
	repeat
	
		local st = state(arg)
		if st ~= nil then
			state = st
		end
	
	until state == stop

end



local lvm_print = print
local log_file = io.open('log/'..string.format("%x",os.time())..'.log','w')

print = function(...)
	log_file:write(...)
	log_file:write('\n')
	lvm_print(...)
end

main()

log_file:close()
print = lvm_print
