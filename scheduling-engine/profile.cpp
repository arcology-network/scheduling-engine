#include "stdafx.h"

void Profile::Init(std::vector<uint32_t>& txIDs, std::vector<Contract<ID_LEN>>& contracts, History* history) {
	uint32_t labelIdx = 0;
	std::map<Contract<ID_LEN>, Callee> dict;
	for (std::size_t i = 0; i < contracts.size(); i++) {
		if (dict.find(contracts[i]) == dict.end() && history->GetIndex(contracts[i]) != INVALID) {
			dict[contracts[i]] = Callee(&contracts[i], history->GetIndex(contracts[i]), dict.size(), txIDs[i]);
			continue;
		}

		if (dict.find(contracts[i]) == dict.end())
			dict[contracts[i]] = Callee(&contracts[i], history->GetIndex(contracts[i]), dict.size(), txIDs[i]);
		else
			dict[contracts[i]].Append(txIDs[i]);
	}

	callees.clear();
	std::for_each(dict.begin(), dict.end(), [&](auto iter) { callees.push_back(iter.second); });
	Label(labelling, callees, history);
}

void Profile::Label(boost::numeric::ublas::matrix<uint8_t>& labelling, std::vector<Callee>& callees, History* history) {
	labelling.resize(callees.size(), callees.size());

	tbb::parallel_for(std::size_t(0), callees.size(), [&](std::size_t i) {
//	for (std::size_t i = 0; i < callees.size(); i++) {
		for (size_t j = 0; j < callees.size(); j++) {
			if (!callees[i].masked && !callees[j].masked)
				labelling(callees[i].index, callees[j].index) = history->At(callees[i].ID, callees[j].ID);
		}
	});
}

/* Special treatment for parallel only transactions*/
std::vector<std::vector<Callee*>> Profile::PrefilterPar(Config* config, History* history, std::function<bool(Config*, History*)> condition) {
	std::vector<Callee*> historyPar;
	for (std::size_t i = 0; i < callees.size(); i++)
		if (!callees[i].masked) {
			Stat stat = history->GetStat(*callees[i].contract);
			if (stat.ID == INVALID) { // Not found in the conflict history 
				historyPar.push_back(&callees[i]);
				callees[i].masked = true;
			}
		}

	return std::vector<std::vector<Callee*>>(1, historyPar);
}

/* Special treatment for parallel only transactions*/
std::vector<std::vector<Callee*>> Profile::PrefilterSeq(Config* config, History* history) {
	std::vector<Callee*> historySeq;
	for (std::size_t i = 0; i < callees.size(); i++)
		if (!callees[i].masked) {
			Stat stat = history->GetStat(*callees[i].contract);
			if (stat.ID != INVALID && stat.numConflicts / std::max<float>(stat.invocations, 0.0) >  config->historyBetweenThresh) {
				historySeq.push_back(&callees[i]);
				callees[i].masked = true;
			}
		}
	return std::vector<std::vector<Callee*>>(1, historySeq);
}


/* Special treatment for sequential only transactions*/
std::vector<std::vector<Callee*>> Profile::FilterSeq(Config* config) {
	tbb::concurrent_vector<Callee*> sequence;
	//for (std::size_t i = 0; i < callees.size(); i++) {
	tbb::parallel_for(std::size_t(0), callees.size(), [&](std::size_t i) {
		if (!callees[i].masked)
			if (IsSeq(callees[i], callees[i].index, config, this->labelling))
				sequence.push_back(&callees[i]);
	});
	tbb::parallel_for(std::size_t(0), sequence.size(), [&](std::size_t i) {sequence[i]->masked = true; });
	tbb::parallel_sort(sequence.begin(), sequence.end());
	if (!sequence.empty())
		return std::vector<std::vector<Callee*>>(1, std::vector<Callee*>(sequence.begin(), sequence.end()));
	return std::vector<std::vector<Callee*>>();
}

std::vector<std::vector<Callee*>> Profile::Filter(
	Config* config,
	std::function<bool(Callee*, Callee*, matrix<uint8_t>&)> source,
	std::function<bool(Callee*, Callee*, matrix<uint8_t>&)> target,
	std::function<bool(Callee*, Callee*, matrix<uint8_t>&)> relationship) {
	std::vector<std::vector<Callee*>> batches;
	while (true) {
		std::vector<Callee*> batch;
		for (std::size_t i = 0; i < callees.size(); i++) {
			if (callees[i].masked || !source(&callees[i], &callees[i], labelling))
				continue;
		
			if (batch.empty() && target(&callees[i], &callees[i], labelling)) {
				batch.push_back(&callees[i]);
				callees[i].masked = true;
				continue;
			}
			                          
			if (!batch.empty() && std::find_if(std::execution::par, batch.begin(), batch.end(), [&](Callee* current) { return !relationship(current, &callees[i], labelling); }) == batch.end()) {
				batch.push_back(&callees[i]);
				callees[i].masked = true;
			}
		}

		if (batch.empty())
			break; // Found nothing new

		batches.push_back(batch);
	}
	return batches;
}


bool Profile::IsSeq(Callee& callee, uint32_t rc, Config* config, matrix<uint8_t>& labelling) {
	uint32_t rowCount = std::count_if((labelling.begin1() + rc).begin() + rc, (labelling.begin1() + rc).end(), [&](uint8_t v) { return v > NON_CONFLICT; });  // Row Count
	uint32_t colCount = std::count_if((labelling.begin2() + rc).begin(), (labelling.begin2() + rc).begin() + rc + 1, [&](uint8_t v) { return v > NON_CONFLICT; }); // Col Count

	float rowRatio = rowCount / float(labelling.size1() - rc);
	float colRatio = colCount / float(rc + 1);

	float ratio = std::max<float>(rowRatio, colRatio);
	return callee.txIDs.size() < config->minParaBatch && ratio > config->presentBetweenThresh;
}


std::string Profile::ToJasonString() {
	std::vector<Callee> sorted = callees;
	std::sort(sorted.begin(), sorted.end(), [](Callee& lft, Callee& rgt) { return lft.ID < rgt.ID; });

	boost::property_tree::ptree calleeNode;
	for (size_t i = 0; i < sorted.size(); i++) {
		calleeNode.add_child("callee", Util::ToJasonNode(sorted[i].ToJasonString()));
	}

	boost::property_tree::ptree root;
	root.add_child("callees",   calleeNode);
	root.add_child("labelling", Util::ToJasonNode(Util::ToJasonString(labelling)));

	std::ostringstream outStream;
	boost::property_tree::write_json(outStream, root);
	return outStream.str();
}


