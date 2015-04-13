forward public ReceivedData( size, data[] )

new online_version{64};

new music{64} = ""; 
new transition{20} = "";

new section_name{64} ="";
new section_grid = 0;

MapInit()
{
	EntityGetSetting("music", music, SELF); 
	EntityGetSetting("section", section_name); 
	section_grid = EntityGetNumber("map"); 

	
	AudioVolume(MUSIC, 64);
	if ( music{0} )
		AudioPlayMusic(music);
}
	
public Init(...)
{

	MapInit();
	TextSprites(1,"alpha02.png");

}

public Close()
{
	TextSprites(0,"");
}

StartGame(n)
{
	new x = (section_grid % 64);
	new y = (section_grid / 64);

	InputSetDefault(n);

	SectionSet(section_name, x, y)
	//TransitionPlayer( 0, 0, 0, section_name, x, y);

}

main()
{
	if ( InputButton(0,2) || InputButton(1,2) ||InputButton(2,2) || InputButton(3,2) )
		InputSetDefault(2);

	if ( InputButton(0,1) || InputButton(1,1) ||InputButton(2,1) || InputButton(3,1) )
		InputSetDefault(1);


	if ( InputButton(6,1) == 1 )
		StartGame(1);

	if ( InputButton(6,2) == 1 )
		StartGame(2);

}


