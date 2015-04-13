#if defined _maps_included
	#endinput
#endif
#define _maps_included

#define MAP_ERROR 0
#define GLOBAL_MAP 0
#define CURRENT_MAP MapCurrentIdent()

///Mask Functions 
native MaskRefresh();
native MaskGetValue( x, y, layer = 0 );
native MaskFill( x, y, w, h, value = 0 );

///Map Functions
native MapOffsetSet( Fixed:x, Fixed:y );
native Fixed:MapOffsetGet( axis );
native MapCurrentIdent();
native MapCurrentDimension( &w, &h );

///Map File Code
native MapSet(map_ident, offset_x, offset_y );
native MapGetIdent( map_file{}, create_new );
native MapReset();
native MapSnapshot();
native MapDelete( map_ident );

///World Function
native WorldSet( section_hash, grid_x, grid_y );
native WorldGetIdent( section_hash, grid_x, grid_y  );
native WorldExist( section_name{} );
native WorldLoad( section_name{} );

//Map Collision Functions
native MapCollisionList();
native MapCollisionCurrent(objecta, objectb, &angle, &dist, &rect );

/***
<api title="Map Functions">
	<function name="MaskRefresh">
		<summary>Updates Mask from current Map Objects.</summary>
		<example>
		function Init( ... )
		{
			while (n &lt; 5)
				ObjectCreate("image.png:wall", SPRITE);
			MaskRefresh();
		}
		</example>
	</function>
	<function name="MaskGetValue">
		<summary>Returns Mask value.</summary>
		<param name="x" default="" type="n">the point you wish to check.</param>
		<param name="y" default="" type="n">the point you wish to check.</param>
		<param name="layer" default="0" type="n">Not used</param>
		<return>Value of mask at that point. Value is capped between 0-255.</return>
	</function>
	<function name="MaskFill">
		<summary>Fills area of mask.</summary>
		<param name="x" default="" type="n">Area of Mask to change.</param>
		<param name="y" default="" type="n">Area of Mask to change.</param>
		<param name="w" default="" type="n">Area of Mask to change.</param>
		<param name="h" default="" type="n">Area of Mask to change.</param>
		<param name="value" default="0" type="n">Set the mask are to this value.</param>
	</function>
	<function name="MapOffsetSet">
		<summary>Set the Offset from the Top-left of the map.</summary>
		<param name="Fixed:x" default="" type="n"></param>
		<param name="Fixed:y" default="" type="n"></param>
	</function>
	<function name="MapOffsetGet">
		<summary>Get the Offset from the Top-left of the map.</summary>
		<param name="axis" default="" type="n"></param>
		<return>Offset for axis.</return>
	</function>
	<function name="MapCurrentIdent">
		<summary>Ident of Current Map.</summary>
		<return>Current Map ID.</return>
	</function>
	<function name="MapCurrentDimension">
		<summary>Dimension of Current Map.</summary>
		<param name="w" default="" type="n">Width of Map.</param>
		<param name="h" default="" type="n">Height of Map.</param>
	</function>
	<function name="MapCurrentGrid">
		<summary>Grid Position of Current Map.</summary>
		<param name="grid_x" default="cellmin" type="n"></param>
		<param name="grid_x" default="cellmin" type="n"></param>
	</function>
	<function name="MapSet">
		<summary>Changes to map id.</summary>
		<param name="map_ident" default="" type="n">Map ID to move to.</param>
		<param name="offset_x" default="-1" type="n">Offset position.</param>
		<param name="offset_y" default="-1" type="n">Offset position.</param>
		<return>Map ID. 0 if error</return>
	</function>
	<function name="MapGetIdent">
		<summary>Check if map id is valid.</summary>
		<param name="map_file" default="" type="s">Map File</param>
		<param name="create_new" default="0" type="n">Create a new map.</param>
		<return>Map Ident</return>
	</function>
	<function name="MapReset">
		<summary>Reset Current Map to initial state.</summary>
		<return></return>
	</function>
	<function name="MapSnapshot">
		<summary>Reset Current Map to initial state.</summary>
		<return></return>
	</function>
	<function name="MapDelete">
		<summary>Remove</summary>
		<param name="map_ident" default="" type="n">ID of the map to remove</param>
	</function>
	<function name="WorldSet">
		<summary>Changes to new World Section using the current grid location.</summary>
		<param name="section_hash" default="" type="n">Section Ident.</param>
		<param name="grid_x" default="-1" type="n">Grid Position</param>
		<param name="grid_y" default="-1" type="n">Grid Position</param>
	</function>
	<function name="WorldGetIdent">
		<summary>Get the Ident.</summary>
		<param name="section_hash" default="" type="n">Section Ident.</param>
		<param name="grid_x" default="-1" type="n">Grid Position</param>
		<param name="grid_y" default="-1" type="n">Grid Position</param>
	</function>
	<function name="WorldExist">
		<summary>Checks to see if world exist.</summary>
		<param name="section_name" default="" type="s"></param>
		<return>World ID or 0 if world does not exist.</return>
	</function>
</api>
***/