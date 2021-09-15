#pragma once

struct History {
	Config* config;
	boost::numeric::ublas::matrix<uint8_t> probMatrix; // conflict probability 
	std::map<Contract<ID_LEN>, uint32_t> conflictContracts;
	std::map<Contract<ID_LEN>,  Stat> stats;
	
	std::size_t numBlocks = 0;
	std::string msg;

	History(Config* config) :config(config) { Load(); }
	~History() { 
		Save();
	}
	
	void AddContractStats(std::vector<Callee>& callees);
	void AddConflictStats(std::vector<Conflict>& conflicts);

	void UpdateHistory(std::vector<Conflict>& conflicts); 	// Add to the conflict history
	uint8_t At(uint32_t row, uint32_t col);

	uint32_t MaxIndex() { return conflictContracts.size(); }
	uint32_t GetIndex(Contract<ID_LEN>& contract) {
		if (conflictContracts.find(contract) != conflictContracts.end())
			return conflictContracts[contract];	
		return INVALID; 
	}

	Stat GetStat(Contract<ID_LEN>& contract) {
		Contract<ID_LEN> shortAddr(contract.To<ID_LEN>());
		return stats[shortAddr];
	}

	std::string Save();
	std::string Load();  // Save the conflict history

	std::string ToJasonString();		
	static void Reformat(std::vector<Conflict> conflicts, std::vector<char>& fromStr, std::vector<char>& toStr);
	
private:
	void Relabel(boost::numeric::ublas::matrix<uint8_t>& probMatrix, std::vector<Conflict>& conflicts);

	friend class boost::serialization::access;
	template<class T>
	void serialize(T & ar, const unsigned int version) {
		ar & conflictContracts;
		ar & stats;
		ar & probMatrix;
	}
};