#include "citrus/search/TTable.h"

TTable::Entry& TTable::operator[](uint64_t key) {
	int index = key & (size - 1); // hash function
	return table[index];
}
