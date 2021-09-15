#pragma once
std::vector<char> Vectorize(std::vector<std::pair<std::string, uint32_t>> callees);
bool RunScheduler(std::string name, void* schdPtr, std::vector<std::pair<std::string, uint32_t>> callees, std::vector<uint32_t> targetOrder, std::vector<uint32_t> targetBranches, std::vector<uint32_t> targeGens);
bool RunUpdateHistory(bool clearHistory, void* schdPtr, std::vector<Conflict> conflicts, std::vector<uint8_t> targetProbVec, std::string logFileName);
std::vector<std::pair<std::string, uint32_t>> ReadCSV(std::string filename);

bool TestScheduleBasic();
bool TestUpdateThenSchedule();
bool TestAllConflict();
bool TestAllConflictExceptItself();
bool TestExternal5();

bool TestUpdateTwiceThenSchedule();
bool TestSelfConflict2x1();

bool Benchmarking1m();