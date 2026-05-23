#pragma once

#include <cstdint>

/**
 * \brief Calculates the population count (or number of pieces) in a given bitboard.
 * 
 * \param bb the bitboard.
 * \return the population count.
 */
int popcount(uint64_t bb);

/**
 * \brief Pops the least significant bit off a given bitboard.
 * 
 * Useful for iterating over occupied squares efficiently.
 * 
 * \param bb the bitboard
 * \return the square of the popped bit (from 0-63)
 */
int pop_lsb(uint64_t &bb);
