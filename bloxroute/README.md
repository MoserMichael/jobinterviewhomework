# bloxroutehomework

Interview assignment for bloxroute.

The assignment
==============

* Parse a file formatted according to bitcoin protocol (clarification: the sample file only had a block message, so that's what I am doing here)
* The relevant bitcoin protocol can be found here (https://en.bitcoin.it/wiki/Protocol_documentation)
* Extract the 100 longest transactions in the block (clarification: by size of transaction data, not by length of transaction chain)
* Create data structure(s) that will allow:
  - Iteration on the data structure(s) based on the transaction length
  - Lookup a transaction by its hash value and return if it in the 100 longest transactions
* For each transaction you should save the following:
  - Sequence position in the block
  - Transaction hash value
  - Length, Starting offset and Ending offset


The deliverable
===============

A command line program that does it.

build it with make (Makefile incldued). Requires gcc and presense of openssl library.
Works on Ubuntu 18.4.2 LTS; used gcc 7.4.0

```
  ./program -h
  ./program [-h|-f <file_name> [-l | -s <hash>]]

   -h        print help message
   -f <file> parse file with bitcoin protocol messages
   -l        list all hunded longest transactions
   -s <hash> lookup entry by hash
   -t        print traces
```

Example usage:

Find a transaction by hash in the list of 100 longest transaction and print it:
```
  ./program -f sample_block_bin -s d483d48decfa82509f4dd070b05010f589efcaebc95016e2f1ec56a328cf67bd 
  lookup of hash=d483d48decfa82509f4dd070b05010f589efcaebc95016e2f1ec56a328cf67bd
  lookup of hash=d483d48decfa82509f4dd070b05010f589efcaebc95016e2f1ec56a328cf67bd

  found entry!
  block_index=0 tx_index=30760 start_pos=6930429 end_pos=6930655 length=226 hash=d483d48decfa82509f4dd070b05010f589efcaebc95016e2f1ec56a328cf67bd
```
List the 100 longest transactions
```
  ./program -f sample_block_bin -l

  block_index=0 tx_index=30812 start_pos=6942172 end_pos=6942398 length=226 hash=d3b53fa254d25d1515ac508a11e40850ad87b8e2bbe7e6b18f6ce01035b51a27
  block_index=0 tx_index=30811 start_pos=6941946 end_pos=6942172 length=226 hash=d3b6c19db83c663b245bf786b59681074add06d24d170c95b51702a940d5d21f
  block_index=0 tx_index=30833 start_pos=6946910 end_pos=6947136 length=226 hash=d37034d89642c8c3360f3f255df509561209bb047904543d4f28865b09c3ae53
  block_index=0 tx_index=30809 start_pos=6941494 end_pos=6941720 length=226 hash=d3bb50037f20d02a054ea3ff9b89e723e1b8e28121a0be45cd71dd0aa333d290
  block_index=0 tx_index=30808 start_pos=6941268 end_pos=6941494 length=226 hash=d3bfea499ca8243e75e601d28331b13575646f6967a4dfb232c7dbe8d7a7cfa5
  block_index=0 tx_index=30807 start_pos=6941042 end_pos=6941268 length=226 hash=e8b92cd54150f45389aa7bae7e2966f0d92abf346b390298b00da4e83a3d05e9
  ....
```
