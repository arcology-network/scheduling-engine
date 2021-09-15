#pragma once
struct Stat {
	uint32_t ID = INVALID;
	uint32_t invocations = 0;
	uint32_t numBlocks = 0;
	uint32_t numConflicts = 0;
	uint32_t selfConflicts = 0;

	Stat() {}
	Stat(Callee& callee, uint32_t ID): ID(ID) { InsertStat(callee); }

	void InsertStat(Callee& callee) {
		invocations += callee.txIDs.size();
		numBlocks++;
	}

	void UpdateConflicts(Conflict& conflict) {
		if (conflict.from == conflict.to)
			selfConflicts++;
		else
			numConflicts++;
	}
	
	std::string ToJasonString() {
		boost::property_tree::ptree root;
		root.put("ID", ID);
		root.put("invocations", invocations);
		root.put("numBlocks", numBlocks);
		root.put("totalConflicts", numConflicts);
		root.put("selfConflicts", selfConflicts);

		std::ostringstream outStream;
		boost::property_tree::write_json(outStream, root);
		return outStream.str();
	}
	
private:
	friend class boost::serialization::access;
	template<class T>
	void serialize(T & ar, const unsigned int version) {
		ar & ID;
		ar & invocations;
		ar & numBlocks;
		ar & numConflicts;
		ar & selfConflicts;
	}
};
