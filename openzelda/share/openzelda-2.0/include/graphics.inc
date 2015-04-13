#if defined _graphics_included
  #endinput
#endif
#define _graphics_included

#include <fixed>
#include <enums>
#include <core>

/***
<api title="Graphics Functions">
	<futurefunction name="AnimationCreate">
		<summary>Creates new animation.</summary>
		<param name="string[]" default="" type="a"></param>
		<param name="length" default="" type="n"></param>
		<return>0 for error.</return>
	</futurefunction>
	<futurefunction name="AnimationAddFrame">
		<summary>Adds a frame to an animation.</summary>
		<param name="string[]" default="" type="a"></param>
		<param name="sprite[]" default="" type="a"></param>
		<return>1 for successful, 0 for error.</return>
	</futurefunction>
	<futurefunction name="PolygonCreate">
		<summary>Create a polygon.</summary>
		<param name="string[]" default="" type="a">name of polygon.</param>
		<return>0 for error.</return>
	</futurefunction>
	<futurefunction name="PolygonAddPoint">
		<summary>Add a point to existing polygon.</summary>
		<param name="string[]" default="" type="a">name of polygon.</param>
		<param name="x" default="" type="n">new point position.</param>
		<param name="y" default="" type="n">new point position.</param>
		<return>1 for successful, 0 for error.</return>
	</futurefunction>
	<function name="CanvasChildInfo">
		<summary>Get the position of an object on canvas.</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
		<param name="child{}" default="" type="a">name of child.</param>
		<param name="&amp;w" default="" type="n">Variable to store the width. </param>
		<param name="&amp;h" default="" type="n">Variable to store the height. </param>
		<param name="&amp;x" default="" type="n">Variable to store the x value. </param>
		<param name="&amp;y" default="" type="n">Variable to store the y value. </param>
		<return>child id for successful, 0 for error.</return>
	</function>
	<function name="GraphicsType">
		<summary>Returns the type of graphic display. Can be used to enable some effects.</summary>
		<return>Current Display Type</return>
	</function>
	<function name="GraphicsDraw">
		<summary>Draw a Display Objects for the current frame.</summary>
		<param name="string[]" default="" type="a">Name of Sprite, Animation, Polygon or Text String, depending on what type of object your drawing.</param>
		<param name="GraphicTypes:type" default="" type="n">Type of Object</param>
		<param name="x" default="" type="n">Position</param>
		<param name="y" default="" type="n">Position</param>
		<param name="z" default="" type="n">Position</param>
		<param name="w" default="" type="n">Dimension (or second x,y location for line).</param>
		<param name="h" default="" type="n">Dimension (or second x,y location for line).</param>
		<param name="c" default="0xFFFFFFFF" type="n">Colour</param>
	</function>
	<function name="SheetReference">
		<summary>Add or remove a reference for sprite sheet. Mainly used to keep a sprite sheet in memory.</summary>
		<param name="sheet[]" default="" type="a"></param>
		<param name="ref" default="1" type="n"></param>
	</function>
	<function name="SheetReplace">
		<summary>Changing one sheet to another.</summary>
		<param name="old_sheet[]" default="" type="a"></param>
		<param name="news_sheet[]" default="" type="a"></param>
	</function>
	<function name="SheetSpriteDimension">
		<summary>Get Dimension of a sprite</summary>
		<param name="sprite{}" default="" type="a"></param>
		<param name="&amp;w" default="" type="n"></param>
		<param name="&amp;h" default="" type="n"></param>
		<return>True if sprite exists.</return>
	</function>

	<function name="AnimationGetLength">
		<summary>Returns length of animation in milliseconds.</summary>
		<param name="sheet[]" default="" type="a"></param>
		<param name="anim[]" default="" type="a"></param>
		<return>length of animation in milliseconds.</return>
	</function>
	<function name="AnimationGetLength2">
		<summary>Returns length of animation in seconds.</summary>
		<param name="sheet[]" default="" type="a"></param>
		<param name="anim[]" default="" type="a"></param>
		<return>Returns length of animation in seconds.</return>
	</function>

	<function name="TextSprites">
		<summary>Ables the use of Sprites for text instead of the internal bitmap font.</summary>
		<param name="able" default="" type="n"></param>
		<param name="sheet[]" default="" type="a"></param>
	</function>
	<function name="ObjectCreate">
		<summary> Creates a new Display Objects. 'string' can be a sprite name or some text.</summary>
		<param name="string[]" default="" type="a">Sprite/animation name or text.</param>
		<param name="GraphicTypes:type" default="" type="n">Type of Object.</param>
		<param name="x" default="0" type="n">Map position of the object.</param>
		<param name="y" default="0" type="n">Map position of the object.</param>
		<param name="Fixed:z" default="0" type="n">Map position of the object.</param>
		<param name="w" default="0" type="n">Width of the object. Sprites and Animations repeat until they fit into the Width , also Width can be zero.</param>
		<param name="h" default="0" type="n">Height of the object. Sprites and Animations repeat until they fit into the  Height, also Height can be zero.</param>
		<param name="c" default="0xFFFFFFFF" type="n">a RGBA hex value such as 0xFF33BBAA, FF is red value, 33 is green, BB is blue and AA would be the alpha.</param>
		<param name="global" default="false" type="n"></param>
		<return>New object ID</return>
		<example>
		new helded = 0xFF00FFFF; 
		 
		new player = 0; 
		new object:obj1 = OBJECT_NONE; 
		new object:obj2 = OBJECT_NONE; 
		new object:obj3 = OBJECT_NONE; 
		new x = 10, y = 10; 

		public Init(...) 
		{
			new Fixed:fx, Fixed:fy, Fixed:fz;
			EntityGetPosition(fx, fy,fz);
			x = fround(fx)+32;
			y = fround(fy)+32;
		 
			player =  EntityGetNumber("player");
			obj1 = ObjectCreate("", RECTANGLE, 11, 10, 5, 16, 64, 0xFF0000FF);
			obj2 = ObjectCreate("", RECTANGLE, 100, 100, 5, 64, 16, 0xFF0000FF);
			obj3 = ObjectCreate("", RECTANGLE, 100, 10, 5, 64, 16, 0xFF0000FF);

		}
		
		public Close() 
		{
			ObjectDelete(obj1); 
			ObjectDelete(obj2); 
			ObjectDelete(obj3); 
		} 

		main() 
		{ 
			new xaxis = 255; 
			new yaxis = -255; 
			new zaxis = 128; 

			xaxis = InputAxis(0, player)/8;
			yaxis = InputAxis(1, player)/8;
			zaxis = InputAxis(2, player)/8;

			if ( xaxis &gt; 0 )
				ObjectPosition(obj1, x, y-5, 3, xaxis, 10); 
			else if ( xaxis &lt; 0 )
				ObjectPosition(obj1, x+xaxis, y-5, 3, 0-xaxis, 10); 
			else
				ObjectPosition(obj1, x-1, y-5, 3, 1, 10); 

			if ( yaxis &gt; 0 )
				ObjectPosition(obj2, x-5, y, 3, 10, yaxis); 
			else if ( yaxis &lt; 0 )
				ObjectPosition(obj2, x-5, y+yaxis, 3, 10, 0-yaxis); 
			else 
				ObjectPosition(obj2, x-5, y-1, 3, 10, 1);
			if ( zaxis &gt; 0 )
				ObjectPosition(obj3, x+44, y, 3, 10, zaxis); 
			else if ( zaxis &lt; 0 )
				ObjectPosition(obj3, x+44, y+zaxis, 3, 10, 0-zaxis); 
			else 
				ObjectPosition(obj3, x+44, y-1, 3, 10, 1); 
		} 
		</example>
	</function>
	<function name="ObjectPosition">
		<summary>Give the Display Objects a new position and dimension.</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
		<param name="x" default="" type="n">Map position of the object.</param>
		<param name="y" default="" type="n">Map position of the object.</param>
		<param name="Fixed:z" default="" type="n">Map position of the object.</param>
		<param name="w" default="0" type="n">Width of the object. Sprites and Animations repeat until they fit into the Width , also Width can be zero.</param>
		<param name="h" default="0" type="n">Height of the object. Sprites and Animations repeat until they fit into the  Height, also Height can be zero.</param>
		<param name="pos" default="POSITION_MAP" type="n"></param>
	</function>
	<function name="ObjectEffect">
		<summary>Change Display Objects effects.</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
		<param name="colour" default="0xFFFFFFFF" type="n">Primary colour of the object.</param>
		<param name="rotate" default="0" type="n"></param>
		<param name="scale_w" default="1000" type="n"></param>
		<param name="scale_h" default="1000" type="n"></param>
		<param name="mirror" default="0" type="n"></param>
		<param name="style" default="STYLE_NORMAL" type="n"></param>
		<param name="colour2" default="0xFFFFFFFF" type="n">Secondary colour of the object.</param>
	</function>
	<function name="ObjectReplace">
		<summary>Convert Display Objects to new type.</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
		<param name="string[]" default="" type="a">Sprite/animation name or text.</param>
		<param name="GraphicTypes:type" default="" type="n">Type of Object.</param>
	</function>
	<function name="ObjectFlag">
		<summary>Sets Display Object flags.</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
		<param name="key" default="" type="n"></param>
		<param name="value" default="" type="n"></param>
	</function>
	<function name="ObjectDelete">
		<summary>Delete a Display Objects.</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
	</function>
	<function name="ObjectInfo">
		<summary>Returns width and height of object.</summary>
		<param name="object:id" default="" type="n">Object's ID.</param>
		<param name="&amp;w" default="" type="n">Variable to store the width. </param>
		<param name="&amp;h" default="" type="n">Variable to store the height. </param>
		<param name="&amp;x" default="" type="n">Variable to store the x value. </param>
		<param name="&amp;y" default="" type="n">Variable to store the y value. </param>
	</function>

	<function name="CameraSetScroll">
		<summary>Enable Smooth scrolling.</summary>
		<param name="bool:scroll" default="" type="n">Set to true to enable Smooth scrolling.</param>
		<notes>
			<p>Smooth Scrolling keeps the map offset in the center of the screen and also keeps the screen on the map.</p>
			<img src="images/CameraScroll.png"/>
			<p>
				Black Rectangle: Screen Area.<br/>
				Red Line: Map offset if smooth scrolling is disable.<br/>
				Yellow Line: Map offset if smooth scrolling is enable.
			</p>
		</notes>

		<similar>
			<link name="MapSetOffset"/>
		</similar>
	</function>
	<function name="LayerSetRotation">
		<summary>Change the rotation of the layer. This feature is still in development and will only be available for OpenGL display system.</summary>
		<param name="layer" default="" type="n">Layer to change. Default setting give layers 0-6.</param>
		<param name="roll" default="" type="n"></param>
		<param name="pitch" default="" type="n"></param>
		<param name="yaw" default="" type="n"></param>
	</function>
	<function name="LayerSetEffect">
		<summary>Change the Effect of the layer.</summary>
		<param name="layer" default="" type="n">Layer to change. Default setting give layers 0-6.</param>
		<param name="effect" default="" type="n"></param>
		<example>

		main()
		{
			LayerSetEffect(1, STYLE_NEGATIVE);
		}
		</example>
	</function>


	<function name="LayerSetOffset">
		<summary>Change the Offset of the layer.</summary>
		<param name="layer" default="" type="n">Layer to change. Default setting give layers 0-6.</param>
		<param name="Fixed:x" default="" type="n"></param>
		<param name="Fixed:y" default="" type="n"></param>
		<example>
		new Fixed:worldx = 0.00;
		new Fixed:worldy = 0.00;
		new Fixed:layer1 = 0.00;
		new Fixed:layer2 = 0.00;

		main()
		{
			new Fixed:q = GameFrame2() * 10.00;
			worldx += q
			layer1 += q;
			layer2 += (q*2);

			LayerSetOffset(-1, 0.00, worldy); // Sets All layers offsets to 0.00, worldy
			LayerSetOffset(1, layer2, worldy); // Change Layer 1 offset
			LayerSetOffset(2, layer1, worldy); // Change Layer 1 offset
			MapSetOffset(worldx, worldy);
		}
		</example>
	</function>
	<function name="LayerColour">
		<summary>Change the base color of the layer.</summary>
		<param name="layer" default="" type="n">Layer to change. Default setting give layers 0-6.</param>
		<param name="colour" default="0xFFFFFFFF " type="n"></param>
		<notes><img src="images/LayerColour.png"/></notes>
		<example>
		new DayNight[24] = {
			0x4E5CAFFF,\
			0x5964A9FF,\
			0x606BAEFF,\
			0x6A76BAFF,\
			0x6E76BAFF,\
			0x7883C6FF,\
			0x98A3E8FF,\
			0xB6BEEEFF,\
			0xD6DAF3FF,\
			0xF4F5FBFF,\
			0xFFFFFFFF,\
			0xFFFFFFFF,\
			0xF7F8ECFF,\
			0xF7F8E7FF,\
			0xEFF2E1FF,\
			0xEDEDD2FF,\
			0xE8ECC8FF,\
			0xE8E1BEFF,\
			0xE4C47EFF,\
			0xE5A67EFF,\
			0xE59E96FF,\
			0x7D7CC8FF,\
			0x766FD8FF,\
			0x645CBEFF\
		};
		new hour = 6;
		new seconds = 0;
		main()
		{
			seconds += GameFrame();
			hour = seconds / 1000;

			LayerColour(0, DayNight[hour]);
			LayerColour(1, DayNight[hour]);
			LayerColour(2, DayNight[hour]);
			LayerColour(3, DayNight[hour]);
			LayerColour(4, DayNight[hour]);
			LayerColour(5, DayNight[hour]);

		}
		</example>
	</function>
</api>
***/

