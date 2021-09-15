#include "stdafx.h"

void History::AddContractStats(std::vector<Callee>& callees) {
	for (std::size_t i = 0; i < callees.size(); i++) {
		Contract<ID_LEN> contractAddr(callees[i].contract->To<ID_LEN>());
		auto iter = stats.find(contractAddr);
		if (iter == stats.end()) 
			stats.insert(std::make_pair(contractAddr, Stat(callees[i], stats.size())));
		else 
			iter->second.InsertStat(callees[i]);
	}
}

void History::AddConflictStats(std::vector<Conflict>& conflicts) {
	auto counter = [](std::array<uint8_t, ID_LEN>& address, std::map<Contract<ID_LEN>, Stat>& stats, Conflict& conflict) {
		auto iter = stats.find(Contract<ID_LEN>((char*)address.data(), ID_LEN));
		if (stats.find(Contract<ID_LEN>((char*)address.data(), ID_LEN)) == stats.end())
			return; // error
		stats[Contract<ID_LEN>((char*)(address.data()), ID_LEN)].UpdateConflicts(conflict);
	};

	for (std::size_t i = 0; i < conflicts.size(); i++) {
		counter(conflicts[i].from.address, stats, conflicts[i]);
		counter(conflicts[i].to.address, stats, conflicts[i]);
	}
}

uint8_t History::At(uint32_t row, uint32_t col) {
	if (row < probMatrix.size1() && col < probMatrix.size2())
		return probMatrix(row, col);
	return NON_CONFLICT;
}

void History::UpdateHistory(std::vector<Conflict>& conflicts) {
	numBlocks++;
	if (conflicts.empty()) {
		return;
	}
	
	std::sort(conflicts.begin(), conflicts.end());
	auto last = std::unique(conflicts.begin(), conflicts.end()); /* Remove duplicates */

	// Get the new address
	std::vector<Conflict> uniqueConflicts(conflicts.begin(), last);
	std::vector<Contract<ID_LEN>> newAddresses = Conflict::ToAddresses(uniqueConflicts);
	for (std::size_t i = 0; i < newAddresses.size(); i++) {
		if (!newAddresses[i].IsEmpty()) {
			if (conflictContracts.find(newAddresses[i]) == conflictContracts.end())
				conflictContracts[newAddresses[i]] = conflictContracts.size();
		}
	}

	/* Label the matrix */
	Util::Resize<uint8_t>(probMatrix, conflictContracts.size(), conflictContracts.size(), NON_CONFLICT);
	Relabel(probMatrix, conflicts);
	AddConflictStats(conflicts);
}

void History::Relabel(boost::numeric::ublas::matrix<uint8_t>& probMatrix, std::vector<Conflict>& conflicts) {
	for (std::size_t i = 0; i < conflicts.size(); i++) {
		if (conflictContracts.find(conflicts[i].from) == conflictContracts.end() || conflictContracts.find(conflicts[i].to) == conflictContracts.end())
			continue;
		probMatrix(conflictContracts[conflicts[i].from], conflictContracts[conflicts[i].to]) = CONFLICT;
	}
}

std::string History::Save() {
	return Util::Save<History>(this, config->historyFile);
}

std::string History::Load() {
	return Util::Load<History>(*this, config->historyFile);
}

std::string History::ToJasonString() {
	boost::property_tree::ptree conflictNode;
	std::for_each(conflictContracts.begin(), conflictContracts.end(), [&](auto iter) {
		auto first = iter.first;
		conflictNode.add_child(std::to_string(iter.second), Util::ToJasonNode(first.ToJasonString()));
	});

	boost::property_tree::ptree statNode;
	std::for_each(stats.begin(), stats.end(), [&](auto iter) {
		auto first = iter.first;
		auto tempNode = Util::ToJasonNode(iter.second.ToJasonString());
		
		std::string str = Util::ToString<uint8_t, ID_LEN>(first.address);
		tempNode.put("address", str);
		statNode.add_child("contract", tempNode);
	});

	boost::property_tree::ptree root;
	root.add_child("config", Util::ToJasonNode(config->ToJasonString()));
	root.add_child("probability", Util::ToJasonNode(Util::ToJasonString(probMatrix)));
	root.add_child("conflictContracts", conflictNode);
	root.add_child("stats", statNode);

	std::ostringstream outStream;
	boost::property_tree::write_json(outStream, root);
	return outStream.str();
}

void History::Reformat(std::vector<Conflict> conflicts, std::vector<char>& fromStr, std::vector<char>& toStr) {
	fromStr.resize(conflicts.size() * ID_LEN);
	toStr.resize(conflicts.size() * ID_LEN);
	for (std::size_t i = 0; i < conflicts.size(); i++) {
		std::copy(conflicts[i].from.address.begin(), conflicts[i].from.address.end(), fromStr.begin() + i * ID_LEN);
		std::copy(conflicts[i].to.address.begin(), conflicts[i].to.address.end(), toStr.begin() + i * ID_LEN);
	//	std::copy(conflicts[i].where.address.begin(), conflicts[i].where.address.end(), whereStr.begin() + i * ID_LEN);
	}
}
