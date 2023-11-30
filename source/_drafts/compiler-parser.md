---
title: compiler-parser
tags: 
- Compiler
- Parser
categories: 
- Notes
---
这学期编译原理写 parser 作业的时候非常痛苦 ~~上课不听导致的~~，彻底搞懂后记录一下方便复习。

## Top-Down Parser

自顶向下的解析器，执行最左推导，每次对最左边的非终结符进行归约。


### FIRST 集

First 集是指，对于文法的每个非终结符，它能推导出的第一个 token 的集合。

First 集的计算方法：

- 如果 A 是一个终结符，那么 First(A) = A
- 如果 A 是一个非终结符，那么 First(A) = First(A) ∪ First(第一个 token)
- 如果 A → ε，那么 First(A) = First(A) ∪ ε

### FOLLOW 集

Follow 集是指，对于文法的每个非终结符，它能推导出的 token 的集合。

Follow 集的计算方法：

- 如果 A 是文法的开始符号，那么 Follow(A) = Follow(A) ∪ $
- 如果 A → X B Y，那么 Follow(B) = Follow(B) ∪ First(Y) - ε
- 如果 A → X B，或者 A → X B Y，且 First(Y) 包含 ε，那么 Follow(B) = Follow(B) ∪ Follow(A)

#### LL(1) Grammar

如果文法中的每个产生式都满足，那么这个文法是 LL(1) 的。

### LL(1) 解析器

LL(1) 解析器的构造方法：

- 构造一个预测分析表，表中的每个元素是一个产生

## Bottom-Up Parser

