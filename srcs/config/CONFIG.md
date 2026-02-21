
1. Overview
	- This file defines the configuration system for our HTTP server
	- The configuration file conatains:
		* One or more `server` blocks.
		* Each `server` block contains one or more `location` blocks.
		* The configuration file rules are strict
		* If a configuration file is not provided, a default configuration system will be used

2. Startup rules
	- No config file provided:
		* a default path is provided
	- Config file present:
		* Be readable.
		* Contains at least one `server` block.
		* Contains no unknown directives.
		* Contains all the required directives.
	- Any error -> program exits with error

3. Config hierarchy
	
config
 └── server
      └── location

4. Server context
	4.1 Required directive
	- The server context aka server block must contain:
	. At least one location
	. At least one listen
	- If missing -> configuration error

	4.2 Allowed directives in server
	
	| Directive				| Required?	| Multiple allowed?
	----------------------------------------------------
	| listen				| yes		| yes
	----------------------------------------------------
	| server_name			| no		| yes
	----------------------------------------------------
	| root					| yes		| no
	----------------------------------------------------
	| index					| no		| yes
	----------------------------------------------------
	| error_page			| no		| yes
	----------------------------------------------------
	| client_max_body_size	| yes		| no
	---------------------------------------------------- 
	| location				| yes		| yes	   
	----------------------------------------------------
	
	* Unknown directives -> error.
	* Nested server inside server -> error.
5. Location Context
	5.1 Required directives:
		None
	!! A location must have a valid path
	5.2 Allowed directives in location:
	
	| Directive		| Multiple Allowed
	-------------------------------------------
	| root			| no
	-------------------------------------------
	| index			| yes
	-------------------------------------------
	
	* Nested locations ==> error

6. Inheritance rules:
	- `location` inherits from `server`:
	
	| Directive				| Inherited?
	-------------------------------------
	| root					| yes
	-------------------------------------
	| index					| yes
	-------------------------------------
	| client_max_body_size	| yes
	-------------------------------------
	| error_page			| yes
	-------------------------------------

	!! If overridden in location -> location value takes precedence
7. Validation Phase
	After parsing, validation phase must check:
	* Required directives
	* Do duplicate single-instance directives -> (marked no in 'Multiple Allowed')
	* Valid port numbers
	* Valid file paths
	* No empty server block

	-> Parsing and validation must be done separately

8. Implementation rules:

- ConfigParse/Loader must use RAII.
- No global variables.
- No mutable static state.
- Config must be immutable after build.
- Parsing errors must abort configuration.