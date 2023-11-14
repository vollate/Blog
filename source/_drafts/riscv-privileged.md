---
title: RISC-V Privileged ISA
date: 2023-10-17 00:43:01
tags: ISA
categories: RISC-V
---
## Introduction

### Terminology

- AEE: application execution environment
- ABI: application binary interface
- SEE: supervisor execution environment
- SBI: supervisor binary interface
- HEE: hypervisor execution environment
- HBI: hypervisor binary interface
- CSRs: control and status registers
- XLEN: X-register length, 表示基础整数寄存器位宽
- hart: hardware thread

![implementation](implementation.png)

### Privileged Levels

任何时候，RISC-V的线程运行在以下某种特权级下，特权级信息贮存在一个或多个CSR中
| Level | Encoding | Name             | Abbreviation |
| -     | -        | -                | -            |
| 0     | 00       | user/Application | U            |
| 1     | 01       | Supervisor       | S            |
| 2     | 10       | Hypervisor       | H            |
| 3     | 11       | Machine          | M            |

A device may not contain all privilege levels:

![privilege modes](privilege-modes.png)

### Debug Mode

D-mode can be considered an **additional privilege mode**, with even more access than M-mode. Debug mode reserves a few CSR addresses that are only accessible in D-mode, and may also reserve some portions of the physical address space on a platform

## CSRs

### CSR Address Mapping Conventions

**RISCV ISA aside a 12-bit encoding space, up to 4096 CSRs.**

Convention:

- 2 bits ( [11:10] ) encode the read or write accessibility of CSRs: r/w(00,10,01), read-only(11)
- 2 bits ( [9:8] ) encode the lowest privilege level that can access the register( as last chart says )

Exception cases:

- Access CSR that not exist
- Write to CSR without appropriate privilege
- Write to read-only CSR

>r/w registers might also contain some bits that are read-only, in which case writes to the read-only bits are ignored

### CSR Listing

>CSR及其对应地址: 定时器、计数器和浮点数 CSR 是标准用户级 CSR，其他寄存器被特权代码使用

**Allocation of RISC-V CSR address ranges**
![2.1](2.1.png)
**Currently allocated RISC-V unprivileged CSR addresses**
![2.2](2.2.png)
**Currently allocated RISC-V supervisor-level CSR addresses**
![2.3](2.3.png)
**Currently allocated RISC-V hypervisor and VS CSR addresses**
![2.4](2.4.png)
**Currently allocated RISC-V machine-level CSR addresses**
![2.5](2.5.png)
![2.6](2.6.png)

### CSR Field Specifications

CSR 字段属性:

- WPRI (Write Preserved, Reads Ignored): 保留字段，可写但是无意义，读取返回不定值或读取操作被忽略
- WLRL (Write/Read Only Legal): 只能写入合法值；读取时除非上次写入合法值或者经过reset操作，不应将返回值视为合法
- WARL (Write Any, Reads Legal): 可以写入任意值，但硬件仅接受合法值(非法不会报错)，读取总是合法值

### CSR Field Modulation

如果写入一个 CSR A 导致另一个 CSR B 的合法范围发生了变化，则 B 会从变化后的合法范围取一个随机值，即使范围变化后 B 原来的值仍然合法

### Implicit Reads of CSRs

实现有时会产生隐式读取，除非特殊说明，隐式读取和显示读取的返回值相同 (eg: 所有 S 模式指令获取都隐式读取 satp CSR )

### CSR Width Modulation

CSR位宽变化处理规则:

1. 将前一个宽度的 CSR 值复制到相同宽度的临时寄存器中。
2. 对于前宽度CSR的只读位，将临时寄存器中相同位置的位设置为零。
3. 临时寄存器的宽度更改为新宽度。如果新的宽度W比先前的宽度窄，则保留临时寄存器的最低有效W位并丢弃较高有效位。如果新的宽度比先前的宽度宽，则临时寄存器被零扩展至更宽的宽度。
4. 新宽度CSR的每个可写字段取临时寄存器中相同位置的位的值

## Machine-Level ISA, Version 1.12

### Machine-Level CSRs

M-mode 的代码可以访问 M 特权级的 CSR 和所有更低特权级的 CSR

#### Machine ISA Register misa

misa寄存器(早期名mcpuid): M 模式下的寄存器，提供了关于 CPU 的重要信息。它的位表示处理器支持的不同 RISC-V ISA 扩展。misa 寄存器是一个 XLEN 位的只读寄存器，包含了 CPU 实现性能相关的信息。这个寄
存器在所有实现中都必须是可读的，但是可以返回一个值 0，表示 CPU ID 特性并没有被实现，
需要通过一个单独的非标准机制来判断 CPU 性能

![misa](misa.png)
MXL(Machine XLEN): 表示当前模式下寄存器位宽
| MXL | XLEN |
| -   | -    |
| 1   | 32   |
| 2   | 64   |
| 3   | 128  |

如果写入misa导致MXL位宽改变，则MXL移动到改变后的最高2bits的位置上
<!--快速判断位宽:将4写入寄存器，左移31位后为零则为rv32-->

Extensions( [25:0] ):
![extensions](extensions.png)


#### Machine Status Registers (mstatus and mstatush)

![mstatus](mstatus.png)
![mstatush](mstatush.png)

##### Privilege and Global Interrupt-Enable Stack in mstatus register

MIE和SIE分别控制 M-mode 和 S-mode 下的全局中断，用于保证当前特权级下中断处理程序的原子性。中断处理遵循以下规则:

- 总是禁用低于自己特权级的中断
- 总是启用高于特权级的全局中断
- 根据当前特权级的 ${x}IE 寄存器位( x 为当前模式)是否为1判断是否启用当前特权级下中断
- 高特权级代码可以设置中断使能位来仅用一些高特权级中断，使得低特权级运行时不会产生该中断

##### Base ISA Control in mstatus Register

