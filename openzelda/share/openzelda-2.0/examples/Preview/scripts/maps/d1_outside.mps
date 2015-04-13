// To ease development, <map_default> may include a Init Function.
// If You wish to use your own uncomment the next line
#define HASINITFUNCTION 1
#tryinclude <map_default>
#tryinclude <map_features/enemy_check>


public Init( ... )
{
	InitMap();
	ResetEnemyCount()
	EntityPublicFunction( ENTITY_MAIN, "SetDay", ''n'', 1);
}

main()
{

	CheckEnemy( "DungeonOutsideDoor", "OpenDoor" );
}
