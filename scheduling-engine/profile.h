#pragma once
using namespace boost::numeric::ublas;

struct Profile {
	std::vector<Callee> callees; // callee
	matrix<uint8_t> labelling;

	Profile() {}
	void Init(std::vector<uint32_t>& txIDs, std::vector<Contract<ID_LEN>>& addresses, History* history);

	std::vector<std::vector<Callee*>> PrefilterPar(Config* config, History* history, std::function<bool(Config*, History*)> condition);
	std::vector<std::vector<Callee*>> PrefilterPar(Config* config, History* history);  /* Based on history only*/
	std::vector<std::vector<Callee*>> PrefilterSeq(Config* config, History* history);  /* Based on history only*/

	std::vector<std::vector<Callee*>> FilterSeq(Config* config);  /* Special treatment for sequential only transactions*/
	std::vector<std::vector<Callee*>> Filter(
		Config* config,
		std::function<bool(Callee*, Callee*, matrix<uint8_t>&)> source,
		std::function<bool(Callee*, Callee*, matrix<uint8_t>&)> target,
		std::function<bool(Callee*, Callee*, matrix<uint8_t>&)> relationship);

	Raw GenerateRaw(Config* config, History* history) {	
		Raw rawSchedule;
		//rawSchedule.fullPar = PrefilterPar(config, history);
		//rawSchedule.fullSeq = PrefilterSeq(config, history);

		std::vector<std::vector<Callee*>> fullSeq = FilterSeq(config); // Filtering out the ones causing the most of conflict
		std::vector<std::vector<Callee*>> fullPar = Filter(config, par, par, par);
		std::vector<std::vector<Callee*>> parSeq  = Filter(config, seq, seq, par);

		Util::Concate(rawSchedule.fullPar, fullPar);
		Util::Concate(rawSchedule.fullSeq, fullSeq);
		Util::Concate(rawSchedule.parSeqs, parSeq);
		return rawSchedule;
	}

	bool parOnly(Callee& callee, uint32_t rc, Config* config, matrix<uint8_t>& labelling) {};
	bool IsSeq(Callee& callee, uint32_t rc, Config* config, matrix<uint8_t>& labelling);

	std::function<bool(Callee*, Callee*, matrix<uint8_t>&)> par = [](Callee* lft, Callee* rgt, matrix<uint8_t>& labelling)->bool {
		return labelling(lft->index, rgt->index) == NON_CONFLICT;
	};

	std::function<bool(Callee*, Callee*, matrix<uint8_t>&)> seq = [](Callee* lft, Callee* rgt, matrix<uint8_t>& labelling)->bool {
		return labelling(lft->index, rgt->index) != NON_CONFLICT;
	};

	static void Label(matrix<uint8_t>& labelling, std::vector<Callee>& targets, History* history);
	std::string ToJasonString();
};
