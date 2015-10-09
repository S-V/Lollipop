/*
=============================================================================
	File:	CVars.h
	Desc:	Console variables.
=============================================================================
*/

#pragma once

//comment from Doom3 SDK
/*
===============================================================================

	Console Variables (CVars) are used to hold scalar or string variables
	that can be changed or displayed at the console as well as accessed
	directly in code.

	CVars are mostly used to hold settings that can be changed from the
	console or saved to and loaded from configuration files. CVars are also
	occasionally used to communicate information between different modules
	of the program.

	CVars are restricted from having the same names as console commands to
	keep the console interface from being ambiguous.

	CVars can be accessed from the console in three ways:
	cvarName			prints the current value
	cvarName X			sets the value to X if the variable exists
	set cvarName X		as above, but creates the CVar if not present

	CVars may be declared in the global namespace, in classes and in functions.
	However declarations in classes and functions should always be static to
	save space and time. Making CVars static does not change their
	functionality due to their global nature.

	CVars should be constructed only through one of the constructors with name,
	value, flags and description. The name, value and description parameters
	to the constructor have to be static strings, do not use va() or the like
	functions returning a string.

	CVars may be declared multiple times using the same name string. However,
	they will all reference the same value and changing the value of one CVar
	changes the value of all CVars with the same name.

	CVars should always be declared with the correct type flag: CVAR_BOOL,
	CVAR_INTEGER or CVAR_FLOAT. If no such flag is specified the CVar
	defaults to type string. If the CVAR_BOOL flag is used there is no need
	to specify an argument auto-completion function because the CVar gets
	one assigned automatically.

	CVars are automatically range checked based on their type and any min/max
	or valid string set specified in the constructor.

	CVars are always considered cheats except when CVAR_NOCHEAT, CVAR_INIT,
	CVAR_ROM, CVAR_ARCHIVE, CVAR_USERINFO, CVAR_SERVERINFO, CVAR_NETWORKSYNC
	is set.

===============================================================================
*/

/*
-----------------------------------------------------------------------------
	CVar
-----------------------------------------------------------------------------
*/
struct CVar
{
	const char *			name;					// name
	//const char *			value;					// value
	const char *			description;			// description

	union
	{
		int		int_value;
	};

	mxTODO("use fast Variant class");
};

mxNAMESPACE_END
