//
// Created by pointerlost on 3/27/26.
//
#pragma once
#include "Core/TypedHandle.h"

namespace Real::core {

    template <typename T>
    class SlotMap {
    public:
        SlotHandle Add(T data) {
            if (!m_FreedSlots.empty()) {
                u32 index = m_FreedSlots.back();
                m_FreedSlots.pop_back();

                m_Slots[index].data      = std::move(data);
                m_Slots[index].occupied  = true;
                // generation already incremented during Remove()

                return { index, m_Slots[index].generation };
            }

            u32 index = static_cast<u32>(m_Slots.size());
            m_Slots.push_back({ std::move(data), 0, true });
            return { index, 0 };
        }

        void Remove(SlotHandle handle) {
            if (!IsValid(handle))
                return;

            ++m_Slots[handle.index].generation;   // invalidate existing handles
            m_Slots[handle.index].occupied = false;
            m_Slots[handle.index].data     = T{}; // reset data

            m_FreedSlots.push_back(handle.index); // only store index
        }

        [[nodiscard]] bool IsValid(SlotHandle handle) const {
            if (handle.index >= m_Slots.size())                        return false;
            if (!m_Slots[handle.index].occupied)                       return false;
            if (m_Slots[handle.index].generation != handle.generation) return false;
            return true;
        }

        T* Get(SlotHandle handle) {
            if (!IsValid(handle))
                return nullptr;
            return &m_Slots[handle.index].data;
        }

        template <typename Func>
        void ForEach(Func&& fn) {
            for (auto& slot : m_Slots)
                if (slot.occupied)
                    fn(slot.data);
        }

    private:
        struct Slot {
            T    data;
            u32  generation = 0;
            bool occupied   = false;
        };

        Vector<Slot> m_Slots;
        Vector<u32>  m_FreedSlots; // recycled indices
    };
}
