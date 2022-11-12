
//very simple mod lol 

CCLayer* (__fastcall* EndLevelLayer_create)();
CCLayer* __fastcall EndLevelLayer_createH(void*) {
	
	auto ly = EndLevelLayer_create();
	
	/*
		ugly but its the best solution
		set visible doesnt work + not creating the layer at all
		makes the esc button not work
	*/
	
	ly->setPositionY(10000000000);
	return ly;
}