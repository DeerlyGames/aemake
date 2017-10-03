local p = premake

local gcc = p.tools.gcc

	function string.ends(String,End)
	   return End=='' or string.sub(String,-string.len(End))==End
	end

	function p.ls(_path)
		if os.host() == "linux" or os.host() == "macosx" then
			return io.popen('ls "'.._path..'"')
		elseif os.host() == "windows" then
			return io.popen('dir /b /on "'.._path..'"')
		end
	end

	function p.GetAndroidGCCToolsFromPrebuilt(_path)
		for folder in p.ls( _path ):lines() do
			_path=_path..folder.."/bin/"
			break
		end
		for folder in p.ls( _path ):lines() do
			if string.find(folder, "ar", -8) and gcc.tools['ar']=="ar" then
				gcc.tools['ar']="".._path..folder..""
			end
			if string.find(folder, "gcc", -8) and gcc.tools['cc']=="gcc" then
				gcc.tools['cc']="".._path..folder..""
			end
			if string.find(folder, "-g++", -8) and gcc.tools['cxx']=="g++" then
				gcc.tools['cxx']="".._path..folder..""
			end				
		end
	end	

	gcc.tools = {
		cc = "gcc",
		cxx = "g++",
		ar = "ar",
		rc = "windres"
	}

	if (NDK_HOME) then		
		local NDK_BUILD = NDK_HOME.."/toolchains/"

		if _OPTIONS["target"]=="Android32" then
			for folder in p.ls( NDK_BUILD ):lines() do
				if string.find(folder, "86", -6) then
					if (os.isdir(NDK_BUILD..folder.."/prebuilt")) then
						NDK_BUILD=NDK_BUILD..folder.."/prebuilt/"
						break
					end
				end
			end
			p.GetAndroidGCCToolsFromPrebuilt(NDK_BUILD)
			
		elseif _OPTIONS["platform"]=="Android64" then
			for folder in p.ls( NDK_BUILD ):lines() do
				if string.find(folder, "x86_64") then
					if (os.isdir(NDK_BUILD..folder.."/prebuilt")) then
						NDK_BUILD=NDK_BUILD..folder.."/prebuilt/"
						break
					end
				end
			end
			p.GetAndroidGCCToolsFromPrebuilt(NDK_BUILD)
			
		elseif _OPTIONS["target"]=="AndroidArm7" then
			for folder in p.ls( NDK_BUILD ):lines() do
				if string.find(folder, "arm") then
					if (os.isdir(NDK_BUILD..folder.."/prebuilt")) then
						NDK_BUILD=NDK_BUILD..folder.."/prebuilt/"
						break
					end
				end
			end
			p.GetAndroidGCCToolsFromPrebuilt(NDK_BUILD)

		elseif _OPTIONS["target"]=="AndroidArm8" then
			for folder in p.ls( NDK_BUILD ):lines() do
				if string.find(folder, "aarch64") then
					if (os.isdir(NDK_BUILD..folder.."/prebuilt")) then
						NDK_BUILD=NDK_BUILD..folder.."/prebuilt/"
						break
					end
				end
			end
			p.GetAndroidGCCToolsFromPrebuilt(NDK_BUILD)

		elseif _OPTIONS["target"]=="AndroidMips32" then
			for folder in p.ls( NDK_BUILD ):lines() do
				if string.find(folder, "mipsel") then
					if (os.isdir(NDK_BUILD..folder.."/prebuilt")) then
						NDK_BUILD=NDK_BUILD..folder.."/prebuilt/"
						break
					end
				end
			end
			p.GetAndroidGCCToolsFromPrebuilt(NDK_BUILD)

		elseif _OPTIONS["target"]=="AndroidMips64" then
			for folder in p.ls( NDK_BUILD ):lines() do
				if string.find(folder, "mips64el") then
					if (os.isdir(NDK_BUILD..folder.."/prebuilt")) then
						NDK_BUILD=NDK_BUILD..folder.."/prebuilt/"
						break
					end
				end
			end
			p.GetAndroidGCCToolsFromPrebuilt(NDK_BUILD)
		end	
	end

function gcc.gettoolname(cfg, tool)
	-- Appending sysroot for crt linking
	if string.contains(_OPTIONS["target"], "Android") then
		local ANDROID_ARCH=""
		if string.contains(_OPTIONS["target"], "Android32") then 
			ANDROID_ARCH="arch-x86"
		elseif string.contains(_OPTIONS["target"], "Android64") then
			ANDROID_ARCH="arch-x86_64"
		elseif string.contains(_OPTIONS["target"], "AndroidArm7") then
			ANDROID_ARCH="arch-arm"		
		elseif string.contains(_OPTIONS["target"], "AndroidArm8") then
			ANDROID_ARCH="arch-arm64"	
		elseif string.contains(_OPTIONS["target"], "AndroidMips32") then
			ANDROID_ARCH="arch-mips"	
		elseif string.contains(_OPTIONS["target"], "AndroidMips64") then
			ANDROID_ARCH="arch-mips64"									
		end
		if (not string.contains(gcc.tools['cc'], "--sysroot")) then
			gcc.tools['cc']=gcc.tools['cc'].." --sysroot="..NDK_HOME.."/platforms/"..cfg.system.."-"..cfg.abi.."/"..ANDROID_ARCH
			gcc.tools['cxx']=gcc.tools['cxx'].." --sysroot="..NDK_HOME.."/platforms/"..cfg.system.."-"..cfg.abi.."/"..ANDROID_ARCH
		end
	end
	return gcc.tools[tool]
end