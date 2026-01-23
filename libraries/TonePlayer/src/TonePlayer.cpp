/*
 * TonePlayer.cpp
 * 
 * A library for playing melody in background process using the builtin tone() function.
 * No needed for delay() function to waste the MCU processing power.
 * It parses melodic tones from PROGMEM to conserve the SRAM.
 * The frequency of each tone is computed instead of being stored as constant values.
 * Supports repeat to a specified number of tones till five levels of stack.
 * 
 * By ZulNs, @Gorontalo, December 2022
 */


// The entire code is in the header file, to provide the user configure options with #define (before they include it).
