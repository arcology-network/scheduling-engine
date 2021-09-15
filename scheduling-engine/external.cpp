#include "stdafx.h"

void* Start(char* configContents, uint32_t length, char* msgBuffer) {
	auto scheduler = new Scheduler(std::string(configContents, length));
	std::string msg = scheduler->GetDebugInfo();
	std::strncpy(msgBuffer, msg.c_str(), std::min<uint32_t>(msg.size(), BUFFER_SIZE));
	return scheduler;
}

void Stop(void* enginePtr) {
	delete (Scheduler*)(enginePtr);
}

void SaveHistory(void* enginePtr, char* msgBuffer) {
	Scheduler* scheduler = (Scheduler*)(enginePtr);

	// Debug only
	std::string msg = scheduler->GetDebugInfo();
	std::strncpy(msgBuffer, msg.c_str(), std::min<uint32_t>(msg.size(), BUFFER_SIZE));
}

void Schedule(void* enginePtr, char* calleeAddrs, uint32_t* txIdPtr, uint32_t* txOrder, uint32_t* branches, uint32_t* generations, uint32_t count, char* msgBuffer) {
	Scheduler* scheduler = (Scheduler*)(enginePtr);
	std::vector<uint32_t> txIDs(count, 0);
	std::vector<Contract<ID_LEN>> addresses(count);
	for (std::size_t i = 0; i < count; i++) {	
		txIDs[i] = ((uint32_t*)txIdPtr)[i];
		addresses[i] = Contract<ID_LEN>(calleeAddrs + i * ID_LEN, ID_LEN);
	}

	std::vector<uint32_t> txVec;
	std::vector<uint32_t> brancheVec;
	std::vector<uint32_t> generationVec;
	scheduler->schedule(txIDs, addresses, txVec, brancheVec, generationVec);
	
	for (std::size_t i = 0; i < txVec.size(); i++) {
		((uint32_t*)txOrder)[i] = txVec[i];
		((uint32_t*)branches)[i] = brancheVec[i];
		((uint32_t*)generations)[i] = generationVec[i];
	}

	// Debug messages	
	if (scheduler->history->config->debug) {
		std::string msg = scheduler->GetDebugInfo();
		for (std::size_t i = 0; i < addresses.size(); i++)
			msg += addresses[i].ToJasonString() + '\n';		
		std::strncpy(msgBuffer, msg.c_str(), std::min<uint32_t>(msg.size(), BUFFER_SIZE));
	}	
}

void UpdateHistory(void* enginePtr, char* from, char* to, char* where, uint32_t count, char* msgBuffer) {
	Scheduler* scheduler = (Scheduler*)(enginePtr);

	std::vector<Conflict> conflicts(count);
	//for (std::size_t i = 0; i < conflicts.size(); i++) {
	tbb::parallel_for(std::size_t(0), conflicts.size(), [&](std::size_t i) {		
		conflicts[i] = Conflict(from + i * ID_LEN, to + i * ID_LEN);
	});

	scheduler->UpdateHistory(conflicts);

	// Debug only
	std::string msg = scheduler->GetDebugInfo();
	std::strncpy(msgBuffer, msg.c_str(), std::min<uint32_t>(msg.size(), BUFFER_SIZE));
}

void GetVersion(char* ver) {
	std::strcpy(ver, version);
}

void GetProduct(char* productInfo) {
	std::strcpy(productInfo, product);
}