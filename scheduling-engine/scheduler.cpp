#include "stdafx.h"

Scheduler::Scheduler(std::string configContents) {
	config = new Config(configContents);
	history = new History(config);
	profile = new Profile();
}

Scheduler::~Scheduler() {
	delete profile;
	delete history;
	delete config;
}

void Scheduler::schedule(std::vector<uint32_t>& txIDs, std::vector<Contract<ID_LEN>>& contracts, std::vector<uint32_t>& txs, std::vector<uint32_t>& branches, std::vector<uint32_t>& generations) {
	std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
	profile->Init(txIDs, contracts, history);
	Raw raw = profile->GenerateRaw(config, history);
	logs.push_back("Initialization + GenerateRaw: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count()) + " ms" + '\n');

	t0 = std::chrono::steady_clock::now();	
	history->AddContractStats(profile->callees);
	logs.push_back("Add stats: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count()) + " ms" + '\n');

	t0 = std::chrono::steady_clock::now();
	/* Fully parallel */
	for (std::size_t i = 0; i < raw.fullPar.size(); i++) {
		for (std::size_t j = 0; j < raw.fullPar[i].size(); j++)
			Util::Concate(txs, raw.fullPar[i][j]->txIDs);	

		Util::Concate(branches, Util::Range<uint32_t>(txs.size() - branches.size(), 0));
		Util::Concate(generations, std::vector<uint32_t>(txs.size() - generations.size(), Util::Last(generations)));
	}

	/* Parallel + sequential */
	for (std::size_t i = 0; i < raw.parSeqs.size(); i++) {
		for (std::size_t j = 0; j < raw.parSeqs[i].size(); j++) {
			Util::Concate(txs, raw.parSeqs[i][j]->txIDs);
			Util::Concate(branches, std::vector<uint32_t>(txs.size() - branches.size(), j));
		}
		Util::Concate(generations, std::vector<uint32_t>(txs.size() - generations.size(), Util::Last(generations)));
	}
	
	/* Clear whatever remains */
	for (std::size_t i = 0; i < raw.fullSeq.size(); i++)
		for (std::size_t j = 0; j < raw.fullSeq[i].size(); j++) 
			Util::Concate(txs, raw.fullSeq[i][j]->txIDs);

	Util::Concate(branches, std::vector<uint32_t>(txs.size() - branches.size(), 0));
	Util::Concate(generations, std::vector<uint32_t>(txs.size() - generations.size(), Util::Last(generations)));
	logs.push_back("Reformation: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count()) + " ms" + '\n');
}

void Scheduler::UpdateHistory(std::vector<Conflict>& conflicts) {
	if (!conflicts.empty())
		history->UpdateHistory(conflicts); // add the conflict records
}

void Scheduler::Safeguard(std::vector<uint32_t>& txs, std::vector<uint32_t>& branches, std::vector<uint32_t>& generations, std::vector<Conflict>& conflicts) {
	//for (std::size_t i = 0; i < profile->callees.size(); i++)
	//	if (!profile->callees[i].masked)
	//		Util::Concate<uint32_t>(txs, profile->callees[i].txIDs);
}

bool Scheduler::CheckTxs(std::vector<uint32_t>& txs) {
	std::vector<uint32_t> txSet;
	txSet.reserve(txs.size());
	for (std::size_t i = 0; i < profile->callees.size(); i++)
		for (std::size_t j = 0; j < profile->callees[i].txIDs.size(); j++)
			Util::Concate(txSet, profile->callees[i].txIDs);

	std::sort(txs.begin(), txs.end());
	std::sort(txSet.begin(), txSet.end());
	return txs == txSet;
}

std::string Scheduler::GetDebugInfo() {
	if (history->config->debug)
		return ToJasonString();
	return "";
}

std::string Scheduler::ToJasonString() {
	boost::property_tree::ptree root;
	for (std::size_t i = 0; i < logs.size(); i++) 
		root.add_child(std::to_string(i), boost::property_tree::ptree(logs[i]));

	root.add_child("product", boost::property_tree::ptree(product));
	root.add_child("version", boost::property_tree::ptree(version));
	root.add_child("time", boost::property_tree::ptree(Util::Now()));
	root.add_child("history", Util::ToJasonNode(history->ToJasonString()));
	root.add_child("profile", Util::ToJasonNode(profile->ToJasonString()));
	
	std::ostringstream outStream;
	boost::property_tree::write_json(outStream, root);
	return outStream.str();
}