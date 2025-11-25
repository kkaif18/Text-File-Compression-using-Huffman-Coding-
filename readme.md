# Text File Compression Tool (Huffman Coding) 🗜️

A high-performance, lossless file compression tool developed in C++. This project implements the **Huffman Coding algorithm** from scratch to compress text files by approximately 30-40% and decompress them back to their original state with zero data loss.

It demonstrates low-level systems programming concepts including **Bitwise Manipulation**, **Binary File I/O**, and **Custom Data Structure Serialization**.

---

## 🚀 Key Features

* **Lossless Compression:** Reduces file size without losing a single byte of data.
* **Bit-Level Packing:** Packs variable-length codes into 8-bit bytes using bitwise operations, maximizing storage efficiency.
* **Custom Header Serialization:** Stores the frequency map metadata at the start of the binary file, making the compressed file self-contained and portable.
* **Deterministic Tree Construction:** Implements a stable sorting mechanism to handle tie-breaking scenarios, ensuring consistent tree structures across different environments.
* **Memory Efficient:** Utilizes `std::priority_queue` (Min-Heap) for efficient $O(N \log N)$ tree building.

---

## 🛠️ Technical Implementation Details

This project goes beyond standard algorithm implementation by handling raw binary data and memory management. Here is how the core components are utilized:

### 1. Huffman Coding Algorithm
The core logic relies on a Greedy Algorithm strategy.
* **Frequency Analysis:** Scans the input text to calculate character frequencies.
* **Tree Build:** Constructs a binary tree where frequently used characters (like 'e' or 'a') are near the root (short codes) and rare characters (like 'z' or 'q') are deep leaves (long codes).
* **Prefix Property:** Ensures no character's binary code is a prefix of another, allowing for unambiguous decoding without separators.

### 2. Advanced Data Structures
* **Min-Heap (`std::priority_queue`):** Used to efficiently extract the two nodes with the lowest frequencies during tree construction. A custom `Compare` struct was implemented to ensure deterministic behavior by using character ASCII values as a tie-breaker.
* **Hash Map (`std::map`):** Used to store frequency counts and map characters to their generated binary strings.
* **Binary Tree (Custom `Node` Struct):** A pointer-based recursive structure used to traverse bits (Left/Right) during the decompression phase.

### 3. Bit Manipulation
Since C++ can only write data in 1-byte (8-bit) chunks, individual bits must be manually packed.
* **Bitwise Left Shift (`<<`):** Used to move bits into the correct position within a buffer byte.
* **Bitwise OR (`|`):** Used to set specific bits to `1` inside the buffer.
* **Bitwise AND (`&`):** Used during decoding to isolate and inspect a single bit from a read byte.
* **Padding Handling:** Logic handles the "leftover" bits if the compressed data stream size isn't a perfect multiple of 8.

### 4. File Handling (`fstream`)
* **Binary Mode (`ios::binary`):** Critical for preventing the OS from interpreting specific byte sequences (like `0x0A` or `0x0D`) as line breaks.
* **Serialization:** The encoder writes the "Header" (Frequency Map) using `write((char*)&int, sizeof(int))` to perform direct memory-to-disk copying.
* **Raw Byte Reading:** The decoder uses `get()` to read the file byte-by-byte to preserve the integrity of the binary stream.

---

## 💻 How to Run

### Prerequisites
* A C++ Compiler (G++ recommended)

### 1. Compilation
Open your terminal and compile both the Encoder and Decoder:

```bash
g++ encoder.cpp -o encoder
g++ decoder.cpp -o decoder
