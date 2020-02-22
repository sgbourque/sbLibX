# sbLibX
This is WIP stuff that should eventually replace previous SBLib iterations. This is mostly only in proof-of-concept state.
For the moment, there's a base dynamic DLL loader (sbDev) allowing to load a single module at a time, each module serving its own purpose.
We can find and run any exported functions  from different modules from the main thread (no threaded dispatch yet).
While for now only one module can be used at a time, they are all supposed to have distinct purpose.

Any module that could be run in standalone should normally export the main() function when compiled as a dll so
that it can be accessed for development. Currently, no target is set in standalone mode but most (all but one) module
are designed to be used straightforwardly as an executable.

Technically, we can load EXE files but static data is not automatically (de-)initialized so it would usually crash the first
time static data is accessed if not deal with manually (which I don't for now as I don't requires it yet... I'll see later what can
be done with Win32MainStartup).

Note: ``If you have trouble debugging (setting breadpoints) or being unable to detach the debugger,
try changing 'ProjectProperty/Debugger/DebuggerType' to "NativeOnly" or to "Auto". For me, one or the other
works... then stop working so I change back and forth... and it kinda work.``


## Folders
Everything is setup such that the root folder should be the working folder.
Folders marked with '[ ]' are development-only folders and should be discarded for distribution.
Folders marked with '*' only contains generated data and can be safely deleted if unneeded.
All final binaries should be copied directly in /bin folder for distribution.
All final data should be placed directly in /data if using for packaged build. No final data should reference directly anything under data/src/... or data/generated/...
```
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
```

## sbDev : [code/dev/sb_dev] 
This is the main development entry point.
Currently supported commands are :
```
	- load [module][/entry_point: optional]
	- unload [module]
	- entry [entry_point]
	- run [module][/entry_point: optional]
	- exit
	- quit
```
This library depends on sbDebug and sbLibX only.
All generic libraries (including those within LibTest folder) can dynamically 
or statically access each others (as far as all data is correctly reallocated whenever parent allocator changes).

## sbDebug : [code/dev/sb_debug] (the LibDev family)
This mostly wraps debugging tools : Console I/O, DynamicLibrary loader, debug CRT flags, etc.
This one can depends at most statically on sbLibX (see sbLibX).

## sbLibX (the LibCommon family)
This library is actually much more a template meta library. However, its meaning is to expose limited
native informations, including the type_info information from RTTI upon which higher level structures are built.

Technically a member of the sbLibCommon family,
it is meant to be the entry point connecting the sbLibX modules
(eventually). Theses are the common files which share common definitions.

Usually, each module exposes a specialised interface (measnt as an access point)
to globally independant categories. For now, only sbLix (common definitions)
and sbWindows (mostly anything that needs "windows.h" to included,
which preventing C\+\+-latest, needs windows-specific C++ extensions, CRT, etc.).

## Test

In the end, this is meant to become unit test development platform for
static and dynamic unit tests. For now, it offers a sandbox for testing.

Also, since these are considered "external" modules (not directly part of sbLibX
more an annex to it). External module should normally be able to run
as individual units.
