# sbLibX
This is WIP stuff that should eventually replace previous SBLib iterations. This is mostly only in proof-of-concept state.
For the moment, there's a base dynamic DLL loader (sbDev) allowing to load a single module at a time, each module serving its own purpose.
We can find and run any exported functions  from different modules from the main thread (no threaded dispatch yet).
While for now only one module can be used at a time, they are all supposed to have distinct purpose.

Any module that could be ran in standalone should normally export the main() function when compiled as a dll so
that it can be accessed for development. Currently, no target is set in standalone mode but most module are designed to be used
straightforwardly as an executable.

If you have trouble debugging or being unable to detach the debugger, try changing LocalDebuggerDebuggerType changing to "NativeOnly" or to "Auto".


##Folders
Everything is setup such that the root folder should be the working folder.
Folders marked with '[ ]' are development-only folders and should be discarded for distribution.
Folders marked with '*' only contains generated data and can be safely deleted if unneeded.
All final binaries should be copied directly in /bin folder for distribution.
All final data should be placed directly in /data if using for packaged build. No final data should reference directly anything under data/src/... or data/generated/...
	/
		/*bin
			/[...architecture...]
		/[code]
			/...module_family...
				/include
				/src
		/data
			/...
				/[src]/...
				/*[generated]/...
				/...
		/*[tmp]

## sbDev : [code/dev/sb_dev]
This is the main development entry point.
Currently supported commands are :
	- load [module][/entry_point: optional]
	- unload [module]
	- entry [entry_point]
	- run [module][/entry_point: optional]
	- exit
	- quit

## sbDev : [code/dev/sb_debug]
This mostly wraps debugging tools : Console I/O, DynamicLibrary loader, debug CRT flags, etc.

## SBLibX
Common files 

