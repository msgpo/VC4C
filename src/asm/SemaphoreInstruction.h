/*
 * Author: doe300
 *
 * See the file "LICENSE" for the full license governing this code.
 */

#ifndef SEMAPHOREINSTRUCTION_H
#define SEMAPHOREINSTRUCTION_H

#include "Instruction.h"

namespace vc4c
{
    namespace qpu_asm
    {
        class SemaphoreInstruction final : public Instruction
        {
        public:
            explicit SemaphoreInstruction(uint64_t code) : Instruction(code) {}
            SemaphoreInstruction(Pack pack, ConditionCode condAdd, ConditionCode condMul, SetFlag sf, WriteSwap ws,
                Address addOut, Address mulOut, bool increment, Semaphore semaphore);

            std::string toASMString() const;
            inline bool isValidInstruction() const
            {
                if(getSig() != SIGNAL_LOAD_IMMEDIATE) // Semaphores and Loads have the same signal bit-mask
                    return false;
                return getEntry<OpSemaphore>(57, MASK_Septuple) == OpSemaphore::SEMAPHORE;
            }

            // NOTE: The pack value includes the pm bit!
            BITFIELD_ENTRY(Pack, Pack, 52, Quintuple)
            BITFIELD_ENTRY(AddCondition, ConditionCode, 49, Triple)
            BITFIELD_ENTRY(MulCondition, ConditionCode, 46, Triple)
            BITFIELD_ENTRY(SetFlag, SetFlag, 45, Bit)

            BITFIELD_ENTRY(IncrementSemaphore, bool, 4, Bit)
            BITFIELD_ENTRY(Semaphore, Semaphore, 0, Quadruple)
        };
    } // namespace qpu_asm
} // namespace vc4c

#endif /* SEMAPHOREINSTRUCTION_H */
