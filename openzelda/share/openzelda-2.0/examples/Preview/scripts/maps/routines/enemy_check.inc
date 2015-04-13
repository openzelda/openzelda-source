/***********************************************
 * Copyright © Luke Salisbury
 *
 * You are free to share, to copy, distribute and transmit this work
 * You are free to adapt this work
 * Under the following conditions:
 *  You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work). 
 *  You may not use this work for commercial purposes.
 * Full terms of use: http://creativecommons.org/licenses/by-nc/3.0/
 * Changes:
 *     2010/01/11 [luke]: new file.
 ***********************************************/
#include <map_features/standard>

forward public KilledEnemy();

stock _enemycount = -1;

stock CheckEnemy( entity_name{}, function{} )
{
	DebugText("CheckEnemy %d", _enemycount);
	if ( _enemycount >= 0 )
	{
		if ( _enemycount == 0 )
		{
			if ( entity_name{0} && function{0} )
				EntityPublicFunction( EntityHash(entity_name), function, [ ARG_END ] );
			else
				DialogShow( 1 );
			_enemycount = -2;
		}
	}
	else if ( _enemycount == -1 )
	{
		_enemycount = 0;
		if ( EntitiesList(map_id) )
		{
			new child_entity;

			while ( EntitiesNext(child_entity, map_id) )
			{
				if ( TYPE_ENEMY == EntityPublicFunction( child_entity, "GetType")  )
					_enemycount++;
			}

		}

	}

}

public KilledEnemy()
{
	_enemycount--;
}

ResetEnemyCount()
{
	_enemycount = -1;
}