///Misc Graphic Functions
native GraphicsDraw(string{}, GraphicTypes:type, x, y, Fixed:layerz, w, h, c = WHITE );
native GraphicsDraw2(string{}, GraphicTypes:type, x, y, Fixed:layerz, w, h, c = WHITE, rotation ) = GraphicsDraw;
native TextSprites(able, sheet{});
native SheetReference(sheet{}, ref = 1);
native SheetReplace(old_sheet{}, news_sheet{});
native SheetSpriteDimension(sprite{}, &w, &h);


///Animation Functions
native AnimationGetLength(sheet{}, anim{});
native Fixed:AnimationGetLengthSeconds(sheet{}, anim{}) = AnimationGetLength;
#pragma deprecated Future Feature
native AnimationCreate(string{}, length);
#pragma deprecated Future Feature
native AnimationAddFrame(string{}, sprite{}, string_size = sizeof string, sprite_size = sizeof sprite);


///Polygon Functions
#pragma deprecated Future Feature
native PolygonCreate(string{});
#pragma deprecated Future Feature
native PolygonAddPoint(string{}, x, y, string_size = sizeof string);

///Canvas Functions
native CanvasChildInfo(object_id, child{}, &x, &y, &w, &h);
native CanvasChildSetEffect(object_id, child_id, colour = WHITE, rotate = 0, scale_w = 1000, scale_h = 1000, mirror = 0, StyleTypes:style = STYLE_NORMAL, colour2 = WHITE);

///Display Objects Functions
native object:ObjectCreate( string{}, GraphicTypes:type, x, y, Fixed:layerz, w = 0, h = 0, c = 0xFFFFFFFF, global = false, string_size = sizeof string);
native ObjectPosition( object:id, x, y, Fixed:layerz, w, h, pos = POSITION_MAP);
native ObjectEffect( object:id, colour = WHITE, rotate = 0, scale_w = 1000, scale_h = 1000, mirror = 0, StyleTypes:style = STYLE_NORMAL, colour2 = WHITE);
native ObjectReplace( object:id, string{}, GraphicTypes:type );
native ObjectFlag( object:id, ObjectFlags:key, value);
native ObjectDelete( object:id);
native ObjectInfo( object:id, &w, &h, &x, &y, &Fixed:layerz );
native ObjectDimensions( object:id, &w, &h ) = ObjectInfo;

///Camera Functions
native CameraSetScroll(bool:scroll);
native LayerSetRotation(layer, roll, pitch, yaw);
native LayerSetOffset(layer, Fixed:x, Fixed:y);
native LayerColour(layer, colour = 0xFFFFFFFF );
native LayerSetEffect(layer, effect);

