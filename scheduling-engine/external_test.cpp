#include "stdafx.h"


bool TestScheduleBasic() {
	std::string config = "{\"presentBetweenThresh\": 0.7, \"minParaBatch\" : 2, \"historyFile\" : \"\", \"debug\" : true}";

	char msg[BUFFER_SIZE];
	void* schdPtr = Start((char*)config.c_str(), config.size(), msg);

	std::vector<std::pair<std::string, uint32_t>> addresses{
		{"0x000000", 3},  // 0
		{"0x111111", 1},
		{"0x222222", 2},
		{"0x333333", 1},
		{"0x444444", 1}
	};

	RunScheduler(
		"TestScheduleBasic()",
		schdPtr,
		addresses,
		Util::Range<uint32_t>(8, 0),
		Util::Range<uint32_t>(8, 0),
		std::vector<uint32_t>(8, 0)
	);

	/* ---------------------------------------------- Update the conflict info-----------------------------------------*/
	std::vector<Conflict> conflicts{
		Conflict("0x111111", "0x222222"),// 1:2
		Conflict("0x111111", "0x333333"),// 1:3
		Conflict("0x222222", "0x444444"),// 2:4
		Conflict("0x333333", "0x444444") // 3:4
	};

	std::vector<uint8_t> target{
		 0,  255, 255,   0,
		 0,   0,   0,   255,
		 0,   0,   0,   255,
		 0,   0,   0,    0,
	};

	RunUpdateHistory(true, schdPtr, conflicts, target, "UpdateHistory.json");

	/* ----------------------------------------  Reschedule with the conflict info--------------------------------------*/
	RunScheduler(
		"TestScheduleBasic()",
		schdPtr,
		addresses,
		std::vector<uint32_t>({ 0, 1, 2, 3, 7, 4, 5, 6 }),
		std::vector<uint32_t>({ 0, 1, 2, 3, 4, 0, 1, 2 }),
		std::vector<uint32_t>({ 0, 0, 0, 0, 0, 1, 1, 1 })
	);

	Stop(schdPtr);
	return true;
}

bool TestUpdateThenSchedule() {
	std::string config = "{\"presentBetweenThresh\": 0.7,	\"minParaBatch\" : 2,\"historyFile\" : \"history.bin\", \"debug\" : true}";

	char msg[BUFFER_SIZE];
	std::memset(msg, BUFFER_SIZE, 0);
	void* schdPtr = Start((char*)config.c_str(), config.size(), msg);

	std::vector<Conflict> conflicts{
		Conflict("0x000000", "0x000000"),// 1:2
		Conflict("0x222222", "0x222222"),// 1:3
	};

	/* Check the probability matrix */
	std::vector<uint8_t> target{
		255,  0,
		0,  255,
	};

	RunUpdateHistory(true, schdPtr, conflicts, target, "first-update.json");
	Stop(schdPtr); /* Update then stop */

	/* ====================== Start Again just to check the prob matrix =====================*/
	std::memset(msg, BUFFER_SIZE, 0);
	schdPtr = Start((char*)config.c_str(), config.size(), msg);
	
	/* Try again */
	std::vector<std::pair<std::string, uint32_t>> addresses{
		{ "0x000000", 3 },  // 0
		{ "0x111111", 1 },
		{ "0x222222", 2 },
		{ "0x333333", 1 },
		{ "0x444444", 1 }
	};

	return RunScheduler(
		"TestUpdateThenSchedule()",
		schdPtr,
		addresses,
		std::vector<uint32_t>({ 3, 6, 7, 0, 1, 2, 4, 5 }),
		std::vector<uint32_t>({ 0, 1, 2, 0, 0, 0, 1, 1 }),
		std::vector<uint32_t>({ 0, 0, 0, 1, 1, 1, 1, 1 })
	);
}

bool TestAllConflict() {
	char msg[BUFFER_SIZE];
	std::string config = "{\"presentBetweenThresh\": 0.7,	\"minParaBatch\" : 6, \"historyFile\" : \"\", \"debug\" : true}";
	void* schdPtr = Start((char*)config.c_str(), config.size(), msg);

	std::vector<Conflict> conflicts;
	std::vector<std::string> addrVec{ "0x000000", "0x111111", "0x222222", "0x333333", "0x444444" };
	for (std::size_t i = 0; i < addrVec.size(); i++)
		for (std::size_t j = 0; j < addrVec.size(); j++)
			conflicts.push_back(Conflict(addrVec[i], addrVec[j]));
		
	std::vector<uint8_t> target {  /* Check the probability matrix */
		255, 255, 255, 255, 255,
		0,   255, 255, 255, 255,
		0,    0,  255, 255, 255,
		0,    0,   0,  255, 255,
		0,    0,   0,   0,  255 };

	RunUpdateHistory(true, schdPtr, conflicts, target, "first-update.json");

	/* ---------------------------------------------- Schedule -----------------------------------------*/
	std::vector<std::pair<std::string, uint32_t>> addresses{
		{ "0x000000", 3 },  // 0
		{ "0x111111", 1 },
		{ "0x222222", 2 },
		{ "0x333333", 1 },
		{ "0x444444", 1 }
	};

	return RunScheduler(
		"TestScheduleBasic()",
		schdPtr,
		addresses,
		std::vector<uint32_t>({ 0, 1, 2, 3, 4, 5, 6, 7 }),
		std::vector<uint32_t>({ 0, 0, 0, 0, 0, 0, 0, 0 }),
		std::vector<uint32_t>({ 0, 0, 0, 0, 0, 0, 0, 0 })
	);
}

