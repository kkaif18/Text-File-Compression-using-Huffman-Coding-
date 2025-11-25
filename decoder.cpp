#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <queue>

using namespace std;

//huffman coding logic
struct Node {
    char ch;
    int freq;
    Node *left, *right;
    
    //Variable to use for tie-breaking
    char minChar; 

    Node(char c, int f, Node* l = nullptr, Node* r = nullptr) {
        ch = c;
        freq = f;
        left = l;
        right = r;

        // If it's a leaf node, minChar is just the character itself
        if (left == nullptr && right == nullptr) {
            minChar = c;
        } 
        // If it's an internal node, pick the smallest char from its children
        else {
            minChar = (l->minChar < r->minChar) ? l->minChar : r->minChar;
        }
    }
};

// Logical Structure Operator that helps to change the default logic of the priority Queue( making the min heap that is, the smallest element is at the top of the tree)
struct Compare {
    bool operator()(Node* l, Node* r) {
        // Primary Sort: By Frequency (Low to High)
        if (l->freq != r->freq) {
            return l->freq > r->freq;
        }
        
        // Secondary Sort (Tie-Breaker): By Character (A before B)
        return l->minChar > r->minChar;
    }
};

// the function that provides the generated Code, more like implementing the huffman Coding Algorithm
void generateCodes(Node* root, string str, unordered_map<char, string> &huffmanCode){
    // base case
    if(root == NULL) return;
    // When a leaf Node is found, we can assign the string to it
    if(root->left == NULL && root->right == NULL){
        huffmanCode[root->ch] = str;
    }
    //when we move left in the tree a zero is added to the string, while a one is added for the right side
    generateCodes(root->left, str + "0", huffmanCode);
    generateCodes(root->right, str + "1", huffmanCode);
}

Node* buildHuffmanTree(map<char, int>& freq){
    
    priority_queue<Node*, vector<Node*>, Compare> pq;

    //filling up the queue
    for (auto pair : freq) {
        // Create a new node for every character found in the map
        pq.push(new Node(pair.first, pair.second));
    }
    // actually building of the huffman tree
    while(pq.size() != 1){
        //taking out the two characters with the least frequencies 
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        // making a new Node that takes the sum of the two frequencies taken earlier
        // this node is just the internal node of the huffman tree, so here the '$' placeholder acts as a dummy as , this internal node doesn't require any character in it
        Node* sum = new Node('$', left->freq + right->freq);

        // making connections
        sum->left = left;
        sum->right = right;

        pq.push(sum);
    }

    // the reamining is the sum of all the frequencies i.e. the length of the string, it acts as the root of the Tree
    Node*root = pq.top();

    // Creating a map that stores the assigned compressed code to each character
    unordered_map<char, string> huffmanCode;
    // running the function that generates the code for each char
    // the string "" will traverse through each node and assign a unique code to leaf node i.e when a character is met
    generateCodes(root, "", huffmanCode);

    return root;
}

map<char, int> createFreqMap(ifstream& inFile){
    map<char, int> freq;

    int size = 0;
    inFile.read((char*)&size, sizeof(size));
    //read is a function that takes a variable and takes values from the inFile, also we feed about how many bytes to take, it also has an cursor like thing, so when the next time it it is called it starts from the bytes it was left
    //here we take the size of the freq map from the bin file
    //similarly building the whole freq map
    for(int i = 0; i < size; i++){
        char ch;
        int fq;

        inFile.read((char*)&ch, sizeof(ch));
        inFile.read((char*)&fq, sizeof(fq));

        //mapping the characters to their frequencies
        freq[ch] = fq;
    }
    //now the reading cursor is exactly standing at the point of our compressed bits
    cout << "--- DEBUG: REBUILT MAP ---" << endl;
    for (auto pair : freq) {
        cout << "'" << pair.first << "' : " << pair.second << endl;
    }
    cout << "--------------------------" << endl;
    return freq;
}

void decoder(Node* root, ifstream& inFile){
    Node* curr = root;
    char byte;        //buffer to hold the 8 bits(1 byte) 

    //creating the output file genetrated which stores the decompressed data
    ofstream outFile("decompressed.txt");

    //inFile.get() raeds the next byte in the file, no matter what it is(the raeding cursor is at the compressed bits and not the freq map)
    //keeps the loop running until it can't raed any more bits in other words, end of the compressed bits
    while(inFile.get(byte)){

        // we loop over the byte, we loop from 7 to 0, because the computer reads the byte right to left
        //example : the byte: 0 0 0 0 0 1 0 1
        //          position: 7 6 5 4 3 2 1 0
        // for compression we are required to read the leftmost bit first, so every time we want to read the ith bit, we right shift by i times(byte >> i)
        // ex. 00001101 to read the 3rd bit(from the right(0-based index)) we use (byte << 3) to make it 00000001
        for(int i = 7; i>=0 ;i--){
            int bit = (byte >> i) & 1;
            // the use of & operator is to check if the bit we are checking is one or not, by & 1 if the last bit is 1 , it returns 1 else 0

            //huffman tree logic
            if(bit == 0) curr = curr->left;
            else curr = curr->right;

            //reached the leaf node, meaning we decoded a character
            if(curr->left == NULL && curr->right == NULL){
                // writing the character found in the decompressed file
                outFile << curr->ch;

                //making it ready for the next char
                curr = root;
            }
        }
    }
}
int main(){
    ifstream inFile("compressed.bin", ios::binary);
    if (!inFile) {
        cout << "File not found!" << endl;
        return 1;
    }
    map<char, int> freq = createFreqMap(inFile);

    Node* root = buildHuffmanTree(freq);

    decoder(root, inFile);
    cout<<"The decompression successfully created in decompressed.txt"<<endl;
}