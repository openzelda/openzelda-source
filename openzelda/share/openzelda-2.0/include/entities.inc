#if defined _entities_included
	#endinput
#endif
#define _entities_included

//title Entity Functions

#include <enums>
#include <fixed>

#define ARG_NUMBER	'n'
#define ARG_RETURN_NUMBER	'd'
#define ARG_ARRAY	'a'
#define ARG_STRING	's'
#define ARG_END	'\0'

#define MAP_ENTITY entity:0xD4824F64
#define MAIN_ENTITY entity:0x1D1A8BF1

#define SKIP_VALUE Fixed:cellmin

///Entity Position Functions
native EntitySetPosition(Fixed:x = Fixed:SKIP_VALUE, Fixed:y = Fixed:SKIP_VALUE, Fixed:layerz = SKIP_VALUE, id = SELF);
native EntityGetPosition(&Fixed:x, &Fixed:y, &Fixed:layerz, id = SELF, whole = 0);
native EntityGetPosition2(&x, &y, &Fixed:layerz, id = SELF, whole = 1 ) = EntityGetPosition;

///Entity Functions
native entity:EntityCreate(parententity{}, id_string{}, Fixed:x, Fixed:y, Fixed:layerz, map_id, args[]=[ARG_END], args_count = sizeof(args), {Fixed,_}:...);
native EntityDelete(entity:id = SELF);
native EntityGetSetting(key{}, string{}, entity:id = SELF, length = sizeof string);
native EntityGetSettingHash(key{}, entity:id = SELF);
native EntityGetNumber(key{}, entity:id = SELF );
native object:EntityGetObject(key{} = "object-id", entity:id = SELF ) = EntityGetNumber;

native EntityPublicFunction(entity:id, function{}, args[]=[ARG_END], args_count = sizeof(args), {Fixed,_}:...);
native EntityPublicVariable(entity:id, variable{});
native EntityPublicVariableSet(entity:id, variable{}, value);

///Entity Collision Functions
native CollisionFromObject(obj, type = 0, entity:id = SELF);
native CollisionCheck(entity:id1, entity:id2, rect1 = -1, rect2 = -1);
native CollisionSet(entity:id = SELF, rect = 0, type = 0, x = 0, y = 0, w = 0, h = 0);
native CollisionGet(entity:id = SELF, rect, &x, &y, &w, &h);
native CollisionCalculate(entity:id = SELF, rect = -1, type = -1);
native CollisionGetCurrent(entity:id = SELF, &current, &angle, &dist, &rect, &type );

///Entity Management Functions
native EntitiesList(map_id = 0);
native EntitiesNext(&entity:id, map_id = 0, entity_name{} = "", maxlength = sizeof(entity_name) );

///Paths Functions
native PathGetPoint( object:id, point, &x, &y, &t);
native PathCount( object:id );
native PathMoveObject( object:id, Fixed:speed, &x, &y, loop);

///Functions related functions
native FunctionCall( ident );
native FunctionIdent( const name{} );

// Misc Functions 
native entity:EntityHash(name{});