bool TestAllConflictExceptItself() {
	char msg[BUFFER_SIZE];
	std::string config = "{\"presentBetweenThresh\": 0.7, \"minParaBatch\" : 2, \"historyFile\" : \"\", \"debug\" : true}";
	void* schdPtr = Start((char*)config.c_str(), config.size(), msg);

	/* ---------------------------------------------- Update History -----------------------------------------*/
	std::vector<Conflict> conflicts;
	std::vector<std::string> addrVec{ "0x000000", "0x111111", "0x222222", "0x333333", "0x444444" };
	for (std::size_t i = 0; i < addrVec.size(); i++)
		for (std::size_t j = 0; j < addrVec.size(); j++)
			if (i != j)
				conflicts.push_back(Conflict(addrVec[i], addrVec[j]));

	/* Check the probability matrix */
	std::vector<uint8_t> target{
		 0,  255, 255, 255, 255,
		 0,   0,  255, 255, 255,
		 0,   0,   0,  255, 255,
		 0,   0,   0,   0,  255,
		 0,   0,   0,   0,   0 };

	RunUpdateHistory(true, schdPtr, conflicts, target, "first-update.json");

	/* ---------------------------------------------- Schedule -----------------------------------------*/
	std::vector<std::pair<std::string, uint32_t>> addr{
		{ "0x000000", 3 },  // a
		{ "0x111111", 1 },
		{ "0x222222", 2 },
		{ "0x333333", 1 },
		{ "0x444444", 1 }
	};

	return RunScheduler(
		"TestScheduleBasic()",
		schdPtr,
		addr,
		std::vector<uint32_t>({ 0, 1, 2, 4, 5, 3, 6, 7 }),
		std::vector<uint32_t>({ 0, 1, 2, 0, 1, 0, 0, 0 }),
		std::vector<uint32_t>({ 0, 0, 0, 1, 1, 2, 2, 2 })
	);
}

bool TestExternal5() {
	char msg[BUFFER_SIZE];
	std::string config = "{\"presentBetweenThresh\": 0.7, \"minParaBatch\" : 2, \"historyFile\" : \"\",\"debug\" : true}";
	void* schdPtr = Start((char*)config.c_str(), config.size(), msg);

	/* ---------------------------------------------- Update History -----------------------------------------*/
	std::vector<Conflict> conflicts{
		Conflict("0x000000", "0x111111"), // Add the entries only
		Conflict("0x222222", "0x333333") // Add the entries only
	};

	std::vector<uint8_t> target{
		0,  255, 0,  0,
		0,   0,  0,  0,
		0,   0,  0, 255,
		0,   0,  0,  0 };

	RunUpdateHistory(true, schdPtr, conflicts, target, "first-update.json");

	/* ---------------------------------------------- Schedule -----------------------------------------*/
	std::vector<std::pair<std::string, uint32_t>> addresses{
		{ "0x000000", 1 },
		{ "0x111111", 1 },
		{ "0x222222", 1 },
		{ "0x333333", 1 },
		{ "0x444444", 1 },
		{ "0x555555", 1 },
	};

	return RunScheduler(
		"TestScheduleBasic()",
		schdPtr,
		addresses,
		std::vector<uint32_t>({ 0, 2, 4, 5, 1, 3 }),
		std::vector<uint32_t>({ 0, 1, 2, 3, 0, 1 }),
		std::vector<uint32_t>({ 0, 0, 0, 0, 1, 1 })
	);

	return true;
}

