#pragma once

struct Scheduler {
	Config* config;
	Profile* profile;
	History* history;
	std::vector<std::string> logs;
	
	Scheduler(std::string msg);
	~Scheduler();

	void schedule(std::vector<uint32_t>& txIDs, std::vector<Contract<ID_LEN>>& callees, std::vector<uint32_t>& txs, std::vector<uint32_t>& branches, std::vector<uint32_t>& generations);
	bool CheckTxs(std::vector<uint32_t>& txs);
	void UpdateHistory(std::vector<Conflict>& conflicts);
		
	std::string GetDebugInfo();
	std::string ToJasonString();

	static void Safeguard(std::vector<uint32_t>& tx, std::vector<uint32_t>& branches, std::vector<uint32_t>& generations, std::vector<Conflict>& conflicts);
};