/***
<api title="Entity Functions">
	<function name="EntitySetPosition">
		<summary>Set Entities position on current map.</summary>
		<param name="Fixed:x" default="_" type="n">New Position. Pass cellmin to skip update that value.</param>
		<param name="Fixed:y" default="_" type="n">New Position. Pass cellmin to skip update that value.</param>
		<param name="layer" default="_" type="n">New Position. Pass cellmin to skip update that value.</param>
		<param name="id" default="_" type="n">Entity ID.</param>
	</function>
	<function name="EntityGetPosition">
		<summary>Get Entity position on current map.</summary>
		<param name="&amp;Fixed:x" default="" type="n">Variable to store Position.</param>
		<param name="&amp;Fixed:y" default="" type="n">Variable to store Position.</param>
		<param name="&amp;layer" default="" type="n">Variable to store Position.</param>
		<param name="id" default="_" type="n">Entity ID.</param>
		<param name="whole" default="_" type="n"></param>
	</function>
	<function name="EntityHash">
		<summary>Convert Entity Name to hash</summary>
		<param name="id_string{}" default="" type="s">ID of the entity.</param>
		<return>Returns the Entity Hash.</return>
	</function>
	<function name="EntityCreate">
		<summary>Creates a new entity.</summary>
		<param name="parententity{}" default="&quot;&quot;" type="s">Entity File name.</param>
		<param name="id_string{}" default="&quot;&quot;" type="s">ID to give the entity.</param>
		<param name="x" default="" type="n">position to give the the entity.</param>
		<param name="y" default="" type="n">position to give the the entity.</param>
		<param name="layer" default="" type="n">position to give the the entity.</param>
		<param name="map_id" default="CURRENT_MAP" type="n">Map to create it on. CURRENT_MAP will created it on the current map. GLOBAL_MAP with create a global entity which always on.</param>
		<param name="args[]" default="[ ARG_END ]" type="a">Defines the follow arguments that will be passed to the entity Init function.
		ARG_ARRAY - Array, last value must be CELLMIN
		ARG_NUMBER - Value, passed by value
		ARG_STRING - String
		ARG_END - End
		[ ARG_STRING, ARG_RETURN_NUMBER, ARG_STRING, ARG_RETURN_NUMBER, ARG_END ] means that the follow argument you pass will be a string, a number, a string and a number.
		</param>
		<param name="args_count" default="_" type="n">Size of args. Just use _</param>
		<param name="..." default="" type="v">List of Arguments to pass to the entity Init function. </param>
		<return>0 on error, otherwise it returns the Entity Hash.</return>
	</function>
	<function name="EntityDelete">
		<summary>Deletes a entity.</summary>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
	</function>
	<function name="EntityGetSetting">
		<summary>Get stored setting for entity. These are set via the Runtime Options.</summary>
		<param name="key{}" default="" type="s">Name of Setting.</param>
		<param name="string{}" default="" type="s">String to store value in.</param>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
	</function>
	<function name="EntityGetNumber">
		<summary>Returns stored setting for entity. These are set via the Runtime Options.</summary>
		<param name="key{}" default="" type="s">Name of Setting.</param>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
	</function>
	<function name="EntityGetObject">
		<summary>Returns "object-id" value for entity, tagged as object. </summary>
	</function>
	<function name="EntityPublicFunction">
		<summary>Calls a function from another entity.</summary>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
		<param name="function{}" default="" type="s">Public Function Name.</param>
		<param name="args[]" default=" [ ARG_END ]" type="a">Defines the follow arguments that will be passed to the function.
		ARG_ARRAY - Array, last value must be CELLMIN
		ARG_RETURN_NUMBER - Value, passed by reference
		ARG_NUMBER - Value, passed by value
		ARG_STRING - String
		ARG_END - End
		[ ARG_STRING, ARG_RETURN_NUMBER, ARG_STRING, ARG_RETURN_NUMBER, ARG_END ] means that the follow argument you pass will be a string, a number, a string and a number.
		</param>
		<param name="args_count" default="_" type="n">Size of args. Just use _</param>
		<param name="..." default="" type="v">List of Arguments to pass to the function.</param>
		<return>What the function returns. May return -1 and -2 on error. </return>
	</function>
	<function name="EntityPublicVariable">
		<summary>Returns a public variable from entity.</summary>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
		<param name="variable{}" default="" type="s">Name of public variable.</param>
		<return>the public variable value.</return>
	</function>
	<function name="EntityPublicVariableSet">
		<summary>Sets a public variable on a entity.</summary>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
		<param name="variable{}" default="" type="s">Name of public variable.</param>
		<param name="value" default="" type="n">new value.</param>
	</function>
	<function name="CollisionFromObject">
		<summary>Set Collision rectangles from the sprite or animation data.</summary>
		<param name="obj" default="" type="n">Display Object to use.</param>
		<param name="type" default="0" type="n">Collision rectangle type.</param>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
	</function>
	<function name="CollisionCheck">
		<summary>Check to see if Entity is colliding with another entity.</summary>
		<param name="id1" default="" type="n">The Hash ID of the first entity you wish to check.</param>
		<param name="id2" default="" type="n">The Hash ID of the second entity you wish to check.</param>
		<param name="rect1" default="-1" type="n">The collision rectangle of first entity you wish to check.</param>
		<param name="rect2" default="-1" type="n">Rhe collision rectangle of second entity you wish to check.</param>
		<return>True if the collision rectangle of two entities are colliding.</return>
	</function>
	<function name="CollisionSet">
		<summary>Set a collision for the entity.</summary>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
		<param name="rect" default="0" type="n">Collision Rectangle.</param>
		<param name="type" default="0" type="n">Collision Type.</param>
		<param name="x" default="0" type="n">Rectangle points. </param>
		<param name="y" default="0" type="n">Rectangle points. </param>
		<param name="w" default="0" type="n">Rectangle points. </param>
		<param name="h" default="0" type="n">Rectangle points. </param>
	</function>
	<function name="CollisionCalculate">
		<summary>Generate a list of entities it's colliding with.</summary>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
		<param name="rect" default="-1" type="n">Collision Rectangle your wish to check. -1 for all.</param>
		<param name="type" default="-1" type="n">Not Used.</param>
		<return>True if a collisions exist.</return>
		<example>
		if ( CollisionCalculate() )
		{
			new current;
			new angle;
			new dist;
			new rect;
			new type;
			while ( CollisionGetCurrent(SELF, current, angle, dist, rect, type) )
			{
				if ( type == TYPE_AWAKING )
				{
					EntityPublicFunction(current, "Awaking", "sn", selfid, rect);
				}
				else if ( type == TYPE_ENEMY || type == TYPE_DOOR )
				{
					EntityPublicFunction(current, "Hit", [ ARG_STRING,ARG_NUMBER,ARG_NUMBER,ARG_NUMBER,ARG_NUMBER,ARG_NUMBER,ARG_NUMBER,ARG_END], _, selfid, angle, dist, APLAYER, 0, dx,dy, rect);
				}
				else if ( type == TYPE_TRANSPORT )
				{
					if ( EntityPublicFunction(current, "MovePlayer", [ ARG_STRING, ARG_NUMBER, ARG_END], _, selfid, _dir_) )
					{
						state = STANDING;
					}
				}
				else if ( type == TYPE_PUSHABLE &amp;&amp; state == PUSHING )
				{
					EntityPublicFunction(current, "Push", [ ARG_STRING, ARG_NUMBER, ARG_NUMBER, ARG_END], _, selfid, rect, angle);
				}
				else if ( type == TYPE_SWITCH )
				{
					EntityPublicFunction(current, "Pressed", [ ARG_STRING,ARG_END], _, selfid);
				}
				else if ( type == TYPE_ITEM )
				{
					if ( EntityPublicFunction(current, "Pickup", [ ARG_STRING,ARG_END], _, selfid) )
					{
						state = LIFTING;
					}
				}
			}
		}
		</example>
	</function>
	<function name="CollisionGetCurrent">
		<summary>Retrieves current collision from generated list.</summary>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
		<param name="&amp;current" default="" type="n">String to store colliding entity ID.</param>
		<param name="&amp;angle" default="" type="n">Variable to store angle.</param>
		<param name="&amp;dist" default="" type="n">Variable to store distance. </param>
		<param name="&amp;rect" default="" type="n">Variable to store rectangle that being colliding with.</param>
		<param name="&amp;type" default="" type="n">Variable to store type of rectangle that your colliding with.</param>
		<return>True if a collision exist.</return>
		<example>
		if ( CollisionCalculate() )
		{
			new current;
			new angle;
			new dist;
			new rect;
			new type;
			while ( CollisionGetCurrent(SELF, current, angle, dist, rect, type) )
			{
				if ( type == TYPE_AWAKING )
				{
					EntityPublicFunction(current, "Awaking", "sn", selfid, rect);
				}
				else if ( type == TYPE_ENEMY || type == TYPE_DOOR )
				{
					EntityPublicFunction(current, "Hit", [ ARG_STRING,ARG_NUMBER,ARG_NUMBER,ARG_NUMBER,ARG_NUMBER,ARG_NUMBER,ARG_NUMBER,ARG_END], _, selfid, angle, dist, APLAYER, 0, dx,dy, rect);
				}
				else if ( type == TYPE_TRANSPORT )
				{
					if ( EntityPublicFunction(current, "MovePlayer", [ ARG_STRING, ARG_NUMBER, ARG_END], _, selfid, _dir_) )
					{
						state = STANDING;
					}
				}
				else if ( type == TYPE_PUSHABLE &amp;&amp; state == PUSHING )
				{
					EntityPublicFunction(current, "Push", [ ARG_STRING, ARG_NUMBER, ARG_NUMBER, ARG_END], _, selfid, rect, angle);
				}
				else if ( type == TYPE_SWITCH )
				{
					EntityPublicFunction(current, "Pressed", [ ARG_STRING,ARG_END], _, selfid);
				}
				else if ( type == TYPE_ITEM )
				{
					if ( EntityPublicFunction(current, "Pickup", [ ARG_STRING,ARG_END], _, selfid) )
					{
						state = LIFTING;
					}
				}
			}
		}
		</example>
	</function>
	<function name="CollisionGet">
		<summary>Retrieves the entity collision rectangle values.</summary>
		<param name="id" default="_" type="n">Entity ID Hash.</param>
		<param name="rect" default="" type="n">Collision Rectangle.</param>
		<param name="&amp;x" default="" type="n">Rectangle points.</param>
		<param name="&amp;y" default="" type="n">Rectangle points.</param>
		<param name="&amp;w" default="" type="n">Rectangle points.</param>
		<param name="&amp;h" default="" type="n">Rectangle points.</param>
	</function>
	<function name="EntitiesList">
		<summary>Generate a list of entities for required map.</summary>
		<param name="map" default="0" type="n">The Map that you wish to check.</param>
		<return>Amount of entities on the list.</return>
	</function>
	<function name="EntitiesNext">
		<summary>Retrieves current entity name from generated list.</summary>
		<param name="&amp;id" default="" type="n">Store the Entity ID.</param>
		<param name="map_id" default="0" type="n">The Map that you wish to check.</param>
		<param name="&amp;entity_name" default="" type="a">String to store the Entity Name.</param>
		<return>True if there still more entities. False if no more is in the list.</return>
	</function>
	<function name="PathGetPoint">
		<summary>Returns Object Path point values.</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
		<param name="point" default="" type="n">requested point in path.</param>
		<param name="&amp;x" default="" type="n">Variable to store the position.</param>
		<param name="&amp;y" default="" type="n">Variable to store the position.</param>
		<param name="&amp;t" default="" type="n">Variable to store the time.</param>
	</function>
	<function name="PathMoveObject">
		<summary>Makes object follow path.</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
		<param name="Fixed:speed" default="" type="n">movement speed.</param>
		<param name="&amp;x" default="" type="n">Variable to store the dimension.</param>
		<param name="&amp;y" default="" type="n">Variable to store the dimension.</param>
		<param name="loop" default="" type="n">loop movement.</param>
	</function>
	<function name="PathCount">
		<summary>Returns Object Path count</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
	</function>
	<function name="FunctionCall">
		<summary>Calls a local function base on it's ID.</summary>
		<param name="ident" default="" type="n">The Function ID</param>
		<return>What the function returns.</return>
	</function>
	<function name="FunctionIdent">
		<summary>Return the id of the function.</summary>
		<param name="name{}" default="" type="s"></param>
	</function>

</api>
***/
