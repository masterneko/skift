#pragma once

// https://five-embeddev.com/riscv-isa-manual/latest/priv-csrs.html

#include <karm-base/std.h>

namespace Riscv {

enum struct Csr : usize {
#define CSR(NUM, _, NAME) NAME = NUM,
#include "csr.inc"
#undef CSR
};

usize csrr(Csr csr) {
    usize tmp;
    switch (csr) {
#define CSR(NUM, name, NAME) \
    case Csr::NAME:          \
        __asm__ __volatile__("csrr %0, " #name : "=r"(tmp));
#include "csr.inc"
#undef CSR
    };
    return tmp;
}

void csrw(Csr csr, usize val) {
    switch (csr) {
#define CSR(NUM, name, NAME) \
    case Csr::NAME:          \
        __asm__ __volatile__("csrw " #name ", %0" ::"r"(val));
#include "csr.inc"
#undef CSR
    };
}

void unimp() { __asm__ __volatile__("unimp"); }

void wfi() { __asm__ __volatile__("wfi"); }

void di() { __asm__ __volatile__("csrci mstatus, 8"); }

void ei() { __asm__ __volatile__("csrsi mstatus, 8"); }

} // namespace Riscv
