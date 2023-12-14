ALL: scripts/count_lineages_country

scripts/count_lineages_country: scripts/count_lineages_country.cpp
	g++ --std=c++17 scripts/count_lineages_country.cpp -o scripts/count_lineages_country -O2 -lstdc++fs -lboost_regex
