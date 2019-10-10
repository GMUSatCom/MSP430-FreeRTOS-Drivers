/*
 * SPI.h
 *
 *  Created on: Sep 20, 2019
 *      Author: David Fiumano
 */

#pragma once

#include "eUSCI_A.h"
#include "../Memory/DMA.h"
#include <stdint.h>
#include <string.h> // size_t

#define SPI_MODE0 {UCSSEL__SMCLK, 0x2, SatLib::SPI::ClockPolarity::LOW, SatLib::SPI::ClockPhase::CPHA_0, SatLib::SPI::BitOrder::MSB, SatLib::SPI::Role::MASTER, SatLib::SPI::CharacterSize::BITS8, 0, 1}
#define SPI_MODE1 {UCSSEL__SMCLK, 0x2, SatLib::SPI::ClockPolarity::LOW, SatLib::SPI::ClockPhase::CPHA_1, SatLib::SPI::BitOrder::MSB, SatLib::SPI::Role::MASTER, SatLib::SPI::CharacterSize::BITS8, 0, 1}
#define SPI_MODE2 {UCSSEL__SMCLK, 0x2, SatLib::SPI::ClockPolarity::HIGH, SatLib::SPI::ClockPhase::CPHA_0, SatLib::SPI::BitOrder::MSB, SatLib::SPI::Role::MASTER, SatLib::SPI::CharacterSize::BITS8, 0, 1}
#define SPI_MODE3 {UCSSEL__SMCLK, 0x2, SatLib::SPI::ClockPolarity::HIGH, SatLib::SPI::ClockPhase::CPHA_1, SatLib::SPI::BitOrder::MSB, SatLib::SPI::Role::MASTER, SatLib::SPI::CharacterSize::BITS8, 0, 1}

namespace SatLib
{
    class SPI
    {
    public:
        enum ModuleType : uint16_t {A, B};
        enum ClockPolarity : uint16_t {LOW = 0, HIGH = UCCKPL};
        enum BitOrder : uint16_t {LSB = 0, MSB = UCMSB};
        enum Role : uint16_t {SLAVE = 0, MASTER = UCMST};
        enum CharacterSize : uint16_t {BITS8 = 0, BITS7 = UC7BIT};
        enum ClockPhase : uint16_t {CPHA_1 = 0, CPHA_0 = UCCKPH};

        enum TransactionError {NO_ERR, RX_DMA_NOT_FREE, TX_DMA_NOT_FREE, UNSUPPORTED_DMA_SELECTION_TX, UNSUPPORTED_DMA_SELECTION_RX, FREQ_TOO_HIGH, UNSUPPORTED_FEATURE};

        typedef struct
        {
            int clockSelect;
            unsigned int ClockDivisor;
            ClockPolarity polarity;
            ClockPhase phase;
            BitOrder order;
            Role role;
            CharacterSize chSize;
            uint8_t DMARxChannel;
            uint8_t DMATxChannel;
        }SPI_Settings;

    private:

        uint8_t * TxBuffer;
        uint8_t * RxBuffer;

        size_t TxBufferIterator = 0;
        size_t TxBufferSize = 0;

        size_t RxBufferIterator = 0;
        size_t RxBufferSize = 0;

        size_t TransactionByteSize = 0;

        eUSCI_A_Type * registers;

        const ModuleType registersType;

        DMA_Channel_type * RxChannel = NULL;
        DMA_Channel_type * TxChannel = NULL;

        unsigned int Tx_DMAxTSEL;
        unsigned int Rx_DMAxTSEL;

        inline TransactionError acquireRxChannel(SPI_Settings settings, TickType_t delay);
        inline TransactionError acquireTxChannel(SPI_Settings settings, TickType_t delay);

        inline void configurePins(); // configures pin muxing for SPI

    public:

        SPI(ModuleType type, uint8_t moduleNumber);

        // takes appropriate DMA mutexes and set the module settings. Blocks for msToWait seconds, unless msToWait is negative, in which case it blocks indefinitely waiting for the Rx and Tx DMA channels
        // max wait time is msToWait * 2 since it waits that long for both Rx and Tx max
        TransactionError beginTransaction(uint8_t * TxBuffer, size_t TxSize, uint8_t * RxBuffer, size_t RxSize, SPI_Settings settings = SPI_MODE0, int16_t msToWait = -1);

        void transfer(); // actually transmits the things you have written. Fills up the RxBuffer based on what is received. If there is a slave select pin, pull it to the desired configuration before calling this function.

        bool write(uint8_t byte);

        size_t write(uint8_t * bytes, size_t size); // writes size number of bytes or until the end of the buffer. returns the actual number of bytes written

        uint8_t read();
        size_t read(uint8_t * bytes, size_t bytesToRead); // reads bytesToRead number of bytes or until the buffer is exhausted. returns the number of bytes actually read

        void endTransaction(); // returns mutexes. If SPI was in the slave role, it does not deinitialize the SPI port. Otherwise, it resets it.

    };
}
