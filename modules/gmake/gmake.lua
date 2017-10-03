--
-- gmake.lua
-- Define the makefile action(s).
-- Copyright (c) 2002-2015 Jason Perkins and the Premake project
--

	local p = premake

	p.modules.gmake = {}
	p.modules.gmake._VERSION = p._VERSION

	-- for backwards compatibility.
	p.make = p.modules.gmake

	local make = p.make
	local project = p.project

--
-- Write out the default configuration rule for a workspace or project.
--
-- @param target
--    The workspace or project object for which a makefile is being generated.
--

	function make.defaultconfig(target)
		-- find the right configuration iterator function for this object
		local eachconfig = iif(target.project, project.eachconfig, p.workspace.eachconfig)
		local iter = eachconfig(target)

		-- grab the first configuration and write the block
		local cfg = iter()
		if cfg then
			_p('ifndef config')
			_x('  config=%s', cfg.shortname)
			_p('endif')
			_p('')
		end
	end


---
-- Escape a string so it can be written to a makefile.
---

	function make.esc(value)
		result = value:gsub("\\", "\\\\")
		result = result:gsub("\"", "\\\"")
		result = result:gsub(" ", "\\ ")
		result = result:gsub("%(", "\\(")
		result = result:gsub("%)", "\\)")

		-- leave $(...) shell replacement sequences alone
		result = result:gsub("$\\%((.-)\\%)", "$(%1)")
		return result
	end


--
-- Get the makefile file name for a workspace or a project. If this object is the
-- only one writing to a location then I can use "Makefile". If more than one object
-- writes to the same location I use name + ".make" to keep it unique.
--

	function make.getmakefilename(this, searchprjs)
		local count = 0
		for wks in p.global.eachWorkspace() do
			if wks.location == this.location then
				count = count + 1
			end

			if searchprjs then
				for _, prj in ipairs(wks.projects) do
					if prj.location == this.location then
						count = count + 1
					end
				end
			end
		end

		if count == 1 then
			return "Makefile"
		else
			return ".make"
		end
	end


--
-- Output a makefile header.
--
-- @param target
--    The workspace or project object for which the makefile is being generated.
--

	function make.header(target)
		local kind = iif(target.project, "project", "workspace")

		_p('# %s %s makefile autogenerated by Premake', p.action.current().shortname, kind)
		_p('')

		if kind == "workspace" then
			local haspch = false
			for _, prj in ipairs(target.projects) do
				for cfg in project.eachconfig(prj) do
					if cfg.pchheader then
						haspch = true
					end
				end
			end

			if haspch then
				_p('.NOTPARALLEL:')
				_p('')
			end
		end

		make.defaultconfig(target)

		_p('ifndef verbose')
		_p('  SILENT = @')
		_p('endif')
		_p('')
	end


--
-- Rules for file ops based on the shell type. Can't use defines and $@ because
-- it screws up the escaping of spaces and parethesis (anyone know a fix?)
--

	function make.mkdir(dirname)
		_p('ifeq (posix,$(SHELLTYPE))')
		_p('\t$(SILENT) mkdir -p %s', dirname)
		_p('else')
		_p('\t$(SILENT) if not exist $(subst /,\\\\,%s) mkdir $(subst /,\\\\,%s)', dirname, dirname)
		_p('endif')
	end

	function make.mkdirRules(dirname)
		_p('%s:', dirname)
		_p('\t@echo Creating %s', dirname)
		make.mkdir(dirname)
		_p('')
	end

--
-- Format a list of values to be safely written as part of a variable assignment.
--

	function make.list(value, quoted)
		quoted = false
		if #value > 0 then
			if quoted then
				local result = ""
				for _, v in ipairs (value) do
					if #result then
						result = result .. " "
					end
					result = result .. p.quoted(v)
				end
				return result
			else
				return " " .. table.concat(value, " ")
			end
		else
			return ""
		end
	end


--
-- Convert an arbitrary string (project name) to a make variable name.
--

	function make.tovar(value)
		value = value:gsub("[ -]", "_")
		value = value:gsub("[()]", "")
		return value
	end


---------------------------------------------------------------------------
--
-- Handlers for the individual makefile elements that can be shared
-- between the different language projects.
--
---------------------------------------------------------------------------

	function make.objdir(cfg)
		_x('  OBJDIR = %s', p.esc(project.getrelative(cfg.project, cfg.objdir)))
	end


	function make.objDirRules(prj)
		make.mkdirRules("$(OBJDIR)")
	end


	function make.phonyRules(prj)
		_p('.PHONY: clean prebuild prelink')
		_p('')
	end


	function make.buildCmds(cfg, event)
		_p('  define %sCMDS', event:upper())
		local steps = cfg[event .. "commands"]
		local msg = cfg[event .. "message"]
		if #steps > 0 then
			steps = os.translateCommandsAndPaths(steps, cfg.project.basedir, cfg.project.location)
			msg = msg or string.format("Running %s commands", event)
			_p('\t@echo %s', msg)
			_p('\t%s', table.implode(steps, "", "", "\n\t"))
		end
		_p('  endef')
	end


	function make.preBuildCmds(cfg, toolset)
		make.buildCmds(cfg, "prebuild")
	end


	function make.preBuildRules(prj)
		_p('prebuild:')
		_p('\t$(PREBUILDCMDS)')
		_p('')
	end


	function make.preLinkCmds(cfg, toolset)
		make.buildCmds(cfg, "prelink")
	end


	function make.preLinkRules(prj)
		_p('prelink:')
		_p('\t$(PRELINKCMDS)')
		_p('')
	end


	function make.postBuildCmds(cfg, toolset)
		make.buildCmds(cfg, "postbuild")
	end


	function make.settings(cfg, toolset)
		if #cfg.makesettings > 0 then
			for _, value in ipairs(cfg.makesettings) do
				_p(value)
			end
		end

		local value = toolset.getmakesettings(cfg)
		if value then
			_p(value)
		end
	end


	function make.shellType()
		_p('SHELLTYPE := msdos')
		_p('ifeq (,$(ComSpec)$(COMSPEC))')
		_p('  SHELLTYPE := posix')
		_p('endif')
		_p('ifeq (/bin,$(findstring /bin,$(SHELL)))')
		_p('  SHELLTYPE := posix')
		_p('endif')
		_p('')
	end


	function make.target(cfg)
		_x('  TARGETDIR = %s', project.getrelative(cfg.project, cfg.buildtarget.directory))
		_x('  TARGET = $(TARGETDIR)/%s', cfg.buildtarget.name)
	end


	function make.targetDirRules(prj)
		make.mkdirRules("$(TARGETDIR)")
	end


	include("gmake_cpp.lua")
	include("gmake_csharp.lua")
	include("gmake_makefile.lua")
	include("gmake_utility.lua")
	include("gmake_workspace.lua")

	return p.modules.gmake
