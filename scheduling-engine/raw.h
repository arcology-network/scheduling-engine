#pragma once

struct Raw {
	std::vector<std::vector<Callee*>> fullSeq ; // Filtering out the ones causing the most of conflict
	std::vector<std::vector<Callee*>> fullPar;
	std::vector<std::vector<Callee*>> parSeqs;
	
	Raw() {}
	Raw(std::vector<std::vector<Callee*>> fullSeq, std::vector<std::vector<Callee*>> fullPar, std::vector<std::vector<Callee*>> parSeqs) : fullSeq(fullSeq), fullPar(fullPar), parSeqs(parSeqs) {}
};