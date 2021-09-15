#pragma once

struct Callee {
	Contract<ID_LEN>* contract; // contract address
	uint32_t ID = INVALID; // contract ID	
	uint32_t index = 0; // Index in the current labeling matrix 
	std::vector<uint32_t> txIDs;
	bool masked = false;

	Callee() {}
	Callee(Contract<ID_LEN>* contract, uint32_t ID, uint32_t index, uint32_t tx) : contract(contract), index(index), ID(ID) { txIDs.push_back(tx); }
	bool operator < (const Callee& other) const { return *contract < *other.contract; }

	void Append(uint32_t tx) {
		txIDs.push_back(tx);
	}

	std::string ToJasonString() {
		boost::property_tree::ptree root;
		root.put("ID", ID);
		root.put("index", index);
		root.add_child("contract", Util::ToJasonNode(contract->ToJasonString()));

		std::ostringstream outStream;
		boost::property_tree::write_json(outStream, root);
		return outStream.str();
	}
};

