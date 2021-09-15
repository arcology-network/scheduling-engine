#include "stdafx.h"

int main() {
	TestScheduleBasic();
	TestUpdateThenSchedule();
	TestAllConflict();
	TestAllConflictExceptItself();
	TestExternal5();

	TestUpdateTwiceThenSchedule();
	TestSelfConflict2x1();
	//Benchmarking1m();
}