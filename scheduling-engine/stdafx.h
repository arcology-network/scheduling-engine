#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>
#include <set>
#include <functional>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <execution>

#include <boost/version.hpp>
#include <boost/config.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/simple_segregated_storage.hpp>
#include <boost/filesystem.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/functional/hash.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>

#if defined __linux__ || defined __APPLE__
#include <tbb/tbb.h>
#include <tbb/parallel_invoke.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_unordered_set.h>
#include <tbb/concurrent_vector.h>
#include <cpuid.h>

#elif _WIN32
#include <ppl.h>
#include <concurrent_unordered_map.h>
#include <concurrent_unordered_set.h>
#include <concurrent_vector.h>
namespace tbb = Concurrency;
#else
#endif

#include "version.h"
#include "util.h"
#include "config.h"
#include "contract.h"
#include "conflict.h"
#include "callee.h"
#include "raw.h"
#include "stat.h"
#include "history.h"
#include "profile.h"
#include "scheduler.h"
#include "external.h"
#include "external_test.h"