bool TestUpdateTwiceThenSchedule() {
	std::vector<std::string> addresses;
	std::vector<std::string> _0x0 = std::vector<std::string>(5, "0x000000");
	std::vector<std::string> _0x1 = std::vector<std::string>(5, "0x111111");

	addresses.insert(addresses.end(), _0x0.begin(), _0x0.end());
	addresses.insert(addresses.end(), _0x1.begin(), _0x1.end());

	std::vector<char> callees(addresses.size() * ID_LEN);
	for (std::size_t i = 0; i < addresses.size(); i++)
		std::copy(addresses[i].begin(), addresses[i].end(), callees.begin() + i * ID_LEN);

	std::vector<uint32_t> txIDs = Util::Range<uint32_t>(addresses.size(), 100);

	std::vector<uint32_t> order(txIDs.size(), 0);
	std::vector<uint32_t> branches(txIDs.size(), 0);
	std::vector<uint32_t> generations(txIDs.size(), 0);

	char msg[BUFFER_SIZE];
	std::memset(&msg, 0, BUFFER_SIZE);
	std::string config = "{ \"presentBetweenThresh\": 0.7, \"minParaBatch\" : 2, \"historyFile\" : \"\", \"debug\" : true}";
	void* schdPtr = Start((char*)config.c_str(), config.size(), msg);
	Util::WriteFile(std::string(msg), "after-start.json");
	
	std::vector<char> lft;
	std::vector<char> rgt;
	History::Reformat({ Conflict("0x000000", "0x000000") }, lft, rgt);
	std::memset(&msg, 0, BUFFER_SIZE);
	UpdateHistory(schdPtr, (char*)lft.data(), (char*)rgt.data(), nullptr, (uint32_t)(1), msg);
	Util::WriteFile(std::string(msg), "after-update.json");
	
	// Add the second batch of conflict entires
	History::Reformat({ Conflict("0x111111", "0x111111") }, lft, rgt);
	std::memset(&msg, 0, BUFFER_SIZE);
	UpdateHistory(schdPtr, (char*)lft.data(), (char*)rgt.data(), nullptr, (uint32_t)(1), msg);

	auto probMatrix = ((Scheduler*)schdPtr)->history->probMatrix;
	Util::Print(probMatrix);

	// Try to generate a new schedule after the updates
	Util::WriteFile(std::string(msg), "pre-Schedule.json");
	std::memset(&msg, 0, BUFFER_SIZE);
	Schedule(schdPtr, (char*)callees.data(), txIDs.data(), order.data(), branches.data(), generations.data(), (uint32_t)callees.size() / ID_LEN, msg);
	Util::WriteFile(std::string(msg), "post-Schedule.json");
	Stop(schdPtr);

	std::sort(order.begin(), order.end());
	assert(order.size() == std::distance(order.begin(), std::unique(order.begin(), order.end())));

	return true;
}


bool TestSelfConflict2x1() {
	char msg[BUFFER_SIZE];
	std::memset(&msg, 0, BUFFER_SIZE);
	std::string config = "{	\"presentBetweenThresh\": 0.7,	\"minParaBatch\" : 2,\"historyFile\" : \"\",\"debug\" : false}";
	void* schdPtr = Start((char*)config.c_str(), config.size(), msg);

	/* ---------------------------------------------- Update History -----------------------------------------*/
	std::vector<Conflict> conflicts{
		Conflict("0x111111", "0x222222"),// 1:2
		Conflict("0x333333", "0x333333"),// 2:4
	};

	/* Check the probability matrix */
	std::vector<uint8_t> target{
		0,  255,  0,
		0,   0,   0,
		0,   0,  255 };

	RunUpdateHistory(true, schdPtr, conflicts, target, "first-update.json");

	/* ---------------------------------------------- Schedule -----------------------------------------*/
	std::vector<std::pair<std::string, uint32_t>> addresses{
		{ "0x111111", 17 },  // 0
		{ "0x222222", 16 },
		{ "0x333333", 1 }
	};

	return RunScheduler(
		"TestScheduleBasic()",
		schdPtr,
		addresses,
		Util::Range<uint32_t>(34, 0),
		Util::Concate<uint32_t> ({Util::Range<uint32_t>(17, 0), Util::Range<uint32_t>(16, 0), Util::Range<uint32_t>(1, 0)}),
		Util::Concate<uint32_t>({ std::vector<uint32_t>(17, 0), std::vector<uint32_t>(16, 1), std::vector<uint32_t>(1, 2) })
	);
}

bool Benchmarking1m() {
	char msg[BUFFER_SIZE];
	std::memset(&msg, 0, BUFFER_SIZE);
	std::string config = "{	\"presentBetweenThresh\": 0.7,	\"minParaBatch\" : 2,\"historyFile\" : \"\",\"debug\" : false}";
	void* schdPtr = Start((char*)config.c_str(), config.size(), msg);

	std::vector<char> callees(50000 * ID_LEN, 0);
	for (std::size_t i = 0; i < 50000; i++) {
		std::string address = "0x" + std::to_string(i);
		std::copy((char*)(address.data()), (char*)(address.data()) + address.size(), (char*)(callees.data() + i * ID_LEN));
	}

	std::vector<uint32_t>txIDs = Util::Range<uint32_t>(callees.size(), 0);

	std::vector<uint32_t> order(txIDs.size(), 0);
	std::vector<uint32_t> branches(txIDs.size(), 0);
	std::vector<uint32_t> generations(txIDs.size(), 0);

	Schedule(schdPtr, (char*)callees.data(), txIDs.data(), order.data(), branches.data(), generations.data(), (uint32_t)callees.size() / ID_LEN, msg);

	std::sort(order.begin(), order.end());
	assert(order.size() == std::distance(order.begin(), std::unique(order.begin(), order.end())));

	return true;
}
