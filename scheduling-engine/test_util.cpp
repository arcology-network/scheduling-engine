#include "stdafx.h"

std::vector<std::pair<std::string, uint32_t>> ReadCSV(std::string filename) {
	std::vector<std::string> addresses = Util::ReadFile(filename);
	std::map<std::string, uint32_t> addressDict;
	for (std::size_t i = 0; i < addresses.size(); i++) {
		std::string addr;
		std::vector<std::string> line;
		boost::split(line, addresses[i], boost::is_any_of(" "));
		for (std::size_t j = 0; j < line.size(); j++) {
			if (!line[j].empty()) {
				int v = uint8_t(line[j].c_str());
				addr.push_back(v);
			}
		}

		if (addressDict.find(addr) == addressDict.end())
			addressDict[addr] = 1;
		else 
			addressDict[addr] ++;	
	}
	std::vector<std::pair<std::string, uint32_t>> vec(addressDict.begin(), addressDict.end());
	std::sort(vec.begin(), vec.end());
	return vec;
}

std::vector<char> Vectorize(std::vector<std::pair<std::string, uint32_t>> callees) {
	std::vector<std::string> addresses;
	for (std::size_t i = 0; i < callees.size(); i++)
		for (std::size_t j = 0; j < callees[i].second; j++) {
			addresses.push_back(callees[i].first);
		}

	std::vector<char> vectorized(addresses.size() * ID_LEN);
	for (std::size_t i = 0; i < addresses.size(); i++)
		std::copy(addresses[i].begin(), addresses[i].end(), vectorized.begin() + i * ID_LEN);
	return vectorized;
}

bool RunScheduler(
	std::string testName,
	void* schdPtr, 
	std::vector<std::pair<std::string, uint32_t>> callees,
	std::vector<uint32_t> targetOrder, 
	std::vector<uint32_t> targetBranches,
	std::vector<uint32_t> targeGens) {

	uint32_t total = std::accumulate(callees.begin(), callees.end(), 0, [](uint32_t sum, auto callee) { return sum + callee.second; });	
	std::vector<uint32_t> txIDs = Util::Range<uint32_t>(total, 0);
	std::vector<uint32_t> order(total, 0);
	std::vector<uint32_t> branches(total, 0);
	std::vector<uint32_t> generations(total, 0);
		
	std::vector<char> vectorized = Vectorize(callees);

	char msg[BUFFER_SIZE];
	std::memset(msg, BUFFER_SIZE, 0);
	Schedule(schdPtr, (char*)vectorized.data(), txIDs.data(), order.data(), branches.data(), generations.data(), (uint32_t)vectorized.size() / ID_LEN, msg);
	
	auto PrintVec = [](std::string title, std::vector<uint32_t>& vec) { std::for_each(vec.begin(), vec.end(), [](auto v) {std::cout << v + ","; }); std::cout << std::endl; };

	PrintVec("Target Order:", targetOrder);
	PrintVec("Actual Order:", order);

	PrintVec("Target branches:", targetBranches);
	PrintVec("Actual branches:", branches);

	PrintVec("Target generations:", targeGens);
	PrintVec("Actual generations:", generations);
	
	assert(order == targetOrder);
	assert(branches == targetBranches);
	assert(generations == targeGens);
	return order == targetOrder && branches == targetBranches && generations == targeGens;
}

/* Update the conflict history */
bool RunUpdateHistory(bool clearHistory, void* schdPtr, std::vector<Conflict> conflicts, std::vector<uint8_t> targetProbVec, std::string logFileName) {
	if (clearHistory)
		std::filesystem::remove("history.bin");

	std::vector<char> lft;
	std::vector<char> rgt;
	std::vector<char> where;
	History::Reformat(conflicts, lft, rgt);

	char msg[BUFFER_SIZE];
	std::memset(msg, BUFFER_SIZE, 0);
	UpdateHistory(schdPtr, (char*)lft.data(), (char*)rgt.data(), (char*)where.data(), (uint32_t)conflicts.size(), msg);
	Util::WriteFile(std::string(msg), logFileName);
	
	auto probMatrix = ((Scheduler*)schdPtr)->history->probMatrix;
	std::vector<uint8_t> probVec(probMatrix.begin1().begin(), probMatrix.end1().begin());
	Util::Print(probMatrix);

	if (probVec != targetProbVec) {
		std::cout << "Error: Wrong probability matrix !";
	}

	assert(targetProbVec == probVec);
	return targetProbVec != probVec;
}
