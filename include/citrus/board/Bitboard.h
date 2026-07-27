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

constexpr uint64_t FILE_A = 0x0101010101010101;
constexpr uint64_t FILE_B = FILE_A << 1;
constexpr uint64_t FILE_C = FILE_A << 2;
constexpr uint64_t FILE_D = FILE_A << 3;
constexpr uint64_t FILE_E = FILE_A << 4;
constexpr uint64_t FILE_F = FILE_A << 5;
constexpr uint64_t FILE_G = FILE_A << 6;
constexpr uint64_t FILE_H = FILE_A << 7;

constexpr uint64_t not_FILE_A = ~FILE_A;
constexpr uint64_t not_FILE_H = ~FILE_H;
constexpr uint64_t not_FILE_AB = ~(FILE_A | FILE_B);
constexpr uint64_t not_FILE_GH = ~(FILE_G | FILE_H);

constexpr uint64_t RANK_1 = 0x00000000000000FF;
constexpr uint64_t RANK_2 = RANK_1 << 8;
constexpr uint64_t RANK_3 = RANK_1 << 16;
constexpr uint64_t RANK_4 = RANK_1 << 24;
constexpr uint64_t RANK_5 = RANK_1 << 32;
constexpr uint64_t RANK_6 = RANK_1 << 40;
constexpr uint64_t RANK_7 = RANK_1 << 48;
constexpr uint64_t RANK_8 = RANK_1 << 56;
